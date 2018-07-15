#include "BuddyMemoryManager.h"
#include "lib/libc/printf.h"

BuddyMemoryManager::BuddyMemoryManager(uint32_t memoryStartAddress, uint32_t memoryEndAddress, uint8_t min_order) : MemoryManager(memoryStartAddress, memoryEndAddress) {
	this->min_order = min_order;
	// align startAddress - endAddress will be aligned through max_order
	uint32_t tmp = memoryStartAddress % (1 << min_order);
	memoryStartAddress -= tmp;

	this->freeMemory = memoryEndAddress - memoryStartAddress;
	this->max_order = 1;
	for(uint32_t i = 0; i < 32; i++) {
		if(((uint32_t) 1 << i) > freeMemory) {
			this->max_order = i - 1;
			break;
		}
	}

	this->freeMemory = max_order;
}

void BuddyMemoryManager::init() {

    uint32_t size = (max_order + 1) * sizeof(struct buddyNode*);
    this->freelist = (struct buddyNode**) new char[size];

    // set initial freelist
    this->freelist[max_order] = (struct buddyNode*) new char[sizeof(struct buddyNode)];
    this->freelist[max_order]->addr = (void*) this->memoryStartAddress;
    this->freelist[max_order]->next = nullptr;
}

BuddyMemoryManager::~BuddyMemoryManager() {
    // free complete freelist
    struct buddyNode *tmp;
    for (int i = 0; i < (this->max_order + 1); i++) {
        tmp = this->freelist[i];
        if(!tmp) { continue; }
        do {
            void* addr = tmp->next;
            delete tmp;
            tmp = (struct buddyNode*) addr;
        } while(tmp);
    }
}

void* BuddyMemoryManager::alloc(uint32_t size) {
    uint8_t i = 1;
    while ( ((uint32_t) 1 << (i)) < size + 1) { i++;} // get next power of two
    uint8_t order = (i < this->min_order)? this->min_order : i;

    // loop through freelist to find a free node
    for(;; i++) {
        // no free node found
        if ( i > this->max_order) {
            return nullptr;
        }
        // found free node in freelist
        if (this->freelist[i]) {
            break;
        }
    }

    void* block = this->freelist[i]->addr;
    // remove found node from freelist and free memory
    struct buddyNode* node_to_free = this->freelist[i];
    this->freelist[i] = this->freelist[i]->next;
    delete node_to_free;

    // split until i==order
    while (i-- > order) {
        // calculate buddyNode of block
    	void* this_block = (void*) ((((char*) block - (char*)memoryStartAddress) ^ (1 << i)) + (char*) memoryStartAddress);
        // create new node
        struct buddyNode* new_node = (struct buddyNode*) new char[sizeof(struct buddyNode)];
        new_node->addr = this_block;
        new_node->next = nullptr;

        // add the new node to the beginning of certain list
        if(!this->freelist[i]) {
            this->freelist[i] = new_node;
        } else {
            new_node->next = this->freelist[i];
            this->freelist[i] = new_node->next;
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

    for (;i <= this->max_order; i++) {
        // calculate buddyNode
        void* this_block = (void*) ((((char*) ptr - (char*)memoryStartAddress) ^ (1 << i)) + (char*) memoryStartAddress);

        // search for this in freelist
        struct buddyNode* list_node = nullptr;
        struct buddyNode *tmp = this->freelist[i];
        while(tmp) {
            if (tmp->addr == this_block) {
                // found this in free list
                list_node = tmp;
                break;
            }
            tmp = tmp->next;
        }

        // this is not in freelist
        if (!list_node) {
            struct buddyNode* new_node = (struct buddyNode*) new char[sizeof(struct buddyNode)];
            new_node->addr = ptr;
            new_node->next = nullptr;

            if(!this->freelist[i]) {
                this->freelist[i] = new_node;
            } else {
                struct buddyNode *tmp = this->freelist[i];
                while(tmp->next) { tmp = tmp->next; }
                tmp->next = new_node;
            }
            return;
        }

        // found memory block -> merge them
        ptr = (ptr < this_block) ? ptr : this_block;

        // remove block from free list
        tmp = this->freelist[i];
        if(tmp == list_node) {
            this->freelist[i] = list_node->next;
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
    printf("____ Freelist Dump ____\n");
    for(int i=0; i<=this->max_order; i++) {
        if(!this->freelist[i]) continue;

        struct buddyNode *tmp = this->freelist[i];
        while(tmp) {
            printf("%p ", (tmp->addr == nullptr)? 0 : tmp->addr);
            tmp= tmp->next;
        }
        printf("\n");
    }
}





