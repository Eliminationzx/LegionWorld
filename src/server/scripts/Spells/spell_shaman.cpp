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

/*
 * Scripts for spells with SPELLFAMILY_SHAMAN and SPELLFAMILY_GENERIC spells used by shaman players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_sha_".
 */

#include "ScriptMgr.h"
#include "GridNotifiers.h"
#include "Unit.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"

// Spirit Link - 98020 : triggered by 98017
// Spirit Link Totem
class spell_sha_spirit_link : public SpellScriptLoader
{
    public:
        spell_sha_spirit_link() : SpellScriptLoader("spell_sha_spirit_link") { }

        class spell_sha_spirit_link_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_spirit_link_SpellScript);

            void RemoveInvalidTargets(std::list<WorldObject*>& targets)
            {
                std::list<WorldObject*> removeList;
                for (std::list<WorldObject*>::iterator itr = targets.begin(); itr != targets.end(); ++itr)
                    if ((*itr) && (*itr)->GetTypeId() != TYPEID_PLAYER)
                        removeList.push_back(*itr);

                for (std::list<WorldObject*>::iterator iter = removeList.begin(); iter != removeList.end(); ++iter)
                    targets.remove(*iter);
            }
            void HandleAfterCast()
            {
                if (Unit* caster = GetCaster())
                {
                    std::list<TargetInfo>* memberList = GetSpell()->GetUniqueTargetInfo();
                    if(memberList->empty())
                        return;

                    float totalRaidHealthPct = 0;
                    for (std::list<TargetInfo>::iterator ihit = memberList->begin(); ihit != memberList->end(); ++ihit)
                    {
                        if(Unit* member = ObjectAccessor::GetUnit(*caster, ihit->targetGUID))
                            totalRaidHealthPct += member->GetHealthPct();
                    }
                    totalRaidHealthPct /= memberList->size() * 100.0f;
                    for (std::list<TargetInfo>::iterator ihit = memberList->begin(); ihit != memberList->end(); ++ihit)
                    {
                        if(Unit* member = ObjectAccessor::GetUnit(*caster, ihit->targetGUID))
                            member->SetHealth(uint32(totalRaidHealthPct * member->GetMaxHealth()));
                    }
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_spirit_link_SpellScript::RemoveInvalidTargets, EFFECT_0, TARGET_UNIT_CASTER_AREA_RAID);
                AfterCast += SpellCastFn(spell_sha_spirit_link_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_spirit_link_SpellScript();
        }
};

// Healing Rain - 73920, 215864
class spell_sha_healing_rain : public SpellScriptLoader
{
    public:
        spell_sha_healing_rain() : SpellScriptLoader("spell_sha_healing_rain") { }

        class spell_sha_healing_rain_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_healing_rain_SpellScript);

            void HandleOnCast()
            {
                if(Unit* caster = GetCaster())
                {
                    Position const* sumpos = GetExplTargetDest();
                    if(TempSummon* summon = caster->SummonCreature(73400, *sumpos))
                    {
                        if(caster->m_SummonSlot[17])
                        {
                            if(Creature* tempsummon = caster->GetMap()->GetCreature(caster->m_SummonSlot[17]))
                                tempsummon->DespawnOrUnsummon(500);
                        }
                        caster->m_SummonSlot[17] = summon->GetGUID();
                    }
                }
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_sha_healing_rain_SpellScript::HandleOnCast);
            }
        };

        class spell_sha_healing_rain_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_healing_rain_AuraScript);

            void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if(Unit* caster = GetCaster())
                {
                    if(caster->m_SummonSlot[17])
                    {
                        if(Creature* summon = caster->GetMap()->GetCreature(caster->m_SummonSlot[17]))
                            summon->DespawnOrUnsummon(500);
                    }
                }
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_sha_healing_rain_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_healing_rain_AuraScript();
        }

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_healing_rain_SpellScript();
        }
};

class EarthenPowerTargetSelector
{
    public:
        EarthenPowerTargetSelector() { }

        bool operator() (WorldObject* target)
        {
            if (!target->ToUnit())
                return true;

            if (!target->ToUnit()->HasAuraWithMechanic(1 << MECHANIC_SNARE))
                return true;

            return false;
        }
};

class spell_sha_bloodlust : public SpellScriptLoader
{
    public:
        spell_sha_bloodlust() : SpellScriptLoader("spell_sha_bloodlust") { }

        class spell_sha_bloodlust_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_bloodlust_SpellScript);

            bool Validate(SpellInfo const* /*SpellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(57724))
                    return false;
                return true;
            }

            void RemoveInvalidTargets(std::list<WorldObject*>& targets)
            {
                std::list<WorldObject*> removeList;

                for (auto itr : targets)
                {
                    if (Unit* unit = itr->ToUnit())
                    {
                        if (unit->HasAura(207970)) // Shard of the Exodar
                            continue;

                        if (unit->HasAura(95809) || unit->HasAura(57724) || unit->HasAura(80354) || unit->HasAura(160455))
                        {
                            removeList.push_back(itr);
                            continue;
                        }
                    }
                }

                if (!removeList.empty())
                {
                    for (auto itr : removeList)
                        targets.remove(itr);
                }
            }

            void ApplyDebuff()
            {
                if (Unit* target = GetHitUnit())
                    target->CastSpell(target, 57724, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_bloodlust_SpellScript::RemoveInvalidTargets, EFFECT_0, TARGET_UNIT_CASTER_AREA_RAID);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_bloodlust_SpellScript::RemoveInvalidTargets, EFFECT_1, TARGET_UNIT_CASTER_AREA_RAID);
                AfterHit += SpellHitFn(spell_sha_bloodlust_SpellScript::ApplyDebuff);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_bloodlust_SpellScript();
        }
};

class spell_sha_heroism : public SpellScriptLoader
{
    public:
        spell_sha_heroism() : SpellScriptLoader("spell_sha_heroism") { }

        class spell_sha_heroism_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_heroism_SpellScript);

            bool Validate(SpellInfo const* /*SpellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(57723))
                    return false;
                return true;
            }

            void RemoveInvalidTargets(std::list<WorldObject*>& targets)
            {
                std::list<WorldObject*> removeList;

                for (auto itr : targets)
                {
                    if (Unit* unit = itr->ToUnit())
                    {
                        if (unit->HasAura(207970)) // Shard of the Exodar
                            continue;

                        if (unit->HasAura(95809) || unit->HasAura(57723) || unit->HasAura(80354) || unit->HasAura(160455))
                        {
                            removeList.push_back(itr);
                            continue;
                        }
                    }
                }

                if (!removeList.empty())
                {
                    for (auto itr : removeList)
                        targets.remove(itr);
                }
            }

            void ApplyDebuff()
            {
                if (Unit* target = GetHitUnit())
                    GetCaster()->CastSpell(target, 57723, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_heroism_SpellScript::RemoveInvalidTargets, EFFECT_0, TARGET_UNIT_CASTER_AREA_RAID);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_heroism_SpellScript::RemoveInvalidTargets, EFFECT_1, TARGET_UNIT_CASTER_AREA_RAID);
                AfterHit += SpellHitFn(spell_sha_heroism_SpellScript::ApplyDebuff);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_heroism_SpellScript();
        }
};

// Chain Heal - 1064
class spell_sha_chain_heal : public SpellScriptLoader
{
    public:
        spell_sha_chain_heal() : SpellScriptLoader("spell_sha_chain_heal") { }

        class spell_sha_chain_heal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_chain_heal_SpellScript);

            void HandleHeal(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster())
                    return;

                int32 _heal = GetHitHeal();

                if (Unit* target = GetHitUnit())
                {
                    if (AuraEffect const* aurEff = GetCaster()->GetAuraEffect(200076, EFFECT_0)) // Deluge
                    {
                        if (target->HasAura(200075) || target->HasAura(61295))
                            _heal += CalculatePct(_heal, aurEff->GetAmount());
                    }
                }
                SetHitHeal(_heal);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_sha_chain_heal_SpellScript::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_chain_heal_SpellScript();
        }
};

// Astral Recall - 556
class spell_sha_astral_recall : public SpellScriptLoader
{
    public:
        spell_sha_astral_recall() : SpellScriptLoader("spell_sha_astral_recall") { }

        class spell_sha_astral_recall_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_astral_recall_SpellScript);

            void HandleAfterCast()
            {
                Unit* caster = GetCaster();
                if(!caster)
                    return;

                if (caster->HasAura(147787)) //Glyph of Astral Fixation
                {
                    if (Player::TeamForRace(caster->getRace()) == HORDE)
                        caster->CastSpell(caster, 147902, true);
                    else
                        caster->CastSpell(caster, 147901, true);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_sha_astral_recall_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_astral_recall_SpellScript();
        }
};

// Glyph of Elemental Familiars - 148118
class spell_sha_elemental_familiars : public SpellScriptLoader
{
    public:
        spell_sha_elemental_familiars() : SpellScriptLoader("spell_sha_elemental_familiars") { }

        class spell_sha_elemental_familiars_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_elemental_familiars_SpellScript);

            void HandleAfterCast()
            {
                if(Unit* caster = GetCaster())
                {
                    Position const* sumpos = GetExplTargetDest();
                    uint32 entry[] = { 73556, 73559, 73560 };
                    SummonPropertiesEntry const* properties = sSummonPropertiesStore.LookupEntry(3221);
                    if(TempSummon* summon = caster->GetMap()->SummonCreature(entry[urand(0, 2)], *sumpos, properties, 600000, caster))
                    {
                        if(caster->m_SummonSlot[17])
                        {
                            if(Creature* tempsummon = caster->GetMap()->GetCreature(caster->m_SummonSlot[17]))
                                tempsummon->DespawnOrUnsummon(500);
                        }
                        caster->m_SummonSlot[17] = summon->GetGUID();
                    }
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_sha_elemental_familiars_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_elemental_familiars_SpellScript();
        }
};

// Cloudburst Totem - 157504
class spell_sha_cloudburst_totem : public SpellScriptLoader
{
    public:
        spell_sha_cloudburst_totem() : SpellScriptLoader("spell_sha_cloudburst_totem") { }

        class spell_sha_cloudburst_totem_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_cloudburst_totem_AuraScript);

            void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetUnitOwner())
                {
                    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(157503);
                    if(!spellInfo || caster->ToTotem())
                        return;
                    float heal = CalculatePct(aurEff->GetAmount(), spellInfo->Effects[EFFECT_1]->CalcValue(caster));
                    caster->CastCustomSpell(caster, 157503, &heal, NULL, NULL, true);
                }
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_sha_cloudburst_totem_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_cloudburst_totem_AuraScript();
        }
};

// Earth Shock - 8042
class spell_sha_earth_shock : public SpellScriptLoader
{
    public:
        spell_sha_earth_shock() : SpellScriptLoader("spell_sha_earth_shock") { }

        class spell_sha_earth_shock_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_earth_shock_SpellScript)

            void HandleDamage(SpellEffIndex /*eff*/)
            {
                SetHitDamage(CalculatePct(GetHitDamage(), GetSpell()->GetPowerCost(POWER_MAELSTROM)));
                if (Unit* target = GetHitUnit())
                    if(Unit* caster = GetCaster())
                        if (caster->HasAura(204398)) // Earthfury (Honor Talent)
                            if (GetSpell()->GetPowerCost(POWER_MAELSTROM) >= 100)
                                caster->CastSpell(target, 204399, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_sha_earth_shock_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript *GetSpellScript() const override
        {
            return new spell_sha_earth_shock_SpellScript();
        }
};

// 188389 - Flame Shock
class spell_sha_flame_shock : public SpellScriptLoader
{
    public:
        spell_sha_flame_shock() : SpellScriptLoader("spell_sha_flame_shock") { }

        class spell_sha_flame_shock_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_flame_shock_AuraScript);

            void CalculateMaxDuration(int32 & duration)
            {
                duration += (duration / 20) * GetAura()->GetPowerCost(POWER_MAELSTROM);
            }

            void Register() override
            {
                DoCalcMaxDuration += AuraCalcMaxDurationFn(spell_sha_flame_shock_AuraScript::CalculateMaxDuration);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_flame_shock_AuraScript();
        }
};

// 196840 - Frost Shock
class spell_sha_frost_shock : public SpellScriptLoader
{
    public:
        spell_sha_frost_shock() : SpellScriptLoader("spell_sha_frost_shock") { }

        class spell_sha_frost_shock_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_frost_shock_AuraScript);

            void CalculateMaxDuration(int32 & duration)
            {
                duration += (duration / 20) * GetAura()->GetPowerCost(POWER_MAELSTROM);
            }

            void Register() override
            {
                DoCalcMaxDuration += AuraCalcMaxDurationFn(spell_sha_frost_shock_AuraScript::CalculateMaxDuration);
            }
        };

        class spell_sha_frost_shock_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_frost_shock_SpellScript)

            void HandleDamage(SpellEffIndex /*eff*/)
            {
                SetHitDamage(GetHitDamage() + CalculatePct(GetHitDamage(), GetSpell()->GetPowerCost(POWER_MAELSTROM) * 5));
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_sha_frost_shock_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript *GetSpellScript() const override
        {
            return new spell_sha_frost_shock_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_frost_shock_AuraScript();
        }
};

// 187837 - Lightning Bolt
class spell_sha_lightning_bolt : public SpellScriptLoader
{
    public:
        spell_sha_lightning_bolt() : SpellScriptLoader("spell_sha_lightning_bolt") { }

        class spell_sha_lightning_bolt_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_lightning_bolt_SpellScript)

            void HandleDamage(SpellEffIndex /*eff*/)
            {
                if (Unit* caster = GetCaster())
                {
                    if (SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(210727))
                    {
                        float dmgMod = 1.f + CalculatePct(1.f, GetSpell()->GetPowerCost(POWER_MAELSTROM) * (spellInfo->Effects[EFFECT_1]->BasePoints / spellInfo->Effects[EFFECT_0]->BasePoints));
                        SetHitDamage(GetHitDamage() * dmgMod);
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_sha_lightning_bolt_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript *GetSpellScript() const override
        {
            return new spell_sha_lightning_bolt_SpellScript();
        }
};

// Healing Surge - 8004, Healing Wave - 77472, talent 200071 - Undulation
class spell_sha_undulation : public SpellScriptLoader
{
    public:
        spell_sha_undulation() : SpellScriptLoader("spell_sha_undulation") { }

        class spell_sha_undulation_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_undulation_SpellScript);

            void HandleAfterCast()
            {
                if(Unit* caster = GetCaster())
                    if(Aura* aura = caster->GetAura(200071)) // Undulation
                    {
                        if(aura->GetCustomData() >= 2)
                        {
                            caster->CastSpell(caster, 216251, true);
                            aura->ModCustomData(-2);
                        }
                        else
                            aura->ModCustomData(1);
                    }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_sha_undulation_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_undulation_SpellScript();
        }
};

// Ancestral Protection - 207498
class spell_sha_ancestral_protection : public SpellScriptLoader
{
    public:
        spell_sha_ancestral_protection() : SpellScriptLoader("spell_sha_ancestral_protection") { }

        class spell_sha_ancestral_protection_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_ancestral_protection_AuraScript);

            void CalculateAmount(AuraEffect const* /*aurEff*/, float& amount, bool& /*canBeRecalculated*/)
            {
                amount = -1;
            }

            void Absorb(AuraEffect* /*aurEff*/, DamageInfo & dmgInfo, float & absorbAmount)
            {
                Unit* target = GetTarget();
                SpellInfo const* spellInfo = dmgInfo.GetSpellInfo();

                if (!target || (spellInfo && spellInfo->IsAffectingArea()))
                    return;

                if (dmgInfo.GetDamage() < target->GetHealth())
                    return;

                if(Unit* caster = GetCaster())
                {
                    GuidList* summonList = caster->GetSummonList(104818);
                    for (GuidList::const_iterator iter = summonList->begin(); iter != summonList->end(); ++iter)
                    {
                        if(Creature* summon = ObjectAccessor::GetCreature(*caster, (*iter)))
                            summon->DespawnOrUnsummon(500);
                    }
                }
            }

            void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* target = GetTarget())
                {
                    AuraRemoveMode removeMode = GetTargetApplication()->GetRemoveMode();
                    if (removeMode == AURA_REMOVE_BY_DEATH)
                        target->CastSpell(target, 207553, true); // Totemic Revival
                }
            }

            void Register() override
            {
                 OnEffectAbsorb += AuraEffectAbsorbFn(spell_sha_ancestral_protection_AuraScript::Absorb, EFFECT_1, SPELL_AURA_SCHOOL_ABSORB);
                 DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_ancestral_protection_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_SCHOOL_ABSORB);
                 OnEffectRemove += AuraEffectApplyFn(spell_sha_ancestral_protection_AuraScript::HandleRemove, EFFECT_1, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_ancestral_protection_AuraScript();
        }
};

// Earthen Shield - 201633
class spell_sha_earthen_shield : public SpellScriptLoader
{
    public:
        spell_sha_earthen_shield() : SpellScriptLoader("spell_sha_earthen_shield") { }

        class spell_sha_earthen_shield_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_earthen_shield_AuraScript);

            void CalculateAmount(AuraEffect const* /*aurEff*/, float& amount, bool& /*canBeRecalculated*/)
            {
                amount = -1;
            }

            void Absorb(AuraEffect* aurEff, DamageInfo & dmgInfo, float & absorbAmount)
            {
                Unit* target = GetTarget();
                Unit* caster = GetCaster();
                if (!caster || !target)
                    return;

                Unit* owner = caster->GetAnyOwner();
                if (caster->isAlive() && owner)
                {
                    float bp = owner->GetSpellPowerDamage(SPELL_SCHOOL_MASK_NATURE);
                    absorbAmount = bp;

                    if (dmgInfo.GetDamage() < uint32(bp))
                        bp = dmgInfo.GetDamage();

                    target->CastCustomSpell(caster, 201657, &bp, NULL, NULL, true);
                }
                else
                    absorbAmount = 0;
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_earthen_shield_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_sha_earthen_shield_AuraScript::Absorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_earthen_shield_AuraScript();
        }
};

// Recall Cloudburst Totem - 201764
class spell_sha_recall_cloudburst_totem : public SpellScriptLoader
{
    public:
        spell_sha_recall_cloudburst_totem() : SpellScriptLoader("spell_sha_recall_cloudburst_totem") { }

        class spell_sha_recall_cloudburst_totem_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_recall_cloudburst_totem_SpellScript);

            void HandleAfterCast()
            {
                if(Unit* caster = GetCaster())
                {
                    GuidList* summonList = caster->GetSummonList(78001);
                    for (GuidList::const_iterator iter = summonList->begin(); iter != summonList->end(); ++iter)
                        if(Creature* summon = ObjectAccessor::GetCreature(*caster, (*iter)))
                            summon->DespawnOrUnsummon(500);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_sha_recall_cloudburst_totem_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_recall_cloudburst_totem_SpellScript();
        }
};

//204288
class spell_sha_earth_shield : public SpellScriptLoader
{
    public:
        spell_sha_earth_shield() : SpellScriptLoader("spell_sha_earth_shield") {}

        class spell_sha_earth_shield_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_earth_shield_AuraScript);

            void OnProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
            {
                if (!GetCaster() || !GetTarget() || eventInfo.GetActor()->GetGUID() == GetTarget()->GetGUID())
                {
                    PreventDefaultAction();
                    return;
                }

                if (eventInfo.GetDamageInfo()->GetDamage() >= GetTarget()->CountPctFromMaxHealth(GetSpellInfo()->Effects[EFFECT_1]->BasePoints))
                    GetCaster()->CastSpell(GetTarget(), 204290, true); // Heal
                else
                    PreventDefaultAction();
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_sha_earth_shield_AuraScript::OnProc, EFFECT_1, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_earth_shield_AuraScript();
        }
};

// Sundering - 197214
class spell_sha_sundering : public SpellScriptLoader
{
    public:
        spell_sha_sundering() : SpellScriptLoader("spell_sha_sundering") { }

        class spell_sha_sundering_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_sundering_SpellScript);

            void HandleDummy()
            {
                Unit* caster = GetCaster();
                Unit* target = GetHitUnit();
                if (caster && target)
                {
                    Position pos;
                    float angle = caster->GetOrientation() - target->GetOrientation(); // Back from caster
                    caster->GetFirstCollisionPosition(pos, 15.0f, 0.0f); // Dist need research
                    if (target->IsInBetweenShift(caster, &pos, 2.5f, 2.5f, 1.5f)) // Is right
                    {
                        target->GetFirstCollisionPosition(pos, 10.0f, angle + 1.5f);
                        sLog->outDebug(LOG_FILTER_SPELLS_AURAS, "spell_sha_sundering IsInBetweenShift 2.5f");
                    }
                    else if (target->IsInBetweenShift(caster, &pos, 2.5f, -2.5f, -1.5f)) // Is left
                    {
                        target->GetFirstCollisionPosition(pos, 10.0f, angle - 1.5f);
                        sLog->outDebug(LOG_FILTER_SPELLS_AURAS, "spell_sha_sundering IsInBetweenShift -2.5f");
                    }
                    else
                    {
                        target->GetFirstCollisionPosition(pos, 10.0f, angle);
                        sLog->outDebug(LOG_FILTER_SPELLS_AURAS, "spell_sha_sundering not IsInBetweenShift");
                    }

                    sLog->outDebug(LOG_FILTER_SPELLS_AURAS, "spell_sha_sundering x %f y %f z %f", pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ());

                    target->CastSpell(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), 197619, true);
                }
            }

            void Register() override
            {
                BeforeHit += SpellHitFn(spell_sha_sundering_SpellScript::HandleDummy);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_sundering_SpellScript();
        }
};

// Lava Beam - 114074
class spell_sha_lava_beam : public SpellScriptLoader
{
    public:
        spell_sha_lava_beam() : SpellScriptLoader("spell_sha_lava_beam") { }

        class spell_sha_lava_beam_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_lava_beam_SpellScript)

            uint32 count = 0;

            void HandleDamage(SpellEffIndex /*eff*/)
            {
                int32 damage = GetHitDamage();
                damage += CalculatePct(damage, GetSpellInfo()->Effects[EFFECT_1]->CalcValue(GetCaster()) * count);
                SetHitDamage(damage);
                count++;
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_sha_lava_beam_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript *GetSpellScript() const override
        {
            return new spell_sha_lava_beam_SpellScript();
        }
};

// Forked Lightning (Honor Talent) - 204350
class spell_sha_forked_lightning_pvp : public SpellScriptLoader
{
    public:
    spell_sha_forked_lightning_pvp() : SpellScriptLoader("spell_sha_forked_lightning_pvp") {}

    class spell_sha_forked_lightning_pvp_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_sha_forked_lightning_pvp_SpellScript);

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            if (Unit* caster = GetCaster())
            {
                targets.remove_if(Trinity::UnitCheckInLos(false, caster));
                targets.sort(Trinity::UnitSortDistance(false, caster));

                uint8 targetCount = irand(1, 2);

                if (targets.size() > targetCount)
                    targets.resize(targetCount);
            }
            
        }

        void Register() override
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_forked_lightning_pvp_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_sha_forked_lightning_pvp_SpellScript();
    }
};

// Spirit Link (Honor Talent) - 204293
class spell_sha_spirit_link_pvp : public SpellScriptLoader
{
    public:
        spell_sha_spirit_link_pvp() : SpellScriptLoader("spell_sha_spirit_link_pvp") { }

        class spell_sha_spirit_link_pvp_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_spirit_link_pvp_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                Unit* caster = GetCaster();
                if (!caster)
                    return;

                targets.sort(Trinity::UnitSortDistance(true, caster));
                if(targets.size() > 3)
                    targets.resize(3);

                GuidList targetList;
                for (std::list<WorldObject*>::iterator itr = targets.begin(); itr != targets.end(); ++itr)
                    if(WorldObject* object = (*itr))
                        targetList.push_back(object->GetGUID());

                GetSpell()->SetEffectTargets(targetList);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_spirit_link_pvp_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_spirit_link_pvp_SpellScript();
        }

        class spell_sha_spirit_link_pvp_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_spirit_link_pvp_AuraScript);

            void CalculateAmount(AuraEffect const* /*aurEff*/, float& amount, bool& /*canBeRecalculated*/)
            {
                amount = -1;
            }

            void Absorb(AuraEffect* aurEff, DamageInfo & dmgInfo, float & absorbAmount)
            {
                absorbAmount = 0;
                Unit* caster = GetTarget();
                if (!caster)
                    return;

                GuidList targets = aurEff->GetBase()->GetEffectTargets();
                if(targets.size() <= 1)
                    return;

                targets.remove(caster->GetGUID());
                float damage = CalculatePct(dmgInfo.GetDamage(), GetSpellInfo()->Effects[EFFECT_1]->CalcValue(GetCaster()));
                absorbAmount = dmgInfo.GetDamage() - damage;
                damage /= targets.size();

                for (GuidList::iterator itr = targets.begin(); itr != targets.end(); ++itr)
                    if (Unit* target = ObjectAccessor::GetUnit(*caster, (*itr)))
                        caster->CastCustomSpell(target, 204314, &damage, NULL, NULL, true);
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_spirit_link_pvp_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_sha_spirit_link_pvp_AuraScript::Absorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_spirit_link_pvp_AuraScript();
        }
};

// Purge - 370
class spell_sha_purge : public SpellScriptLoader
{
    public:
        spell_sha_purge() : SpellScriptLoader("spell_sha_purge") { }

        class spell_sha_purge_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_purge_SpellScript);

            void OnSuccessfulDispel(SpellEffIndex /*effIndex*/)
            {
                if(Unit* caster = GetCaster())
                    if (Unit* target = GetHitUnit())
                        if (caster->HasAura(206642)) // Electrocute (Honor Talent)
                            caster->CastSpell(target, 206647, true);
            }

            void Register() override
            {
                OnEffectSuccessfulDispel += SpellEffectFn(spell_sha_purge_SpellScript::OnSuccessfulDispel, EFFECT_0, SPELL_EFFECT_DISPEL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_purge_SpellScript();
        }
};

// Grounding Totem (Honor Talent) - 242900
class spell_sha_grounding_totem : public SpellScriptLoader
{
    public:
        spell_sha_grounding_totem() : SpellScriptLoader("spell_sha_grounding_totem") { }

        class spell_sha_grounding_totem_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_grounding_totem_AuraScript);

            void CalculateAmount(AuraEffect const* aurEff, float& amount, bool& /*canBeRecalculated*/)
            {
                amount = -1;
            }

            void Absorb(AuraEffect* /*aurEff*/, DamageInfo& dmgInfo, float& absorbAmount)
            {
                absorbAmount = dmgInfo.GetDamage();
            }

            void Register() override
            {
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_sha_grounding_totem_AuraScript::Absorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_grounding_totem_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_grounding_totem_AuraScript();
        }
};

// Shamanism (Honor Talent) - 193876
class spell_sha_shamanism : public SpellScriptLoader
{
    public:
        spell_sha_shamanism() : SpellScriptLoader("spell_sha_shamanism") { }

        class spell_sha_shamanism_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_shamanism_AuraScript);

            void CalculateAmount(AuraEffect const* aurEff, float& amount, bool& /*canBeRecalculated*/)
            {
                amount = 0;
                if (!GetCaster())
                    return;

                if (Player* _player = GetCaster()->ToPlayer())
                {
                    if (_player->GetTeamId() == TEAM_HORDE)
                        amount = 204361;
                    else
                        amount = 204362;
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_shamanism_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_shamanism_AuraScript();
        }
};

// Stormstrike(17364), Windstrike(115356)
class spell_sha_stormflurry : public SpellScriptLoader
{
    public:
    spell_sha_stormflurry() : SpellScriptLoader("spell_sha_stormflurry") { }

        class spell_sha_stormflurry_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_stormflurry_SpellScript);

            void HandleOnHit(SpellEffIndex /*effIndex*/)
            {
                if (Unit* caster = GetCaster())
                {
                    if (Aura* aura = caster->GetAura(198367))
                    {
                        uint8 castCount = 0;
                        float bp0 = 0;

                        if (AuraEffect* eff = aura->GetEffect(EFFECT_0))
                            bp0 = eff->GetAmount();

                        for (uint8 i = 0; i < 2; i++)
                        {
                            if (!roll_chance_f(bp0))
                                break;

                            castCount++;
                        }

                        if (!castCount)
                            return;

                        uint32 spellId0 = 32175;
                        uint32 spellId1 = 32176;

                        if (GetSpellInfo()->Id == 115356)
                        {
                            spellId0 = 115357;
                            spellId1 = 115360;
                        }

                        if (Unit* target = GetHitUnit())
                        {
                            if (AuraEffect* eff = aura->GetEffect(EFFECT_1))
                            {
                                for (uint8 i = 0; i < castCount; i++)
                                {
                                    caster->CastSpell(target, spellId0, true, NULL, eff);
                                    caster->CastSpell(target, spellId1, true, NULL, eff);
                                }
                            }
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_sha_stormflurry_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_TRIGGER_SPELL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_stormflurry_SpellScript();
        }
};

// Alpha Wolf - 198486
class spell_sha_alpha_wolf : public SpellScriptLoader
{
    public:
        spell_sha_alpha_wolf() : SpellScriptLoader("spell_sha_alpha_wolf") { }

        class spell_sha_alpha_wolf_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_alpha_wolf_AuraScript);

            void OnTick(AuraEffect const* aurEff)
            {
                if (Unit* caster = GetTarget())
                {
                    switch (caster->GetDisplayId())
                    {
                        case 55290:
                            caster->CastSpell(caster, 198455, true);
                            break;
                        case 66843:
                            caster->CastSpell(caster, 198480, true);
                            break;
                        case 66844:
                            caster->CastSpell(caster, 198483, true);
                            break;
                        case 66845:
                            if (Unit* target = caster->getVictim())
                                caster->CastSpell(target, 198485, true);
                            break;
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_alpha_wolf_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_alpha_wolf_AuraScript();
        }
};

// Crash Lightning - 187874
class spell_sha_crash_lightning : public SpellScriptLoader
{
    public:
        spell_sha_crash_lightning() : SpellScriptLoader("spell_sha_crash_lightning") { }

        class spell_sha_crash_lightning_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_crash_lightning_SpellScript);

            void HandleAfterCast()
            {
                Unit* caster = GetCaster();
                if(!caster)
                    return;

                if (AuraEffect const* aurEff = caster->GetAuraEffect(198299, EFFECT_0)) // Gathering Storms
                {
                    float bp0 = aurEff->GetAmount() * GetSpell()->GetTargetCount();
                    caster->CastCustomSpell(caster, 198300, &bp0, NULL, NULL, true);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_sha_crash_lightning_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_crash_lightning_SpellScript();
        }
};

// Sense of Urgency - 2825, 32182, 80353, 90355, 146555, 160452, 178207
class spell_sha_sense_of_urgency : public SpellScriptLoader
{
    public:
        spell_sha_sense_of_urgency() : SpellScriptLoader("spell_sha_sense_of_urgency") { }

        class spell_sha_sense_of_urgency_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_sense_of_urgency_AuraScript);

            void CalculateMaxDuration(int32 & duration)
            {
                if (Unit* caster = GetUnitOwner())
                {
                    if (AuraEffect const* aurEff = caster->GetAuraEffect(234814, EFFECT_0)) // Uncertain Reminder
                        duration += aurEff->GetAmount() * 1000;
                    if (caster->HasAura(207355)) // Sense of Urgency
                        caster->CastSpellDuration(caster, 208416, true, duration);
                }
            }

            void Register() override
            {
                DoCalcMaxDuration += AuraCalcMaxDurationFn(spell_sha_sense_of_urgency_AuraScript::CalculateMaxDuration);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_sense_of_urgency_AuraScript();
        }
};

// 207778
class spell_monk_gift_of_queen : public SpellScriptLoader
{
    public:
        spell_monk_gift_of_queen() : SpellScriptLoader("spell_monk_gift_of_queen") { }

        class spell_monk_gift_of_queen_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_gift_of_queen_SpellScript);

            void HandleDummy()
            {
                if (auto caster = GetCaster())
                    if (caster->HasAura(238143))
                        if (WorldLocation const* pos = GetExplTargetDest())
                        {
                            Position posit = pos->GetPosition();
                            caster->CastSpellDelay(posit, 255227, true, 3000);
                        }
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_monk_gift_of_queen_SpellScript::HandleDummy);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_gift_of_queen_SpellScript();
        }
};

void AddSC_shaman_spell_scripts()
{
    new spell_sha_spirit_link();
    new spell_sha_healing_rain();
    new spell_sha_bloodlust();
    new spell_sha_heroism();
    new spell_sha_chain_heal();
    new spell_sha_astral_recall();
    new spell_sha_elemental_familiars();
    new spell_sha_cloudburst_totem();
    new spell_sha_earth_shock();
    new spell_sha_flame_shock();
    new spell_sha_frost_shock();
    new spell_sha_lightning_bolt();
    new spell_sha_undulation();
    new spell_sha_ancestral_protection();
    new spell_sha_earthen_shield();
    new spell_sha_recall_cloudburst_totem();
    new spell_sha_earth_shield();
    new spell_sha_sundering();
    new spell_sha_lava_beam();
    new spell_sha_spirit_link_pvp();
    new spell_sha_purge();
    new spell_sha_grounding_totem();
    new spell_sha_shamanism();
    new spell_sha_stormflurry();
    new spell_sha_alpha_wolf();
    new spell_sha_crash_lightning();
    new spell_sha_sense_of_urgency();
    new spell_sha_forked_lightning_pvp();
    new spell_monk_gift_of_queen();
}
