/*
    https://uwow.biz/
*/

#include "antorus.h"

// 127233
struct npc_atbt_flamewear : public ScriptedAI
{
    npc_atbt_flamewear(Creature* cre) : ScriptedAI(cre) {}

    EventMap events;
    std::vector<ObjectGuid> targs{};

    void Reset() override
    {
        events.Reset();
    }

    void EnterCombat(Unit* who) override
    {
        events.ScheduleEvent(EVENT_1, 1500);
        events.ScheduleEvent(EVENT_2, 10000);
    }

    void SetGUID(const ObjectGuid& guid, int32 type) override
    {
        targs.push_back(guid);
        if (targs.size() != 2)
            return;
        for (bool x : {false, true})
            if (Player * player = ObjectAccessor::GetPlayer(*me, targs[x]))
                if (Aura* aura = player->GetAura(252621))
                    aura->SetScriptGuid(0, targs[!x]);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_1:
                DoCast(252614);
                events.ScheduleEvent(EVENT_1, 3500);
                break;
            case EVENT_2:
                targs.clear();
                DoCast(252621);
                events.ScheduleEvent(EVENT_2, urand(1000, 14000));
                break;
            }
        }

        DoMeleeAttackIfReady();
    }

};

// 125819
struct npc_atbt_battleship_controller : public ScriptedAI
{
    npc_atbt_battleship_controller(Creature* cre) : ScriptedAI(cre)
    {
        SetCanSeeEvenInPassiveMode(true);
    }

    EventMap events;
    bool start = false;

    void Reset() override
    {
        events.Reset();
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (start)
            return;

        if (!who->IsPlayer())
            return;

        if (me->GetDistance(who) > 98.0f)
            return;

        start = true;

        events.ScheduleEvent(EVENT_1, 3000);

        if (Creature* cre = me->FindNearestCreature(125480, 90.0, true)) // really need it =C
            cre->AI()->Talk(0);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!start)
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_1:
                me->SummonCreature(NPC_BATTLE_SHIP, -3268.71f, 10120.37f, -67.19f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 9000);
                events.ScheduleEvent(EVENT_1, urand(3000, 5000));
                break;
            }
        }
    }

};

std::vector<Position> box
{
    { -3285.51f, 10104.72f, -122.55f },
    { -3266.17f, 10099.20f, -122.65f },
    { -3303.73f, 9971.47f, -112.40f },
    { -3331.37f, 9987.23f, -112.44f }
};

// 125771
struct npc_atbt_battleship : public ScriptedAI
{
    npc_atbt_battleship(Creature* cre) : ScriptedAI(cre) {}

    void IsSummonedBy(Unit* owner) override
    {
        me->SetReactState(REACT_PASSIVE);
        me->SetCanFly(true);
        me->GetMotionMaster()->MovePoint(0, -3312.39f, 9992.18f, -87.09f); 

        me->AddDelayedEvent(urand(2000, 3000), [this]() -> void
        {
            uint8 i = 0;
            for (float y = 10099.79f; y >= 9980.05f; y -= frand(2, 7))
            {
                me->AddDelayedEvent(100 * ++i, [this, y]() -> void
                {
                    float x = frand(-3305.59f, -3286.11f);
                    me->CastSpell(x, y, -115.73f, 249933, true);
                });
            }
        });
    }
};

// 128289
struct npt_atbt_teleport : public ScriptedAI
{
    npt_atbt_teleport(Creature* cre) : ScriptedAI(cre) {}

    void OnSpellClick(Unit* who) override
    {
        who->CastSpell(who, 253773);
        who->GetMotionMaster()->MoveIdle();
        who->GetMotionMaster()->MovePath(me->GetPositionX() < -3100.0f ? 12871196 : 12871197, false, frand(-2, 2), frand(-2, 2));
    }
};


//125720, 128303 128304
class npt_atbt_tele_gates : public CreatureScript
{
public:
    npt_atbt_tele_gates() : CreatureScript("npt_atbt_tele_gates") {}


    bool OnGossipHello(Player* player, Creature* me)  override
    {
        InstanceScript* instance = me->GetInstanceScript();
        if (!instance)
            return false;

        bool isDone = true;
        for (uint8 i = DATA_WORLDBREAKER; i <= DATA_EONAR; ++i)
            if (instance->GetBossState(i) != DONE)
                isDone = false;

        if (isDone && me->GetEntry() != 128303) 
            player->ADD_GOSSIP_ITEM_DB(21377, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1); // imonar

        if (me->GetEntry() != 125720) 
            player->ADD_GOSSIP_ITEM_DB(21377, 3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2); // start

        isDone = true;
        for (uint8 i = DATA_IMONAR; i <= DATA_KINGAROTH; ++i)
            if (instance->GetBossState(i) != DONE)
                isDone = false;

        if (me->GetEntry() != 128304)
            player->ADD_GOSSIP_ITEM_DB(21377, 1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3); // Varimatras


        player->SEND_GOSSIP_MENU(32550, me->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* me, uint32 sender, uint32 action)  override
    {

        if (me->GetEntry() == 125720)
            player->CastSpell(player, 254498);
        else if (me->GetEntry() == 128303)
            player->CastSpell(player, 254499);

        switch (action)
        {
        case GOSSIP_ACTION_INFO_DEF + 1:
            player->NearTeleportTo(-10574.47f, 8772.07f, 1871.46f, 4.68f);
            break;
        case GOSSIP_ACTION_INFO_DEF + 2:
            player->NearTeleportTo(-3437.5f, 10156.9f, -150.022f, 0.0f);
            break;
        case GOSSIP_ACTION_INFO_DEF + 3:
            player->NearTeleportTo(-12633.91f, -3369.47f, 2513.82f, 0.0f);
            break;
        }

        player->CLOSE_GOSSIP_MENU();
        return true;
    }

};

// 128169
class npc_atbt_muradin : public CreatureScript
{
public:
    npc_atbt_muradin() : CreatureScript("npc_atbt_muradin") {}


    bool OnGossipSelect(Player* player, Creature* me, uint32 sender, uint32 action)  override
    {
        player->CastSpell(player, 254311);
        player->NearTeleportTo(2825.47f, -4567.23f, 291.94f, 0.0f);
        player->CLOSE_GOSSIP_MENU();
        return true;
    }

};

// 252621
class spell_atbt_bound_by_fel : public AuraScript
{
    PrepareAuraScript(spell_atbt_bound_by_fel);

    ObjectGuid second{};

    void OnTick(AuraEffect const* /*aurEff*/)
    {
        Unit* target = GetTarget();
        if (!target)
            return;

        if (target->GetAuraCount(252621) >= 20)
        {
            target->RemoveAurasDueToSpell(252621);
            return;
        }

        if(second.IsEmpty())
            return;

        Unit* second_targ = ObjectAccessor::GetPlayer(*target, second);
        if (!second_targ)
            return;

        target->CastSpell(second_targ, 252622);
        if (target->GetDistance(second_targ) <= 10)
            target->CastSpell(second_targ, 252623);
        else if (Unit* caster = GetCaster())
            if (caster->isAlive())
                caster->CastSpell(target, 252621);
    }

    void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (!GetCaster() || !GetTarget())
            return;

        Creature* cre = GetCaster()->ToCreature();
        if (!cre)
            return;

        Unit* target = GetTarget();
        cre->AI()->SetGUID(target->GetGUID());
    }

    void SetGuid(uint32 type, ObjectGuid const& guid) override
    {
        second = guid;
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_atbt_bound_by_fel::OnTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
        OnEffectApply += AuraEffectApplyFn(spell_atbt_bound_by_fel::OnApply, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);

    }
};

// 253600
class spell_atbt_soulburn : public AuraScript
{
    PrepareAuraScript(spell_atbt_soulburn);

    void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        Unit* target = GetTarget();
        if (!target)
            return;

        target->CastSpell(target, 253601);

        std::list<Player*> players;
        target->GetPlayerListInGrid(players, 8.0f);
        players.remove_if([](Player* player)
        {
            if (!player) 
                return true;
        
            return !player->HasAura(253600);
        });
        Trinity::Containers::RandomResizeList(players, 5);
        for (Player* player : players)
            player->CastSpell(player, 253600);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectApplyFn(spell_atbt_soulburn::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 252740
class spell_atbt_anihilation : public SpellScript
{
    PrepareSpellScript(spell_atbt_anihilation);

    bool check = false;

    void HandleAfterCast()
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;
        
        for (uint8 i = 0; i < 3; ++i)
        {
            Position pos(caster->GetPosition());
            caster->GetFirstCollisionPosition(pos, 15.0f * frand(0.3f, 1.0f), static_cast<float>(rand_norm()) * static_cast<float>(2.0f * M_PI));
            caster->CastSpell(pos, 252742, false);
            caster->CastSpell(pos, 252741, false);
        }
    }

    void Register()
    {
        AfterCast += SpellCastFn(spell_atbt_anihilation::HandleAfterCast);
    }
};


class eventobject_antorus_into : public EventObjectScript
{
public:
    eventobject_antorus_into() : EventObjectScript("eventobject_antorus_into")
    {}

    bool eventDone = false;

    bool OnTrigger(Player* player, EventObject* eo) override
    {
        if (eventDone)
            return true;

        if (eo->GetEntry() != 805)
            eventDone = true;

        InstanceScript* instance = player->GetInstanceScript();
        if (!instance)
            return false;
        
        switch (eo->GetEntry())
        {
        case 800:
            player->CreateConversation(5545);
            player->AddDelayedEvent(7000, [player]() -> void
            {
                player->CreateConversation(5522);
            });
            break;
        case 801:
            player->AddDelayedEvent(1000, [player]() -> void
            {
                player->CreateConversation(5524);
            });
            break;
        case 802:
            player->AddDelayedEvent(8000, [player, instance]() -> void
            {
                instance->instance->LoadGrid(-3705.06f, 1357.21f);
                if (Creature* cre = instance->instance->GetCreature(instance->GetGuidData(NPC_HASABEL)))
                    cre->AI()->ZoneTalk(0);
            });
            break;
        case 803:
            if (instance->GetBossState(DATA_HASABEL) != DONE)
            {
                eventDone = false;
                return false;
            }
            eo->SummonCreature(NPC_IMAGE_OF_EONAR, eo->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, 21000);
            break;
        case 804:
            if (Creature* eonar = eo->SummonCreature(NPC_EONAR_EVENT, -4299.92f, -11179.68f, 817.63f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 40000))
            {
                player->CreateConversation(5703);
                eonar->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_ATTACKABLE_1);
            }
            break;
        case 805:
            if (!player->HasAura(SPELL_SURGE_OF_LIFE_OVERRIDE))
            {
                std::list<Creature*> mobs;
                player->GetCreatureListWithEntryInGrid(mobs, 127681, 30.0f);
                for (auto& npc : mobs)
                    npc->CastSpell(player, 254097);
                player->AddAura(SPELL_SURGE_OF_LIFE_OVERRIDE, player);
            }
            break;
        case 806:
            player->AddDelayedEvent(1000, [player]() -> void
            {
                player->CastSpell(player, 249441);
            });
            break;
        case 807:
            if (instance->GetBossState(DATA_IMONAR) == DONE)
                break;
            instance->instance->LoadGrid(-10599.29f, 9036.34f);
            player->CreateConversation(5608);

            if (Creature* cre = eo->SummonCreature(NPC_IMONAR_INTRO, -10574.15f, 8606.92f, 1909.52f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 30000))
            {
                instance->instance->LoadGrid(-10754.15f, 8606.92f);
                cre->GetMotionMaster()->MovePoint(0, -10572.75f, 8715.77f, 1891.68f);
                cre->AddDelayedEvent(9000, [cre]() -> void
                {
                    InstanceScript* script = cre->GetInstanceScript();
                    if (!script)
                        return;
                    
                    if (Creature* boss = script->instance->GetCreature(script->GetGuidData(NPC_IMONAR)))
                    {
                        boss->SetVisible(true);
                        boss->CastSpell(boss, 249569);
                        boss->AI()->Talk(0);
                    }

                    cre->AddDelayedEvent(3000, [cre]() -> void
                    {
                        cre->GetMotionMaster()->MovePoint(0, -10791.75f, 8883.12f, 2056.20f);
                    });
                });
            }
            break;
        case 808:
            player->AddDelayedEvent(1000, [player]() -> void
            {
                player->CastSpell(player, 249802);
            });
            break;
        case 809:
            player->AddDelayedEvent(1000, [player]() -> void
            {
                player->CastSpell(player, 250695);
            });
            instance->instance->LoadGrid(-12694.59f, -3597.12f);
            break;      
        case 810:
        {
            player->AddDelayedEvent(1000, [player]() -> void
            {
                player->CastSpell(player, 250797);
            });
            uint32 const entries[3]{ 125683, 124913, 125682 };
            Position const pos[3]{
                {-12639.84f, -3029.82f, 2498.75f, 1.57f},
                {-12631.39f, -3029.82f, 2498.97f, 1.57f},
                {-12626.20f, -3029.82f, 2498.97f, 1.57f }
            };
            for (uint8 i = 0; i < 3; ++i)
                if (Creature* cre = eo->SummonCreature(entries[i], pos[i]))
                {
                    cre->SetReactState(REACT_ATTACK_OFF);
                    cre->AddDelayedEvent(1000, [cre]() -> void
                    {
                        cre->GetMotionMaster()->MovePoint(1, cre->GetPositionX(), -2947.84f + frand(-3.0f, 3.0f), 2499.12f);
                    });
                }

            break;
        }
        case 811:
            player->AddDelayedEvent(1000, [player]() -> void
            {
                player->CastSpell(player, 250698);
            });
            break;
        case 812:
            instance->instance->LoadGrid(-12633.20f, -2231.10f);
            player->AddDelayedEvent(1000, [player]() -> void
            {
                player->CastSpell(player, 250700);
                player->AddDelayedEvent(50000, [player]() -> void
                {
                    player->CastSpell(player, 250728);
                });
            });
            break;
        }
        return true;
    }
};


void AddSC_antorus()
{
    RegisterCreatureAI(npc_atbt_flamewear);
    RegisterCreatureAI(npc_atbt_battleship_controller);
    RegisterCreatureAI(npc_atbt_battleship);
    RegisterCreatureAI(npt_atbt_teleport);
    new npt_atbt_tele_gates;
    new npc_atbt_muradin();
    RegisterAuraScript(spell_atbt_bound_by_fel);
    RegisterAuraScript(spell_atbt_soulburn);
    RegisterSpellScript(spell_atbt_anihilation);
    new eventobject_antorus_into();
}
