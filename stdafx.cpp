#include "stdafx.h"

ofstream LogFile("log.txt");

unsigned int RandomChance(unsigned int n) {
	//chance = (100 / n) %
	if(n <= 1) return 1;
	return (rand() % n > n - 2);
}

int RandomValue(int Min, int Max) {
	if(Min > Max) return 0;
	if(Min == Max) return Min;
	return rand() % (Max - Min + 1) + Min;
}

bool IsPowerOfTwo(unsigned long x) {
	return (x != 0) && ((x & (x - 1)) == 0);
}
