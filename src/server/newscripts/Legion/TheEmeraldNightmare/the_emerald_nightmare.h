/*
    https://uwow.biz/
*/

#include "ScriptMgr.h"
#include "ScriptedCreature.h"

#ifndef THE_EMERALD_NIGHTMARE_H_
#define THE_EMERALD_NIGHTMARE_H_

enum eData
{
    DATA_NYTHENDRA              = 0,
    DATA_RENFERAL               = 1,
    DATA_ILGYNOTH               = 2,
    DATA_URSOC                  = 3,
    DATA_DRAGON_NIGHTMARE       = 4,
    DATA_CENARIUS               = 5,
    DATA_XAVIUS                 = 6,
    DATA_PRE_EVENT_XAVIUS       = 7,
    MAX_ENCOUNTER,

    DATA_EYE_ILGYNOTH,
};

enum eCreatures
{
    //Nythendra
    NPC_NYTHENDRA                   = 102672,
    NPC_GELATINIZED_DECAY           = 111004,
    NPC_BREATH_STALKER              = 102930,
    NPC_CORRUPTED_VERMIN            = 102998,
    NPC_CORRUPTED_VERMIN_2          = 111005,

    //Elerethe Renferal
    NPC_ELERETHE_RENFERAL           = 106087,
    NPC_TWISTING_SHADOWS            = 106350, //Суммонится под игроком
    NPC_TWISTING_SHADOWS_BRIDGE     = 111439, //Суммонятся на паучих мостах
    NPC_VILE_AMBUSH_STALKER         = 107969,
    NPC_WEB                         = 109519,
    NPC_VENOMOUS_SPIDERLING         = 107459,
    NPC_SURGING_EGG_SAC             = 108540,
    NPC_PULSING_EGG_SAC             = 112078, //Has aura 215581 - Pulsing Egg Cosmetic

    //Ilgynoth
    NPC_ILGYNOTH                    = 105393,
    NPC_EYE_OF_ILGYNOTH             = 105906,
    NPC_DOMINATOR_TENTACLE          = 105304,
    NPC_DEATHGLARE_TENTACLE         = 105322,
    NPC_CORRUPTOR_TENTACLE          = 105383,
    NPC_NIGHTMARE_HORROR            = 105591,
    NPC_NIGHTMARE_ICHOR             = 105721,
    NPC_DEATH_BLOSSOM               = 108659, //Mythic
    NPC_SHRIVELED_EYESTALK          = 108821, //Mythic

    //Ursoc
    NPC_URSOC                       = 100497,
    NPC_NIGHTMARE_IMAGE             = 100576,

    //Dragons of Nightmare
    NPC_YSONDRE                     = 102679,
    NPC_TAERAR                      = 102681,
    NPC_LETHON                      = 102682,
    NPC_EMERISS                     = 102683,
    NPC_NIGHTMARE_BLOOM             = 102804,
    NPC_ESSENCE_OF_CORRUPTION       = 103691,
    NPC_SHADE_OF_TAERAR             = 103145,
    NPC_SEEPING_FOG                 = 103697,
    NPC_SPIRIT_SHADE                = 103100,
    NPC_DREAD_HORROR                = 103044,
    NPC_CORRUPTED_MUSHROOM_SMALL    = 103095,
    NPC_CORRUPTED_MUSHROOM_MEDIUM   = 103096,
    NPC_CORRUPTED_MUSHROOM_BIG      = 103097,

    //Cenarius
    NPC_CENARIUS                    = 104636,
    NPC_MALFURION_STORMRAGE         = 106482,

    NPC_NIGHTMARE_SAPLING           = 106427,
    NPC_TORMENTED_SOULS             = 106895,
    NPC_CORRUPTED_EMERALD_EGG       = 106898,
    NPC_CORRUPTED_NATURE            = 106899,

    NPC_NIGHTMARE_ANCIENT           = 105468,
    NPC_CLEANSED_ANCIENT            = 106667,
    NPC_CORRUPTED_WISP              = 106304,
    NPC_ALLIES_WISP                 = 106659,
    NPC_ROTTEN_DRAKE                = 105494,
    NPC_EMERALD_DRAKE               = 106809,
    NPC_TWISTED_SISTER              = 105495,
    NPC_REDEEMED_SISTER             = 106831,

    NPC_ROTTEN_BREATH_TRIG          = 106562,
    NPC_NIGHTMARE_BRAMBLES          = 106167,
    NPC_ENTANGLING_ROOTS            = 108040,

    NPC_BEAST_OF_NIGHTMARE          = 108208, //Mythic

    //Xavius
    NPC_XAVIUS                      = 103769,
    NPC_SLEEPING_VERSION            = 104096,
    NPC_DREAD_ABOMINATION           = 105343,
    NPC_LURKING_TERROR              = 103694,
    NPC_CORRUPTION_HORROR           = 103695,
    NPC_NIGHTMARE_BLADES            = 104422,
    NPC_INCONCEIVABLE_HORROR        = 105611,
    NPC_NIGHTMARE_TENTACLE          = 104592,
    
    // Events
    NPC_IN_THE_SHADOW               = 111370,
    NPC_NIGHTMARE_AMALGAMATION      = 111398,
    NPC_SHADOW_POUNDER              = 111405,
    NPC_DARK_DEVOURERS              = 111413,
    NPC_YZERA                       = 105245,
    NPC_SMALL_TRASH                 = 111350
};

enum eGameObjects
{
    //Nythendra
    GO_NYTHENDRA_DOOR_1         = 251533,
    GO_NYTHENDRA_DOOR_2         = 251534,
    GO_NYTHENDRA_DOOR_3         = 251535,
    GO_NYTHENDRA_DOOR_4         = 251536,
    GO_NYTHENDRA_DOOR_5         = 260550,
    GO_NYTHENDRA_DOOR_6         = 260551,

    //Elerethe Renferal
    GO_RENFERAL_DOOR            = 255000,

    //Ursoc
    GO_URSOC_DOOR_1             = 252042,
    GO_URSOC_DOOR_2             = 252043,

    //Ilgynoth
    GO_ILGYNOTH_DOOR_1          = 248848,
    GO_ILGYNOTH_DOOR_2          = 248849,
    GO_ILGYNOTH_DOOR_3          = 248850,
    GO_EYE_OF_ILGYNOTH_DOOR     = 251555,

    //Cenarius
    GO_CENARIUS_CHEST           = 254168,
    GO_CENARIUS_PORTAL          = 251980,
};

enum eOtherSpells
{
    //Portal to Cenarius events
    SPELL_PORTAL_IRIS_COSMETIC  = 225937,
    SPELL_PORTAL_IRIS           = 224881,
};

#endif