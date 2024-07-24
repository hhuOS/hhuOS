#include "doomgeneric.h"
#include <stdio.h>


int main(int argc, char **argv) {
	doomgeneric_Create(argc, argv);
	while (1) {
		doomgeneric_Tick();
	}
	return 0;
}



void DG_Init() {
	printf("DG_Init\n");
}

void DG_DrawFrame() {
}

void DG_SleepMs(uint32_t ms) {
}

uint32_t DG_GetTicksMs() {
	return 0;
}

int DG_GetKey(int * pressed, unsigned char * key) {
}

void DG_SetWindowTitle(const char * title) {
}