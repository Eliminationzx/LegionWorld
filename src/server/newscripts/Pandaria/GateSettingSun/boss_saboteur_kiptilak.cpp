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

#include "gate_setting_sun.h"

enum eSpells
{
    SPELL_PLANT_EXPLOSIVE               = 107187,

    SPELL_SABOTAGE                      = 107268,
    SPELL_SABOTAGE_EXPLOSION            = 113645,
    
    SPELL_PLAYER_EXPLOSION              = 113654,

    SPELL_MUNITION_STABLE               = 109987,
    SPELL_MUNITION_EXPLOSION            = 107153,
    SPELL_MUNITION_EXPLOSION_AURA       = 120551,
    
    SPELL_ACHIEV_CREDIT                 = 119342,
};

enum eEvents
{
    EVENT_EXPLOSIVES        = 1,
    EVENT_SABOTAGE          = 2
};

enum eWorldInFlames
{
    WIF_NONE    = 0,
    WIF_70      = 1,
    WIF_30      = 2
};

class boss_saboteur_kiptilak : public CreatureScript
{
    public:
        boss_saboteur_kiptilak() : CreatureScript("boss_saboteur_kiptilak") {}

        struct boss_saboteur_kiptilakAI : public BossAI
        {
            boss_saboteur_kiptilakAI(Creature* creature) : BossAI(creature, DATA_KIPTILAK)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            uint8 WorldInFlamesEvents;

            void Reset() override
            {
                _Reset();
                
                events.ScheduleEvent(EVENT_EXPLOSIVES, urand(7500,  10000));
                events.ScheduleEvent(EVENT_SABOTAGE,   urand(22500, 30000));

                WorldInFlamesEvents = 0;
            }

            void EnterCombat(Unit* /*who*/) override
            {
                _EnterCombat();
            }

            void JustReachedHome() override
            {
                instance->SetBossState(DATA_KIPTILAK, FAIL);
                summons.DespawnAll();
            }

            void DamageTaken(Unit* attacker, uint32 &damage, DamageEffectType dmgType) override
            {
                if (attacker->GetTypeId() != TYPEID_PLAYER)
                {
                    damage = 0; 
                    return;
                }

                float nextHealthPct = ((float(me->GetHealth()) - damage)  / float(me->GetMaxHealth())) * 100;

                if (WorldInFlamesEvents < WIF_70 && nextHealthPct <= 70.0f)
                {
                    DoWorldInFlamesEvent();
                    ++WorldInFlamesEvents;
                }
                else if (WorldInFlamesEvents < WIF_30 && nextHealthPct <= 30.0f)
                {
                    DoWorldInFlamesEvent();
                    ++WorldInFlamesEvents;
                }
            }

            void DoWorldInFlamesEvent()
            {
                std::list<Creature*> munitionList;
                GetCreatureListWithEntryInGrid(munitionList, me, NPC_STABLE_MUNITION, 100.0f);

                for (std::list<Creature*>::iterator itr = munitionList.begin(); itr != munitionList.end(); ++itr)
                {
                    (*itr)->RemoveAurasDueToSpell(SPELL_MUNITION_STABLE);
                    (*itr)->CastSpell((*itr), SPELL_MUNITION_EXPLOSION, true);
                    (*itr)->DespawnOrUnsummon(2000);
                }
            }

            void JustSummoned(Creature* summoned) override
            {
                if (summoned->GetEntry() == NPC_STABLE_MUNITION)
                    summoned->AddAura(SPELL_MUNITION_STABLE, summoned);

                summoned->SetReactState(REACT_PASSIVE);

                summons.Summon(summoned);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                switch(events.ExecuteEvent())
                {
                    case EVENT_EXPLOSIVES:
                        for (uint8 i = 0; i < urand(1, 3); ++i)
                            me->CastSpell(frand(702, 740), frand(2292, 2320), 388.5f, SPELL_PLANT_EXPLOSIVE, true);
                        events.ScheduleEvent(EVENT_EXPLOSIVES, urand(7500, 12500));
                        break;
                    case EVENT_SABOTAGE:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true))
                            me->CastSpell(target, SPELL_SABOTAGE, true);
                        events.ScheduleEvent(EVENT_SABOTAGE, urand(22500, 30000));
                        break;
                    default:
                        break;
                }
                DoMeleeAttackIfReady();
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_saboteur_kiptilakAI(creature);
        }
};

class npc_munition_explosion_bunny : public CreatureScript
{
public:
    npc_munition_explosion_bunny() : CreatureScript("npc_munition_explosion_bunny") { }

    struct npc_munition_explosion_bunnyAI : public ScriptedAI
    {
        npc_munition_explosion_bunnyAI(Creature* creature) : ScriptedAI(creature) 
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
        float orientation;
        uint32 checkTimer;

        void Reset() override
        {
            me->SetReactState(REACT_PASSIVE);
            orientation = 0.0f;
            checkTimer = 1000;

            switch (me->GetEntry())
            {
                case NPC_EXPLOSION_BUNNY_N_M:
                case NPC_EXPLOSION_BUNNY_N_P:
                    orientation = 0.0f;
                    break;
                case NPC_EXPLOSION_BUNNY_S_M:
                case NPC_EXPLOSION_BUNNY_S_P:
                    orientation = M_PI;
                    break;
                case NPC_EXPLOSION_BUNNY_E_M:
                case NPC_EXPLOSION_BUNNY_E_P:
                    orientation = 4.71f;
                    break;
                case NPC_EXPLOSION_BUNNY_W_M:
                case NPC_EXPLOSION_BUNNY_W_P:
                    orientation = 1.57f;
                    break;
            }

            float x = 0.0f;
            float y = 0.0f;
            GetPositionWithDistInOrientation(me, 40.0f, orientation, x, y);
            me->GetMotionMaster()->MovePoint(1, x, y, me->GetPositionZ());

            me->AddAura(SPELL_MUNITION_EXPLOSION_AURA, me);
            instance->DoUpdateAchievementCriteria(CRITERIA_TYPE_BE_SPELL_TARGET2, SPELL_ACHIEV_CREDIT, 0, 0, me);
        }

        void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType dmgType) override
        {
            damage = 0;
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (id == 1)
                me->DespawnOrUnsummon();
        }

        void EnterCombat(Unit* /*who*/) override
        {
            return;
        }
        
        void UpdateAI(uint32 diff) override
        {
            if (checkTimer <= diff)
            {
                checkTimer = 500;
                if (Creature* munition = GetClosestCreatureWithEntry(me, NPC_STABLE_MUNITION, 2.0f, true))
                {
                    if (munition->HasAura(SPELL_MUNITION_STABLE))
                    {
                        munition->RemoveAurasDueToSpell(SPELL_MUNITION_STABLE);
                        munition->CastSpell(munition, SPELL_MUNITION_EXPLOSION, true);
                        munition->DespawnOrUnsummon(2000);
                    }
                }
            }
            else checkTimer -= diff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_munition_explosion_bunnyAI (creature);
    }
};

class CheckMunitionExplosionPredicate
{
    public:
        CheckMunitionExplosionPredicate(Unit* caster) : _caster(caster) {}

        bool operator()(WorldObject* target)
        {
            if (!_caster || !target)
                return true;

            if (!_caster->ToTempSummon())
                return true;

            Unit* creator = _caster->ToTempSummon()->GetSummoner();

            if (!creator || creator == target)
                return true;

            return false;
        }

    private:
        Unit* _caster;
};

class spell_kiptilak_munitions_explosion : public SpellScriptLoader
{
    public:
        spell_kiptilak_munitions_explosion() : SpellScriptLoader("spell_kiptilak_munitions_explosion") { }

        class spell_kiptilak_munitions_explosion_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_kiptilak_munitions_explosion_SpellScript);

            void FilterTargets(std::list<WorldObject*>& unitList)
            {
                if (Unit* caster = GetCaster())
                    unitList.remove_if(CheckMunitionExplosionPredicate(caster));
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_kiptilak_munitions_explosion_SpellScript::FilterTargets, EFFECT_0, TARGET_SRC_CASTER);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_kiptilak_munitions_explosion_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_kiptilak_munitions_explosion_SpellScript();
        }
};

class spell_kiptilak_sabotage : public SpellScriptLoader
{
    public:
        spell_kiptilak_sabotage() :  SpellScriptLoader("spell_kiptilak_sabotage") { }

        class spell_kiptilak_sabotage_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_kiptilak_sabotage_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();

                if (!target)
                    return;

                target->CastSpell(target, SPELL_PLAYER_EXPLOSION, true);
                target->CastSpell(target, SPELL_SABOTAGE_EXPLOSION, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_kiptilak_sabotage_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_kiptilak_sabotage_AuraScript();
        }
};

class npc_rope : public CreatureScript
{
    public:
        npc_rope() : CreatureScript("npc_rope") {}

        struct npc_ropeAI : public CreatureAI
        {
            npc_ropeAI(Creature* creature) : CreatureAI(creature)
            {
                instance = creature->GetInstanceScript();
                me->SetDisplayId(43636);
            }

            InstanceScript* instance;

            void Reset() override {}
            
            void OnSpellClick(Unit* clicker) override
            {
                if (instance)
                {
                   if (clicker->GetTypeId() == TYPEID_PLAYER)
                       clicker->NearTeleportTo(866.290f, 2299.666f, 296.1068f, 6.216f);
                }
            }
            
            void EnterEvadeMode() override {}

            void EnterCombat(Unit* who) override {}

            void UpdateAI(uint32 diff) override
            {
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_ropeAI(creature);
        }
};

void AddSC_boss_saboteur_kiptilak()
{
    new boss_saboteur_kiptilak();
    new npc_munition_explosion_bunny();
    new spell_kiptilak_munitions_explosion();
    new spell_kiptilak_sabotage();
    new npc_rope();
}
