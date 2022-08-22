#include <combo.h>

void DisplayTextBox(void* play, s16 textId);

#if defined(GAME_OOT)
# define ITEM_BASE_MM 0x100
#else
# define ITEM_BASE_MM 0x0
#endif

static void addItemOot(u16 itemId)
{
    switch (itemId)
    {
    case ITEM_OOT_OCARINA_OF_TIME:
        gOotSave.inventory[7] = ITEM_OOT_OCARINA_OF_TIME;
        gComboSave.ootOcarinas |= 0x2;
        break;
    case ITEM_OOT_FAIRY_OCARINA:
        if (gOotSave.inventory[7] == ITEM_NONE)
            gOotSave.inventory[7] = ITEM_OOT_FAIRY_OCARINA;
        gComboSave.ootOcarinas |= 0x1;
        break;
    /* Strength */
    case ITEM_OOT_GORON_BRACELET:
        gOotSave.upgrades.strength = 1;
        break;
    case ITEM_OOT_SILVER_GAUNTLETS:
        gOotSave.upgrades.strength = 2;
        break;
    case ITEM_OOT_GOLDEN_GAUNTLETS:
        gOotSave.upgrades.strength = 3;
        break;
    /* Dive */
    case ITEM_OOT_SILVER_SCALE:
        gOotSave.upgrades.dive = 1;
        break;
    case ITEM_OOT_GOLDEN_SCALE:
        gOotSave.upgrades.dive = 2;
        break;
    }

    /* DEBUG */
    gOotSave.inventory[22] = ITEM_OOT_POCKET_EGG;
    gComboSave.ootTradeAdult = 0xffffffff;

    gOotSave.inventory[23] = ITEM_OOT_WEIRD_EGG;
    gComboSave.ootTradeChild = 0xffffffff;

    gOotSave.inventory[9] = ITEM_OOT_LONGSHOT;
    gComboSave.ootHookshots = 0xffffffff;
}

static void addItemMm(u16 itemId)
{
    gOotSave.inventory[0] = ITEM_OOT_MASK_OF_TRUTH;
    gMmSave.inventory.items[0] = ITEM_MM_MASK_TRUTH;
}

int comboAddItem(void* play, u16 itemId)
{
    if ((itemId & 0x100) == ITEM_BASE_MM)
        addItemMm(itemId & 0xff);
    else
        addItemOot(itemId & 0xff);

    return -1;
}

int comboAddItemFromChest(void* play, s16 chestId)
{
    u16 itemId;
    u16 textId;

    chestId--;
    itemId = kExtendedGetItems[chestId].itemId;
    itemId |= (chestId & 0x100);

    comboTextHijackItem(play, itemId);
    return comboAddItem(play, itemId);
}
