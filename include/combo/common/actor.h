#ifndef COMBO_COMMON_ACTOR_H
#define COMBO_COMMON_ACTOR_H

#include <combo/types.h>

#if defined(GAME_OOT)
# define AC_ITEM_CUSTOM      0x001
# define AC_EN_BOX           0x00a
# define AC_ITEM00           0x015
# define AC_DOOR_WARP1       0x05d
# define AC_ITEM_B_HEART     0x05f
# define AC_BG_TOKI_SWD      0x06c
# define AC_EN_DU            0x098
# define AC_EN_MA1           0x0e7
# define AC_ITEM_OCARINA     0x0f1
# define AC_ITEM_ETCETERA    0x10f
# define AC_BG_SPOT03_TAKI   0x11f
# define AC_EN_DIVING_GAME   0x124
# define AC_EN_NIW_LADY      0x13c
# define AC_EN_SA            0x146
# define AC_EN_OWL           0x14d
#endif

#if defined(GAME_MM)
# define AC_EN_BOX          0x006
# define AC_ITEM00          0x00e
# define AC_ITEM_CUSTOM     0x013
# define AC_DOOR_WARP1      0x038
# define AC_ITEM_B_HEART    0x03a
# define AC_DM_HINA         0x140
#endif

typedef struct GameState_Play GameState_Play;

typedef struct PACKED ALIGNED(0x4) Actor
{
    u16         id;
    u8          type;
    u8          room;
    s32         flags;
    char        unk[0xc];
    Vector3s    initRot;
    char        unk2[0x02];
    u16         variable;
    s8          objTableIndex;
    char        unk3[5];
    Vector3f    position;
    Vector3s    speedRot;
    u16         unk_36;
#if defined(GAME_MM)
    u32         unk_mm0;
#endif
    Vector3f    pos3;
    Vector3s    rot1;
#if defined(GAME_MM)
    u32         unk_mm1;
#endif
    char        unk4[0x6a];
    Vector3s    rot2;
    char        unk5[0x6e];
    void*       init;
    void*       fini;
    void*       update;
    void*       draw;
    void*       ovl;
}
Actor;

#if defined(GAME_OOT)
_Static_assert(sizeof(Actor) == 0x13c, "OoT Actor size is wrong");
#endif

#if defined(GAME_MM)
_Static_assert(sizeof(Actor) == 0x144, "MM Actor size is wrong");
#endif

#endif