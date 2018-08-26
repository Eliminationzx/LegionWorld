/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "shadopan_monastery.h"

enum eSpells
{
    // Gu
    SPELL_KILL_GUARDIANS            = 114927,

    SPELL_INVOKE_LIGHTNING          = 106984,
    SPELL_CHARGING_SOUL             = 110945,
    
    SPELL_OVERCHARGED_SOUL          = 110852,
    SPELL_OVERCHARGED_SOUL_DAMAGE   = 111129,

    // Azure Serpent
    SPELL_LIGHTNING_SHIELD          = 123496,
    SPELL_STATIC_FIELD              = 106923,

    SPELL_LIGHTNING_BREATH          = 102573,
    SPELL_MAGNETIC_SHROUD           = 107140, // TODO
};

enum eEvents
{
    // Gu
    EVENT_INVOKE_LIGHTNING  = 1,
    EVENT_OVERCHARGED_SOUL  = 2,

    // Azure Serpent
    EVENT_STATIC_FIELD      = 3,
    EVENT_LIGHTNING_BREATH  = 4,
    EVENT_MAGNETIC_SHROUD   = 5,
};

enum eActions
{
    // Gu
    ACTION_INTRO                = 1,
    ACTION_GU_P_3               = 2,
    
    // Azure Serpent
    ACTION_AZURE_SERPENT_P_1    = 3,
    ACTION_AZURE_SERPENT_P_2    = 4,
    ACTION_AZURE_SERPENT_RESET  = 5,
};

enum ePhases
{
    PHASE_ONE   = 1,
    PHASE_TWO   = 2,
    PHASE_THREE = 3
};

class boss_gu_cloudstrike : public CreatureScript
{
    public:
        boss_gu_cloudstrike() : CreatureScript("boss_gu_cloudstrike") {}

        struct boss_gu_cloudstrikeAI : public BossAI
        {
            boss_gu_cloudstrikeAI(Creature* creature) : BossAI(creature, DATA_GU_CLOUDSTRIKE)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            uint8 phase;

            void Reset() override
            {
                _Reset();
                phase = 1;
                me->SetReactState(REACT_AGGRESSIVE);
                me->RemoveAurasDueToSpell(SPELL_CHARGING_SOUL);
            }

            Creature* GetAzureSerpent()
            {
                return instance->instance->GetCreature(instance->GetGuidData(NPC_AZURE_SERPENT));
            }

            void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType dmgType) override
            {
                if (phase == 1 && me->HealthBelowPctDamaged(50, damage))
                {
                    phase = 2;
                    events.CancelEventGroup(PHASE_ONE);
                    events.ScheduleEvent(EVENT_OVERCHARGED_SOUL, 2500, PHASE_TWO);
                    me->CastSpell(me, SPELL_CHARGING_SOUL, false);
                    if (Creature* azureSerpent = GetAzureSerpent())
                        if (azureSerpent && azureSerpent->isAlive())
                            azureSerpent->AI()->DoAction(ACTION_AZURE_SERPENT_P_2);
                }
            }

            void JustDied(Unit* killer) override
            {
                _JustDied();
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MAGNETIC_SHROUD);
            }

            void DoAction(const int32 action) override
            {
                if (action == ACTION_GU_P_3)
                {
                    me->CastSpell(me, SPELL_OVERCHARGED_SOUL, true);
                    me->RemoveAurasDueToSpell(SPELL_CHARGING_SOUL);
                }
            }

            void JustSummoned(Creature* summoned) override
            {
                summons.Summon(summoned);
            }

            void EnterCombat(Unit* who) override
            {
                events.ScheduleEvent(EVENT_INVOKE_LIGHTNING, urand(5000, 10000));
                if (Creature* azureSerpent = me->SummonCreature(56754, 3726.13f, 2677.06f, 773.01f, 0, TEMPSUMMON_CORPSE_DESPAWN))
                {
                    azureSerpent->SetInCombatWithZone();
                    azureSerpent->SetFacingTo(3.625f);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                switch(events.ExecuteEvent())
                {
                    case EVENT_INVOKE_LIGHTNING:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
                            me->CastSpell(target, SPELL_INVOKE_LIGHTNING, false);
                        events.ScheduleEvent(EVENT_INVOKE_LIGHTNING, urand(5000, 10000));
                        break;
                    case EVENT_OVERCHARGED_SOUL:
                        me->CastSpell(me, SPELL_OVERCHARGED_SOUL_DAMAGE, false);
                        events.ScheduleEvent(EVENT_OVERCHARGED_SOUL, 2500);
                        break;
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_gu_cloudstrikeAI(creature);
        }
};

Position azureSerpentPositions[4] =
{
    {3835.01f, 2906.63f, 753.33f},
    {3850.37f, 2738.14f, 814.84f},
    {3758.79f, 2692.08f, 778.60f},
    {3736.37f, 2680.89f, 778.60f}
};

class npc_azure_serpent : public CreatureScript
{
    public:
        npc_azure_serpent() :  CreatureScript("npc_azure_serpent") { }

        struct npc_azure_serpentAI : public ScriptedAI
        {
            npc_azure_serpentAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
                me->SetCanFly(true);
                me->SetDisableGravity(true);
            }

            InstanceScript* instance;
            EventMap events;
            uint8 phase;

            void Reset() override
            {
                phase = 1;
                me->setActive(true);
                me->SetReactState(REACT_PASSIVE);
                events.Reset();
                me->AddAura(SPELL_LIGHTNING_SHIELD, me);
                me->SetSpeed(MOVE_FLIGHT, 5.0f);
                DoZoneInCombat();
                events.ScheduleEvent(EVENT_STATIC_FIELD, 5000);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (me->ToTempSummon())
                    if (Creature* gu = me->ToTempSummon()->GetSummoner()->ToCreature())
                        gu->AI()->DoAction(ACTION_GU_P_3);
            }

            void DoAction(const int32 action) override
            {
                switch (action)
                {
                    case ACTION_AZURE_SERPENT_P_2:
                        events.CancelEventGroup(PHASE_ONE);
                        events.ScheduleEvent(EVENT_MAGNETIC_SHROUD, urand(10000, 15000), PHASE_TWO);
                        events.ScheduleEvent(EVENT_LIGHTNING_BREATH, urand (2500, 7500), PHASE_TWO);
                        me->RemoveAurasDueToSpell(SPELL_LIGHTNING_SHIELD);
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                switch(events.ExecuteEvent())
                {
                    case EVENT_STATIC_FIELD:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
                            me->CastSpell(target, SPELL_STATIC_FIELD, false);
                        events.ScheduleEvent(EVENT_STATIC_FIELD, 10000);
                        break;
                    case EVENT_MAGNETIC_SHROUD:
                        me->CastSpell(me, SPELL_MAGNETIC_SHROUD, false);
                        events.ScheduleEvent(EVENT_MAGNETIC_SHROUD, urand(10000, 15000));
                        break;
                    case EVENT_LIGHTNING_BREATH:
                        if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO, 0, 0, true))
                            me->CastSpell(target, SPELL_LIGHTNING_BREATH, false);
                        events.ScheduleEvent(EVENT_LIGHTNING_BREATH, urand(7500, 12500));
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_azure_serpentAI(creature);
        }
};

class AreaTrigger_at_gu_intro : public AreaTriggerScript
{
    public:
        AreaTrigger_at_gu_intro() : AreaTriggerScript("at_gu_intro") {}

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/, bool apply) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
                if (Creature* gu = instance->instance->GetCreature(instance->GetGuidData(NPC_GU_CLOUDSTRIKE)))
                    if (gu->AI())
                        gu->AI()->DoAction(ACTION_INTRO);

            return false;
        }
};

class OnlyGuardianPredicate
{
    public:
        OnlyGuardianPredicate() {}

        bool operator()(WorldObject* target)
        {
            return target->GetEntry() != NPC_GUARDIAN;
        }
};

class spell_kill_guardians : public SpellScriptLoader
{
    public:
        spell_kill_guardians() : SpellScriptLoader("spell_kill_guardians") { }

        class spell_kill_guardians_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_kill_guardians_SpellScript);

            bool Validate(SpellInfo const* spell) override
            {
                return true;
            }

            // set up initial variables and check if caster is creature
            // this will let use safely use ToCreature() casts in entire script
            bool Load() override
            {
                return true;
            }

            void SelectTarget(std::list<WorldObject*>& targetList)
            {
                targetList.remove_if(OnlyGuardianPredicate());
                _targetList = targetList;
            }

            void KillTarget(std::list<WorldObject*>& targetList)
            {
                targetList = _targetList;

                for (std::list<WorldObject*>::iterator itr = targetList.begin(); itr != targetList.end(); ++itr)
                    if (Creature* target = (*itr)->ToCreature())
                        target->DespawnOrUnsummon(2000);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_kill_guardians_SpellScript::SelectTarget, EFFECT_0, TARGET_SRC_CASTER);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_kill_guardians_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_kill_guardians_SpellScript::KillTarget, EFFECT_1, TARGET_SRC_CASTER);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_kill_guardians_SpellScript::KillTarget, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
            }

            std::list<WorldObject*> _targetList;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_kill_guardians_SpellScript();
        }
};

class spell_overcharged_soul_damage : public SpellScriptLoader
{
    public:
        spell_overcharged_soul_damage() : SpellScriptLoader("spell_overcharged_soul_damage") { }

        class spell_overcharged_soul_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_overcharged_soul_damage_SpellScript);

            void ChangeDamage(SpellEffIndex effIndex)
            {
                if (Unit* caster = GetCaster())
                    SetHitDamage(25000 / caster->GetHealthPct());
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_overcharged_soul_damage_SpellScript::ChangeDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_overcharged_soul_damage_SpellScript();
        }
};

void AddSC_boss_gu_cloudstrike()
{
    new boss_gu_cloudstrike();
    new npc_azure_serpent();
    new AreaTrigger_at_gu_intro();
    new spell_kill_guardians();
    new spell_overcharged_soul_damage();
}
