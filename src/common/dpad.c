#include <combo.h>

#define DPAD_DOWN   0
#define DPAD_UP     1
#define DPAD_LEFT   2
#define DPAD_RIGHT  3

static void* sDpadIconBuffer;
static u8 sDpadItems[] = { ITEM_NONE, ITEM_NONE, ITEM_NONE, ITEM_NONE };
static u8 sDpadItemsOld[] = { ITEM_NONE, ITEM_NONE, ITEM_NONE, ITEM_NONE };

static const int kDpadPosX = 265;
static const int kDpadPosY = 70;

static const int kDpadOffX[] = { 0, 0, -1, 1 };
static const int kDpadOffY[] = { 1, -1, 0, 0 };

static float kDpadItemScale = 0.4f;

static int canShowDpad(void)
{
#if defined(GAME_OOT)
    if (gSaveContext.noInterface)
        return 0;
#endif
    return 1;
}

static int canUseDpad(void)
{
    if (!canShowDpad())
        return 0;
    return 1;
}

static void reloadIcons(GameState_Play* play)
{
    if (!sDpadIconBuffer)
    {
        sDpadIconBuffer = malloc(32 * 32 * 4 * 4);
    }

    for (int i = 0; i < 4; ++i)
    {
        if (sDpadItems[i] != sDpadItemsOld[i] && sDpadItems[i] != ITEM_NONE)
        {
#if defined(GAME_OOT)
            DMARomToRam((kComboDmaData[8].pstart + 0x1000 * sDpadItems[i]) | PI_DOM1_ADDR2, sDpadIconBuffer + (i * 32 * 32 * 4), 32 * 32 * 4);
#else
            LoadIcon(0xa36c10, sDpadItems[i], sDpadIconBuffer + (i * 32 * 32 * 4), 0x1000);
#endif
            sDpadItemsOld[i] = sDpadItems[i];
        }
    }
}

void comboDpadDraw(GameState_Play* play)
{
    u8 alpha;

    if (!canShowDpad())
        return;

    alpha = 0xff;
    reloadIcons(play);

#if defined(GAME_OOT)
    alpha = (u8)play->interfaceCtx.alpha.health;
#endif

    /* Init */
    OPEN_DISPS(play->gs.gfx);
    gDPPipeSync(OVERLAY_DISP++);
    gSPSegment(OVERLAY_DISP++, 0x06, gCustomKeep);
    gSPSegment(OVERLAY_DISP++, 0x07, sDpadIconBuffer);
    gDPSetPrimColor(OVERLAY_DISP++, 0, 0x80, 0xff, 0xff, 0xff, alpha);
    CLOSE_DISPS();

    /* Draw */
    comboDrawInit2D(play);
    comboDrawBlit2D(play, 0x06000000, 32, 32, kDpadPosX, kDpadPosY, 0.5f);

    for (int i = 0; i < 4; ++i)
    {
        if (sDpadItems[i] != ITEM_NONE)
        {
            comboDrawBlit2D(play, 0x07000000 | (i * 32 * 32 * 4), 32, 32, kDpadPosX + kDpadOffX[i] * 16, kDpadPosY + kDpadOffY[i] * 16, kDpadItemScale);
        }
    }
}

#if defined(GAME_OOT)
static void toggleBoots(GameState_Play* play, s16 itemId)
{
    u16 targetBoots;

    targetBoots = (itemId == ITEM_OOT_HOVER_BOOTS) ? 3 : 2;
    if (gSave.currentEquipment.boots == targetBoots)
        gSave.currentEquipment.boots = 1;
    else
        gSave.currentEquipment.boots = targetBoots;
    UpdateEquipment(play, GET_LINK(play));
    PlaySound(0x835);
}
#endif

#if defined(GAME_OOT)
static void dpadUseItem(GameState_Play* play, int index)
{
    s16 itemId;
    void (*Player_UseButton)(GameState_Play* play, Actor_Player* link, s16 itemId);

    itemId = sDpadItems[index];
    if (itemId == ITEM_NONE)
        return;
    if (itemId == ITEM_OOT_HOVER_BOOTS || itemId == ITEM_OOT_IRON_BOOTS)
    {
        toggleBoots(play, itemId);
    }
    else
    {
        Player_UseButton = OverlayAddr(0x80834000);
        Player_UseButton(play, GET_LINK(play), itemId);
    }
}
#endif

#if defined(GAME_MM)
static void dpadUseItem(GameState_Play* play, int index)
{

}
#endif

#if defined(GAME_OOT)
static void dpadSetItems(GameState_Play* play)
{
    /* Update the items */
    sDpadItems[DPAD_DOWN] = gSave.inventory[ITS_OOT_OCARINA];

    if (gSave.age == AGE_CHILD)
    {
        sDpadItems[DPAD_LEFT] = ITEM_NONE;
        sDpadItems[DPAD_RIGHT] = ITEM_NONE;
    }
    else
    {
        sDpadItems[DPAD_LEFT] = (gSave.equipment.boots & EQ_OOT_BOOTS_HOVER) ? ITEM_OOT_HOVER_BOOTS : ITEM_NONE;
        sDpadItems[DPAD_RIGHT] = (gSave.equipment.boots & EQ_OOT_BOOTS_IRON) ? ITEM_OOT_IRON_BOOTS : ITEM_NONE;
    }
}
#endif

#if defined(GAME_MM)
static void dpadSetItems(GameState_Play* play)
{
    /* Update the items */
    sDpadItems[DPAD_DOWN] = gSave.inventory.items[ITS_MM_OCARINA];
    sDpadItems[DPAD_UP] = gSave.inventory.items[ITS_MM_MASK_DEKU];
    sDpadItems[DPAD_LEFT] = gSave.inventory.items[ITS_MM_MASK_GORON];
    sDpadItems[DPAD_RIGHT] = gSave.inventory.items[ITS_MM_MASK_ZORA];
}
#endif

void comboDpadUpdate(GameState_Play* play)
{
    u32 buttons;

    dpadSetItems(play);
    if (!canUseDpad())
        return;

    /* Detect button press */
    buttons = play->gs.input[0].pressed.buttons;
    if (buttons & U_JPAD)
        dpadUseItem(play, DPAD_UP);
    else if (buttons & D_JPAD)
        dpadUseItem(play, DPAD_DOWN);
    else if (buttons & L_JPAD)
        dpadUseItem(play, DPAD_LEFT);
    else if (buttons & R_JPAD)
        dpadUseItem(play, DPAD_RIGHT);
}
