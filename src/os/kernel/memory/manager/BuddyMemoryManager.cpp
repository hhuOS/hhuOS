/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include "BuddyMemoryManager.h"

extern "C" {
#include "lib/libc/string.h"
#include "lib/libc/printf.h"
}

BuddyMemoryManager::BuddyMemoryManager(uint8_t minOrder) :
        MemoryManager(),
        minOrder(minOrder) {
}

BuddyMemoryManager::BuddyMemoryManager(const BuddyMemoryManager &copy) : BuddyMemoryManager() {

}

BuddyMemoryManager::~BuddyMemoryManager() {
    if(this->freeList == nullptr) {
        return;
    }

    // free complete freeList
    BuddyNode *tmp;

    for (int i = 0; i < (this->maxOrder + 1); i++) {
        tmp = this->freeList[i];
        if(!tmp) { continue; }
        do {
            void* addr = tmp->next;
            delete tmp;
            tmp = (BuddyNode*) addr;
        } while(tmp);
    }
}

void BuddyMemoryManager::init(uint32_t memoryStartAddress, uint32_t memoryEndAddress, bool doUnmap) {
    MemoryManager::init(memoryStartAddress, memoryEndAddress, doUnmap);

    // align startAddress; endAddress will be aligned through maxOrder
    this->memoryStartAddress -= memoryStartAddress % (1 << minOrder);

    this->maxOrder = 1;
    for(uint32_t i = 0; i < 32; i++) {
        if(((uint32_t) 1 << i) > freeMemory) {
            this->maxOrder = (uint8_t) (i - 1);
            break;
        }
    }

    uint32_t size = (maxOrder + 1) * sizeof(BuddyNode*);
    this->freeList = (BuddyNode**) new char[size];

    memset(this->freeList, 0, size);

    // set initial freeList
    this->freeList[maxOrder] = (BuddyNode*) new char[sizeof(BuddyNode)];
    this->freeList[maxOrder]->addr = (void*) this->memoryStartAddress;
    this->freeList[maxOrder]->next = nullptr;
}

String BuddyMemoryManager::getName() {
    return NAME;
}

void* BuddyMemoryManager::alloc(uint32_t size) {
    uint8_t i = 1;
    while ( ((uint32_t) 1 << (i)) < size + 1) { i++;} // get next power of two
    uint8_t order = (i < this->minOrder)? this->minOrder : i;

    // loop through freeList to find a free node
    for(;; i++) {
        // no free node found
        if ( i > this->maxOrder) {
            return nullptr;
        }
        // found free node in freeList
        if (this->freeList[i]) {
            break;
        }
    }

    void* block = this->freeList[i]->addr;
    // remove found node from freeList and free memory
    BuddyNode* node_to_free = this->freeList[i];
    this->freeList[i] = this->freeList[i]->next;
    delete node_to_free;

    // split until i==order
    while (i-- > order) {
        // calculate buddyNode of block
    	void* this_block = (void*) ((((char*) block - (char*) memoryStartAddress) ^ (1 << i)) + (char*) memoryStartAddress);

        // create new node
        auto* new_node = (BuddyNode*) new char[sizeof(BuddyNode)];
        new_node->addr = this_block;
        new_node->next = nullptr;

        // add the new node to the beginning of certain list
        if(!this->freeList[i]) {
            this->freeList[i] = new_node;
        } else {
            new_node->next = this->freeList[i];
            this->freeList[i] = new_node->next;
        }

    }

    // store order at the beginning
    *((uint8_t*)block) = order;
    freeMemory -= ((uint32_t) 1 << order);
    return (void*)((uint8_t*) block + 1);
}

void BuddyMemoryManager::free(void* ptr) {
    // get order from previous byte
    uint8_t  i = *((uint8_t*)ptr - 1);
    freeMemory += ((uint32_t) 1 << i);
    ptr = (void*)((uint8_t*)ptr - 1);

    for (;i <= this->maxOrder; i++) {
        // calculate buddyNode
        void* this_block = (void*) ((((char*) ptr - (char*)memoryStartAddress) ^ (1 << i)) + (char*) memoryStartAddress);

        // search for this in freeList
        BuddyNode* list_node = nullptr;
        BuddyNode *tmp = this->freeList[i];

        while(tmp) {
            if (tmp->addr == this_block) {
                // found this in free list
                list_node = tmp;
                break;
            }
            tmp = tmp->next;
        }

        // this is not in freeList
        if (!list_node) {
            auto* new_node = (BuddyNode*) new char[sizeof(BuddyNode)];
            new_node->addr = ptr;
            new_node->next = nullptr;

            if(!this->freeList[i]) {
                this->freeList[i] = new_node;
            } else {
                BuddyNode *tmp2 = this->freeList[i];
                while(tmp2->next) { tmp2 = tmp2->next; }
                tmp2->next = new_node;
            }
            return;
        }

        // found memory block -> merge them
        ptr = (ptr < this_block) ? ptr : this_block;

        // remove block from free list
        tmp = this->freeList[i];
        if(tmp == list_node) {
            this->freeList[i] = list_node->next;
        } else {
            while(tmp->next != list_node) {
                tmp = tmp->next;
            }
            tmp->next = list_node->next;
        }

        delete list_node;
    }
}

void BuddyMemoryManager::dump() {
    printf("  BuddyMemoryManager: Free list dump\n");
    printf("  ================\n");

    for(int i=0; i<=this->maxOrder; i++) {
        if(!this->freeList[i]) continue;

        BuddyNode *tmp = this->freeList[i];
        while(tmp) {
            printf("%p ", (tmp->addr == nullptr) ? nullptr : tmp->addr);
            tmp= tmp->next;
        }
        printf("\n");
    }
}





