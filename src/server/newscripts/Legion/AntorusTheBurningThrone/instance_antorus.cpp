/*
    https://uwow.biz/
*/

#include "antorus.h"

DoorData const doorData[] =
{
    {GO_STONE,                     DATA_WORLDBREAKER,      DOOR_TYPE_PASSAGE,      BOUNDARY_NONE},
    {GO_INVISIBLE_WALL,            DATA_WORLDBREAKER,      DOOR_TYPE_PASSAGE,      BOUNDARY_NONE},
    {GO_ARCANE_BARRIER,            DATA_IMONAR,            DOOR_TYPE_PASSAGE,      BOUNDARY_NONE},
    {GO_KINGAROTH_DOOR,            DATA_KINGAROTH,         DOOR_TYPE_ROOM,         BOUNDARY_NONE}
};

ObjectData const creatureData[] =
{
    {NPC_WORLDBREAKER,     NPC_WORLDBREAKER},
    {NPC_HASABEL,          NPC_HASABEL},
    {NPC_IMONAR,           NPC_IMONAR},
    {NPC_AZARA,            NPC_AZARA},
    {NPC_KINGAROTH,        NPC_KINGAROTH},
    {NPC_SHATUG,           NPC_SHATUG},
    {NPC_FHARG,            NPC_FHARG},
    {NPC_MOLTEN_TOUCH,     NPC_MOLTEN_TOUCH}
};

ObjectData const objectData[] =
{
    {GO_ELEVATOR,                   GO_ELEVATOR},
    {GO_PORTAL_TO_ELUNARIES,        GO_PORTAL_TO_ELUNARIES},
    {GO_ARCANE_BARRIER_VISUAL,      GO_ARCANE_BARRIER_VISUAL},
    {GO_KINGAROTH_TRAP_LEFT_1,      GO_KINGAROTH_TRAP_LEFT_1},
    {GO_KINGAROTH_TRAP_LEFT_2,      GO_KINGAROTH_TRAP_LEFT_2},
    {GO_KINGAROTH_TRAP_RIGHT_1,     GO_KINGAROTH_TRAP_RIGHT_1},
    {GO_KINGAROTH_TRAP_RIGHT_2,     GO_KINGAROTH_TRAP_RIGHT_2},
    {GO_KINGAROTH_TRAP_BACK_1,      GO_KINGAROTH_TRAP_BACK_1},
    {GO_KINGAROTH_TRAP_BACK_2,      GO_KINGAROTH_TRAP_BACK_2}
};


Position const dreadWingsPos[2]
{
    { -3290.76f,	9519.75f,	22.1289f },
    { -3301.51f,	9595.35f,	36.3317f, }
};

class instance_antorus : public InstanceMapScript
{
public:
    instance_antorus() : InstanceMapScript("instance_antorus", 1712) {}

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_antorus_InstanceMapScript(map);
    }

    struct instance_antorus_InstanceMapScript : InstanceScript
    {
        explicit instance_antorus_InstanceMapScript(Map* map) : InstanceScript(map)
        {
            SetBossNumber(MAX_ENCOUNTER);
        }

        uint32 timerIntroEvent{}, timerIntroEvent2{}, timerConstEvent{};
        WorldLocation loc_res_pla;
        std::list<ObjectGuid> introGuids{};
        std::vector<ObjectGuid> wireframesGuids{};
        std::vector<ObjectGuid> introFirstBossGuids{};
        std::vector<ObjectGuid> secondBossOutro{};
        std::unordered_set<ObjectGuid> mobsForBridge{};
        std::vector<ObjectGuid> mobsFromBridge{};
        std::unordered_set<ObjectGuid> mobsForElevator{};
        std::vector<ObjectGuid> mobsToSeondPath{};
        std::vector<ObjectGuid> mobsAfterKingaroth{};

        void Initialize() override
        {
            LoadDoorData(doorData);
            LoadObjectData(creatureData, objectData);

            timerIntroEvent = urand(4000, 7000);
            timerIntroEvent2 = urand(3000, 6000);
            timerConstEvent = urand(3000, 7000);
        }

        void OnCreatureCreate(Creature* creature) override
        {
            InstanceScript::OnCreatureCreate(creature);
            
            switch (creature->GetEntry())
            {
                case NPC_DREADWING:
                    creature->SetReactState(REACT_ATTACK_OFF);
                    introGuids.push_back(creature->GetGUID());
                    break;
                case NPC_LIGHTFORGED_WIREFRAME:
                    if (creature->GetPositionZ() >= -10)
                    {
                        wireframesGuids.push_back(creature->GetGUID());
                        creature->CastSpell(creature, SPELL_LIGHT_SHIELD);
                    }
                    break;
                case NPC_LIGHT_CENTURION:
                case NPC_LIGHT_COMANDIR:
                case NPC_LIGHT_PRIEST:
                    if (creature->GetPositionY() >= 1610.0f)
                    {
                        creature->SetReactState(REACT_ATTACK_OFF);
                        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);

                        if (creature->GetDistance(-3285.94f, 10254.57f, -127.32f) <= 10)
                        {
                            if (creature->GetEntry() == NPC_LIGHT_CENTURION)
                                creature->CastSpell(creature, 64165);
                        }   
                        else
                            creature->SetVisible(false);
                    }
                    break;
                case NPC_RUN_NPC_1:
                case NPC_RUN_NPC_2:
                    creature->SetReactState(REACT_ATTACK_OFF);
                    creature->CastSpell(creature, 211762);
                    break;
                case NPC_FEL_PORTAL_1:
                case NPC_FEL_PORTAL_2:
                    creature->SetReactState(REACT_ATTACK_OFF);
                    creature->setRegeneratingHealth(false);
                    break;
                case NPC_IMPLOSIONS:
                    mobsAfterKingaroth.push_back(creature->GetGUID());
                    break;
                case NPC_WIND_STALKER:
                    creature->SetReactState(REACT_ATTACK_OFF);
                    creature->CastSpell(creature, 246198);
                    creature->GetMotionMaster()->MoveRandom(7.0f);
                    break;
                default:
                    break;
            }

            if (!creature->isAlive())
                return;

            if (creature->getFaction() == 35 || creature->getFaction() == 2921 || creature->getFaction() == 2916)
            {
                if (creature->GetDistance2d(-3296.24f, 9763.55f) <= 50 && GetBossState(DATA_WORLDBREAKER) != DONE) // frist
                {
                    introFirstBossGuids.push_back(creature->GetGUID());
                    creature->SetVisible(false);
                    creature->SetReactState(REACT_ATTACK_OFF);
                }
                if (creature->GetDistance2d(-3293.42f, 10340.64f) <= 25.0f && GetBossState(DATA_FELHOUNDS) != DONE) // second boss
                {
                    secondBossOutro.push_back(creature->GetGUID());
                    creature->SetVisible(false);
                    creature->SetReactState(REACT_ATTACK_OFF); 
                }
                if (creature->GetDistance2d(-3186.41f, 10345.27f) <= 25.0f  && GetBossState(DATA_ANTORAN) != DONE) // bridge TP
                {
                    mobsFromBridge.push_back(creature->GetGUID());
                    creature->SetVisible(false);
                    creature->SetReactState(REACT_ATTACK_OFF);
                }
                if (creature->GetPositionZ() <= -133.0f && GetBossState(DATA_EONAR) != DONE)
                {
                    mobsToSeondPath.push_back(creature->GetGUID());
                    creature->SetVisible(false);
                    creature->SetReactState(REACT_ATTACK_OFF);
                }
                if (creature->GetDistance(-10575.48f, 8211.12f, 1871.40f) <= 10.0f && GetBossState(DATA_KINGAROTH) != DONE)
                {
                    mobsAfterKingaroth.push_back(creature->GetGUID());
                    creature->SetVisible(false);
                    creature->SetReactState(REACT_ATTACK_OFF);
                }
            }
            else if (creature->getFaction() == 16)
            {
                if (creature->GetDistance2d(-3216.10f, 10302.62f) <= 15)
                    mobsForBridge.insert(creature->GetGUID());
                else if (creature->GetDistance(2891.67f, 10768.20f, -90.77f) <= 10)
                    mobsForElevator.insert(creature->GetGUID());
            }
        }

        void OnUnitDeath(Unit* unit) override
        {
            if (mobsForBridge.find(unit->GetGUID()) != mobsForBridge.end())
            {
                mobsForBridge.erase(unit->GetGUID());
                if (mobsForBridge.empty())
                {
                    for (const auto& guid : mobsFromBridge)
                        if (Creature* cre = instance->GetCreature(guid))
                        {
                            cre->SetVisible(true);
                            cre->CastSpell(cre, SPELL_SPAWN);
                        }
                }
            }
            else if (mobsForElevator.find(unit->GetGUID()) != mobsForElevator.end())
            {
                mobsForElevator.erase(unit->GetGUID());
                if (mobsForElevator.empty())
                {
                    if (GameObject* go = instance->GetGameObject(GetGuidData(GO_ELEVATOR)))
                        go->SetPhaseMask(1, true);
                }
            }
            switch (unit->GetEntry())
            {
            case NPC_ANNIHILATOR_OF_KINGAROTH:
                for(const auto& guid : introFirstBossGuids)
                    if (Creature* cre = instance->GetCreature(guid))
                    {
                        cre->SetVisible(true);
                        cre->CastSpell(cre, SPELL_SPAWN);

                        if (cre->GetEntry() == NPC_EXARH_TURALION)
                        {
                            cre->AddDelayedCombat(3000, [cre]() -> void
                            {
                                cre->CreateConversation(5515);
                                cre->AddDelayedEvent(3000, [cre]() -> void
                                {
                                    cre->GetMotionMaster()->MovePoint(0, -3299.39f, 9733.63f, -63.24f);
                                    cre->AddDelayedEvent(2000, [cre]() -> void
                                    {
                                        cre->GetMotionMaster()->MoveTargetedHome();
                                        cre->SetVisible(false);
                                    });
                                });
                            });
                            continue;
                        }

                        if (cre->GetEntry() == NPC_LIGHTFORGED_WIREFRAME_1 || cre->GetEntry() == NPC_TELEPORT_OF_LIGHTFORGED_1)
                            continue;


                        cre->GetMotionMaster()->MovePoint(0, -3294.85f+frand(-5, 5), 9799.49f + frand(-5, 5), -63.49f);
                        cre->AddDelayedCombat(7000, [cre]() -> void
                        {
                            cre->CastStop();
                            cre->Kill(cre);
                            cre->DespawnOrUnsummon(500);
                        });
                    }
                break;
            case NPC_PRIESTESS_OF_DELIRIUM:
                if (unit->ToCreature()->GetHomePosition().GetPositionY() <= 2900.00f)
                {
                    for (auto id : { 125683, 124913, 125682 })
                        if (Creature* cre = GetCreatureByEntry(id))
                            cre->GetMotionMaster()->MovePath(12899604, false, frand(-3.0f, 3.0f), -frand(3.0f, 3.0f));

                    unit->CastSpell(unit, 254650);
                }
                break;
            }
        }

        void OnGameObjectCreate(GameObject* go) override
        {
            InstanceScript::OnGameObjectCreate(go);

            switch (go->GetEntry())
            {
            case GO_ELEVATOR:
                if(GetBossState(DATA_ANTORAN) != DONE)
                    go->SetPhaseMask(2, true);
                break;
            case GO_PORTAL_TO_ELUNARIES:
                if (GetBossState(DATA_HASABEL) != DONE)
                    go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                break;
            case GO_ARCANE_BARRIER_VISUAL:
                if (GetBossState(DATA_IMONAR) == DONE)
                    go->SetAnimKitId(6132, false);
                break;
                default:
                    break;
            }
        }

        void OnGameObjectRemove(GameObject* go) override
        {
            InstanceScript::OnGameObjectRemove(go);
        }

        bool SetBossState(uint32 type, EncounterState state) override
        {
            if (!InstanceScript::SetBossState(type, state))
                return false;

            if (!mobsToSeondPath.empty())
            {
                bool isDone = true;
                for (uint8 i = DATA_WORLDBREAKER; i <= DATA_EONAR; ++i)
                    if (GetBossState(i) != DONE)
                        isDone = false;

                if (isDone)
                {
                    for (auto& guid : mobsToSeondPath)
                        if (Creature* cre = instance->GetCreature(guid))
                            cre->SetVisible(true);

                    mobsToSeondPath.clear();
                }
            }

            switch (state)
            {
                case DONE:
                {
                    switch (type)
                    {
                    case DATA_WORLDBREAKER:
                        for (const auto& guid : introFirstBossGuids)
                            if (Creature* cre = instance->GetCreature(guid))
                                if (cre->GetEntry() == NPC_LIGHTFORGED_WIREFRAME_1 || cre->GetEntry() == NPC_EXARH_TURALION || cre->GetEntry() == NPC_TELEPORT_OF_LIGHTFORGED_1)
                                {
                                    cre->SetVisible(true);
                                    cre->CastSpell(cre, SPELL_SPAWN);
                                }
                        break;
                    case DATA_FELHOUNDS:
                        for (const auto& guid : secondBossOutro)
                            if (Creature* cre = instance->GetCreature(guid))
                            {
                                cre->SetVisible(true);
                                Position pos{};
                                cre->GetMotionMaster()->MovePoint(1, -3218.40f + frand(-10, 10), 10374.50f + frand(-10, 10), -155.47f);
                                if (cre->GetEntry() == NPC_LIGHTFORGED_WIREFRAME)
                                    cre->AddDelayedEvent(14000, [cre]() -> void
                                {
                                    cre->CastSpell(cre, SPELL_LIGHT_SHIELD);
                                    cre->CastSpell(cre, 252787);
                                });
                            }
                        break;
                    case DATA_HASABEL:
                        if (GameObject* go = instance->GetGameObject(GetGuidData(GO_PORTAL_TO_ELUNARIES)))
                            go->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        break;
                    case DATA_KINGAROTH:
                        for (auto& guid : mobsAfterKingaroth)
                            if (Creature* cre = instance->GetCreature(guid))
                            {
                                cre->SetVisible(true);
                                cre->CastSpell(cre, SPELL_SPAWN);

                                if (cre->GetEntry() == NPC_IMPLOSIONS)
                                    if (cre->GetPositionZ() >= 1876.0f)
                                        cre->CastSpell(cre, 251052);
                            }
                        break;
                    }
                    break;
                }
            }
            return true;
        }

        ObjectGuid GetGuidData(uint32 type) const override
        {
            //switch (type)
            //{
            //    default:
                    return InstanceScript::GetGuidData(type);
            //}
        }

        bool CheckRequiredBosses(uint32 bossId, uint32 /*entry*/, Player const* /*player*/ = nullptr) const override
        {
            return true;
        }

        std::string GetSaveData() override
        {
            std::ostringstream saveStream;
            saveStream << "A T B T " << GetBossSaveData();
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

            char dataHead1, dataHead2, dataHead3, dataHead4;

            std::istringstream loadStream(data);
            loadStream >> dataHead1 >> dataHead2 >> dataHead3 >> dataHead4;

            if (dataHead1 == 'A' && dataHead2 == 'T' && dataHead3 == 'B' && dataHead4 == 'T')
            {
                for (uint32 i = 0; i < MAX_ENCOUNTER; ++i)
                {
                    uint32 tmpState;
                    loadStream >> tmpState;
                    if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                        tmpState = NOT_STARTED;
                    SetBossState(i, EncounterState(tmpState));
                }
            }
            else
                OUT_LOAD_INST_DATA_FAIL;

            OUT_LOAD_INST_DATA_COMPLETE;

        }

        WorldLocation* GetClosestGraveYard(float x, float y, float z) override
        {
            uint32 graveyardId = 6161;

            if (WorldSafeLocsEntry const* gy = sWorldSafeLocsStore.LookupEntry(graveyardId))
            {
                loc_res_pla.Relocate(gy->Loc.X, gy->Loc.Y, gy->Loc.Z);
                loc_res_pla.SetMapId(gy->MapID);
            }

            return &loc_res_pla;
        }

        void Update(uint32 diff) override
        {
            if (timerIntroEvent <= diff)
            {
                uint8 rand = urand(1, 2);
                for (uint8 i = 0; i < rand; ++i)
                    if (Creature* dreadwing = instance->SummonCreature(NPC_DREADWING, dreadWingsPos[urand(0, 1)], nullptr, urand(9000, 11000)))
                    {
                        if (urand(1, 2) == 1)
                            dreadwing->GetMotionMaster()->MoveCirclePath(-3322.34f + frand(-10, 10), 9544.79f + frand(-10, 10), 24.30f + frand(-10, 5), frand(50, 60), urand(0, 1), 12);
                        else
                            dreadwing->GetMotionMaster()->MoveRandom(40.0f);
                    }
                timerIntroEvent = urand(4000, 6000);
            }
            else
                timerIntroEvent -= diff;


            if (timerIntroEvent2 <= diff)
            {
                introGuids.remove_if([this](const ObjectGuid& guid) {
                    Creature* cre = instance->GetCreature(guid);
                    return !cre || !cre->isAlive();
                });

                if (!wireframesGuids.empty())
                    if (Creature* protect = instance->GetCreature(wireframesGuids[urand(0, wireframesGuids.size())]))
                    {
                        Creature* cre = nullptr;
                        for (const auto& guid : introGuids)
                            if (Creature* cur = instance->GetCreature(guid))
                                if (!cre || protect->GetDistance(cur) <= protect->GetDistance(cre))
                                    cre = cur;
                        if (cre)
                            protect->CastSpell(cre, SPELL_INTRO_FIELD_OF_FIRE);

                    }

                timerIntroEvent2 = urand(5000, 7000);
            }
            else
                timerIntroEvent2 -= diff;

            if (timerConstEvent <= diff)
            {
                if (TempSummon* cre = instance->SummonCreature((urand(1, 2) == 1 ? NPC_RUN_NPC_1 : NPC_RUN_NPC_2), { -2934.23f + frand(-9, 9), 10312.14f + frand(-9,9), -165.48f }, nullptr, 35000 ))
                {
                    cre->GetMotionMaster()->MovePoint(0, -3164.60f + frand(-9, 9), 10452.94f + frand(-9, 9), -194.78f);
                    cre->ToCreature()->AddDelayedEvent(35000, [cre]() -> void {cre->Kill(cre); });
                }
                timerConstEvent = urand(3000, 4000);

                instance->LoadGrid(-2953.89f, 10324.03f);
                instance->LoadGrid(-3050.18f, 10380.69f);
            }
            else
                timerConstEvent -= diff;

        } 
    };
};

void AddSC_instance_antorus()
{
    new instance_antorus();
}
