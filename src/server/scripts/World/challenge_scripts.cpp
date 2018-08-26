/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
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

#include "ScriptMgr.h"
#include "ChallengeMgr.h"
#include "ScenarioMgr.h"
#include "GameTables.h"

/// 206150 - Challenger's Might
class spell_challengers_might : public SpellScriptLoader
{
public:
    spell_challengers_might() : SpellScriptLoader("spell_challengers_might") { }

    class spell_challengers_might_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_challengers_might_AuraScript);

        uint8 volcanicTimer = 0;
        uint32 felExplosivesTimer = 0;
        uint32 necroticProcDelay = 0;

        void CalculateAmount(AuraEffect const* aurEff, float& amount, bool& /*canBeRecalculated*/)
        {
            Unit* caster = GetCaster();

            Scenario* progress = sScenarioMgr->GetScenario(caster->GetInstanceId());
            if (!progress)
                return;

            auto const& challenge = progress->GetChallenge();
            if (!challenge)
                return;

            uint32 challengeLevel = challenge->GetChallengeLevel();
            GtChallengeModeHealthEntry const* gtHealth = sChallengeModeHealthTable.GetRow(challengeLevel);
            GtChallengeModeDamageEntry const* gtDamage = sChallengeModeDamageTable.GetRow(challengeLevel);
            if (!gtHealth || !gtDamage)
                return;

            float modHealth = (gtHealth->Scalar - 1.0f) * 100.0f;
            float modDamage = (gtDamage->Scalar - 1.0f) * 100.0f;

            bool isDungeonBoss = false;
            if (Creature* creature = caster->ToCreature())
                if (creature->IsDungeonBoss())
                    isDungeonBoss = true;

            if (isDungeonBoss)
            {
                if (challenge->HasAffix(Affixes::Tyrannical))
                {
                    modHealth *= 1.4f;
                    modDamage *= 1.15f;
                }
            }
            else if (challenge->HasAffix(Affixes::Fortified))
            {
                modHealth *= 1.2f;
                modDamage *= 1.3f;
            }

            switch (aurEff->GetEffIndex())
            {
                case EFFECT_0:
                    amount = modHealth;
                    break;
                case EFFECT_1:
                    amount = modDamage;
                    break;
                case EFFECT_2:
                    amount = (challenge->HasAffix(Affixes::Raging) && !isDungeonBoss) ? 1 : 0;
                    break;
                case EFFECT_3:
                    amount = challenge->HasAffix(Affixes::Bolstering) ? 1 : 0;
                    break;
                case EFFECT_4:
                    amount = challenge->HasAffix(Affixes::Tyrannical) && isDungeonBoss ? 1 : 0;
                    break;
                case EFFECT_7:
                    amount = challenge->HasAffix(Affixes::Volcanic) ? 1 : 0;
                    break;
                case EFFECT_8:
                    amount = challenge->HasAffix(Affixes::Necrotic) ? 1 : 0;
                    break;
                case EFFECT_9:
                    amount = challenge->HasAffix(Affixes::Fortified) && !isDungeonBoss ? 1 : 0;
                    break;
                case EFFECT_10:
                    amount = challenge->HasAffix(Affixes::Sanguine) ? 1 : 0;
                    break;
                case EFFECT_11:
                    amount = challenge->HasAffix(Affixes::Quaking) ? 1 : 0;
                    break;
                case EFFECT_12:
                    amount = challenge->HasAffix(Affixes::FelExplosives) ? 1 : 0;
                    felExplosivesTimer = urandms(2, 10);
                    break;
                case EFFECT_13:
                    amount = challenge->HasAffix(Affixes::Bursting) ? 1 : 0;
                    break;
                default:
                    break;
            }
        }

        // Volcanic
        void OnTick(AuraEffect const* aurEff)
        {
            if (!aurEff->GetAmount() || !GetCaster()->isInCombat())
                return;

            if (volcanicTimer == 7)
                volcanicTimer = 0;
            else
            {
                ++volcanicTimer;
                return;
            }

            auto caster = GetCaster()->ToCreature();
            if (!caster || caster->IsAffixDisabled() || (caster->AI() && caster->AI()->IsInControl()) || !caster->IsHostileToPlayers())
                return;

            if (auto owner = caster->GetAnyOwner())
            {
                if (owner->IsPlayer())
                    return;

                if (owner->IsCreature() && !owner->ToCreature()->IsDungeonBoss())
                    return;
            }

            auto _map = caster->GetMap();
            if (!_map)
                return;

            Map::PlayerList const& players = _map->GetPlayers();
            for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
            {
                if (auto player = itr->getSource())
                {
                    if (player->isInCombat() && player->IsValidAttackTarget(caster))
                        if (caster->GetDistance(player) > 15.0f && caster->GetDistance(player) < 60.0f) // Offlike 10m
                            caster->CastSpell(player, ChallengerSummonVolcanicPlume, true);
                }
            }
        }

        // Necrotic
        void OnProc(AuraEffect const* /*auraEffect*/, ProcEventInfo& eventInfo)
        {
            if (necroticProcDelay)
                PreventDefaultAction();
            else
                necroticProcDelay = 1000;
        }

        // Fel Explosives
        void OnUpdate(uint32 diff, AuraEffect* aurEff)
        {
            if (necroticProcDelay)
            {
                if (necroticProcDelay <= diff)
                    necroticProcDelay = 0;
                else
                    necroticProcDelay -= diff;
            }

            if (!aurEff->GetAmount() || aurEff->GetEffIndex() != EFFECT_12 || !GetCaster()->isInCombat())
                return;

            if (felExplosivesTimer <= diff)
                felExplosivesTimer = urandms(8, 16);
            else
            {
                felExplosivesTimer -= diff;
                return;
            }

            auto caster = GetCaster()->ToCreature();
            if (!caster || caster->IsAffixDisabled() || (caster->AI() && caster->AI()->IsInControl()) || !caster->IsHostileToPlayers())
                return;

            if (auto owner = caster->GetAnyOwner())
            {
                if (owner->IsPlayer())
                    return;

                if (owner->IsCreature() && !owner->ToCreature()->IsDungeonBoss())
                    return;
            }

            auto _map = caster->GetMap();
            if (!_map)
                return;

            Map::PlayerList const& players = _map->GetPlayers();
            for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
            {
                if (auto player = itr->getSource())
                {
                    if (player->isInCombat() && player->IsValidAttackTarget(caster))
                        if (caster->GetDistance(player) < 60.0f)
                        {
                            caster->CastSpell(caster, SPELL_FEL_EXPLOSIVES_SUMMON_2, true);
                            return;
                        }
                }
            }
        }

        void Register() override
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_challengers_might_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_challengers_might_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_challengers_might_AuraScript::CalculateAmount, EFFECT_2, SPELL_AURA_DUMMY);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_challengers_might_AuraScript::CalculateAmount, EFFECT_3, SPELL_AURA_DUMMY);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_challengers_might_AuraScript::CalculateAmount, EFFECT_4, SPELL_AURA_DUMMY);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_challengers_might_AuraScript::CalculateAmount, EFFECT_7, SPELL_AURA_PERIODIC_DUMMY);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_challengers_might_AuraScript::CalculateAmount, EFFECT_8, SPELL_AURA_DUMMY);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_challengers_might_AuraScript::CalculateAmount, EFFECT_9, SPELL_AURA_DUMMY);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_challengers_might_AuraScript::CalculateAmount, EFFECT_10, SPELL_AURA_DUMMY);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_challengers_might_AuraScript::CalculateAmount, EFFECT_11, SPELL_AURA_DUMMY);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_challengers_might_AuraScript::CalculateAmount, EFFECT_12, SPELL_AURA_DUMMY);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_challengers_might_AuraScript::CalculateAmount, EFFECT_13, SPELL_AURA_DUMMY);
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_challengers_might_AuraScript::OnTick, EFFECT_7, SPELL_AURA_PERIODIC_DUMMY);
            OnEffectProc += AuraEffectProcFn(spell_challengers_might_AuraScript::OnProc, EFFECT_8, SPELL_AURA_DUMMY);
            OnEffectUpdate += AuraEffectUpdateFn(spell_challengers_might_AuraScript::OnUpdate, EFFECT_11, SPELL_AURA_DUMMY);
            OnEffectUpdate += AuraEffectUpdateFn(spell_challengers_might_AuraScript::OnUpdate, EFFECT_12, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_challengers_might_AuraScript();
    }
};

/// 206151 - Challenger's Burden
class spell_challengers_burden : public SpellScriptLoader
{
public:
    spell_challengers_burden() : SpellScriptLoader("spell_challengers_burden") { }

    class spell_challengers_burden_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_challengers_burden_AuraScript);

        void CalculateAmount(AuraEffect const* aurEff, float& amount, bool& /*canBeRecalculated*/)
        {
            Unit* caster = GetCaster();

            Player* _player = caster->ToPlayer();
            if (!_player)
                return;

            Scenario* progress = sScenarioMgr->GetScenario(caster->GetInstanceId());
            if (!progress)
                return;

            auto const& challenge = progress->GetChallenge();
            if (!challenge)
                return;

            switch (aurEff->GetEffIndex())
            {
                case EFFECT_1:
                    if (!challenge->HasAffix(Affixes::Overflowing))
                        amount = 0;
                    break;
                case EFFECT_2:
                    if (!challenge->HasAffix(Affixes::Skittish) || !_player->isInTankSpec())
                        amount = 0;
                    break;
                default:
                    break;
            }
        }

        void Register() override
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_challengers_burden_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_DUMMY);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_challengers_burden_AuraScript::CalculateAmount, EFFECT_2, SPELL_AURA_MOD_THREAT);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_challengers_burden_AuraScript();
    }
};

// 105877 - Volcanic Plume
class npc_challenger_volcanic_plume : public CreatureScript
{
public:
    npc_challenger_volcanic_plume() : CreatureScript("npc_challenger_volcanic_plume") {}

    struct npc_challenger_volcanic_plumeAI : ScriptedAI
    {
        npc_challenger_volcanic_plumeAI(Creature* creature) : ScriptedAI(creature) {}

        EventMap events;

        void Reset() override
        {
            events.Reset();
            events.ScheduleEvent(EVENT_1, 250);
        }

        void UpdateAI(uint32 diff) override
        {
            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_1:
                        me->CastSpell(me, ChallengerVolcanicPlume, false);
                        break;
                    default:
                        break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_challenger_volcanic_plumeAI(creature);
    }
};

// 120651 - Fel Explosives
class npc_challenger_fel_explosives : public CreatureScript
{
public:
    npc_challenger_fel_explosives() : CreatureScript("npc_challenger_fel_explosives") {}

    struct npc_challenger_fel_explosivesAI : ScriptedAI
    {
        npc_challenger_fel_explosivesAI(Creature* creature) : ScriptedAI(creature)
        {
            me->AddUnitTypeMask(UNIT_MASK_TOTEM);
            me->SetReactState(REACT_PASSIVE);
        }

        EventMap events;

        void Reset() override {}

        void IsSummonedBy(Unit* summoner) override
        {
            DoCast(me, SPELL_FEL_EXPLOSIVES_VISUAL, true);
            events.ScheduleEvent(EVENT_1, 500);
        }

        void SpellFinishCast(SpellInfo const* spellInfo) override
        {
            if (spellInfo->Id == SPELL_FEL_EXPLOSIVES_DMG)
                me->DespawnOrUnsummon(100);
        }

        void UpdateAI(uint32 diff) override
        {
            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_1:
                        DoCast(SPELL_FEL_EXPLOSIVES_DMG);
                        break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_challenger_fel_explosivesAI(creature);
    }
};

class item_challenge_key : public ItemScript
{
public:
    item_challenge_key() : ItemScript("item_challenge_key") { }

    bool OnCreate(Player* player, Item* item) override
    {
        if (player->HasItemCount(138019, 1, true))
            player->InitChallengeKey(item);
        else
            player->CreateChallengeKey(item);
        return false;
    }
};

// 209859 - Bolster
class spell_challengers_bolster : public SpellScriptLoader
{
    public:
        spell_challengers_bolster() : SpellScriptLoader("spell_challengers_bolster") { }

        class spell_challengers_bolster_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_challengers_bolster_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (targets.empty())
                    return;

                std::list<WorldObject*> temp;
                for (auto object : targets)
                {
                    if (auto unit = object->ToUnit())
                    {
                        if (!unit->isInCombat() || unit->IsPlayer())
                            continue;

                        auto owner = unit->GetAnyOwner();
                        if (owner && owner->IsPlayer())
                            continue;

                        if (Creature* creature = unit->ToCreature())
                            if (creature->IsDungeonBoss())
                                continue;

                        temp.push_back(object);
                    }
                }
                targets = temp;
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_challengers_bolster_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_challengers_bolster_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ALLY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_challengers_bolster_SpellScript::FilterTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ALLY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_challengers_bolster_SpellScript::FilterTargets, EFFECT_3, TARGET_UNIT_SRC_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_challengers_bolster_SpellScript();
        }
};

//240443
class spell_challengers_burst : public SpellScriptLoader
{
    public:
        spell_challengers_burst() : SpellScriptLoader("spell_challengers_burst") {}

        class spell_challengers_burst_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_challengers_burst_AuraScript);

            void OnTick(AuraEffect const* /*aurEff*/)
            {
                Unit* target = GetTarget();
                if (!target)
                    return;

                if (SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(243237))
                {
                    float bp = spellInfo->GetEffect(EFFECT_0)->BasePoints * target->GetAuraCount(GetId());
                    target->CastCustomSpell(target, 243237, &bp, NULL, NULL, true);
                }

            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_challengers_burst_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_challengers_burst_AuraScript();
        }
};

void AddSC_challenge_scripts()
{
    new spell_challengers_might();
    new spell_challengers_burden();
    new npc_challenger_volcanic_plume();
    new npc_challenger_fel_explosives();
    new item_challenge_key();
    new spell_challengers_bolster();
    new spell_challengers_burst();
}
