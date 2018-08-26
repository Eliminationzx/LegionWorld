/*
    http://uwow.biz
    Dungeon : Dark Heart Thicket 100-110
    Encounter: Arch-druid glaidalis
    Normal: 100%, Heroic: 100%, Mythic: 100%
*/

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "darkheart_thicket.h"

 enum Says
{
    SAY_EVENT_BOSS         = 0,
    SAY_AGGRO              = 1,
    SAY_NIGHTFALL          = 2,
    SAY_RAMPAGE            = 3,
    SAY_GREIEVOUS          = 4,
    SAY_20                 = 5,
    SAY_DEATH              = 6,
    //event
    SAY_EVENT              = 0,
};

enum Spells
{
    SPELL_PRE_GRIEVOUS_LEAP     = 197709, // Используется что бы убить треш. Пред-эвент спелл.
    SPELL_GRIEVOUS_LEAP         = 196346,
    SPELL_GRIEVOUS_LEAP_MORPH   = 196348,
    SPELL_GRIEVOUS_LEAP_RETURN  = 198095,
    SPELL_GRIEVOUS_LEAP_DOT     = 196376,
    SPELL_GRIEVOUS_LEAP_AOE_1   = 196354,
    SPELL_GRIEVOUS_LEAP_AOE_2   = 198269,
    SPELL_PRIMAL_RAMPAGE_MORPH  = 198360,
    SPELL_PRIMAL_RAMPAGE_KNOCK  = 198376,
    SPELL_PRIMAL_RAMPAGE_CHARGE = 198379,
    SPELL_NIGHTFALL             = 198400,
    SPELL_NIGHTFALL_DOT         = 198408,
    SPELL_SUM_ABOMINATION       = 198432, //Mythic

    //pre-event
    SPELL_VISUAL                = 197642,
    //end
    SPELL_TALK                  = 202882,
};

enum eEvents
{
    EVENT_PRE_GRIEVOUS_LEAP     = 1, // Для пред эвента.
    EVENT_GRIEVOUS_LEAP         = 2,
    EVENT_PRIMAL_RAMPAGE        = 3,
    EVENT_NIGHTFALL             = 4,
};

//96512
class boss_arch_druid_glaidalis : public CreatureScript
{
public:
    boss_arch_druid_glaidalis() : CreatureScript("boss_arch_druid_glaidalis") {}

    struct boss_arch_druid_glaidalisAI : public BossAI
    {
        boss_arch_druid_glaidalisAI(Creature* creature) : BossAI(creature, DATA_GLAIDALIS) 
        {
            introEvent = false;
            _introDone = false;
            _say_20 = false;
            instance->HandleGameObject(instance->GetGuidData(GO_GLAIDALIS_INVIS_DOOR), true);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
        }

        bool introEvent;
        bool _introDone;
        bool _say_20;
        uint32 primalDelayTimer;

        void Reset() override
        {
            _Reset();
            _say_20 = false;
            primalDelayTimer = 0;

            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_NIGHTFALL_DOT);
        }

        void EnterCombat(Unit* /*who*/) override
        {
            Talk(SAY_AGGRO);
            _EnterCombat();
            events.ScheduleEvent(EVENT_GRIEVOUS_LEAP, 6000);
            events.ScheduleEvent(EVENT_PRIMAL_RAMPAGE, 14000);
            events.ScheduleEvent(EVENT_NIGHTFALL, 20000);
        }

        void JustDied(Unit* /*killer*/) override
        {
            Talk(SAY_DEATH);
            _JustDied();
            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_NIGHTFALL_DOT);
            DoCast(me, SPELL_TALK, true);
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type == EFFECT_MOTION_TYPE)
                if (id == SPELL_GRIEVOUS_LEAP_MORPH && me->HasAura(SPELL_GRIEVOUS_LEAP_MORPH))
                {
                    DoCastVictim(SPELL_GRIEVOUS_LEAP_RETURN);
                }
        }

        void SpellHitTarget(Unit* target, const SpellInfo* spell) override
        {
            switch (spell->Id)
            {
                case SPELL_GRIEVOUS_LEAP_AOE_1:
                case SPELL_GRIEVOUS_LEAP_AOE_2:
                    if (me->GetGUID() != target->GetGUID())
                        DoCast(target, SPELL_GRIEVOUS_LEAP_DOT, true);
                    break;
            }
        }

        void MoveInLineOfSight(Unit* who) override
        {  
            if (!who || !who->IsPlayer() || who->ToPlayer()->isGameMaster())
                return;

            if (!_introDone && me->IsWithinDistInMap(who, 48.0f))
            {
                _introDone = true;
                instance->HandleGameObject(instance->GetGuidData(GO_GLAIDALIS_INVIS_DOOR), false);
                std::list<Creature*> list;
                me->GetCreatureListWithEntryInGrid(list, NPC_DRUIDIC_PRESERVER, 150.0f);
                if (!list.empty())
                {
                    Talk(SAY_EVENT_BOSS);
                    for (auto const& itr : list)
                    {
                        if (Creature* target = itr->FindNearestCreature(100404, 50.0f, true))  //trigger
                            itr->CastSpell(target, SPELL_VISUAL); 
                    }

                    std::list<Creature*>::const_iterator itr = list.begin();
                    std::advance(itr, urand(0, list.size() - 1));
                    (*itr)->AI()->Talk(SAY_EVENT);
                }
                events.ScheduleEvent(EVENT_PRE_GRIEVOUS_LEAP, 3000);
            }
        }

        void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType dmgType) override
        {
            if (me->HealthBelowPct(21) && !_say_20)
            {
                _say_20 = true;
                Talk(SAY_20);
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim() && introEvent)
                return;

            events.Update(diff);

            if (!primalDelayTimer)
            {
                if (CheckHomeDistToEvade(diff, 35.0f, 2896.79f, 1966.86f, 189.08f))
                    return;
            }
            else
            {
                if (primalDelayTimer <= diff)
                    primalDelayTimer = 0;
                else
                    primalDelayTimer -= diff;
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                   case EVENT_PRE_GRIEVOUS_LEAP:
                        if (Creature* target = me->FindNearestCreature(NPC_DRUIDIC_PRESERVER, 200.0f, true))
                        {
                            me->CastSpell(target, SPELL_PRE_GRIEVOUS_LEAP);
                            me->Kill(target);
                            events.ScheduleEvent(EVENT_PRE_GRIEVOUS_LEAP, 700);
                        }
                        else
                        {
                            BossAI::EnterEvadeMode(); //на начальное место
                            if (Creature* trigger = me->FindNearestCreature(102851, 200.0f, true))   //trigger aura
                                trigger->RemoveAurasDueToSpell(203257);
                            instance->HandleGameObject(instance->GetGuidData(GO_GLAIDALIS_INVIS_DOOR), true);
                            introEvent = true;
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                        }
                        break;
                    case EVENT_GRIEVOUS_LEAP:
                        if (urand(0, 1) == 1) 
                            Talk(SAY_GREIEVOUS);
                        DoCast(SPELL_GRIEVOUS_LEAP);
                        events.ScheduleEvent(EVENT_GRIEVOUS_LEAP, 16000);
                        break;
                    case EVENT_PRIMAL_RAMPAGE:                        
                        if (urand(0, 1) == 1) 
                            Talk(SAY_RAMPAGE);
                        primalDelayTimer = 10000;
                        DoCast(me, SPELL_PRIMAL_RAMPAGE_MORPH, true);
                        if (Unit* pTaget = me->getVictim())
                        {
                            DoCast(pTaget, SPELL_PRIMAL_RAMPAGE_KNOCK, true);
                            DoCast(pTaget, SPELL_PRIMAL_RAMPAGE_CHARGE);
                        }
                        events.ScheduleEvent(EVENT_PRIMAL_RAMPAGE, 16000);
                        break;
                    case EVENT_NIGHTFALL:
                        Talk(SAY_NIGHTFALL);
                        DoCast(SPELL_NIGHTFALL);
                        events.ScheduleEvent(EVENT_NIGHTFALL, 16000);
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_arch_druid_glaidalisAI (creature);
    }
};

//196346
class spell_glaidalis_grievous_leap : public SpellScriptLoader
{
public:
    spell_glaidalis_grievous_leap() : SpellScriptLoader("spell_glaidalis_grievous_leap") { }

    class spell_glaidalis_grievous_leap_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_glaidalis_grievous_leap_SpellScript);

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            Unit* caster = GetCaster();
            if (targets.empty() || !caster)
                return;

            targets.clear();

            if (Unit* target = caster->GetAI()->SelectTarget(SELECT_TARGET_FARTHEST, 0, 55.0f, true))
                targets.push_back(target);
        }

        void HandleOnHit()
        {
            if (!GetCaster() || !GetHitUnit())
                return;

            GetCaster()->CastSpell(GetHitUnit(), SPELL_GRIEVOUS_LEAP_MORPH, true);
        }

        void Register() override
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_glaidalis_grievous_leap_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            OnHit += SpellHitFn(spell_glaidalis_grievous_leap_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_glaidalis_grievous_leap_SpellScript();
    }
};

//196376
class spell_glaidalis_grievous_tear : public AuraScript
{
    PrepareAuraScript(spell_glaidalis_grievous_tear)

    uint32 checkTimer = 500;

    void OnUpdate(uint32 diff, AuraEffect* auraEffect)
    {
        if (checkTimer <= diff)
        {
            checkTimer = 500;

            if (GetUnitOwner() && GetUnitOwner()->HealthAbovePct(90))
                GetAura()->Remove();
        }
        else
            checkTimer -= diff;
    }
    
    void Absorb(AuraEffect* /*AuraEffect**/, DamageInfo& dmgInfo, float& absorbAmount)
    {
        absorbAmount = 0;
    }

    void Register() override
    {
        OnEffectUpdate += AuraEffectUpdateFn(spell_glaidalis_grievous_tear::OnUpdate, EFFECT_1, SPELL_AURA_SCHOOL_HEAL_ABSORB);
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_glaidalis_grievous_tear::Absorb, EFFECT_1, SPELL_AURA_SCHOOL_HEAL_ABSORB);
    }
};

//200684 trash
class spell_nightmare_toxin : public SpellScriptLoader
{
    public:
        spell_nightmare_toxin() : SpellScriptLoader("spell_nightmare_toxin") {}

        class spell_nightmare_toxin_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_nightmare_toxin_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();
                if (Creature* spider = target->FindNearestCreature(101679, 40.0f, true))
                    {
                        spider->CastSpell(target, 200686, true);
                    }
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_nightmare_toxin_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_nightmare_toxin_AuraScript();
        }
};



void AddSC_boss_arch_druid_glaidalis()
{
    new boss_arch_druid_glaidalis();
    new spell_glaidalis_grievous_leap();
    RegisterAuraScript(spell_glaidalis_grievous_tear);
    new spell_nightmare_toxin();
}