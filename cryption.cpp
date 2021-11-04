#include "cryption.h"
#include <stdio.h>

Cryption::Cryption(unsigned int ai[4]) {
	cryptVar1 = 0;
	cryptVar2 = 0;
	cryptVar3 = 0;

	for(int i = 0; i < 256; i++) {
		keySetArray[i] = 0;
		cryptArray[i] = 0;
	}

	for(int i = 0; i < 4; i++)
        keySetArray[i] = ai[i];

	initializeKeySet();
}

Cryption::~Cryption() {
	for(int i = 0; i < 256; i++) {
		keySetArray[i] = 0;
		cryptArray[i] = 0;
	}
	cryptVar1 = 0;
	cryptVar2 = 0;
	cryptVar3 = 0;
}

unsigned int Cryption::getNextKey() {
	if(keyArrayIdx-- == 0)
	{
		generateNextKeySet();
		keyArrayIdx = 255;
	}
	return keySetArray[keyArrayIdx];
}

void Cryption::generateNextKeySet()
{
	cryptVar2 += ++cryptVar3;
	for(int i = 0; i < 256; i++)
	{
		unsigned int j = cryptArray[i];
		if((i & 3) == 0)
			cryptVar1 ^= cryptVar1 << 13;
		else
		if((i & 3) == 1)
			cryptVar1 ^= cryptVar1 >> 6;
		else
		if((i & 3) == 2)
			cryptVar1 ^= cryptVar1 << 2;
		else
		if((i & 3) == 3)
			cryptVar1 ^= cryptVar1 >> 16;
		cryptVar1 += cryptArray[i + 128 & 0xff];
		unsigned int k;
		cryptArray[i] = k = cryptArray[(j & 0x3fc) >> 2] + cryptVar1 + cryptVar2;
		keySetArray[i] = cryptVar2 = cryptArray[(k >> 8 & 0x3fc) >> 2] + j;
	}
}

void Cryption::initializeKeySet() {
    unsigned int i1;
    unsigned int j1;
    unsigned int k1;
    unsigned int l1;
    unsigned int i2;
    unsigned int j2;
    unsigned int k2;
    unsigned l = i1 = j1 = k1 = l1 = i2 = j2 = k2 = 0x9e3779b9;
    for(int i = 0; i < 4; i++)
    {
        l ^= i1 << 11;
        k1 += l;
        i1 += j1;
        i1 ^= j1 >> 2;
        l1 += i1;
        j1 += k1;
        j1 ^= k1 << 8;
        i2 += j1;
        k1 += l1;
        k1 ^= l1 >> 16;
        j2 += k1;
        l1 += i2;
        l1 ^= i2 << 10;
        k2 += l1;
        i2 += j2;
        i2 ^= j2 >> 4;
        l += i2;
        j2 += k2;
        j2 ^= k2 << 8;
        i1 += j2;
        k2 += l;
        k2 ^= l >> 9;
        j1 += k2;
        l += i1;
    }

    for(int j = 0; j < 256; j += 8)
    {
        l += keySetArray[j];
        i1 += keySetArray[j + 1];
        j1 += keySetArray[j + 2];
        k1 += keySetArray[j + 3];
        l1 += keySetArray[j + 4];
        i2 += keySetArray[j + 5];
        j2 += keySetArray[j + 6];
        k2 += keySetArray[j + 7];
        l ^= i1 << 11;
        k1 += l;
        i1 += j1;
        i1 ^= j1 >> 2;
        l1 += i1;
        j1 += k1;
        j1 ^= k1 << 8;
        i2 += j1;
        k1 += l1;
        k1 ^= l1 >> 16;
        j2 += k1;
        l1 += i2;
        l1 ^= i2 << 10;
        k2 += l1;
        i2 += j2;
        i2 ^= j2 >> 4;
        l += i2;
        j2 += k2;
        j2 ^= k2 << 8;
        i1 += j2;
        k2 += l;
        k2 ^= l >> 9;
        j1 += k2;
        l += i1;
        cryptArray[j] = l;
        cryptArray[j + 1] = i1;
        cryptArray[j + 2] = j1;
        cryptArray[j + 3] = k1;
        cryptArray[j + 4] = l1;
        cryptArray[j + 5] = i2;
        cryptArray[j + 6] = j2;
        cryptArray[j + 7] = k2;
    }

    for(int k = 0; k < 256; k += 8)
    {
        l += cryptArray[k];
        i1 += cryptArray[k + 1];
        j1 += cryptArray[k + 2];
        k1 += cryptArray[k + 3];
        l1 += cryptArray[k + 4];
        i2 += cryptArray[k + 5];
        j2 += cryptArray[k + 6];
        k2 += cryptArray[k + 7];
        l ^= i1 << 11;
        k1 += l;
        i1 += j1;
        i1 ^= j1 >> 2;
        l1 += i1;
        j1 += k1;
        j1 ^= k1 << 8;
        i2 += j1;
        k1 += l1;
        k1 ^= l1 >> 16;
        j2 += k1;
        l1 += i2;
        l1 ^= i2 << 10;
        k2 += l1;
        i2 += j2;
        i2 ^= j2 >> 4;
        l += i2;
        j2 += k2;
        j2 ^= k2 << 8;
        i1 += j2;
        k2 += l;
        k2 ^= l >> 9;
        j1 += k2;
        l += i1;
        cryptArray[k] = l;
        cryptArray[k + 1] = i1;
        cryptArray[k + 2] = j1;
        cryptArray[k + 3] = k1;
        cryptArray[k + 4] = l1;
        cryptArray[k + 5] = i2;
        cryptArray[k + 6] = j2;
        cryptArray[k + 7] = k2;
    }

    generateNextKeySet();
    keyArrayIdx = 256;
}
