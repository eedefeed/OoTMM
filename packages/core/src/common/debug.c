#include <combo.h>
#include <combo/custom.h>
#include <combo/entrance.h>

#if defined(DEBUG)

#define DEBUGMENU_PAGE_NONE     0
#define DEBUGMENU_PAGE_MAIN     1
#define DEBUGMENU_PAGE_WARP     2
#define DEBUGMENU_PAGE_WARP2    3
#define DEBUGMENU_PAGE_TIME     4

#define DEBUG_X 30
#define DEBUG_Y 30

#define OPTION_NONE     0
#define OPTION_OK       1
#define OPTION_CANCEL   2

typedef struct
{
    const char* name;
    u32         data;
}
DebugMenuEntry;

u8 gDebugMenuOpen;
static s16 sCursor[3];
static u8 sDebugPage;
static ControllerInput sInput;
static const DebugMenuEntry* sMenuWarp;

#if defined(GAME_MM)
static int sTimeDay = 1;
static int sTimeHour = 6;
static int sTimeMinute = 0;
#endif

typedef void (*DebugMenuFunc)(void);

static const Gfx kDlistLoadIA4_8x12[] = {
    gsDPLoadTextureTile_4b(
        0x09000000,
        G_IM_FMT_IA,
        8, 12,
        0, 0,
        7, 11,
        0,
        G_TX_WRAP, G_TX_WRAP,
        G_TX_NOMASK, G_TX_NOMASK,
        G_TX_NOLOD, G_TX_NOLOD
    ),
    gsDPTileSync(),
    gsSPEndDisplayList(),
};

static const DebugMenuEntry kMenuWarpCommon[] = {
    { "Link's House",   ENTR_OOT_HOUSE_LINK },
    { "Temple of Time", ENTR_OOT_TEMPLE_OF_TIME },
    { "Clock Town",     MASK_FOREIGN_ENTRANCE | ENTR_MM_CLOCK_TOWN },
    { NULL, 0 },
};

static const DebugMenuEntry kMenuWarpDungeons[] = {
    { "Deku Tree",              ENTR_OOT_DEKU_TREE },
    { "Dodongo Cavern",         ENTR_OOT_DODONGO_CAVERN },
    { "Jabu-Jabu",              ENTR_OOT_JABU_JABU },
    { "Forest",                 ENTR_OOT_TEMPLE_FOREST },
    { "Fire",                   ENTR_OOT_TEMPLE_FIRE },
    { "Water",                  ENTR_OOT_TEMPLE_WATER },
    { "Shadow",                 ENTR_OOT_TEMPLE_SHADOW },
    { "Spirit",                 ENTR_OOT_TEMPLE_SPIRIT },
    { "Ganon",                  ENTR_OOT_GANON_CASTLE },
    { "Tower",                  ENTR_OOT_GANON_TOWER },
    { "BotW",                   ENTR_OOT_BOTTOM_OF_THE_WELL },
    { "Ice Cavern",             ENTR_OOT_ICE_CAVERN },
    { "GTG",                    ENTR_OOT_GERUDO_TRAINING_GROUNDS },
    { "Woodfall",               MASK_FOREIGN_ENTRANCE | ENTR_MM_TEMPLE_WOODFALL },
    { "Snowhead",               MASK_FOREIGN_ENTRANCE | ENTR_MM_TEMPLE_SNOWHEAD },
    { "Great Bay",              MASK_FOREIGN_ENTRANCE | ENTR_MM_TEMPLE_GREAT_BAY },
    { "Stone Tower",            MASK_FOREIGN_ENTRANCE | ENTR_MM_TEMPLE_STONE_TOWER },
    { "Stone Tower Inverted",   MASK_FOREIGN_ENTRANCE | ENTR_MM_TEMPLE_STONE_TOWER_INVERTED },
    { "SSH",                    MASK_FOREIGN_ENTRANCE | ENTR_MM_SPIDER_HOUSE_SWAMP },
    { "OSH",                    MASK_FOREIGN_ENTRANCE | ENTR_MM_SPIDER_HOUSE_OCEAN },
    { "Pirates",                MASK_FOREIGN_ENTRANCE | ENTR_MM_PIRATE_FORTRESS },
    { "Ikana Castle",           MASK_FOREIGN_ENTRANCE | ENTR_MM_IKANA_CASTLE },
    { "BtW",                    MASK_FOREIGN_ENTRANCE | ENTR_MM_BENEATH_THE_WELL },
    { "BtW End",                MASK_FOREIGN_ENTRANCE | ENTR_MM_BENEATH_THE_WELL_BACK },
    { NULL, 0 },
};

static const DebugMenuEntry kMenuWarpBoss[] = {
    { "Gohma",           ENTR_OOT_BOSS_DEKU_TREE },
    { "King Dodongo",    ENTR_OOT_BOSS_DODONGO_CAVERN },
    { "Barinade",        ENTR_OOT_BOSS_JABU_JABU },
    { "Phantom Ganon",   ENTR_OOT_BOSS_TEMPLE_FOREST },
    { "Volvagia",        ENTR_OOT_BOSS_TEMPLE_FIRE },
    { "Morpha",          ENTR_OOT_BOSS_TEMPLE_WATER },
    { "Bongo Bongo",     ENTR_OOT_BOSS_TEMPLE_SHADOW },
    { "Twinrova",        ENTR_OOT_BOSS_TEMPLE_SPIRIT },
    { "Odolwa",          MASK_FOREIGN_ENTRANCE | ENTR_MM_BOSS_TEMPLE_WOODFALL },
    { "Goht",            MASK_FOREIGN_ENTRANCE | ENTR_MM_BOSS_TEMPLE_SNOWHEAD },
    { "Gyorg",           MASK_FOREIGN_ENTRANCE | ENTR_MM_BOSS_TEMPLE_GREAT_BAY },
    { "Twinmold",        MASK_FOREIGN_ENTRANCE | ENTR_MM_BOSS_TEMPLE_STONE_TOWER },
    { NULL, 0 },
};

static const DebugMenuEntry kMenuWarp[] = {
    { "Common",     (u32)&kMenuWarpCommon },
    { "Boss",       (u32)&kMenuWarpBoss },
    { "Dungeons",   (u32)&kMenuWarpDungeons },
    { NULL, 0 },
};

static const DebugMenuEntry kMenuMain[] = {
    { "Warp", DEBUGMENU_PAGE_WARP },
#if defined(GAME_MM)
    { "Time", DEBUGMENU_PAGE_TIME },
#endif
    { NULL, 0 },
};

int btnHeld(u16 but)
{
    return ((sInput.current.buttons & but) == but);
}

int btnPressed(u16 but)
{
    return ((sInput.pressed.buttons & but) == but);
}

static void debugDrawChar(int x, int y, char c)
{
    x = DEBUG_X + (x * 8);
    y = DEBUG_Y + (y * 12);

    OPEN_DISPS(gPlay->gs.gfx);
    gSPSegment(POLY_OPA_DISP++, 0x09, (char*)gCustomKeep + CUSTOM_KEEP_FONT + ((c - ' ') * 0x30));
    gSPDisplayList(POLY_OPA_DISP++, kDlistLoadIA4_8x12);
    gSPTextureRectangle(
        POLY_OPA_DISP++,
        x * 2, y * 2,
        x * 2 + 8 * 2, y * 2 + 12 * 2,
        0,
        0, 0,
        (1 << 11), (1 << 11)
    );
    CLOSE_DISPS();
}

static void debugDrawStr(int x, int y, const char* str)
{
    int i;
    char c;

    i = 0;
    for (;;)
    {
        c = str[i];
        if (c == 0)
            break;
        debugDrawChar(x + i, y, c);
        i++;
    }
}

static u8 menu(const DebugMenuEntry* entries, s16* cursor, u32* data)
{
    int entryCount;
    int tmpCursor;

    /* Count entries */
    entryCount = 0;
    for (;;)
    {
        if (entries[entryCount].name == NULL)
            break;
        entryCount++;
    }

    for (int i = 0; i < entryCount; ++i)
    {
        debugDrawStr(2, i, entries[i].name);
    }
    debugDrawChar(0, *cursor, '>');

    /* Movement */
    tmpCursor = *cursor;
    if (btnPressed(D_JPAD)) tmpCursor++;
    if (btnPressed(U_JPAD)) tmpCursor--;
    if (btnPressed(L_JPAD)) tmpCursor -= 5;
    if (btnPressed(R_JPAD)) tmpCursor += 5;
    if (tmpCursor < 0) tmpCursor = 0;
    if (tmpCursor >= entryCount) tmpCursor = entryCount - 1;
    *cursor = (u16)tmpCursor;

    if (btnPressed(A_BUTTON))
    {
        *data = entries[*cursor].data;
        return OPTION_OK;
    }
    else if (btnPressed(B_BUTTON))
    {
        return OPTION_CANCEL;
    }

    return OPTION_NONE;
}

static void DebugHandler_None(void)
{
    if (btnHeld(Z_TRIG | L_TRIG))
    {
        sDebugPage = DEBUGMENU_PAGE_MAIN;
        sCursor[0] = 0;
    }
}

static void DebugHandler_Main(void)
{
    u32 data;

    switch (menu(kMenuMain, &sCursor[0], &data))
    {
    case OPTION_OK:
        sDebugPage = data;
        sCursor[1] = 0;
        break;
    case OPTION_CANCEL:
        sDebugPage = DEBUGMENU_PAGE_NONE;
        break;
    }
}

static void DebugHandler_Warp(void)
{
    u32 data;

    switch (menu(kMenuWarp, &sCursor[1], &data))
    {
    case OPTION_OK:
        sMenuWarp = (const DebugMenuEntry*)data;
        sDebugPage = DEBUGMENU_PAGE_WARP2;
        sCursor[2] = 0;
        break;
    case OPTION_CANCEL:
        sDebugPage = DEBUGMENU_PAGE_MAIN;
        break;
    }
}

static void DebugHandler_Warp2(void)
{
    u32 entrance;
    u8 ret;

    ret = menu(sMenuWarp, &sCursor[2], &entrance);
    if (ret == OPTION_CANCEL)
    {
        sDebugPage = DEBUGMENU_PAGE_WARP;
        return;
    }

    if (ret != OPTION_OK)
        return;

    sDebugPage = DEBUGMENU_PAGE_NONE;
#if defined(GAME_MM)
    entrance ^= MASK_FOREIGN_ENTRANCE;
#endif
    comboTransition(gPlay, entrance);
}

#if defined(GAME_MM)
static void DebugHandler_Time(void)
{
    int isNightOld;
    int isNightNew;
    u16 timeOld;
    u16 timeNew;
    int delta;

    /* Cursor */
    if (btnPressed(D_JPAD)) sCursor[1]++;
    if (btnPressed(U_JPAD)) sCursor[1]--;
    if (sCursor[1] < 0) sCursor[1] = 2;
    if (sCursor[1] > 2) sCursor[1] = 0;

    /* Change */
    delta = 0;
    if (btnPressed(L_JPAD)) delta--;
    if (btnPressed(R_JPAD)) delta++;
    switch (sCursor[1])
    {
    case 0:
        sTimeDay += delta;
        if (sTimeDay < 1) sTimeDay = 1;
        if (sTimeDay > 4) sTimeDay = 4;
        break;
    case 1:
        sTimeHour += delta;
        if (sTimeHour < 0) sTimeHour = 23;
        if (sTimeHour > 23) sTimeHour = 0;
        break;
    case 2:
        sTimeMinute += delta * 10;
        if (sTimeMinute < 0) sTimeMinute = 50;
        if (sTimeMinute > 50) sTimeMinute = 0;
        break;
    }

    /* Draw */
    debugDrawStr(2, 0, "Day");
    debugDrawStr(2, 1, "Hour");
    debugDrawStr(2, 2, "Minute");

    debugDrawChar(11, 0, '0' + sTimeDay);
    debugDrawChar(10, 1, '0' + sTimeHour / 10);
    debugDrawChar(11, 1, '0' + sTimeHour % 10);
    debugDrawChar(10, 2, '0' + sTimeMinute / 10);
    debugDrawChar(11, 2, '0' + sTimeMinute % 10);

    debugDrawChar(0, sCursor[1], '>');

    /* Actions */
    if (btnPressed(B_BUTTON))
    {
        sDebugPage = DEBUGMENU_PAGE_MAIN;
        return;
    }

    if (!btnPressed(A_BUTTON))
        return;

    sDebugPage = DEBUGMENU_PAGE_NONE;
    timeNew = (((s32)sTimeHour * 60 + sTimeMinute) * 0x10000) / (24 * 60);
    timeOld = gSave.time;
    isNightNew = (timeNew >= 0xc000 || timeNew < 0x4000);
    isNightOld = (timeOld >= 0xc000 || timeOld < 0x4000);
    if (gSave.day != sTimeDay || isNightOld != isNightNew)
        comboTransition(gPlay, gSave.entranceIndex); /* Needs a reload */
    gSave.day = sTimeDay;
    gSave.time = timeNew;
    gSave.isNight = isNightNew;
}
#endif

static const DebugMenuFunc kDebugMenuFuncs[] = {
    DebugHandler_None,
    DebugHandler_Main,
    DebugHandler_Warp,
    DebugHandler_Warp2,
#if defined(GAME_MM)
    DebugHandler_Time,
#else
    NULL,
#endif
};

void Debug_Input(void)
{
    ControllerInput* src;

    src = &gPlay->gs.input[0];
    memcpy(&sInput, src, sizeof(ControllerInput));
    if (sDebugPage != DEBUGMENU_PAGE_NONE)
        bzero(src, sizeof(ControllerInput));
}

void Debug_Init(void)
{
    sDebugPage = DEBUGMENU_PAGE_NONE;
}

void Debug_Update(void)
{
    GfxContext* ctx;
    Gfx* opaOriginal;
    Gfx* opaTarget;
    Gfx* opaNew;

    if (sDebugPage)
    {
        ctx = gPlay->gs.gfx;
        opaOriginal = ctx->polyOpa.append;
        ctx->polyOpa.append++;
        opaTarget = ctx->polyOpa.append;

        OPEN_DISPS(gPlay->gs.gfx);
        gDPSetRenderMode(POLY_OPA_DISP++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
        gDPSetCombineMode(POLY_OPA_DISP++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, 0xc0);
        gDPFillRectangle(POLY_OPA_DISP++, 0, 0, 319, 239);
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0xff, 0xff, 0xff, 0xff);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);
        CLOSE_DISPS();
        kDebugMenuFuncs[sDebugPage]();
        gSPEndDisplayList(ctx->polyOpa.append++);

        opaNew = ctx->polyOpa.append;
        gSPBranchList(opaOriginal, opaNew);
        gSPDisplayList(ctx->overlay.append++, opaTarget);
    }
    else
    {
        DebugHandler_None();
    }
}
#endif