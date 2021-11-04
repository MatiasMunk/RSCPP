#include "stream.h"

Stream::Stream() {
	buffer = new char[5000];
	memset(buffer, 0, 5000); 

	currentOffset = 0;
	for(int i = 0; i < 32; i++)
		bitMaskOut[i] = (1 << i) - 1;

	frameStackPtr = -1;
	memset(frameStack, 0, frameStackSize*sizeof(int));
}

Stream::Stream(int newSize) {
	buffer = new char[newSize];
	memset(buffer, 0, newSize);

	currentOffset = 0;
	for(int i = 0; i < 32; i++)
		bitMaskOut[i] = (1 << i) - 1;
	frameStackPtr = -1;
	memset(frameStack, 0, frameStackSize*sizeof(int));
}

Stream::~Stream() {
 delete [] buffer;
}

void Stream::createFrame(int id) {

	//TODO
	buffer[currentOffset++] = (unsigned char) (id + packetEncryption->getNextKey());
}

void Stream::createFrameVarSize(int id) { // creates a variable sized
											// frame
	buffer[currentOffset++] = (unsigned char) (id + packetEncryption->getNextKey());
	buffer[currentOffset++] = 0; // placeholder for size byte
	if (frameStackPtr >= frameStackSize - 1) {
		printf("Stack overflow\n");
	} else
		frameStack[++frameStackPtr] = currentOffset;
}

void Stream::createFrameVarSizeWord(int id) { // creates a variable sized
												// frame
	buffer[currentOffset++] = (unsigned char) (id + packetEncryption->getNextKey());
	writeWord(0); // placeholder for size word
	if (frameStackPtr >= frameStackSize - 1) {
		printf("Stack overflow\n");
	} else
		frameStack[++frameStackPtr] = currentOffset;
}

void Stream::endFrameVarSize() {// ends a variable sized frame
	if (frameStackPtr < 0)
		printf("Stack empty\n");
	else
		writeFrameSize(currentOffset - frameStack[frameStackPtr--]);
}

void Stream::endFrameVarSizeWord() { // ends a variable sized frame
	if (frameStackPtr < 0)
		printf("Stack empty\n");
	else
		writeFrameSizeWord(currentOffset - frameStack[frameStackPtr--]);
}


//Write types
void Stream::writeByte(int i) {
	buffer[currentOffset++] = (char) i; //umm (byte)?
}

void Stream::writeByteA(int i) {
	buffer[currentOffset++] = (char) (i + 128);
}

void Stream::writeByteC(int i) {
	buffer[currentOffset++] = (char) (-i);
}

void Stream::writeBytes(char abyte0[], int i, int j) {
	for (int k = j; k < j + i; k++)
		buffer[currentOffset++] = abyte0[k];

}

void Stream::writeByteS(int i) {
	buffer[currentOffset++] = (char) (128 - i);
}

void Stream::writeBytes_reverse(char abyte0[], int i, int j) {
	for (int k = (j + i) - 1; k >= j; k--)
		buffer[currentOffset++] = abyte0[k];

}

void Stream::writeBytes_reverseA(char abyte0[], int i, int j) {
	for (int k = (j + i) - 1; k >= j; k--)
		buffer[currentOffset++] = (char) (abyte0[k] + 128);

}

void Stream::write3Byte(int i) {
	buffer[currentOffset++] = (char) (i >> 16);
	buffer[currentOffset++] = (char) (i >> 8);
	buffer[currentOffset++] = (char) i;
}

void Stream::writeDWord(int i) {
	buffer[currentOffset++] = (char) (i >> 24);
	buffer[currentOffset++] = (char) (i >> 16);
	buffer[currentOffset++] = (char) (i >> 8);
	buffer[currentOffset++] = (char) i;
}

void Stream::writeDWord_v1(int i) {
	buffer[currentOffset++] = (char) (i >> 8);
	buffer[currentOffset++] = (char) i;
	buffer[currentOffset++] = (char) (i >> 24);
	buffer[currentOffset++] = (char) (i >> 16);
}

void Stream::writeDWord_v2(int i) {
	buffer[currentOffset++] = (char) (i >> 16);
	buffer[currentOffset++] = (char) (i >> 24);
	buffer[currentOffset++] = (char) i;
	buffer[currentOffset++] = (char) (i >> 8);
}

void Stream::writeDWordBigEndian(int i) {
	buffer[currentOffset++] = (char) i;
	buffer[currentOffset++] = (char) (i >> 8);
	buffer[currentOffset++] = (char) (i >> 16);
	buffer[currentOffset++] = (char) (i >> 24);
}

void Stream::writeFrameSize(int i) {
	buffer[currentOffset - i - 1] = (char) i;
}

void Stream::writeFrameSizeWord(int i) {
	buffer[currentOffset - i - 2] = (char) (i >> 8);
	buffer[currentOffset - i - 1] = (char) i;
}

void Stream::writeQWord(uint64_t l) {
	buffer[currentOffset++] = (char) (unsigned int) (l >> 56);
	buffer[currentOffset++] = (char) (unsigned int) (l >> 48);
	buffer[currentOffset++] = (char) (unsigned int) (l >> 40);
	buffer[currentOffset++] = (char) (unsigned int) (l >> 32);
	buffer[currentOffset++] = (char) (unsigned int) (l >> 24);
	buffer[currentOffset++] = (char) (unsigned int) (l >> 16);
	buffer[currentOffset++] = (char) (unsigned int) (l >> 8);
	buffer[currentOffset++] = (char) (unsigned int) l;
}

void Stream::writeString(char* s) {
	memcpy(buffer+currentOffset,s,strlen(s));
	currentOffset += strlen(s);
	buffer[currentOffset++] = 10;
}

void Stream::writeWord(int i) {
	buffer[currentOffset++] = (char) (i >> 8);
	buffer[currentOffset++] = (char) i;
}

void Stream::writeWordA(int i) {
	buffer[currentOffset++] = (char) (i >> 8);
	buffer[currentOffset++] = (char) (i + 128);
}

void Stream::writeWordBigEndian(int i) {
	buffer[currentOffset++] = (char) i;
	buffer[currentOffset++] = (char) (i >> 8);
}
void Stream::writeWordBigEndian_dup(int i) {
	buffer[currentOffset++] = (char) i;
	buffer[currentOffset++] = (char) (i >> 8);
}

void Stream::writeWordBigEndianA(int i) {
	buffer[currentOffset++] = (char) (i + 128);
	buffer[currentOffset++] = (char) (i >> 8);
}

//bit editing
void Stream::initBitAccess() {
	bitPosition = currentOffset * 8;
}

void Stream::writeBits(int numBits, int value) {
	int bytePos = bitPosition >> 3;
	int bitOffset = 8 - (bitPosition & 7);
	bitPosition += numBits;
	for(; numBits > bitOffset; bitOffset = 8) {
		buffer[bytePos] &= ~ bitMaskOut[bitOffset];		// mask out the desired area
		buffer[bytePos++] |= (value >> (numBits-bitOffset)) & bitMaskOut[bitOffset];
		numBits -= bitOffset;
	}
	if(numBits == bitOffset) {
		buffer[bytePos] &= ~ bitMaskOut[bitOffset];
		buffer[bytePos] |= value & bitMaskOut[bitOffset];
	} else {
		buffer[bytePos] &= ~ (bitMaskOut[numBits]<<(bitOffset - numBits));
		buffer[bytePos] |= (value&bitMaskOut[numBits]) << (bitOffset - numBits);
	}
}

void Stream::finishBitAccess() {
	currentOffset = (bitPosition + 7) / 8;
}

//read types

void Stream::readBytes(char abyte0[], int i, int j) {
	for (int k = j; k < j + i; k++)
		abyte0[k] = buffer[currentOffset++];

}

void Stream::readBytes_reverse(char abyte0[], int i, int j) {
	for (int k = (j + i) - 1; k >= j; k--) {
		abyte0[k] = buffer[currentOffset++];

	}
}

void Stream::readBytes_reverseA(char abyte0[], int i, int j) {
	for (int k = (j + i) - 1; k >= j; k--)
		abyte0[k] = (char) (buffer[currentOffset++] - 128);

}

DWORD Stream::readDWord() {
	currentOffset += 4;
	return ((buffer[currentOffset - 4] & 0xff) << 24)
			+ ((buffer[currentOffset - 3] & 0xff) << 16)
			+ ((buffer[currentOffset - 2] & 0xff) << 8)
			+ (buffer[currentOffset - 1] & 0xff);
}

int Stream::readDWord_v1() {
	currentOffset += 4;
	return ((buffer[currentOffset - 2] & 0xff) << 24)
			+ ((buffer[currentOffset - 1] & 0xff) << 16)
			+ ((buffer[currentOffset - 4] & 0xff) << 8)
			+ (buffer[currentOffset - 3] & 0xff);
}

int Stream::readDWord_v2() {
	currentOffset += 4;
	return ((buffer[currentOffset - 3] & 0xff) << 24)
			+ ((buffer[currentOffset - 4] & 0xff) << 16)
			+ ((buffer[currentOffset - 1] & 0xff) << 8)
			+ (buffer[currentOffset - 2] & 0xff);
}

uint64_t Stream::readQWord() {
	DWORD dw1 = readDWord();
	DWORD dw2 = readDWord();
	return (((uint64_t) dw1) << 32) | (uint64_t)dw2;
}

char Stream::readSignedByte() {
	return buffer[currentOffset++];
}

char Stream::readSignedByteA() {
	return (char) (buffer[currentOffset++] - 128);
}

char Stream::readSignedByteC() {
	return (char) (-buffer[currentOffset++]);
}

char Stream::readSignedByteS() {
	return (char) (128 - buffer[currentOffset++]);
}

int Stream::readSignedWord() {
	currentOffset += 2;
	int i = ((buffer[currentOffset - 2] & 0xff) << 8)
			+ (buffer[currentOffset - 1] & 0xff);
	if (i > 32767) {
		i -= 0x10000;
	}
	return i;
}
int Stream::readSignedWordA() {
	currentOffset += 2;
	int i = ((buffer[currentOffset - 2] & 0xff) << 8)
			+ (buffer[currentOffset - 1] - 128 & 0xff);
	if (i > 32767) {
		i -= 0x10000;
	}
	return i;
}

int Stream::readSignedWordBigEndian() {
	currentOffset += 2;
	int i = ((buffer[currentOffset - 1] & 0xff) << 8)
			+ (buffer[currentOffset - 2] & 0xff);
	if (i > 32767)
		i -= 0x10000;
	return i;
}

int Stream::readSignedWordBigEndianA() {
	currentOffset += 2;
	int i = ((buffer[currentOffset - 1] & 0xff) << 8)
			+ (buffer[currentOffset - 2] - 128 & 0xff);
	if (i > 32767)
		i -= 0x10000;
	return i;
}

void Stream::readString(char* output, int readSize) { //yah C++ sux u can't return char*'s without using supdup() which is very dangerous
	int i = currentOffset;
	while (buffer[currentOffset++] != 10);
	if((currentOffset - i) - 1 > readSize) return;
	memcpy(output, buffer+i, (currentOffset - i) - 1);

	output[(currentOffset - i)-1] = '\0'; //null terminator
}

unsigned char Stream::readUnsignedByte() {
	return buffer[currentOffset++] & 0xff;
}

unsigned char Stream::readUnsignedByteA() {
	return buffer[currentOffset++] - 128 & 0xff;
}

unsigned char Stream::readUnsignedByteC() {
	return -buffer[currentOffset++] & 0xff;
}

unsigned char Stream::readUnsignedByteS() {
	return 128 - buffer[currentOffset++] & 0xff;
}

int Stream::readUnsignedWord() {
	currentOffset += 2;
	return ((buffer[currentOffset - 2] & 0xff) << 8)
			+ (buffer[currentOffset - 1] & 0xff);
}

int Stream::readUnsignedWordA() {
	currentOffset += 2;
	return ((buffer[currentOffset - 2] & 0xff) << 8)
			+ (buffer[currentOffset - 1] - 128 & 0xff);
}

int Stream::readUnsignedWordBigEndian() {
	currentOffset += 2;
	return ((buffer[currentOffset - 1] & 0xff) << 8)
			+ (buffer[currentOffset - 2] & 0xff);
}

int Stream::readUnsignedWordBigEndianA() {
	currentOffset += 2;
	return ((buffer[currentOffset - 1] & 0xff) << 8)
			+ (buffer[currentOffset - 2] - 128 & 0xff);
}
