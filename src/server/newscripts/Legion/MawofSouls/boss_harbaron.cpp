/*
    http://uwow.biz
    Dungeon : Maw of Souls 100-110
    Encounter: Harbaron
    Normal: 100%, Heroic: 100%, Mythic: 100%
*/

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "maw_of_souls.h"

enum Says
{
    SAY_INTRO               = 0,
    SAY_AGGRO               = 1,
    SAY_SUMMON              = 2,
    SAY_FRAGMENT            = 3,
    SAY_EMOTE_FRAGMENT      = 4,
    SAY_DEATH               = 5,
}; 

enum Spells
{
    SPELL_COSMIC_SCYTHE             = 205330,
    SPELL_COSMIC_SCYTHE_VIS         = 198580,
    SPELL_COSMIC_SCYTHE_VIS_2       = 194667,
    SPELL_COSMIC_SCYTHE_DMG         = 194218,
    SPELL_SUM_SHACKLED_SERVITOR     = 194231,
    SPELL_FRAGMENT                  = 194325,

    //Heroic
    SPELL_NETHER_RIP_AURA           = 194668,

    //Summons
    SPELL_SHACKLED_SERVITOR         = 194259,
    SPELL_VOID_SNAP                 = 194266,
};

enum eEvents
{
    EVENT_COSMIC_SCYTHE             = 1,
    EVENT_SUM_SHACKLED_SERVITOR     = 2,
    EVENT_FRAGMENT                  = 3,
    EVENT_NETHER_RIP                = 4,
};

//96754
class boss_harbaron : public CreatureScript
{
public:
    boss_harbaron() : CreatureScript("boss_harbaron") {}

    struct boss_harbaronAI : public BossAI
    {
        boss_harbaronAI(Creature* creature) : BossAI(creature, DATA_HARBARON) {}

        std::list<ObjectGuid> listGuid;
        std::list<ObjectGuid> trashGUID;
        bool _introDone;
        uint8 scytheRange;

        void Reset() override
        {
            _Reset();
        }

        void EnterCombat(Unit* /*who*/) override
        {
            Talk(SAY_AGGRO);
            _EnterCombat();
            trashGUID.clear();
            events.ScheduleEvent(EVENT_COSMIC_SCYTHE, 4000);
            events.ScheduleEvent(EVENT_SUM_SHACKLED_SERVITOR, 7000);
            events.ScheduleEvent(EVENT_FRAGMENT, 18000);

            if (GetDifficultyID() != DIFFICULTY_NORMAL)
                events.ScheduleEvent(EVENT_NETHER_RIP, 13000);
        }

        void JustDied(Unit* /*killer*/) override
        {
            Talk(SAY_DEATH);
            DespawnSouls();
            _JustDied();

            if (Creature* skjal = instance->instance->GetCreature(instance->GetGuidData(DATA_SKJAL)))
                skjal->AI()->DoAction(ACTION_1);
        }

        void EnterEvadeMode()
        {
            DespawnSouls();
            BossAI::EnterEvadeMode();
        }

        void DespawnSouls()
        {
            if (!trashGUID.empty())
                for (auto const& guid : trashGUID)
                    if (Creature* soul = Creature::GetCreature(*me, guid))
                        if (soul->IsInWorld())
                            soul->DespawnOrUnsummon();
        }

        void SpellHitTarget(Unit* target, const SpellInfo* spell) override
        {
            if (spell->Id == SPELL_COSMIC_SCYTHE)
            {
                Position pos;
                float angle = me->GetRelativeAngle(target);
                scytheRange = 0;
                for (uint8 i = 0; i < 6; ++i)
                {
                    scytheRange += 5;
                    me->GetNearPosition(pos, scytheRange, angle);
                    me->m_Events.AddEvent(new DelaySummonEvent(*me, NPC_COSMIC_SCYTHE_2, pos, 5.0f), me->m_Events.CalculateTime(100 * i));
                }
            }

            if (spell->Id == SPELL_FRAGMENT)
                for (uint8 i = 0; i < 2; ++i)
                    if (auto soul = target->SummonCreature(NPC_SOUL_FRAGMENT, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ() + 1.0f, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000))
                        trashGUID.push_back(soul->GetGUID());
        }

        void MoveInLineOfSight(Unit* who) override
        {  
            if (who->GetTypeId() != TYPEID_PLAYER)
                return;

            if (!_introDone && me->IsWithinDistInMap(who, 45.0f))
            {
                _introDone = true;
                Talk(SAY_INTRO);
            }
        }

        bool GetObjectData(ObjectGuid const& guid) override
        {
            bool find = false;

            for (auto targetGuid : listGuid)
                if (targetGuid == guid)
                    find = true;

            if (!find)
                listGuid.push_back(guid);

            return find;
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (CheckHomeDistToEvade(diff, 3.0f, 0.0, 0.0f, 554.28f, true))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_COSMIC_SCYTHE:
                        listGuid.clear();
                        DoCast(SPELL_COSMIC_SCYTHE);
                        events.ScheduleEvent(EVENT_COSMIC_SCYTHE, 8000);
                        break;
                    case EVENT_SUM_SHACKLED_SERVITOR:
                    {
                        Talk(SAY_SUMMON);
                        Position pos;
                        me->GetRandomNearPosition(pos, 15.0f);
                        me->CastSpell(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), SPELL_SUM_SHACKLED_SERVITOR);
                        events.ScheduleEvent(EVENT_SUM_SHACKLED_SERVITOR, 23000);
                        break;
                    }
                    case EVENT_FRAGMENT:
                        Talk(SAY_FRAGMENT);
                        if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 60.0f, true))
                        {
                            Talk(SAY_EMOTE_FRAGMENT, pTarget->GetGUID());
                            DoCast(pTarget, SPELL_FRAGMENT);
                        }
                        events.ScheduleEvent(EVENT_FRAGMENT, 30000);
                        break;
                    case EVENT_NETHER_RIP:
                        me->AddAura(SPELL_NETHER_RIP_AURA, me);
                        events.ScheduleEvent(EVENT_NETHER_RIP, 13000);
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_harbaronAI (creature);
    }
};

//100839
class npc_harbaron_scythe : public CreatureScript
{
public:
    npc_harbaron_scythe() : CreatureScript("npc_harbaron_scythe") { }

    struct npc_harbaron_scytheAI : public ScriptedAI
    {
        npc_harbaron_scytheAI(Creature* creature) : ScriptedAI(creature) 
        {
            me->SetReactState(REACT_PASSIVE);
        }

        EventMap events;

        void Reset() override
        {
            DoCast(me, SPELL_COSMIC_SCYTHE_VIS, true);
            events.ScheduleEvent(EVENT_1, 2000);
        }

        void UpdateAI(uint32 diff) override
        {
            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_1:
                        me->RemoveAurasDueToSpell(SPELL_COSMIC_SCYTHE_VIS);
                        DoCast(me, SPELL_COSMIC_SCYTHE_VIS_2, true);
                        me->CastSpell(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() - 5, SPELL_COSMIC_SCYTHE_DMG);
                        me->DespawnOrUnsummon(500);
                        break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_harbaron_scytheAI(creature);
    }
};

//98693
class npc_harbaron_shackled_servitor : public CreatureScript
{
public:
    npc_harbaron_shackled_servitor() : CreatureScript("npc_harbaron_shackled_servitor") {}

    struct npc_harbaron_shackled_servitorAI : public ScriptedAI
    {
        npc_harbaron_shackled_servitorAI(Creature* creature) : ScriptedAI(creature) 
        {
            SetCombatMovement(false);
        }

        EventMap events;

        void Reset() override {}

        void IsSummonedBy(Unit* summoner) override
        {
            DoZoneInCombat(me, 100.0f);
            DoCast(me, SPELL_SHACKLED_SERVITOR, true);
        }

        void EnterCombat(Unit* /*who*/) override
        //22:06
        {
            events.ScheduleEvent(EVENT_1, 2000); //22:08, 22:15, 22:21
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
                        DoCast(SPELL_VOID_SNAP);
                        events.ScheduleEvent(EVENT_1, 6000);
                        break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_harbaron_shackled_servitorAI(creature);
    }
};

//98761
class npc_harbaron_soul_fragment : public CreatureScript
{
public:
    npc_harbaron_soul_fragment() : CreatureScript("npc_harbaron_soul_fragment") { }

    struct npc_harbaron_soul_fragmentAI : public ScriptedAI
    {
        npc_harbaron_soul_fragmentAI(Creature* creature) : ScriptedAI(creature) 
        {
            me->SetReactState(REACT_PASSIVE);
        }

        void Reset() override {}

        void IsSummonedBy(Unit* summoner) override
        {
            summoner->CastSpell(me, 194345, true); //Clone Caster
            DoCast(summoner, 194344, true); //Dmg tick summoner
            DoCast(me, 194381, true); //Scale
            me->GetMotionMaster()->MoveRandom(15.0f);
        }

        void JustDied(Unit* /*killer*/) override
        {
            if (me->ToTempSummon())
                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                {
                    if (Aura* aura = summoner->GetAura(194344, me->GetGUID()))
                        aura->Remove();

                    if (!summoner->HasAura(194344))
                        summoner->RemoveAurasDueToSpell(194327);
                }
        }

        void UpdateAI(uint32 diff) override {}
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_harbaron_soul_fragmentAI(creature);
    }
};

//194668
class spell_harbaron_nether_rip : public SpellScriptLoader
{
    public:
        spell_harbaron_nether_rip() : SpellScriptLoader("spell_harbaron_nether_rip") { }

        class spell_harbaron_nether_rip_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_harbaron_nether_rip_AuraScript);

            void PeriodicTick(AuraEffect const* aurEff)
            {
                if (!GetCaster())
                    return;

                switch (aurEff->GetTickNumber())
                {
                    case 1:
                        GetCaster()->CastSpell(GetCaster(), 198726, true);
                        break;
                    case 2:
                        GetCaster()->CastSpell(GetCaster(), 198724, true);
                        break;
                    case 3:
                        GetCaster()->CastSpell(GetCaster(), 198727, true);
                        break;
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_harbaron_nether_rip_AuraScript::PeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_harbaron_nether_rip_AuraScript();
        }
};

//194218
class spell_harbaron_cosmic_scythe_filter : public SpellScriptLoader
{
    public:
        spell_harbaron_cosmic_scythe_filter() : SpellScriptLoader("spell_harbaron_cosmic_scythe_filter") { }

        class spell_harbaron_cosmic_scythe_filter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_harbaron_cosmic_scythe_filter_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (Unit* caster = GetCaster())
                {
                    if (Creature* owner = caster->GetAnyOwner()->ToCreature())
                        for (std::list<WorldObject*>::const_iterator itr = targets.begin(); itr != targets.end(); ++itr)
                            if (owner->AI()->GetObjectData((*itr)->GetGUID()))
                                targets.remove(*itr++);
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_harbaron_cosmic_scythe_filter_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_harbaron_cosmic_scythe_filter_SpellScript();
        }
};

void AddSC_boss_harbaron()
{
    new boss_harbaron();
    new npc_harbaron_scythe();
    new npc_harbaron_shackled_servitor();
    new npc_harbaron_soul_fragment();
    new spell_harbaron_nether_rip();
    new spell_harbaron_cosmic_scythe_filter();
}