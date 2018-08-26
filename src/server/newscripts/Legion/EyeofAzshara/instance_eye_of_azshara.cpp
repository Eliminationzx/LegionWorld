/*
    http://uwow.biz
    Dungeon : Eye of Azshara 100-110
*/

#include "Group.h"
#include "eye_of_azshara.h"

DoorData const doorData[] =
{
    //{GO_,       DATA_,      DOOR_TYPE_PASSAGE,      BOUNDARY_NONE},
    {GO_DEEPBEARD_DOOR,      DATA_DEEPBEARD,      DOOR_TYPE_ROOM,      BOUNDARY_NONE}, // END
};

class instance_eye_of_azshara : public InstanceMapScript
{
public:
    instance_eye_of_azshara() : InstanceMapScript("instance_eye_of_azshara", 1456) {}

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_eye_of_azshara_InstanceMapScript(map);
    }

    struct instance_eye_of_azshara_InstanceMapScript : public InstanceScript
    {
        instance_eye_of_azshara_InstanceMapScript(Map* map) : InstanceScript(map) 
        {
            SetBossNumber(MAX_ENCOUNTER);
        }

        ObjectGuid AzsharaGUID;
        ObjectGuid NagasContainerGUID[4];

        bool onInitEnterState;
        bool StartEvent;
        bool WindsActive;
        bool StormActive;
        uint8 NagasCount;
        uint16 shelterTimer;
        uint32 WindsTimer;
        uint32 PlayerCount;
        uint32 StormTimer;

        void Initialize() override
        {
            LoadDoorData(doorData);

            NagasCount = 0;
            WindsTimer = 0;
            StormTimer = 0;
            shelterTimer = 2000;
            onInitEnterState = false;
            WindsActive = false;
            StormActive = false;

            PlayerCount = 0;
            StartEvent = false;
        }

        void OnCreatureCreate(Creature* creature) override
        {
            switch (creature->GetEntry())
            {
                case NPC_WRATH_OF_AZSHARA:
                    AzsharaGUID = creature->GetGUID(); 
                    break;
                case NPC_MYSTIC_SSAVEH:
                case NPC_RITUALIST_LESHA:
                case NPC_CHANNELER_VARISZ:
                case NPC_BINDER_ASHIOI:
                    NagasContainerGUID[NagasCount] = creature->GetGUID();
                    NagasCount++;
                    break;
            }
        }

        void OnGameObjectCreate(GameObject* go) override
        {
            switch (go->GetEntry())
            {
                case GO_DEEPBEARD_DOOR:
                    AddDoor(go, true);
                    break;
                default:
                    break;
            }
        }

        bool SetBossState(uint32 type, EncounterState state) override
        {
            if (!InstanceScript::SetBossState(type, state))
                return false;

            DoEventCreatures();

            if (state == DONE)
                ChangeWeather();

            return true;
        }

        void SetData(uint32 type, uint32 data) override
        {
            /*switch (type)
            {
                default:
                    break;
            }*/
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
            return 0;
        }

        bool checkBossDone()
        {
            uint8 bossDiedCount = 0;

            for (uint8 i = 0; i < DATA_WRATH_OF_AZSHARA; i++)
                if (GetBossState(i) == DONE)
                    bossDiedCount++;

            if (!WindsActive && bossDiedCount >= 2)
            {
                WindsActive = true;
                WindsTimer = 60 * IN_MILLISECONDS;
            }

            if (!StormActive && bossDiedCount >= 3)
            {
                StormActive = true;
                StormTimer = 15 * IN_MILLISECONDS;
            }

            for (uint8 i = 0; i < DATA_WRATH_OF_AZSHARA; i++)
                if (GetBossState(i) != DONE)
                    return false;

            return true;
        }

        void DoEventCreatures()
        {
            if (!checkBossDone())
                return;

            for (uint8 i = 0; i < 4; i++)
                if (Creature* naga = instance->GetCreature(NagasContainerGUID[i]))
                    naga->AI()->DoAction(true);   

            if (Creature* boss = instance->GetCreature(AzsharaGUID))
            {
               boss->SetVisible(true);
               boss->AI()->ZoneTalk(0); // "THE STORM AWAKENS"
            }
        }

        void OnPlayerEnter(Player* player) override
        {
            if (!StartEvent)
            {
                if (PlayerCount < 5)
                {
                    PlayerCount++;
                } 
                else
                {
                    StartEvent = true;
                    if (Group *g = player->GetGroup())
                        if (Player* leader = ObjectAccessor::GetPlayer(*player, g->GetLeaderGUID()))
                            if (Creature* target = leader->FindNearestCreature(100216, 50.0f, true))
                               target->AI()->Talk(0);
                }
            }

            ChangeWeather();

            if (onInitEnterState)
                return;

            onInitEnterState = true;

            DoEventCreatures();
        }

        void ChangeWeather()
        {
            uint8 bossDiedCount = 0;

            for (uint8 i = 0; i < DATA_WRATH_OF_AZSHARA; ++i)
                if (GetBossState(i) == DONE)
                    ++bossDiedCount;

            switch (bossDiedCount)
            {
                case 1:
                    DoCastSpellOnPlayers(SPELL_SKYBOX_RAIN);
                    break;
                case 2:
                    DoCastSpellOnPlayers(SPELL_SKYBOX_WIND);
                    break;
                case 3:
                    DoCastSpellOnPlayers(SPELL_SKYBOX_LIGHTNING);
                    break;
                case 4:
                    DoCastSpellOnPlayers(SPELL_SKYBOX_HURRICANE);
                    break;
                default:
                    break;
            }
        }

        void Update(uint32 diff) override
        {
            if (WindsTimer)
            {
                if (WindsTimer <= diff)
                {
                    Map::PlayerList const& players = instance->GetPlayers();
                    for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                    {
                        if (Player* player = itr->getSource())
                        {
                            if (player->isAlive() && !player->isGameMaster() && !player->HasAura(197134)) //Shelter
                                player->CastSpell(player, 191797, true); //Violent Winds
                        }
                    }
                    WindsTimer = 90 * IN_MILLISECONDS;
                }
                else WindsTimer -= diff;            
            }

            if (StormTimer <= diff)
            {
                Map::PlayerList const& pPlayers = instance->GetPlayers();
                std::list<Player*> targets;
                targets.clear();
                for (Map::PlayerList::const_iterator itr = pPlayers.begin(); itr != pPlayers.end(); ++itr)
                {
                    if (Player* player = itr->getSource())
                       targets.push_back(player);
                }

                if (!targets.empty())
                {
                    std::list<Player*>::const_iterator itr = targets.begin();
                    std::advance(itr, urand(0, targets.size() - 1));
                    if ((*itr)->GetAreaId() == 8040 || (*itr)->GetAreaId() == 8084) // only needed area
                        (*itr)->CastSpell((*itr), 192796, true); //Lightning Strikes
                }
                StormTimer = 4000;
            }
            else StormTimer -= diff;

            if (shelterTimer <= diff)
            {
                Map::PlayerList const& players = instance->GetPlayers();
                for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                    if (Player* player = itr->getSource())
                    {
                        if (player->GetAreaId() == 8083)
                        {
                            if (!player->GetMap()->IsOutdoors(player->GetPositionX(), player->GetPositionY(), player->GetPositionZ()))
                            {
                                if (!player->HasAura(197134))
                                    player->CastSpell(player, 197134, true);
                            }
                            else
                            {
                                if (player->HasAura(197134))
                                    player->RemoveAurasDueToSpell(197134);
                            }
                            if (player->HasAura(191797))
                                player->RemoveAurasDueToSpell(191797);
                        }
                        else if (player->HasAura(197134))
                            player->RemoveAurasDueToSpell(197134);
                    }
                shelterTimer = 2 * IN_MILLISECONDS;
            }
            else shelterTimer -= diff;
        }
    };
};

void AddSC_instance_eye_of_azshara()
{
    new instance_eye_of_azshara();
}
