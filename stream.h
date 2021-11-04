#ifndef STREAM_H
#define STREAM_H

#include <cstring>
#include <stdlib.h> //for NULL (conversations s2i s2l s2ul s2d) rand srand system
#include <stdio.h>  //for printf()
#include <stdint.h>

#define QWORD unsigned long int
#define DWORD unsigned int
#define WORD unsigned short int
#define BYTE unsigned char


#include "cryption.h"

class Cryption;

static const int frameStackSize = 10;

class Stream {
	public:
		Stream();  //construct0r
		Stream(int newSize); //new size
		~Stream(); //destruct0r of all pointers
		void createFrame(int id);

		void createFrameVarSize(int id);
		void createFrameVarSizeWord(int id);
		void endFrameVarSize();
		void endFrameVarSizeWord();
		void writeByte(int i);
		void writeByteA(int i);
		void writeByteC(int i);
		void writeBytes(char abyte0[], int i, int j);
		void writeByteS(int i);
		void writeBytes_reverse(char abyte0[], int i, int j);
		void writeBytes_reverseA(char abyte0[], int i, int j);
		void write3Byte(int i);
		void writeDWord(int i);
		void writeDWord_v1(int i);
		void writeDWord_v2(int i);
		void writeDWordBigEndian(int i);
		void writeFrameSize(int i);
		void writeFrameSizeWord(int i);
		void writeQWord(uint64_t l);
		void writeString(char* s);
		void writeWord(int i);
		void writeWordA(int i);
		void writeWordBigEndian(int i);
		void writeWordBigEndian_dup(int i);
		void writeWordBigEndianA(int i);

		void readBytes(char abyte0[], int i, int j);
		void readBytes_reverse(char abyte0[], int i, int j);
		void readBytes_reverseA(char abyte0[], int i, int j);
		DWORD readDWord();
		int readDWord_v1();
		int readDWord_v2();
		uint64_t readQWord();
		char readSignedByte();
		char readSignedByteA();
		char readSignedByteC();
		char readSignedByteS(); 
		int readSignedWord();
		int readSignedWordA();
		int readSignedWordBigEndian();
		int readSignedWordBigEndianA();
		void readString(char* output, int readSize);
		unsigned char readUnsignedByte();
		unsigned char readUnsignedByteA();
		unsigned char readUnsignedByteC();
		unsigned char readUnsignedByteS(); 
		int readUnsignedWord();
		int readUnsignedWordA();
		int readUnsignedWordBigEndian();
		int readUnsignedWordBigEndianA();

		char* buffer;
		int currentOffset; //offset of last position in buffer.

		void initBitAccess();
		void writeBits(int numBits, int value);
		void finishBitAccess();
		Cryption* packetEncryption;

	private:
		int bitPosition;
		int bitMaskOut[32];
		int frameStackPtr;
		int frameStack[frameStackSize];
};
#endif
