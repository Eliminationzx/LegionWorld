/*
    Dungeon : Template of Mogu'shan Palace 87-89
    Instance General Script
    Jade servers
*/

#include "VMapFactory.h"
#include "mogu_shan_palace.h"
#include "Packets/WorldStatePackets.h"

class instance_mogu_shan_palace : public InstanceMapScript
{
public:
    instance_mogu_shan_palace() : InstanceMapScript("instance_mogu_shan_palace", 994) { }

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_mogu_shan_palace_InstanceMapScript(map);
    }

    struct instance_mogu_shan_palace_InstanceMapScript : public InstanceScript
    {
        /*
        ** Trial of the king.
        */
        ObjectGuid xin_guid;
        ObjectGuid kuai_guid;
        ObjectGuid ming_guid;
        ObjectGuid haiyan_guid;
        GuidList scrappers;
        GuidList adepts;
        GuidList grunts;
        /*
        ** End of the trial of the king.
        */
        /*
        ** Gekkan.
        */
        ObjectGuid gekkan;
        ObjectGuid glintrok_ironhide;
        ObjectGuid glintrok_skulker;
        ObjectGuid glintrok_oracle;
        ObjectGuid glintrok_hexxer;
        /*
        ** End of Gekkan.
        */
        /*
        ** Xin the weaponmaster.
        */
        GuidList animated_staffs;
        GuidList animated_axes;
        GuidList swordLauncherGuids;
        /*
        ** End of Xin the weaponmaster.
        */

        ObjectGuid doorBeforeTrialGuid;
        ObjectGuid trialChestGuid;
        ObjectGuid doorAfterTrialGuid;
        ObjectGuid doorBeforeKingGuid;
        ObjectGuid secretdoorGuid;
        
        uint8 JadeCount;
        uint8 GemCount;

        instance_mogu_shan_palace_InstanceMapScript(Map* map) : InstanceScript(map) {}

        void Initialize() override
        {
            xin_guid.Clear();
            kuai_guid.Clear();
            ming_guid.Clear();
            haiyan_guid.Clear();

            doorBeforeTrialGuid.Clear();
            trialChestGuid.Clear();
            doorAfterTrialGuid.Clear();
            doorBeforeKingGuid.Clear();
            secretdoorGuid.Clear();

            gekkan.Clear();
            glintrok_ironhide.Clear();
            glintrok_skulker.Clear();
            glintrok_oracle.Clear();
            glintrok_hexxer.Clear();
            
            JadeCount = 0;
            GemCount = 0;
        }

        void FillInitialWorldStates(WorldPackets::WorldState::InitWorldStates& packet) override
        {
            packet.Worldstates.emplace_back(static_cast<WorldStates>(6761), JadeCount > 0 && JadeCount != 5); // Show_Jade
            packet.Worldstates.emplace_back(static_cast<WorldStates>(6748), JadeCount); // Jade_Count
        }

        bool SetBossState(uint32 id, EncounterState state) override
        {
            if (!InstanceScript::SetBossState(id, state))
                return false;

            switch (id)
            {
                case DATA_TRIAL_OF_THE_KING:
                    /*HandleGameObject(doorBeforeTrialGuid, state != IN_PROGRESS);
                    if (GameObject* chest = instance->GetGameObject(trialChestGuid))
                        chest->SetPhaseMask(state == DONE ? 1: 128, true);*/
                    break;
                case DATA_GEKKAN:
                    HandleGameObject(doorAfterTrialGuid, state == DONE);
                    // Todo : mod temp portal phasemask
                    break;
                case DATA_XIN_THE_WEAPONMASTER:
                    //HandleGameObject(doorBeforeTrialGuid, state != IN_PROGRESS);
                    break;
            }

            return true;
        }

        void OnGameObjectCreate(GameObject* go) override
        {
            switch (go->GetEntry())
            {
                case GO_DOOR_BEFORE_TRIAL:  doorBeforeTrialGuid = go->GetGUID();    break;
                case GO_TRIAL_CHEST:
                case GO_TRIAL_CHEST2:
                    trialChestGuid = go->GetGUID();
                    break;
                case GO_DOOR_AFTER_TRIAL:   doorAfterTrialGuid = go->GetGUID();     break;
                case GO_DOOR_BEFORE_KING:   doorBeforeKingGuid = go->GetGUID();     break;
                case GO_SECRET_DOOR:        secretdoorGuid = go->GetGUID();         break;
            }
        }

        void OnCreatureCreate(Creature* creature) override
        {
            OnCreatureCreate_gekkan(creature);
            OnCreatureCreate_trial_of_the_king(creature);
            OnCreatureCreate_xin_the_weaponmaster(creature);
        }

        void OnUnitDeath(Unit* unit) override
        {
            OnUnitDeath_gekkan(unit);
        }

        void SetData(uint32 type, uint32 data) override
        {
            switch (type)
            {
                case DATA_GEKKAN_ADDS:
                    if (Creature* pGekkan = instance->GetCreature(gekkan))
                    {
                        if (Unit * target = pGekkan->SelectNearestTarget(100.0f))
                        {
                            pGekkan->AI()->AttackStart(target);

                            if (Creature* ironhide = instance->GetCreature(glintrok_ironhide))
                                ironhide->AI()->AttackStart(target);

                            if (Creature* skulker = instance->GetCreature(glintrok_skulker))
                                skulker->AI()->AttackStart(target);

                            if (Creature* oracle = instance->GetCreature(glintrok_oracle))
                                oracle->AI()->AttackStart(target);

                            if (Creature* hexxer = instance->GetCreature(glintrok_hexxer))
                                hexxer->AI()->AttackStart(target);
                        }
                    }
                    break;
                case TYPE_JADECOUNT:
                {
                    JadeCount = data;
                    DoUpdateWorldState(static_cast<WorldStates>(6761), 1);
                    DoUpdateWorldState(static_cast<WorldStates>(6748), JadeCount);
                    if (JadeCount == 5)
                    {
                        DoCastSpellOnPlayers(SPELL_ACHIEV_JADE_QUILEN);
                        DoUpdateWorldState(static_cast<WorldStates>(6761), 0);
                    }
                    break;
                }
                case TYPE_GEMCOUNT:
                {
                    GemCount = data;
                    break;
                }
            }

            SetData_trial_of_the_king(type, data);
            SetData_xin_the_weaponmaster(type, data);
        }

        uint32 GetData(uint32 type) const override
        {
            if (type == TYPE_JADECOUNT)
                return JadeCount;
            if (type == TYPE_GEMCOUNT)
                return GemCount;

            return 0;
        }

        ObjectGuid GetGuidData(uint32 type) const override
        {
            switch (type)
            {
            case TYPE_GET_ENTOURAGE_0:
                return glintrok_hexxer;
            case TYPE_GET_ENTOURAGE_1:
                return glintrok_ironhide;
            case TYPE_GET_ENTOURAGE_2:
                return glintrok_oracle;
            case TYPE_GET_ENTOURAGE_3:
                return glintrok_skulker;
            }
            return ObjectGuid::Empty;
        }

        bool isWipe()
        {
            Map::PlayerList const& PlayerList = instance->GetPlayers();

            if (!PlayerList.isEmpty())
            {
                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                {
                    if(Player* plr = i->getSource())
                        if (plr->isAlive() && !plr->isGameMaster())
                            return false;
                }
            }
            return true;
        }

        void SetData_xin_the_weaponmaster(uint32 type, uint32 data)
        {
            switch (type)
            {
                case TYPE_ACTIVATE_ANIMATED_STAFF:
                {
                    if (Creature* creature = instance->GetCreature(Trinity::Containers::SelectRandomContainerElement(animated_staffs)))
                        if (creature->GetAI())
                            creature->GetAI()->DoAction(0); //ACTION_ACTIVATE
                    break;
                }
                case TYPE_ACTIVATE_ANIMATED_AXE:
                {
                    for (GuidList::iterator guid = animated_axes.begin(); guid != animated_axes.end(); ++guid)
                    {
                        if (Creature* creature = instance->GetCreature(*guid))
                        {
                            if (data)
                            {
                                creature->AddAura(SPELL_AXE_TOURBILOL, creature);
                                creature->AddAura(SPELL_PERMANENT_FEIGN_DEATH, creature);
                                creature->GetMotionMaster()->MoveRandom(50.0f);
                            }
                            else
                            {
                                creature->RemoveAurasDueToSpell(SPELL_AXE_TOURBILOL);
                                creature->RemoveAurasDueToSpell(SPELL_PERMANENT_FEIGN_DEATH);
                                creature->GetMotionMaster()->MoveTargetedHome();
                            }
                        }
                    }
                    break;
                }
                case TYPE_ACTIVATE_SWORD:
                {
                    Position center;
                    center.Relocate(-4632.39f, -2613.20f, 22.0f);

                    bool randPos = urand(0, 1);

                    /*     Y
                           -
                       ***********
                       -> 1 * 2 <-
                     + *********** - X
                       -> 3 * 4 <-
                       ***********
                           +         */

                    for (GuidList::iterator guid = swordLauncherGuids.begin(); guid != swordLauncherGuids.end(); ++guid)
                    {
                        bool mustActivate = false;

                        if (Creature* launcher = instance->GetCreature(*guid))
                        {
                            if (randPos) // Zone 2 & 3
                            {
                                if (launcher->GetPositionX() > center.GetPositionX() && launcher->GetPositionY() > center.GetPositionY()
                                    || launcher->GetPositionX() < center.GetPositionX() && launcher->GetPositionY() < center.GetPositionY())
                                    mustActivate = true;
                            }
                            else // Zone 1 & 4
                            {
                                if (launcher->GetPositionX() > center.GetPositionX() && launcher->GetPositionY() < center.GetPositionY()
                                    || launcher->GetPositionX() < center.GetPositionX() && launcher->GetPositionY() > center.GetPositionY())
                                    mustActivate = true;
                            }

                            if (data && mustActivate)
                                launcher->AddAura(SPELL_THROW_AURA, launcher);
                            else
                                launcher->RemoveAurasDueToSpell(SPELL_THROW_AURA);
                        }
                    }
                }
                break;
            }
        }
        void OnCreatureCreate_xin_the_weaponmaster(Creature* creature)
        {
            switch (creature->GetEntry())
            {
                case 59481:
                    creature->SetReactState(REACT_PASSIVE);
                    creature->SetDisplayId(11686);
                    break;
                case CREATURE_ANIMATED_STAFF:
                    animated_staffs.push_back(creature->GetGUID());
                    break;
                case CREATURE_ANIMATED_AXE:
                    animated_axes.push_back(creature->GetGUID());
                    creature->SetReactState(REACT_PASSIVE);
                    creature->SetVirtualItem(0, 30316);
                    break;
                case CREATURE_LAUNCH_SWORD:
                    swordLauncherGuids.push_back(creature->GetGUID());
                    creature->AddAura(SPELL_PERMANENT_FEIGN_DEATH, creature);
                    break;
            }
        }

        void OnUnitDeath_gekkan(Unit* unit)
        {
            if (unit->ToCreature())
            {
                switch (unit->ToCreature()->GetEntry())
                {
                    case CREATURE_GLINTROK_IRONHIDE:
                    case CREATURE_GLINTROK_SKULKER:
                    case CREATURE_GLINTROK_ORACLE:
                    case CREATURE_GLINTROK_HEXXER:
                    {
                        if (Creature* c = instance->GetCreature(gekkan))
                            if (c->GetAI())
                                c->GetAI()->DoAction(0); //ACTION_ENTOURAGE_DIED
                    }
                    break;
                }
            }
        }
        void OnCreatureCreate_gekkan(Creature* creature)
        {
            switch (creature->GetEntry())
            {
                case CREATURE_GEKKAN:
                    gekkan = creature->GetGUID();
                    break;
                case CREATURE_GLINTROK_IRONHIDE:
                    glintrok_ironhide = creature->GetGUID();
                    break;
                case CREATURE_GLINTROK_SKULKER:
                    glintrok_skulker = creature->GetGUID();
                    break;
                case CREATURE_GLINTROK_ORACLE:
                    glintrok_oracle = creature->GetGUID();
                    break;
                case CREATURE_GLINTROK_HEXXER:
                    glintrok_hexxer = creature->GetGUID();
                    break;
            }
        }

        void SetData_trial_of_the_king(uint32 type, uint32 data)
        {
            switch (type)
            {
            case TYPE_OUTRO_05:
                {
                    if (Creature* haiyan = instance->GetCreature(haiyan_guid))
                        if (haiyan->GetAI())
                            haiyan->GetAI()->DoAction(1); //ACTION_OUTRO_02
                }
                break;
            case TYPE_OUTRO_04:
                {
                    if (Creature* kuai = instance->GetCreature(kuai_guid))
                        if (kuai->GetAI())
                            kuai->GetAI()->DoAction(3); //ACTION_OUTRO_02
                }
                break;
            case TYPE_OUTRO_03:
                {
                    if (Creature* ming = instance->GetCreature(ming_guid))
                        if (ming->GetAI())
                            ming->GetAI()->DoAction(2); //ACTION_OUTRO_02
                }
                break;
            case TYPE_OUTRO_02:
                {
                    if (Creature* haiyan = instance->GetCreature(haiyan_guid))
                        if (haiyan->GetAI())
                            haiyan->GetAI()->DoAction(0); //ACTION_OUTRO_01
                }
                break;
            case TYPE_OUTRO_01:
                {
                    if (Creature* ming = instance->GetCreature(ming_guid))
                        if (ming->GetAI())
                            ming->GetAI()->DoAction(1); //ACTION_OUTRO_01
                }
                break;
            case TYPE_MING_INTRO:
                {
                    if (Creature* ming = instance->GetCreature(ming_guid))
                        if (ming->GetAI())
                            ming->GetAI()->DoAction(0); //ACTION_INTRO
                }
                break;
            case TYPE_WIPE_FIRST_BOSS:
                {
                    Creature* xin = instance->GetCreature(xin_guid);
                    if (!xin)
                        return;
                    xin->SetVisible(true);
                    if (xin->GetAI())
                        xin->GetAI()->Reset();
                    switch (data)
                    {
                    case 0:
                        for (GuidList::iterator guid = adepts.begin(); guid != adepts.end(); ++guid)
                        {
                            Creature* creature = instance->GetCreature(*guid);
                            if (!creature)
                                continue;

                            if (creature && creature->GetAI())
                                creature->GetAI()->DoAction(1); //EVENT_RETIRE
                            creature->RemoveAura(121569);
                        }
                        break;
                    case 1:
                        for (GuidList::iterator guid = scrappers.begin(); guid != scrappers.end(); ++guid)
                        {
                            Creature* creature = instance->GetCreature(*guid);
                            if (!creature)
                                continue;

                            if (creature && creature->GetAI())
                                creature->GetAI()->DoAction(1); //EVENT_RETIRE
                            creature->RemoveAura(121569);
                        }
                        break;
                    case 2:
                        for (GuidList::iterator guid = grunts.begin(); guid != grunts.end(); ++guid)
                        {
                            Creature* creature = instance->GetCreature(*guid);
                            if (!creature)
                                continue;

                            if (creature && creature->GetAI())
                                creature->GetAI()->DoAction(1); //EVENT_RETIRE
                            creature->RemoveAura(121569);
                        }
                        break;
                    }
                }
                break;
            case TYPE_MING_ATTACK:
                {
                    //Move the adepts
                    for (GuidList::iterator guid = adepts.begin(); guid != adepts.end(); ++guid)
                    {
                        Creature* creature = instance->GetCreature(*guid);

                        if (creature && creature->GetAI())
                            creature->GetAI()->DoAction(0); //EVENT_ENCOURAGE
                    }
                    Creature* ming = instance->GetCreature(ming_guid);
                    if (!ming)
                        return;
                    ming->GetMotionMaster()->MovePoint(0, -4237.658f, -2613.860f, 16.48f);
                    ming->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                    ming->SetReactState(REACT_AGGRESSIVE);
                }
                break;
            case TYPE_KUAI_ATTACK:
                {
                    //Move the scrappers
                    for (GuidList::iterator guid = scrappers.begin(); guid != scrappers.end(); ++guid)
                    {
                        Creature* creature = instance->GetCreature(*guid);

                        if (creature && creature->GetAI())
                            creature->GetAI()->DoAction(0); //EVENT_ENCOURAGE
                    }
                    Creature* kuai = instance->GetCreature(kuai_guid);
                    if (!kuai)
                        return;
                    kuai->GetMotionMaster()->MovePoint(0, -4215.359f, -2601.283f, 16.48f);
                    kuai->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                    kuai->SetReactState(REACT_AGGRESSIVE);
                }
                break;
            case TYPE_HAIYAN_ATTACK:
                {
                    //Move the scrappers
                    for (GuidList::iterator guid = grunts.begin(); guid != grunts.end(); ++guid)
                    {
                        Creature* creature = instance->GetCreature(*guid);

                        if (creature && creature->GetAI())
                            creature->GetAI()->DoAction(0); //EVENT_ENCOURAGE
                    }
                    Creature* haiyan = instance->GetCreature(haiyan_guid);
                    if (!haiyan)
                        return;
                    haiyan->GetMotionMaster()->MovePoint(0, -4215.772f, -2627.216f, 16.48f);
                    haiyan->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                    haiyan->SetReactState(REACT_AGGRESSIVE);
                }
                break;
            case TYPE_ALL_ATTACK:
                {
                    for (GuidList::iterator guid = adepts.begin(); guid != adepts.end(); ++guid)
                    {
                        Creature* creature = instance->GetCreature(*guid);

                        if (creature && creature->GetAI())
                            creature->GetAI()->DoAction(2); //ACTION_ATTACK
                        
                        GuidList::iterator itr = grunts.begin();
                        std::advance(itr, urand(0, grunts.size() - 1));

                        Creature* grunt = instance->GetCreature(*itr);
                        if (creature && grunt)
                            creature->Attack(grunt, true);
                    }
                    for (GuidList::iterator guid = grunts.begin(); guid != grunts.end(); ++guid)
                    {
                        Creature* creature = instance->GetCreature(*guid);

                        if (creature && creature->GetAI())
                            creature->GetAI()->DoAction(2); //ACTION_ATTACK

                        GuidList::iterator itr = scrappers.begin();
                        std::advance(itr, urand(0, scrappers.size() - 1));

                        Creature* scrapper = instance->GetCreature(*itr);
                        if (creature && scrapper)
                            creature->Attack(scrapper, true);
                    }
                    for (GuidList::iterator guid = scrappers.begin(); guid != scrappers.end(); ++guid)
                    {
                        Creature* creature = instance->GetCreature(*guid);

                        if (creature && creature->GetAI())
                            creature->GetAI()->DoAction(2); //ACTION_ATTACK

                        GuidList::iterator itr = adepts.begin();
                        std::advance(itr, urand(0, adepts.size() - 1));

                        Creature* adept = instance->GetCreature(*itr);
                        if (creature && adept)
                            creature->Attack(adept, true);
                    }

                    SetBossState(DATA_TRIAL_OF_THE_KING, DONE);
                }
                break;
            case TYPE_MING_RETIRED:
                //Retire the adepts
                for (GuidList::iterator guid = adepts.begin(); guid != adepts.end(); ++guid)
                {
                    Creature* creature = instance->GetCreature(*guid);

                    if (creature && creature->GetAI())
                        creature->GetAI()->DoAction(1); //EVENT_RETIRE
                }
                break;
            case TYPE_KUAI_RETIRED:
                //Retire the adepts
                for (GuidList::iterator guid = scrappers.begin(); guid != scrappers.end(); ++guid)
                {
                    Creature* creature = instance->GetCreature(*guid);

                    if (creature && creature->GetAI())
                        creature->GetAI()->DoAction(1); //EVENT_RETIRE
                }
                break;
            case TYPE_HAIYAN_RETIRED:
                //Retire the adepts
                for (GuidList::iterator guid = grunts.begin(); guid != grunts.end(); ++guid)
                {
                    Creature* creature = instance->GetCreature(*guid);

                    if (creature && creature->GetAI())
                        creature->GetAI()->DoAction(1); //EVENT_RETIRE
                }
                
                if (Creature* xin = instance->GetCreature(xin_guid))
                {
                    xin->DespawnOrUnsummon();
                    HandleGameObject(doorAfterTrialGuid, true);
                }

                if (GameObject* chest = instance->GetGameObject(trialChestGuid))
                {
                    chest->SetPhaseMask(1, true);
                    chest->SetRespawnTime(604800);
                    instance->SummonCreature(CREATURE_JADE_QUILEN, otherPos[0]);
                }
                
                if (GameObject* go = instance->GetGameObject(secretdoorGuid))
                    go->SetGoState(GO_STATE_ACTIVE);

                break;
            }
        }

        void OnCreatureCreate_trial_of_the_king(Creature* creature)
        {
            switch (creature->GetEntry())
            {
            case CREATURE_GURTHAN_SCRAPPER:
                scrappers.push_back(creature->GetGUID());
                break;
            case CREATURE_HARTHAK_ADEPT:
                adepts.push_back(creature->GetGUID());
                break;
            case CREATURE_KARGESH_GRUNT:
                grunts.push_back(creature->GetGUID());
                break;
            case CREATURE_KUAI_THE_BRUTE:
                kuai_guid = creature->GetGUID();
                creature->SetReactState(REACT_PASSIVE);
                break;
            case CREATURE_MING_THE_CUNNING:
                ming_guid = creature->GetGUID();
                creature->SetReactState(REACT_PASSIVE);
                break;
            case CREATURE_HAIYAN_THE_UNSTOPPABLE:
                haiyan_guid = creature->GetGUID();
                creature->SetReactState(REACT_PASSIVE);
                break;
            case CREATURE_XIN_THE_WEAPONMASTER_TRIGGER:
                xin_guid = creature->GetGUID();
                creature->SetReactState(REACT_PASSIVE);
                break;
            case CREATURE_WHIRLING_DERVISH:
                break;
            }
        }
    };

};

class go_mogushan_palace_temp_portal : public GameObjectScript
{
public:
    go_mogushan_palace_temp_portal() : GameObjectScript("go_mogushan_palace_temp_portal") { }

    bool OnGossipHello(Player* player, GameObject* go) override
    {
        if (go->GetPositionZ() < 0.0f)
            player->NearTeleportTo(go->GetPositionX(), go->GetPositionY(), 22.31f, go->GetOrientation());
        else
            player->NearTeleportTo(go->GetPositionX(), go->GetPositionY(), -39.0f, go->GetOrientation());

        return false;
    }
};

void AddSC_instance_mogu_shan_palace()
{
    new instance_mogu_shan_palace();
    new go_mogushan_palace_temp_portal();
}
