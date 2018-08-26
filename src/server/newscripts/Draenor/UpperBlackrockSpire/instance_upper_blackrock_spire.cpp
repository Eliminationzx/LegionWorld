/*
    http://epicwow.com/
    Dungeon : Upper Blackrock Spire 90-100
*/

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "upper_blackrock_spire.h"
#include "WorldStatePackets.h"

DoorData const doorData[] =
{
    {GO_ENTER_ROOM_DOOR,        DATA_GORASHAN,      DOOR_TYPE_ROOM,         BOUNDARY_NONE},
    {GO_EXIT_ROOM_DOOR,         DATA_GORASHAN,      DOOR_TYPE_PASSAGE,      BOUNDARY_NONE},
    {GO_EXIT_ROOM_DOOR,         DATA_KYRAK,         DOOR_TYPE_ROOM,         BOUNDARY_NONE},
    {GO_KYRAK_EXIT_DOOR,        DATA_KYRAK,         DOOR_TYPE_PASSAGE,      BOUNDARY_NONE},
    {GO_KYRAK_EXIT_DOOR_2,      DATA_KYRAK,         DOOR_TYPE_PASSAGE,      BOUNDARY_NONE},
    {GO_THARBEK_ENTER_DOOR,     DATA_THARBEK,       DOOR_TYPE_ROOM,         BOUNDARY_NONE},
    {GO_THARBEK_EXIT_DOOR,      DATA_THARBEK,       DOOR_TYPE_PASSAGE,      BOUNDARY_NONE},
    {GO_RAGEWING_ENTER_DOOR,    DATA_RAGEWING,      DOOR_TYPE_ROOM,         BOUNDARY_NONE},
    {GO_RAGEWING_EXIT_DOOR,     DATA_RAGEWING,      DOOR_TYPE_PASSAGE,      BOUNDARY_NONE},
};

class instance_upper_blackrock_spire : public InstanceMapScript
{
public:
    instance_upper_blackrock_spire() : InstanceMapScript("instance_upper_blackrock_spire", 1358) { }

    InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_upper_blackrock_spire_InstanceMapScript(map);
    }

    struct instance_upper_blackrock_spire_InstanceMapScript : public InstanceScript
    {
        instance_upper_blackrock_spire_InstanceMapScript(Map* map) : InstanceScript(map) 
        {
            SetBossNumber(MAX_ENCOUNTER);
        }

        ObjectGuid runeGlowGUID;
        ObjectGuid runeGlowGUIDdoor;
        ObjectGuid tharbekPortcullis;
        ObjectGuid LeeroyJenkinsGUID;
        uint8 runeglow_count;
        uint32 EventFinalJenkins{};

        void Initialize()
        {
            LoadDoorData(doorData);
            
            runeglow_count = 0;

            runeGlowGUIDdoor.Clear();
            tharbekPortcullis.Clear();
        }

        bool SetBossState(uint32 type, EncounterState state)
        {
            if (!InstanceScript::SetBossState(type, state))
                return false;

            return true;
        }

        void OnCreatureCreate(Creature* creature)
        {
            switch (creature->GetEntry())
            {
                case NPC_RUNE_GLOW:
                    if (creature->isAlive())
                    {
                        runeglow_count++;
                        if (GetData(DATA_FIRST_DOOR) != IN_PROGRESS)
                            SetData(DATA_FIRST_DOOR, IN_PROGRESS);
                    }
                    break;
                case NPC_LEEROY_JENKINS:
                        LeeroyJenkinsGUID = creature->GetGUID();
                        break;
            }
        }

        void FillInitialWorldStates(WorldPackets::WorldState::InitWorldStates& packet)
        {
            packet.Worldstates.emplace_back(static_cast<WorldStates>(9523), 0);
            packet.Worldstates.emplace_back(static_cast<WorldStates>(9524), 0);
        }

        void OnGameObjectCreate(GameObject* go)
        {
            switch (go->GetEntry())
            {
                case GO_ENTRANCE_DOOR:
                    runeGlowGUIDdoor = go->GetGUID();
                    break;
                case GO_ENTER_ROOM_DOOR:
                case GO_EXIT_ROOM_DOOR:
                case GO_KYRAK_EXIT_DOOR:
                case GO_KYRAK_EXIT_DOOR_2:
                case GO_THARBEK_ENTER_DOOR:
                case GO_THARBEK_EXIT_DOOR:
                case GO_RAGEWING_ENTER_DOOR:
                case GO_RAGEWING_EXIT_DOOR:
                    AddDoor(go, true);
                    break;
                case GO_THARBEK_PORTCULLIS:
                    tharbekPortcullis = go->GetGUID();
                    break;
                default:
                    break;
            }
        }

        void SetData(uint32 type, uint32 data)
        {
            switch (type)
            {
                case DATA_FIRST_DOOR:
                {
                    switch (data)
                    {
                        case IN_PROGRESS:
                            HandleGameObject(runeGlowGUIDdoor, false);
                            break;
                        case DONE:
                            HandleGameObject(runeGlowGUIDdoor, true);
                            break;
                    }
                }
                case DATA_THARBEK_OPENGATE:
                {
                    switch (data)
                    {
                        case IN_PROGRESS:
                            if (GameObject* go = instance->GetGameObject(tharbekPortcullis))
                                go->UseDoorOrButton();
                            break;
                    }
                }
                case DATA_FINAL_EVENT_JENKINS:
                    EventFinalJenkins = data;
                    break;
                default:
                    break;
            }
        }

        ObjectGuid GetGuidData(uint32 type) const
        {
            /* switch (type)
            {
                case NPC_RUNE_GLOW:   
                    return runeGlowGUID;
            } */
            return ObjectGuid::Empty;
        }

        uint32 GetData(uint32 type) const
        {
            switch (type)
            {
            case DATA_FINAL_EVENT_JENKINS:
                return EventFinalJenkins;
            default:
                return 0;
            }
        }

        void CreatureDies(Creature* creature, Unit* /*killer*/)
        {
            switch(creature->GetEntry())
            {
                case NPC_RUNE_GLOW:
                    runeglow_count--;
                    if (!runeglow_count)
                        SetData(DATA_FIRST_DOOR, DONE);
                    break;
            }
        }

        /* void Update(uint32 diff) 
        {
            // Challenge
            InstanceScript::Update(diff);
        } */
    };
};

void AddSC_instance_upper_blackrock_spire()
{
    new instance_upper_blackrock_spire();
}