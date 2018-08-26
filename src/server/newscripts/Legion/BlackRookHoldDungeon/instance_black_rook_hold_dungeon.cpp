/*
    http://uwow.biz
    Dungeon : Black Rook Hold Dungeon 100-110
*/

#include "black_rook_hold_dungeon.h"

DoorData const doorData[] =
{
    {GO_AMALGAM_DOOR_1,         DATA_AMALGAM,       DOOR_TYPE_ROOM,         BOUNDARY_NONE},
    {GO_AMALGAM_DOOR_2,         DATA_AMALGAM,       DOOR_TYPE_ROOM,         BOUNDARY_NONE},
    {GO_AMALGAM_DOOR_3,         DATA_AMALGAM,       DOOR_TYPE_ROOM,         BOUNDARY_NONE},
    {GO_AMALGAM_DOOR_4,         DATA_AMALGAM,       DOOR_TYPE_ROOM,         BOUNDARY_NONE},
    {GO_ILLYSANNA_DOOR_1,       DATA_ILLYSANNA,     DOOR_TYPE_PASSAGE,      BOUNDARY_NONE},
    {GO_ILLYSANNA_DOOR_2,       DATA_ILLYSANNA,     DOOR_TYPE_PASSAGE,      BOUNDARY_NONE},
    {GO_ILLYSANNA_DOOR_3,       DATA_ILLYSANNA,     DOOR_TYPE_ROOM,         BOUNDARY_NONE},
    {GO_SMASHSPITE_DOOR,        DATA_SMASHSPITE,    DOOR_TYPE_ROOM,         BOUNDARY_NONE},
    {GO_SMASH_KURT_DOOR_1,      DATA_SMASHSPITE,    DOOR_TYPE_PASSAGE,      BOUNDARY_NONE},
    {GO_SMASH_KURT_DOOR_2,      DATA_SMASHSPITE,    DOOR_TYPE_PASSAGE,      BOUNDARY_NONE},
    {GO_KURTALOS_DOOR,          DATA_KURTALOS,      DOOR_TYPE_ROOM,         BOUNDARY_NONE},
};

class instance_black_rook_hold_dungeon : public InstanceMapScript
{
public:
    instance_black_rook_hold_dungeon() : InstanceMapScript("instance_black_rook_hold_dungeon", 1501) { }

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_black_rook_hold_dungeon_InstanceMapScript(map);
    }

    struct instance_black_rook_hold_dungeon_InstanceMapScript : public InstanceScript
    {
        instance_black_rook_hold_dungeon_InstanceMapScript(Map* map) : InstanceScript(map) 
        {
            SetBossNumber(MAX_ENCOUNTER);
        }

        WorldLocation loc_res_pla;

        ObjectGuid AmalgamGateGUID;
        ObjectGuid IllysannaGateGUID;

        uint8 KurtalosState;
        uint8 AmalgamState;

        void Initialize() override
        {
            LoadDoorData(doorData);
            KurtalosState = 0;
            AmalgamState = 0;
        }

        void OnCreatureCreate(Creature* creature) override
        {
            /* switch (creature->GetEntry())
            {
                case NPC_:    
                    GUID = creature->GetGUID(); 
                    break;
            } */
        }

        void OnGameObjectCreate(GameObject* go) override
        {
            switch (go->GetEntry())
            {
                case GO_AMALGAM_DOOR_1:
                case GO_AMALGAM_DOOR_2:
                case GO_AMALGAM_DOOR_3:
                case GO_AMALGAM_DOOR_4:
                case GO_ILLYSANNA_DOOR_1:
                case GO_ILLYSANNA_DOOR_2:
                case GO_ILLYSANNA_DOOR_3:
                case GO_SMASHSPITE_DOOR:
                case GO_SMASH_KURT_DOOR_1:
                case GO_SMASH_KURT_DOOR_2:
                case GO_KURTALOS_DOOR:
                    AddDoor(go, true);
                    break;
                case GO_AMALGAM_DOOR_EXIT:
                    AmalgamGateGUID = go->GetGUID();
                    if (GetBossState(DATA_AMALGAM) == DONE)
                        go->SetGoState(GO_STATE_ACTIVE);
                    break;                
                case GO_ILLYSANNA_DOOR_4:
                    IllysannaGateGUID = go->GetGUID();
                    break;
                default:
                    break;
            }
        }

        bool SetBossState(uint32 type, EncounterState state) override
        {
            if (!InstanceScript::SetBossState(type, state))
                return false;
            
            return true;
        }

        void SetData(uint32 type, uint32 data) override
        {
            switch (type)
            {
                case DATA_KURTALOS_STATE:
                    KurtalosState = data;
                    break;               
                case DATA_AMALGAM_OUTRO:
                    if (data == 0)
                       AmalgamState++;
                    if (data == DONE)
                       HandleGameObject(AmalgamGateGUID, true);
                    break;                
                case DATA_ILLYSANNA_INTRO:
                    if (data == DONE)
                       HandleGameObject(IllysannaGateGUID, false);
                    if (data != DONE)
                       HandleGameObject(IllysannaGateGUID, true);
                    break;
                default:
                    break;
            }
        }

        /* ObjectGuid GetGuidData(uint32 type) const
        {
            switch (type)
            {
                case NPC_:   
                    return GUID;
            }
            return ObjectGuid::Empty;
        } */

        uint32 GetData(uint32 type) const override
        {
            switch (type)
            {
                case DATA_KURTALOS_STATE:
                    return KurtalosState;
                case DATA_AMALGAM_OUTRO:
                    return AmalgamState;
            }
            return 0;
        }

        WorldLocation* GetClosestGraveYard(float x, float y, float z) override
        {
            loc_res_pla.Relocate(x, y, z);
            loc_res_pla.SetMapId(1501);

            uint32 graveyardId = 5352;

            if (GetBossState(DATA_SMASHSPITE) == DONE)
                graveyardId = 5486;
            else if (GetBossState(DATA_ILLYSANNA) == DONE)
                graveyardId = 5485;
            else if (GetBossState(DATA_AMALGAM) == DONE)
                graveyardId = 5484;

            if (graveyardId)
            {
                if (WorldSafeLocsEntry const* gy = sWorldSafeLocsStore.LookupEntry(graveyardId))
                {
                    loc_res_pla.Relocate(gy->Loc.X, gy->Loc.Y, gy->Loc.Z);
                    loc_res_pla.SetMapId(gy->MapID);
                }
            }
            return &loc_res_pla;
        }

        std::string GetSaveData() override
        {
            std::ostringstream saveStream;
            saveStream << "B R H " << GetBossSaveData() << " " << AmalgamState;
            return saveStream.str();
        }

        void Load(const char* data) override
        {
            if (!data)
            {
                OUT_LOAD_INST_DATA_FAIL;
                return;
            }

            OUT_LOAD_INST_DATA(data);

            char dataHead1, dataHead2, dataHead3;
            std::istringstream loadStream(data);
            loadStream >> dataHead1 >> dataHead2 >> dataHead3;

            if (dataHead1 == 'B' && dataHead2 == 'R' && dataHead3 == 'H')
            {
                for (uint32 i = 0; i < MAX_ENCOUNTER; ++i)
                {
                    uint32 tmpState;
                    loadStream >> tmpState;
                    if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                        tmpState = NOT_STARTED;
                    SetBossState(i, EncounterState(tmpState));
                }
                loadStream >> AmalgamState;
            }
            else
                OUT_LOAD_INST_DATA_FAIL;

            OUT_LOAD_INST_DATA_COMPLETE;
        }

        /* void Update(uint32 diff) 
        {
            // Challenge
            InstanceScript::Update(diff);
        } */
    };
};

void AddSC_instance_black_rook_hold_dungeon()
{
    new instance_black_rook_hold_dungeon();
}