/*
    http://uwow.biz
    Dungeon : Black Rook Hold Dungeon 100-110
*/

#ifndef BLACK_ROOK_HOLD_DUNGEON_H_
#define BLACK_ROOK_HOLD_DUNGEON_H_

enum eData
{
    DATA_AMALGAM        = 0,
    DATA_ILLYSANNA      = 1,
    DATA_SMASHSPITE     = 2,
    DATA_KURTALOS       = 3,
    MAX_ENCOUNTER,

    DATA_KURTALOS_STATE,
    DATA_AMALGAM_OUTRO,
    DATA_ILLYSANNA_INTRO,
};

enum eCreatures
{
    //Amalgam of Souls
    NPC_SOUL_ECHOES_STALKER     = 99090,
    NPC_RESTLESS_SOUL           = 99664,

    //Illysanna
    NPC_EYE_BEAM_STALKER        = 100436,
    NPC_RISEN_ARCANIST          = 98280,
    NPC_SOUL_TORN_VANGUARD      = 100485,

    //Smashspite
    NPC_FEL_BAT                 = 100759,

    //Kurtalos
    NPC_KURTALOS                = 98965,
    NPC_LATOSIUS                = 98970,
    NPC_DANTALIONAX             = 99611,
    NPC_SOUL_KURTALOS           = 101054,
    NPC_IMAGE_OF_LATOSIUS       = 101028,
    NPC_KURTALOS_BLADE_TRIGGER  = 100861, //Cast AT 198782
    NPC_CLOUD_OF_HYPNOSIS       = 100994,
    NPC_STINGING_SWARM          = 101008,
};

enum eGameObjects
{
    GO_AMALGAM_DOOR_1           = 247403,
    GO_AMALGAM_DOOR_2           = 247404,
    GO_AMALGAM_DOOR_3           = 247405,
    GO_AMALGAM_DOOR_4           = 247406,
    GO_AMALGAM_DOOR_EXIT        = 247407,
    GO_ILLYSANNA_DOOR_1         = 247401,
    GO_ILLYSANNA_DOOR_2         = 247399,
    GO_ILLYSANNA_DOOR_3         = 247400,
    GO_ILLYSANNA_DOOR_4         = 245079,
    GO_SMASHSPITE_DOOR          = 247402,
    GO_SMASH_KURT_DOOR_1        = 245104,
    GO_SMASH_KURT_DOOR_2        = 245105,
    GO_KURTALOS_DOOR            = 247498,
};

#endif