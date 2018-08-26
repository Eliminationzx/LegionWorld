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
    SPELL_CORRUPTED                 = 131530,

    SPELL_RISING_HATE               = 107356,
    SPELL_RING_OF_MALICE            = 131521,
    SPELL_SHA_BLAST                 = 114999,
    SPELL_SUMMON_GRIPPING_HATRED    = 115002,

    // Gripping Hatred
    SPELL_GRIP_OF_HATE              = 115010,
    SPELL_POOL_OF_SHADOWS           = 112929,
    
    SPELL_ACHIEV_CREDIT             = 123095,
};

enum eEvents
{
    EVENT_RISING_HATE               = 1,
    EVENT_RING_OF_MALICE            = 2,
    EVENT_SHA_BLAST                 = 3,
    EVENT_SUMMON_GRIPPING_HATRED    = 4,

    EVENT_GRIP_OF_HATE              = 5,
};

class boss_taran_zhu : public CreatureScript
{
    public:
        boss_taran_zhu() : CreatureScript("boss_taran_zhu") {}

        struct boss_taran_zhuAI : public BossAI
        {
            boss_taran_zhuAI(Creature* creature) : BossAI(creature, DATA_TARAN_ZHU)
            {
                instance = creature->GetInstanceScript();
                me->AddAura(SPELL_CORRUPTED, me);
            }

            InstanceScript* instance;

            void Reset() override
            {
                _Reset();
                summons.DespawnAll();
            }

            void EnterCombat(Unit*who) override
            {
                if (instance)
                    instance->SetBossState(DATA_TARAN_ZHU, IN_PROGRESS);
                events.ScheduleEvent(EVENT_RISING_HATE,             urand(25000, 35000));
                events.ScheduleEvent(EVENT_RING_OF_MALICE,          urand(7500,  12500));
                events.ScheduleEvent(EVENT_SHA_BLAST,               urand(2500,  5000));
                events.ScheduleEvent(EVENT_SUMMON_GRIPPING_HATRED,  urand(10000, 15000));
            }

            void DamageDealt(Unit* target, uint32& damage, DamageEffectType damageType) override
            {
                if (Player* player = target->ToPlayer())
                {
                    uint32 newPower = player->GetPower(POWER_ALTERNATE) + std::floor(damage / 1000.0f);
                    player->SetPower(POWER_ALTERNATE, newPower > 100 ? 100: newPower);
                }
            }

            void DamageTaken(Unit* who, uint32& damage, DamageEffectType dmgType) override
            {
                if (damage >= me->GetHealth())
                {
                    damage = 0;
                    if (instance)
                        instance->SetBossState(DATA_TARAN_ZHU, DONE);
                    me->setFaction(35);
                    me->SetFullHealth();
                    me->RemoveAurasDueToSpell(SPELL_CORRUPTED);
                    DoCast(SPELL_ACHIEV_CREDIT);
                    me->GetMap()->UpdateEncounterState(ENCOUNTER_CREDIT_CAST_SPELL, SPELL_ACHIEV_CREDIT, me, who);
                    if (checkaura() && me->GetMap()->GetDifficultyID() == DIFFICULTY_HEROIC)
                        instance->DoUpdateAchievementCriteria(CRITERIA_TYPE_BE_SPELL_TARGET, 124979);
                }
            }

            bool checkaura()
            {
                auto const& players = me->GetMap()->GetPlayers();
                if (!players.isEmpty())
                    for (auto const& itr : players)
                        if (Player* player = itr.getSource())
                        {
                            if (!player->HasAura(107217))
                                return false;
                        }
                return true;
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
                    case EVENT_RISING_HATE:
                        me->CastSpell(me, SPELL_RISING_HATE, false);
                        events.ScheduleEvent(EVENT_RISING_HATE, urand(10000, 15000));
                        break;
                    case EVENT_RING_OF_MALICE:
                        me->CastSpell(me, SPELL_RING_OF_MALICE, true);
                        events.ScheduleEvent(EVENT_SHA_BLAST,  urand(2000, 4000));
                        events.ScheduleEvent(EVENT_RING_OF_MALICE, urand(27500, 32500));
                        break;
                    case EVENT_SHA_BLAST:
                        if (!me->HasAura(SPELL_RING_OF_MALICE))
                            break;

                        if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO, 0, 0, true))
                            me->CastSpell(target, SPELL_SHA_BLAST, false);
                        
                        events.ScheduleEvent(EVENT_SHA_BLAST,  urand(2500, 5000));
                        break;
                    case EVENT_SUMMON_GRIPPING_HATRED:
                        me->CastSpell(me, SPELL_SUMMON_GRIPPING_HATRED, false);
                        events.ScheduleEvent(EVENT_SUMMON_GRIPPING_HATRED,  urand(20000, 30000));
                        break;
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_taran_zhuAI(creature);
        }
};

class npc_gripping_hatred : public CreatureScript
{
public:
    npc_gripping_hatred() : CreatureScript("npc_gripping_hatred") { }

    struct npc_gripping_hatredAI : public ScriptedAI
    {
        npc_gripping_hatredAI(Creature* creature) : ScriptedAI(creature) {}

        void Reset() override
        {
            me->SetReactState(REACT_PASSIVE);
            me->CastSpell(me, SPELL_POOL_OF_SHADOWS, true);
        }

        void UpdateAI(uint32 diff) override
        {
            if (!me->HasUnitState(UNIT_STATE_CASTING))
                me->CastSpell(me, SPELL_GRIP_OF_HATE, false);
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_gripping_hatredAI (creature);
    }
};

class spell_taran_zhu_hate : public SpellScriptLoader
{
    public:
        spell_taran_zhu_hate() : SpellScriptLoader("spell_taran_zhu_hate") { }

        class spell_taran_zhu_hate_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_taran_zhu_hate_AuraScript);

            void HandlePeriodic(AuraEffect const* /*aurEff*/)
            {
                if (Unit* target = GetTarget())
                    if (target->GetPower(POWER_ALTERNATE) >= 100)
                        if (!target->HasAura(SPELL_HAZE_OF_HATE))
                        {
                            target->CastSpell(target, SPELL_HAZE_OF_HATE, true);
                            target->CastSpell(target, SPELL_HAZE_OF_HATE_VISUAL, true);
                        }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_taran_zhu_hate_AuraScript::HandlePeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_taran_zhu_hate_AuraScript();
        }
};

class spell_taran_zhu_meditation : public SpellScriptLoader
{
    public:
        spell_taran_zhu_meditation() : SpellScriptLoader("spell_taran_zhu_meditation") { }

        class spell_taran_zhu_meditation_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_taran_zhu_meditation_AuraScript);

            void OnRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
                    if (Unit* target = GetTarget())
                    {
                        target->SetPower(POWER_ALTERNATE, 0);
                        target->RemoveAurasDueToSpell(SPELL_HAZE_OF_HATE);
                        target->RemoveAurasDueToSpell(SPELL_HAZE_OF_HATE_VISUAL);
                    }
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_taran_zhu_meditation_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_taran_zhu_meditation_AuraScript();
        }
};

class spell_taran_zhu_grip_of_hate : public SpellScriptLoader
{
    public:
        spell_taran_zhu_grip_of_hate() : SpellScriptLoader("spell_taran_zhu_grip_of_hate") { }

        class spell_taran_zhu_grip_of_hate_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_taran_zhu_grip_of_hate_SpellScript);

            void HandleScriptEffect(SpellEffIndex effIndex)
            {
                if (Unit* caster = GetCaster())
                    if (Unit* target = GetHitUnit())
                        target->CastSpell(caster, GetSpellInfo()->Effects[effIndex]->BasePoints, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_taran_zhu_grip_of_hate_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_taran_zhu_grip_of_hate_SpellScript();
        }
};

void AddSC_boss_taran_zhu()
{
    new boss_taran_zhu();
    new npc_gripping_hatred();
    new spell_taran_zhu_hate();
    new spell_taran_zhu_meditation();
    new spell_taran_zhu_grip_of_hate();
}
