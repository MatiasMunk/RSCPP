#ifndef CRYPTION_H
#define CRYPTION_H
class Cryption {
	public:
		Cryption(unsigned int ai[4]); //construct0r
		~Cryption(); //destruct of all pointers
		unsigned int getNextKey();
	private:
		void generateNextKeySet();
		void initializeKeySet();
		unsigned int keyArrayIdx;
		unsigned int keySetArray[256];
		unsigned int cryptArray[256];
		unsigned int cryptVar1;
		unsigned int cryptVar2;
		unsigned int cryptVar3;
};
#endif
