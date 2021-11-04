#ifndef ITEM_H
#define ITEM_H

#include "constants.h"
#include "misc.h"
#include <fstream> //reading files.

using namespace std;

class Item {
	public:
		Item();  //construct0r
		~Item(); //destruct0r of all pointers
		bool isItemStackable(int itemId);
		bool isItemNoted(int itemId);
		int itemType(int itemId); //this may be slow.. need to sort array and maybe do binarysearch
		bool isPlate(int itemID);
		bool isFullHelm(int itemID);
		bool isFullMask(int itemID);
		const char* getBonusName(int index);
		ItemBonus getItemBonuses(int itemId);
		int randomCape();
		int randomBoots();
		int randomGloves();
		int randomShield();
		int randomHat();
		int randomAmulet();
		int randomArrows();
		int randomRing();
		int randomBody();
		int randomLegs();
		int randomWeapon();
	private:
        Item(const Item &orig);
        void operator=(const Item &rhs);
        // constructor, destructor, copy constructor and assignment are all
        // private (and in the case of assignment and the cctor, 
        // unimplemented even!) so this class *cannot* be copied under 
        // any circumstances.
};

extern Item item;

#endif
