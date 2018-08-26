/*
    http://uwow.biz
    Dungeon : Neltharions Lair 100-110
    Encounter: Rokmora
    Normal: 100%, Heroic: 100%, Mythic: 100%
*/

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "neltharions_lair.h"

enum Says
{
    SAY_AGGRO           = 0,
    SAY_RAZOR           = 1,
    SAY_DEATH           = 2,
}; 

enum Spells
{
    SPELL_GAIN_ENERGY               = 193245,
    SPELL_BRITTLE                   = 187714,
    SPELL_INTRO_ULAROGG             = 209390, //Boss 01 Intro Ularogg Cast
    SPELL_INTRO_EMERGE              = 209394, //Boss 01 Intro Emerge

    SPELL_SHATTER_START_CALL_1      = 198122, //Conversation Shatter
    SPELL_SHATTER_END_CALL_1        = 198135, //Conversation Shatter
    SPELL_SHATTER_END_CALL_2        = 198136, //Conversation Shatter
    SPELL_SHATTER                   = 188114,
    SPELL_RAZOR_SHARDS_CALL         = 198125, //Conversation Razor Shards
    SPELL_RAZOR_SHARDS              = 188169,
    SPELL_RAZOR_SHARDS_VISUAL       = 197920,
    SPELL_RAZOR_SHARDS_FILTER       = 209718,
    //Heroic
    SPELL_CRYSTALLINE_GROUND        = 198024,
    SPELL_CRYSTALLINE_GROUND_DMG    = 198028,
    SPELL_RUPTURING_SKITTER         = 215929,

    SPELL_CHOKING_DUST_AT           = 192799,
};

enum eEvents
{
    EVENT_RAZOR_SHARDS              = 1,
    EVENT_CRYSTALLINE_GROUND        = 2,
    EVENT_RUPTURING_SKITTER         = 3,
    EVENT_DEAD_CONVERSATION         = 4,
};

//91003
class boss_rokmora : public CreatureScript
{
public:
    boss_rokmora() : CreatureScript("boss_rokmora") {}

    struct boss_rokmoraAI : public BossAI
    {
        boss_rokmoraAI(Creature* creature) : BossAI(creature, DATA_ROKMORA) 
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
            introDone = false;
        }

        bool introDone;

        void Reset() override
        {
            _Reset();

            DoCast(me, SPELL_BRITTLE, true);
            me->RemoveAurasDueToSpell(SPELL_GAIN_ENERGY);
            me->SetMaxPower(POWER_MANA, 100);
            me->SetPower(POWER_MANA, 0);        
        }

        void EnterCombat(Unit* /*who*/) override
        {
            Talk(SAY_AGGRO); //Rok SMASH!
            _EnterCombat();
            DoCast(me, SPELL_GAIN_ENERGY, true);

            events.ScheduleEvent(EVENT_RAZOR_SHARDS, 30000);

            if (GetDifficultyID() != DIFFICULTY_NORMAL)
                events.ScheduleEvent(EVENT_CRYSTALLINE_GROUND, 4000);
        }

        void JustDied(Unit* /*killer*/) override
        {
            Talk(SAY_DEATH);
            _JustDied();
            events.ScheduleEvent(EVENT_DEAD_CONVERSATION, 3000);
        }

        void MoveInLineOfSight(Unit* who) override
        {  
            if (who->GetTypeId() != TYPEID_PLAYER)
                return;

            if (!introDone && me->IsWithinDistInMap(who, 40.0f))
            {
                introDone = true;
                me->SummonCreature(NPC_NAVARROGG_INTRO, 2917.32f, 1402.29f, -2.28f, 2.744620f, TEMPSUMMON_TIMED_DESPAWN, 22000);
                if (Creature* ularogg = me->SummonCreature(NPC_ULAROGG_INTRO, 2900.33f, 1410.06f, -2.32f, 4.05f, TEMPSUMMON_TIMED_DESPAWN, 22000))
                    ularogg->CastSpell(me, SPELL_INTRO_ULAROGG, true);

                DoCast(me, 209374, true); //Convers
                DoCast(SPELL_INTRO_EMERGE);
            }
        }

        void SpellFinishCast(const SpellInfo* spell)
        {
            if (spell->Id == SPELL_INTRO_EMERGE)
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);

            if (spell->Id == SPELL_RAZOR_SHARDS)
            {
                for (uint8 i = 0; i < 7; ++i)
                    me->CastSpellDelay(me, SPELL_RAZOR_SHARDS_VISUAL, true, 50 * i);
            }
        }

        void DoAction(int32 const action) override
        {
            if (GetDifficultyID() != DIFFICULTY_NORMAL)
            {
                events.ScheduleEvent(EVENT_RUPTURING_SKITTER, 4000);
                events.ScheduleEvent(EVENT_CRYSTALLINE_GROUND, 4000);
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (CheckHomeDistToEvade(diff, 45.0f))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_RAZOR_SHARDS:
                        DoCast(me, SPELL_RAZOR_SHARDS_CALL, true);
                        if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                        {
                            Position pos;
                            me->GetNearPosition(pos, 60.0f, me->GetRelativeAngle(target));
                            if (Creature* summon = me->SummonCreature(105811, pos, TEMPSUMMON_TIMED_DESPAWN, 10000))
                            {
                                summon->StopAttack();
                                DoCast(summon, SPELL_RAZOR_SHARDS);
                            }
                        }
                        Talk(SAY_RAZOR);
                        events.ScheduleEvent(EVENT_RAZOR_SHARDS, 26000);
                        break;
                    case EVENT_RUPTURING_SKITTER:
                        DoCast(me, SPELL_RUPTURING_SKITTER, true);
                        break;
                    case EVENT_CRYSTALLINE_GROUND:
                        //Talk();
                        DoCast(SPELL_CRYSTALLINE_GROUND);
                        break;
                    case EVENT_DEAD_CONVERSATION:
                        //DoCast(199392); пока офф, крашит
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_rokmoraAI (creature);
    }
};

//97720
class npc_rokmora_blightshard_skitter : public CreatureScript
{
public:
    npc_rokmora_blightshard_skitter() : CreatureScript("npc_rokmora_blightshard_skitter") {}

    struct npc_rokmora_blightshard_skitterAI : public ScriptedAI
    {
        npc_rokmora_blightshard_skitterAI(Creature* creature) : ScriptedAI(creature) {}

        void Reset() override {}

        void JustDied(Unit* /*killer*/) override
        {
            if (Unit* owner = me->GetAnyOwner())
                me->CastSpell(me, SPELL_CHOKING_DUST_AT, true, nullptr, nullptr, owner->GetGUID());
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_rokmora_blightshard_skitterAI(creature);
    }
};

//193245
class spell_rokmora_gain_energy : public SpellScriptLoader
{
public:
    spell_rokmora_gain_energy() : SpellScriptLoader("spell_rokmora_gain_energy") { }

    class spell_rokmora_gain_energy_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_rokmora_gain_energy_AuraScript);

        bool checkTalk = false;

        void OnTick(AuraEffect const* aurEff)
        {
            Creature* caster = GetCaster()->ToCreature();
            if (!caster)
                return;

            if (caster->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (caster->GetPower(POWER_MANA) == 0 && checkTalk)
            {
                checkTalk = false;
                uint8 rand = urand(0, 1);
                caster->CastSpell(caster, rand ? SPELL_SHATTER_END_CALL_1 : SPELL_SHATTER_END_CALL_2, true);
            }

            if (caster->GetPower(POWER_MANA) >= 100)
            {
                caster->CastSpell(caster, SPELL_SHATTER_START_CALL_1, true);
                caster->CastSpell(caster, SPELL_SHATTER);
                caster->AI()->DoAction(true);
                checkTalk = true;
            }
        }

        void Register() override
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_rokmora_gain_energy_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_ENERGIZE);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_rokmora_gain_energy_AuraScript();
    }
};

//215898
class spell_rokmora_crystalline_ground : public SpellScriptLoader
{
    public:
        spell_rokmora_crystalline_ground() : SpellScriptLoader("spell_rokmora_crystalline_ground") { }

        class spell_rokmora_crystalline_ground_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rokmora_crystalline_ground_AuraScript);

            void OnPeriodic(AuraEffect const* aurEff)
            {
                Unit* target = GetTarget();
                if (!target)
                    return;

                if (target->isMoving())
                    target->CastSpell(target, SPELL_CRYSTALLINE_GROUND_DMG, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_rokmora_crystalline_ground_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rokmora_crystalline_ground_AuraScript();
        }
};

// trash 183433
class spell_submerge : public SpellScriptLoader
{
    public:
        spell_submerge() : SpellScriptLoader("spell_submerge") { }

        class spell_submerge_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_submerge_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();
                std::list<Player*> targets;
                GetPlayerListInGrid(targets, target, 40);
                Trinity::Containers::RandomResizeList(targets, 1);
                for (std::list<Player*>::iterator itr = targets.begin(); itr != targets.end(); ++itr)
                    if (!targets.empty())
                    {
                        target->CastSpell((*itr), 183438);
                        target->CastSpell((*itr), 183430, true);
                    }
                    target->SetVisible(true);          
            }

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* target = GetTarget())
                    target->SetVisible(false);      
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_submerge_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
                OnEffectApply += AuraEffectApplyFn(spell_submerge_AuraScript::OnApply, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_submerge_AuraScript();
        }
};

//209888
class spell_entrance_run_plr_move : public SpellScriptLoader
{
    public:
        spell_entrance_run_plr_move() : SpellScriptLoader("spell_entrance_run_plr_move") {}

        class spell_entrance_run_plr_move_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_entrance_run_plr_move_AuraScript);

            void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (Player* player = GetTarget()->ToPlayer())
                {
                    player->GetMotionMaster()->MoveIdle();
                    player->GetMotionMaster()->MovePath(20988800, false);
                }
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_entrance_run_plr_move_AuraScript::OnApply, EFFECT_2, SPELL_AURA_MOD_NO_ACTIONS, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_entrance_run_plr_move_AuraScript();
        }
};

void AddSC_boss_rokmora()
{
    new boss_rokmora();
    new npc_rokmora_blightshard_skitter();
    new spell_rokmora_gain_energy();
    new spell_rokmora_crystalline_ground();
    new spell_submerge();
    new spell_entrance_run_plr_move();
}