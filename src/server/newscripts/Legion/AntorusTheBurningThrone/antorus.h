/*
    https://uwow.biz/
*/

#ifndef AntorusH_
#define AntorusH_

enum eData
{
    DATA_WORLDBREAKER       = 0,
    DATA_FELHOUNDS,
    DATA_ANTORAN,
    
    DATA_HASABEL,
    DATA_EONAR,
    DATA_IMONAR,
    
    DATA_KINGAROTH,
    DATA_VARIMATHRAS,
    DATA_COVEN,
    
    DATA_AGGRAMAR,
    DATA_ARGUS,

    MAX_ENCOUNTER,
};

enum eCreatures
{
    NPC_DREADWING                   = 125487,
    NPC_LIGHTFORGED_WIREFRAME       = 125476,
    NPC_LIGHTFORGED_WIREFRAME_1     = 127256,
    NPC_LIGHT_CENTURION             = 125478,
    NPC_LIGHT_COMANDIR              = 125480,
    NPC_LIGHT_PRIEST                = 124778,

    NPC_EXARH_TURALION              = 125512, 
    NPC_TELEPORT_OF_LIGHTFORGED_1   = 130137,

    NPC_BATTLE_SHIP                 = 125771,

    NPC_RUN_NPC_1                   = 127221,
    NPC_RUN_NPC_2                   = 126444,

    NPC_FEL_PORTAL_1                = 127050,
    NPC_FEL_PORTAL_2                = 126527,

    NPC_WORLDBREAKER                = 122450, // boss
    NPC_ANNIHILATOR_OF_KINGAROTH    = 123398,
    NPC_WORLDBREAKER_DECIMATOR      = 122773,
    NPC_WORLDBREAKER_ANNIHILATOR    = 122778,
    NPC_WB_ANNIHILATION_TRIGGER     = 122818,
    NPC_WB_ANNIHILATION_TRIGGER_2   = 124330,
    NPC_SURGING_FEL_TRIGGER         = 124167, //Heroic+
    NPC_SURGING_FEL_TRIGGER_MOVER   = 128429, //Heroic+

    NPC_SHATUG                      = 122135, //Boss
    NPC_FHARG                       = 122477,
    NPC_MOLTEN_TOUCH                = 122507,

    NPC_HASABEL                     = 122104, // boss

    NPC_IMAGE_OF_EONAR              = 128352,
    NPC_EONAR_EVENT                 = 122500,

    NPC_IMONAR_INTRO                = 125692,
    NPC_IMONAR                      = 124158, // boss

    NPC_IMPLOSIONS                  = 125603,
    NPC_PRIESTESS_OF_DELIRIUM       = 128060,

    NPC_AZARA                       = 122467, // boss

    NPC_WIND_STALKER                = 123719,

    NPC_KINGAROTH                   = 122578, // Boss
    NPC_DETONATION_CHARGE           = 122585,
    NPC_RUINER                      = 124230,
    NPC_INCINERATOR_STALKER         = 124879,
    NPC_INFERNAL_TOWER_1            = 122634,
    NPC_INFERNAL_TOWER_2            = 122740,
    NPC_INFERNAL_TOWER_3            = 122885,
    NPC_GAROTHI_ANNIHILATOR         = 123906,
    NPC_GAROTHI_DECIMATOR           = 123921,
    NPC_GAROTHI_DEMOLISHER          = 123929,
    NPC_APOCALYPSE_BLAST_STALKER    = 125462,
    NPC_ANNIHILATION_TRIGGER        = 124160,
    NPC_EMPOWERED_RUINER_STALKER    = 125646,
};

enum eGameObjects
{
    GO_STONE                        = 278488,
    GO_INVISIBLE_WALL               = 277365,
    GO_ELEVATOR                     = 278815,
    GO_PORTAL_TO_ELUNARIES          = 277474,

    GO_ARCANE_BARRIER               = 249386,
    GO_ARCANE_BARRIER_VISUAL        = 273911,

    GO_KINGAROTH_DOOR               = 277531,
    GO_KINGAROTH_TRAP_LEFT_1        = 276288,
    GO_KINGAROTH_TRAP_LEFT_2        = 276294,
    GO_KINGAROTH_TRAP_RIGHT_1       = 276292,
    GO_KINGAROTH_TRAP_RIGHT_2       = 276289,
    GO_KINGAROTH_TRAP_BACK_1        = 276290,
    GO_KINGAROTH_TRAP_BACK_2        = 276293,
};  

enum eSpells
{
    SPELL_INTRO_FIELD_OF_FIRE       = 249100,
    SPELL_SPAWN                     = 247963,
    SPELL_LIGHT_SHIELD              = 248515,

    SPELL_SURGE_OF_LIFE_OVERRIDE    = 254506, // TO-DO FIND REAL SPELL OF FIX  254506
};

#endif
