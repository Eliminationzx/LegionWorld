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
 * Scripts for spells with SPELLFAMILY_PRIEST and SPELLFAMILY_GENERIC spells used by priest players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_pri_".
 */

#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "Group.h"

// Called by Heal - 2050, Flash Heal - 2061, Greater Heal - 2060 and Prayer of Healing - 596
// Spirit Shell - 109964
class spell_pri_spirit_shell : public SpellScriptLoader
{
    public:
        spell_pri_spirit_shell() : SpellScriptLoader("spell_pri_spirit_shell") { }

        class spell_pri_spirit_shell_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_spirit_shell_SpellScript);

            void HandleOnHit()
            {
                Unit* target = GetHitUnit();
                Unit* caster = GetCaster();
                if (!target || !caster)
                    return;

                float bp = GetHitHeal();
                if (GetSpellInfo()->Id == 2061)
                {
                    if (AuraEffect* aurEff = caster->GetAuraEffect(200128, 0)) // Trail of Light
                    {
                        if (Aura* auraTrail = aurEff->GetBase())
                        {
                            GuidList saveTargets = auraTrail->GetEffectTargets();
                            if (!saveTargets.empty())
                            {
                                ObjectGuid const& targetFirst = saveTargets.front();
                                ObjectGuid const& targetLast = saveTargets.back();

                                if (targetFirst != target->GetGUID())
                                {
                                    if(Unit* targetTrail = ObjectAccessor::GetPlayer(*caster, targetFirst))
                                    {
                                        if(caster->GetDistance(targetTrail) <= 40.0f)
                                        {
                                            float heal_ = CalculatePct(bp, aurEff->GetAmount());
                                            caster->CastCustomSpell(targetTrail, 234946, &heal_, NULL, NULL,  true);
                                        }
                                        else
                                            auraTrail->RemoveEffectTarget(targetFirst);
                                    }
                                    else
                                        auraTrail->RemoveEffectTarget(targetFirst);

                                    if (targetFirst == targetLast)
                                        auraTrail->AddEffectTarget(target->GetGUID());
                                    else if (targetLast != target->GetGUID())
                                    {
                                        auraTrail->RemoveEffectTarget(targetFirst);
                                        auraTrail->AddEffectTarget(target->GetGUID());
                                    }
                                }
                            }
                            else
                                auraTrail->AddEffectTarget(target->GetGUID());
                        }
                    }
                }
                if (caster->HasAura(109964))
                {
                    if (AuraEffect* aurEff = target->GetAuraEffect(114908, 0))
                        bp += aurEff->GetAmount();
                    if (bp > int32(CalculatePct(target->GetMaxHealth(), 60)))
                        bp = int32(CalculatePct(target->GetMaxHealth(), 60));
                    caster->CastCustomSpell(target, 114908, &bp, NULL, NULL,  true);
                    SetHitHeal(0);
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_pri_spirit_shell_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_spirit_shell_SpellScript();
        }
};

// Halo - 120692
class spell_pri_halo_heal : public SpellScriptLoader
{
    public:
        spell_pri_halo_heal() : SpellScriptLoader("spell_pri_halo_heal") { }

        class spell_pri_halo_heal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_halo_heal_SpellScript);

            void HandleHeal(SpellEffIndex /*effIndex*/)
            {
                if (Unit* caster = GetCaster())
                {
                    if (Unit* target = GetHitUnit())
                    {
                        int32 _heal = GetHitHeal();
                        float Distance = caster->GetDistance(target);
                        float pct = (0.5f * pow((1.01f),(-1 * pow(((Distance - 25.0f) / 2), 4))) + 0.1f + 0.015f*Distance);
                        _heal = int32(_heal * pct);
                        SetHitHeal(_heal);
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_pri_halo_heal_SpellScript::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_halo_heal_SpellScript;
        }
};

// Void Shift - 108968, 142723
class spell_pri_void_shift : public SpellScriptLoader
{
    public:
        spell_pri_void_shift() : SpellScriptLoader("spell_pri_void_shift") { }

        class spell_pri_void_shift_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_void_shift_SpellScript);

            SpellCastResult CheckTarget()
            {
                if (GetExplTargetUnit())
                    if (GetExplTargetUnit()->GetTypeId() != TYPEID_PLAYER)
                        return SPELL_FAILED_BAD_TARGETS;

                return SPELL_CAST_OK;
            }

            void HandleOnHit()
            {
                if (Unit* caster = GetCaster())
                {
                    if (Unit* target = GetExplTargetUnit())
                    {
                        float casterPct;
                        float targetPct;
                        float basePct = GetSpellInfo()->Effects[EFFECT_0]->BasePoints;
                        float casterHeal = 0.f, targetHeal = 0.f;
                        float casterDamage = 0.f, targetDamage = 0.f;

                        casterPct = caster->GetHealthPct();
                        targetPct = target->GetHealthPct();

                        if (casterPct < basePct)
                        {
                            if(target->HasAura(47788))
                                targetHeal = target->CountPctFromMaxHealth(int32(basePct * 1.6f));
                            else
                                targetHeal = target->CountPctFromMaxHealth(int32(basePct));
                        }
                        else
                            targetHeal = target->CountPctFromMaxHealth(int32(casterPct));

                        if (targetPct < basePct)
                        {
                            if(caster->HasAura(47788))
                                casterHeal = int32(caster->CountPctFromMaxHealth(basePct * 1.6f));
                            else
                                casterHeal = int32(caster->CountPctFromMaxHealth(basePct));
                        }
                        else
                            casterHeal = caster->CountPctFromMaxHealth(int32(targetPct));

                        if(target->GetHealth() > uint32(targetHeal))
                        {
                            targetDamage = target->GetHealth() - targetHeal;
                            targetHeal = 0;
                        }
                        else
                            targetHeal -= target->GetHealth();

                        if(caster->GetHealth() > uint32(casterHeal))
                        {
                            casterDamage = caster->GetHealth() - casterHeal;
                            casterHeal = 0;
                        }
                        else
                            casterHeal -= caster->GetHealth();

                        caster->CastCustomSpell(caster, 118594, &casterDamage, &casterHeal, NULL, true);
                        caster->CastCustomSpell(target, 118594, &targetDamage, &targetHeal, NULL, true);
                        caster->CastSpell(target, 134977, true);
                    }
                }
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_pri_void_shift_SpellScript::CheckTarget);
                OnHit += SpellHitFn(spell_pri_void_shift_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_void_shift_SpellScript;
        }
};

// Guardian Spirit - 47788
class spell_pri_guardian_spirit : public SpellScriptLoader
{
    public:
        spell_pri_guardian_spirit() : SpellScriptLoader("spell_pri_guardian_spirit") { }

        class spell_pri_guardian_spirit_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_guardian_spirit_AuraScript);

            uint32 healPct;

            bool Validate(SpellInfo const* /*SpellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(48153))
                    return false;
                return true;
            }

            bool Load() override
            {
                healPct = GetSpellInfo()->Effects[EFFECT_1]->CalcValue();
                return true;
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, float & amount, bool & /*canBeRecalculated*/)
            {
                // Set absorbtion amount to unlimited
                amount = -1;
            }

            void Absorb(AuraEffect* /*aurEff*/, DamageInfo & dmgInfo, float & absorbAmount)
            {
                Unit* target = GetTarget();
                if (dmgInfo.GetDamage() < target->GetHealth())
                    return;

                float healAmount = int32(target->CountPctFromMaxHealth(healPct));
                // remove the aura now, we don't want 40% healing bonus
                Remove(AURA_REMOVE_BY_ENEMY_SPELL);
                target->CastCustomSpell(target, 48153, &healAmount, NULL, NULL, true);
                absorbAmount = dmgInfo.GetDamage();
            }

            void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
            {
                Unit* target = GetTarget();
                if (!target)
                    return;

                if (Unit* caster = GetCaster())
                    if (caster->HasAura(196437)) // Guardians of the Light
                        if (target->GetGUID() != caster->GetGUID())
                            caster->CastSpell(caster, 47788, true);
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_guardian_spirit_AuraScript::CalculateAmount, EFFECT_2, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_pri_guardian_spirit_AuraScript::Absorb, EFFECT_2, SPELL_AURA_SCHOOL_ABSORB);
                AfterEffectApply += AuraEffectApplyFn(spell_pri_guardian_spirit_AuraScript::OnApply, EFFECT_2, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_guardian_spirit_AuraScript();
        }
};

// Voidform - 194249
class spell_pri_voidform : public SpellScriptLoader
{
    public:
        spell_pri_voidform() : SpellScriptLoader("spell_pri_voidform") { }

        class spell_pri_voidform_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_voidform_AuraScript);

            int32 saveAmount = 0;
            int32 saveStack = 0;

            void OnTick(AuraEffect const* /*aurEff*/)
            {
                if (Unit* caster = GetCaster())
                {
                    if (caster->HasAura(205065) || caster->HasAura(47585) || caster->HasAura(219772))
                    {
                        if (!saveAmount)
                            if (AuraEffect* aurEff = GetAura()->GetEffect(EFFECT_1))
                            {
                                saveAmount = aurEff->GetAmount();
                                aurEff->ChangeAmount(0);
                            }

                        saveStack++;
                    }
                    else
                    {
                        if (saveAmount)
                        {
                            GetAura()->ModStackAmount(saveStack + 1);
                            if (AuraEffect* effect = GetAura()->GetEffect(EFFECT_1))
                                effect->ChangeAmount(saveAmount);
                            saveAmount = 0;
                            saveStack = 0;
                        }
                        else
                        {
                            if (caster->GetPower(POWER_INSANITY) <= 0)
                            {
                                if (caster->HasAura(193223))
                                    caster->CastSpell(caster, 195455, true);
                                else
                                    GetAura()->Remove();
                            }
                            else
                            {
                                float mod = caster->HasAura(242273) ? 0.9f : 1.f;
                                GetAura()->ModStackAmount(1);
                                if (AuraEffect* effect = GetAura()->GetEffect(EFFECT_1))
                                    effect->ChangeAmount((-3000.f + (-300.f * GetAura()->GetStackAmount())) * mod);
                                if (AuraEffect* effect = GetAura()->GetEffect(EFFECT_2))
                                    effect->ChangeAmount(GetAura()->GetStackAmount());
                            }
                        }
                    }

                    if (AuraEffect const* aurEff0 = caster->GetAuraEffect(194378, EFFECT_0)) // Mass Hysteria
                        if (AuraEffect* effect = GetAura()->GetEffect(EFFECT_9))
                            effect->ChangeAmount(aurEff0->GetAmount() * GetAura()->GetStackAmount());

                    switch (GetAura()->GetStackAmount())
                    {
                        case 15:
                            caster->CastSpell(caster, 210198, true);
                            break;
                        case 30:
                            caster->CastSpell(caster, 210197, true);
                            break;
                        case 45:
                            caster->CastSpell(caster, 210196, true);
                            break;
                    }
                }
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* caster = GetCaster();
                if (!caster)
                    return;

                if (caster->HasAura(194179)) // Sphere of Insanity
                    caster->RemoveAllMinionsByFilter(98680);

                AuraRemoveMode removeMode = GetTargetApplication()->GetRemoveMode();
                if (removeMode == AURA_REMOVE_BY_DEATH)
                    return;

                int8 _count = GetAura()->GetStackAmount();
                if (Unit* caster = GetCaster())
                    if (caster->HasAura(199849)) // Lingering Insanity
                        for (int i = 0; i < _count; i++)
                            caster->CastSpell(caster, 197937, true);

                caster->RemoveAurasDueToSpell(218413);
                caster->RemoveAurasDueToSpell(210198);
                caster->RemoveAurasDueToSpell(210197);
                caster->RemoveAurasDueToSpell(210196);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_pri_voidform_AuraScript::OnTick, EFFECT_4, SPELL_AURA_PERIODIC_DUMMY);
                OnEffectRemove += AuraEffectRemoveFn(spell_pri_voidform_AuraScript::OnRemove, EFFECT_4, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_voidform_AuraScript();
        }
};

// Divine Hymn - 64844
class spell_pri_divine_hymn : public SpellScriptLoader
{
    public:
        spell_pri_divine_hymn() : SpellScriptLoader("spell_pri_divine_hymn") { }

        class spell_pri_divine_hymn_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_divine_hymn_SpellScript);

            void HandleOnHit()
            {
                if (Unit* caster = GetCaster())
                    if (Player* _player = caster->ToPlayer())
                        if (Group* group = _player->GetGroup())
                            if (group->isRaidGroup())
                                return;

                SetHitHeal(GetHitHeal() * 2);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_pri_divine_hymn_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_divine_hymn_SpellScript();
        }
};

// Light's Wrath - 207946
class spell_pri_lights_wrath : public SpellScriptLoader
{
    public:
        spell_pri_lights_wrath() : SpellScriptLoader("spell_pri_lights_wrath") { }

        class spell_pri_lights_wrath_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_lights_wrath_SpellScript);

            void HandleDamage(SpellEffIndex /*effIndex*/)
            {
                if (Unit* caster = GetCaster())
                    if (Unit* target = GetHitUnit())
                    {
                        uint32 count = caster->GetCountMyAura(194384);
                        int32 perc = GetSpellInfo()->Effects[EFFECT_1]->CalcValue(caster) * count;
                        int32 damage = GetHitDamage();
                        damage += CalculatePct(damage, perc);
                        SetHitDamage(damage);
                        if(AuraEffect const* aurEff = caster->GetAuraEffect(233978, EFFECT_0)) // Kam Xi'raff
                        {
                            int32 duration = 1000 + aurEff->GetAmount() * count * 1000;
                            caster->CastSpellDuration(caster, 233997, true, duration);
                        }
                    }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_pri_lights_wrath_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_lights_wrath_SpellScript();
        }
};

// Shadow Word: Death - 32379
class spell_pri_shadow_word_death : public SpellScriptLoader
{
    public:
        spell_pri_shadow_word_death() : SpellScriptLoader("spell_pri_shadow_word_death") { }

        class spell_pri_shadow_word_death_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_shadow_word_death_SpellScript);

            float bp = 0.f;
            Unit* target = NULL;

            void HandleCast()
            {
                if (Unit* caster = GetCaster())
                    if (Unit* target = GetExplTargetUnit())
                    {
                        float bp0 = 1500.f;
                        if (target->isAlive())
                            caster->CastCustomSpell(caster, 190714, &bp0, NULL, NULL, true);
                        else
                            caster->CastSpell(caster, 190714, true);
                    }
            }

            void Register() override
            {
                OnFinishCast += SpellCastFn(spell_pri_shadow_word_death_SpellScript::HandleCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_shadow_word_death_SpellScript();
        }
};

// Shadow Mend - 186263
class spell_pri_shadow_mend : public SpellScriptLoader
{
    public:
        spell_pri_shadow_mend() : SpellScriptLoader("spell_pri_shadow_mend") { }

        class spell_pri_shadow_mend_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_shadow_mend_SpellScript);

            void HandleHeal(SpellEffIndex /*effIndex*/)
            {
                if (Unit* caster = GetCaster())
                    if (Unit* target = GetHitUnit())
                    {
                        float bp0 = GetHitHeal() / 20.f;
                        float bp1 = GetHitHeal() / 2.f;

                        if (caster->GetGUID() == target->GetGUID() && caster->HasAura(193063)) // Masochism
                        {
                            caster->CastCustomSpell(caster, 193065, &bp0, NULL, NULL, true);
                            return;
                        }

                        caster->CastCustomSpell(target, 187464, &bp0, &bp1, NULL, true);
                    }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_pri_shadow_mend_SpellScript::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_shadow_mend_SpellScript();
        }
};

// Clarity of Will - 152118
class spell_pri_clarity_of_will : public SpellScriptLoader
{
    public:
        spell_pri_clarity_of_will() : SpellScriptLoader("spell_pri_clarity_of_will") { }

        class spell_pri_clarity_of_will_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_clarity_of_will_AuraScript);

            void CalculateAmount(AuraEffect const* aurEff, float& amount, bool& /*canBeRecalculated*/)
            {
                int32 pctHP = amount * 2;
                if (Unit* target = GetUnitOwner())
                {
                    if(AuraEffect const* aurEff = target->GetAuraEffect(194384, EFFECT_1))
                        amount += CalculatePct(amount, aurEff->GetAmount());
                }
                amount += aurEff->GetOldBaseAmount();
                if (amount > pctHP)
                    amount = pctHP;
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_clarity_of_will_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_clarity_of_will_AuraScript();
        }
};

// Power Word: Shield - 17, 210027
class spell_pri_power_word_shield : public SpellScriptLoader
{
    public:
        spell_pri_power_word_shield() : SpellScriptLoader("spell_pri_power_word_shield") { }

        class spell_pri_power_word_shield_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_power_word_shield_AuraScript);

            void CalculateAmount(AuraEffect const* aurEff, float& amount, bool& /*canBeRecalculated*/)
            {
                if (Unit* caster = GetCaster())
                {
                    if(AuraEffect const* aurEff = caster->GetAuraEffect(194384, EFFECT_1))
                        amount += CalculatePct(amount, aurEff->GetAmount());
                    if(AuraEffect const* aurEff = caster->GetAuraEffect(214576, EFFECT_1)) // legendary item bonus
                        amount += CalculatePct(amount, aurEff->GetAmount());
                    if(AuraEffect const* aurEff = caster->GetAuraEffect(197729, EFFECT_0)) // artifact perk
                        amount += CalculatePct(amount, aurEff->GetAmount());
                    if(AuraEffect const* aurEff = caster->GetAuraEffect(238135, EFFECT_0)) // artifact perk
                        amount += CalculatePct(amount, aurEff->GetAmount());
                }
            }

            /*void OnTick(AuraEffect const* aurEff)
            {
                Unit* caster = GetCaster();
                if (!caster)
                    return;

                if (caster->HasAura(238135))
                    if (AuraEffect* aurEff0 = const_cast<AuraEffect*>(aurEff->GetBase()->GetEffect(EFFECT_0)))
                        aurEff0->ChangeAmount(aurEff0->GetAmount() - aurEff->GetAmount());
            }*/

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_power_word_shield_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                //OnEffectPeriodic += AuraEffectPeriodicFn(spell_pri_power_word_shield_AuraScript::OnTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_power_word_shield_AuraScript();
        }

        class spell_pri_power_word_shield_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_power_word_shield_SpellScript);

            void HandleOnHit()
            {
                if (auto player = GetCaster())
                    if (auto target = GetHitUnit())
                        if (target != player && player->HasAura(197781))
                        {
                            player->CastSpell(player, 210027, true);
                        }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_pri_power_word_shield_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_power_word_shield_SpellScript;
        }
};

// Leap of Faith - 73325
class spell_pri_leap_of_faith : public SpellScriptLoader
{
    public:
        spell_pri_leap_of_faith() : SpellScriptLoader("spell_pri_leap_of_faith") { }

        class spell_pri_leap_of_faith_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_leap_of_faith_SpellScript);

            void HandleOnHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                {
                    if (Unit* target = GetHitUnit())
                    {
                        if(_player->HasAura(196611)) // Delivered from Evil(Honor Talent)
                            target->RemoveAurasWithMechanic((1<<MECHANIC_SNARE)|(1<<MECHANIC_ROOT));
                        target->CastSpell(_player, 92832, true);
                    }
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_pri_leap_of_faith_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_leap_of_faith_SpellScript;
        }
};

// Desperate Prayer - 19236
class spell_pri_desperate_prayer : public SpellScriptLoader
{
    public:
        spell_pri_desperate_prayer() : SpellScriptLoader("spell_pri_desperate_prayer") { }

        class spell_pri_desperate_prayer_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_desperate_prayer_AuraScript);

            void OnTick(AuraEffect const* aurEff)
            {
                if (AuraEffect* aurEff0 = const_cast<AuraEffect*>(aurEff->GetBase()->GetEffect(EFFECT_0)))
                    //if (aurEff0->GetAmount() >= aurEff->GetAmount())
                        aurEff0->ChangeAmount(aurEff0->GetAmount() - aurEff->GetAmount());
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_pri_desperate_prayer_AuraScript::OnTick, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_desperate_prayer_AuraScript();
        }
};

// Lingering Insanity - 197937
class spell_pri_lingering_insanity : public SpellScriptLoader
{
    public:
        spell_pri_lingering_insanity() : SpellScriptLoader("spell_pri_lingering_insanity") { }

        class spell_pri_lingering_insanity_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_lingering_insanity_AuraScript);

            void OnTick(AuraEffect const* /*aurEff*/)
            {
                if (Unit* caster = GetCaster())
                    if(AuraEffect const* aurEff = caster->GetAuraEffect(199849, EFFECT_0))
                        GetAura()->ModStackAmount(-aurEff->GetAmount());
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_pri_lingering_insanity_AuraScript::OnTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_lingering_insanity_AuraScript();
        }
};

// Purify - 527
class spell_pri_purify : public SpellScriptLoader
{
    public:
        spell_pri_purify() : SpellScriptLoader("spell_pri_purify") { }

        class spell_pri_purify_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_purify_SpellScript);

            void OnSuccessfulDispel(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                Unit* target = GetHitUnit();
                if (!caster || !target)
                    return;

                if (AuraEffect* aurEff = caster->GetAuraEffect(196439, 0)) // Purified Resolve (Honor Talent)
                {
                    float absort = target->CountPctFromMaxHealth(aurEff->GetAmount());
                    caster->CastCustomSpell(target, 196440, &absort, NULL, NULL, true);
                }
            }

            void Register() override
            {
                OnEffectSuccessfulDispel += SpellEffectFn(spell_pri_purify_SpellScript::OnSuccessfulDispel, EFFECT_0, SPELL_EFFECT_DISPEL);
                OnEffectSuccessfulDispel += SpellEffectFn(spell_pri_purify_SpellScript::OnSuccessfulDispel, EFFECT_1, SPELL_EFFECT_DISPEL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_purify_SpellScript();
        }
};

// Ray of Hope (Honor Talent) - 197268
class spell_pri_ray_of_hopet : public SpellScriptLoader
{
    public:
        spell_pri_ray_of_hopet() : SpellScriptLoader("spell_pri_ray_of_hopet") { }

        class spell_pri_ray_of_hopet_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_ray_of_hopet_AuraScript);

            float absorbDamage = 0.f;
            float absorbHeals = 0.f;

            void CalculateAmount(AuraEffect const* /*aurEff*/, float & amount, bool & /*canBeRecalculated*/)
            {
                amount = -1;
            }

            void AbsorbDam(AuraEffect* /*aurEff*/, DamageInfo & dmgInfo, float & absorbAmount)
            {
                absorbAmount = dmgInfo.GetDamage();
                absorbDamage += dmgInfo.GetDamage();
            }

            void AbsorbHeal(AuraEffect* /*aurEff*/, DamageInfo & dmgInfo, float & absorbAmount)
            {
                absorbAmount = dmgInfo.GetDamage();
                absorbHeals += dmgInfo.GetDamage() + CalculatePct(dmgInfo.GetDamage(), GetSpellInfo()->Effects[EFFECT_4]->CalcValue(GetCaster()));
            }

            void OnRemove0(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                    if (Unit* target = GetTarget())
                        caster->CastCustomSpell(target, 197341, &absorbDamage, NULL, NULL, true);
            }

            void OnRemove1(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                    if (Unit* target = GetTarget())
                        caster->CastCustomSpell(target, 197336, &absorbHeals, NULL, NULL, true);
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_ray_of_hopet_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_ray_of_hopet_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_SCHOOL_HEAL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_pri_ray_of_hopet_AuraScript::AbsorbDam, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_pri_ray_of_hopet_AuraScript::AbsorbHeal, EFFECT_1, SPELL_AURA_SCHOOL_HEAL_ABSORB);
                OnEffectRemove += AuraEffectRemoveFn(spell_pri_ray_of_hopet_AuraScript::OnRemove0, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_pri_ray_of_hopet_AuraScript::OnRemove1, EFFECT_1, SPELL_AURA_SCHOOL_HEAL_ABSORB, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_ray_of_hopet_AuraScript();
        }
};

// Shadow Mania - 199572
class spell_pri_shadow_mania : public SpellScriptLoader
{
    public:
        spell_pri_shadow_mania() : SpellScriptLoader("spell_pri_shadow_mania") { }

        class spell_pri_shadow_mania_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_shadow_mania_AuraScript);

            void OnTick(AuraEffect const* aurEff)
            {
                if (Unit* caster = GetCaster())
                {
                    if (caster->GetCountMyAura(34914) >= aurEff->GetAmount())
                    {
                        if (!caster->HasAura(199579))
                            caster->CastSpell(caster, 199579, true);
                    }
                    else
                        caster->RemoveAurasDueToSpell(199579);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_pri_shadow_mania_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_shadow_mania_AuraScript();
        }
};

// Premonition - 209780
class spell_pri_premonition : public SpellScriptLoader
{
    public:
        spell_pri_premonition() : SpellScriptLoader("spell_pri_premonition") { }

        class spell_pri_premonition_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_premonition_SpellScript);

            void HandleOnCast()
            {
                if(Unit* caster = GetCaster())
                {
                    if (caster->GetCountMyAura(194384) > 0)
                        caster->CastSpell(caster, 209885, true);

                    caster->CastSpell(caster, 194384, true);
                }
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_pri_premonition_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_premonition_SpellScript();
        }
};

// Premonition (Honor Talent) - 209885
class spell_pri_premonition_pvp : public SpellScriptLoader
{
    public:
        spell_pri_premonition_pvp() : SpellScriptLoader("spell_pri_premonition_pvp") { }

        class spell_pri_premonition_pvp_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_premonition_pvp_SpellScript);

            void HandleDamage(SpellEffIndex /*effIndex*/)
            {
                SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(209780);
                if(!spellInfo)
                    return;

                if (Unit* caster = GetCaster())
                {
                    std::list<TargetInfo>* memberList = GetSpell()->GetUniqueTargetInfo();
                    if(memberList->empty())
                        return;

                    int32 duration = spellInfo->Effects[EFFECT_0]->BasePoints;
                    uint32 count = 0;
                    for (std::list<TargetInfo>::iterator ihit = memberList->begin(); ihit != memberList->end(); ++ihit)
                    {
                        if (ihit->effectMask & (1 << EFFECT_0))
                        {
                            if(Unit* member = ObjectAccessor::GetUnit(*caster, ihit->targetGUID))
                                if(Aura* aura = member->GetAura(194384))
                                    aura->SetDuration(aura->GetDuration() + duration);
                            ++count;
                        }
                    }
                    if (!count)
                        return;

                    SetHitDamage(caster->GetSpellPowerDamage(spellInfo->GetSchoolMask()) * spellInfo->Effects[EFFECT_0]->BonusCoefficient * count);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_pri_premonition_pvp_SpellScript::HandleDamage, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_premonition_pvp_SpellScript();
        }
};

// Mental Fortitude - 194022
class spell_pri_mental_fortitude : public SpellScriptLoader
{
    public:
        spell_pri_mental_fortitude() : SpellScriptLoader("spell_pri_mental_fortitude") { }

        class spell_pri_mental_fortitude_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_mental_fortitude_AuraScript);

            void CalculateAmount(AuraEffect const* aurEff, float & amount, bool & /*canBeRecalculated*/)
            {
                if (Unit* target = GetUnitOwner())
                {
                    amount += aurEff->GetOldBaseAmount();
                    int32 pctHP = 0;
                    if(AuraEffect const* aurEff0 = target->GetAuraEffect(194018, EFFECT_0)) // Mental Fortitude
                        pctHP = target->CountPctFromMaxHealth(aurEff0->GetAmount());

                    if (amount > pctHP)
                        amount = pctHP;
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_mental_fortitude_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_mental_fortitude_AuraScript();
        }
};

// Penance - 47666
class spell_pri_penance : public SpellScriptLoader
{
    public:
        spell_pri_penance() : SpellScriptLoader("spell_pri_penance") { }

        class spell_pri_penance_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_penance_SpellScript);

            void HandleDamage(SpellEffIndex /*effIndex*/)
            {
                if (Unit* caster = GetCaster())
                {
                    if (!caster->HasAura(225683)) // Promises of N'ero
                        return;

                    uint32 heal_ = GetHitDamage();
                    if (Unit::AuraList* _auraList = caster->GetMyAura(81782))
                        for (Unit::AuraList::iterator itr = _auraList->begin(); itr != _auraList->end(); ++itr)
                            if (Aura* _aura = (*itr))
                                if (Unit* unitTarget = _aura->GetUnitOwner())
                                    caster->HealBySpell(unitTarget, GetSpellInfo(), heal_);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_pri_penance_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_penance_SpellScript();
        }
};

// Vampiric Touch - 34914
class spell_pri_vampiric_touch : public SpellScriptLoader
{
    public:
        spell_pri_vampiric_touch() : SpellScriptLoader("spell_pri_vampiric_touch") { }

        class spell_pri_vampiric_touch_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_vampiric_touch_AuraScript);

            void HandleDispel(DispelInfo* dispelInfo)
            {
                Unit* caster = GetCaster();
                Unit* dispeller = dispelInfo->GetDispeller();
                if (!caster || !dispeller)
                    return;

                dispeller->CastSpell(dispeller, 87204, true, 0, NULL, caster->GetGUID());
            }

            void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
            {
                if (GetCaster()->HasAura(238558))
                    GetCaster()->CastSpell(GetTarget(), 589, true);
            }

            void Register() override
            {
                OnDispel += AuraDispelFn(spell_pri_vampiric_touch_AuraScript::HandleDispel);
                AfterEffectApply += AuraEffectApplyFn(spell_pri_vampiric_touch_AuraScript::OnApply, EFFECT_1, SPELL_AURA_PERIODIC_LEECH, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_vampiric_touch_AuraScript();
        }
};

// Penance - 47540
class spell_pri_penance_cast : public SpellScriptLoader
{
    public:
        spell_pri_penance_cast() : SpellScriptLoader("spell_pri_penance_cast") { }
 
        class spell_pri_penance_cast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_penance_cast_SpellScript);
  
            SpellCastResult CheckCast()
            {
                if (Unit* caster = GetCaster())
                    if (Unit* target = GetExplTargetUnit())
                        if (caster->IsFriendlyTo(target) && !caster->HasAura(200347))
                            return SPELL_FAILED_BAD_TARGETS;
                return SPELL_CAST_OK;
            }
			
            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Player* _player = GetCaster()->ToPlayer())
                {
                    if (Unit* unitTarget = GetHitUnit())
                    {
                        if (!unitTarget->isAlive())
                            return;
 
                        if (_player->IsFriendlyTo(unitTarget))
                            _player->CastSpell(unitTarget, 47757, true);
                        else
                            _player->CastSpell(unitTarget, 47758, true);
                    }
                }
            }
 
            void Register() override
            {
                OnEffectLaunchTarget += SpellEffectFn(spell_pri_penance_cast_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnCheckCast += SpellCheckCastFn(spell_pri_penance_cast_SpellScript::CheckCast);
            }
        };
 
        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_penance_cast_SpellScript;
        }
};

// Smite - 208772 
class spell_pri_smite_dummy : public SpellScriptLoader
{
    public:
        spell_pri_smite_dummy() : SpellScriptLoader("spell_pri_smite_dummy") { }

        class spell_pri_smite_dummy_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_smite_dummy_AuraScript);

            void CalculateDummy(AuraEffect const* aurEff, float & amount, bool & canBeRecalculated)
            {
                if (Unit* caster = GetCaster())
                    if (AuraEffect* aurEff = caster->GetAuraEffect(231682, EFFECT_0))
                        amount = CalculatePct(caster->GetSpellPowerDamage(SPELL_SCHOOL_MASK_HOLY), aurEff->GetAmount());   
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_smite_dummy_AuraScript::CalculateDummy, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_smite_dummy_AuraScript();
        }
};

// Smite - 208771 
class spell_pri_smite_absorb : public SpellScriptLoader
{
    public:
        spell_pri_smite_absorb() : SpellScriptLoader("spell_pri_smite_absorb") { }

        class spell_pri_smite_absorb_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_smite_absorb_AuraScript);

            void Absorb(AuraEffect* /*aurEff*/, DamageInfo & dmgInfo, float & absorbAmount)
            {
                Unit* caster = GetCaster();
                Unit* attacker = dmgInfo.GetAttacker();
                if (!caster || !attacker)
                    return;

                AuraEffect* aurEff = attacker->GetAuraEffect(208772, EFFECT_0);
                if (!aurEff || !aurEff->GetBase() || caster != aurEff->GetBase()->GetCaster())
                    return;

                absorbAmount = aurEff->GetAmount();
                aurEff->GetBase()->Remove();
            }

            void Register() override
            {
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_pri_smite_absorb_AuraScript::Absorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_smite_absorb_AuraScript();
        }
};

// 232698 - Shadowform
class spell_pri_shadowform : public SpellScriptLoader
{
    public:
        spell_pri_shadowform() : SpellScriptLoader("spell_pri_shadowform") { }

        class spell_pri_shadowform_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_shadowform_AuraScript);

            uint32 update = 0;

            void OnUpdate(uint32 diff, AuraEffect* /*aurEff*/)
            {
                auto caster = GetCaster();
                if (!caster)
                {
                    update = 0;
                    return;
                }

                update += diff;

                if (update >= 500)
                {
                    // check power
                    switch (RoundingFloatValue(caster->GetPowerPct(POWER_INSANITY)))
                    {
                        case 0:
                            caster->CastSpell(caster, 185908, true);
                            break;
                        case 25:
                            caster->RemoveAurasDueToSpell(185908);
                            caster->CastSpell(caster, 185909, true);
                            break;
                        case 50:
                            caster->RemoveAurasDueToSpell(185909);
                            caster->CastSpell(caster, 185910, true);
                            break;
                        case 75:
                            caster->RemoveAurasDueToSpell(185910);
                            caster->CastSpell(caster, 185911, true);
                            break;
                    }
                }
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (auto caster = GetCaster())
                {
                    caster->RemoveAurasDueToSpell(185908);
                    caster->RemoveAurasDueToSpell(185909);
                    caster->RemoveAurasDueToSpell(185910);
                    caster->RemoveAurasDueToSpell(185911);
                }
            }

            void Register() override
            {
                OnEffectUpdate += AuraEffectUpdateFn(spell_pri_shadowform_AuraScript::OnUpdate, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
                AfterEffectRemove += AuraEffectRemoveFn(spell_pri_shadowform_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_shadowform_AuraScript();
        }
};

// 81751 - Atonement (Heal)
class spell_pri_atonement_heal : public SpellScript
{
    PrepareSpellScript(spell_pri_atonement_heal);

    void HandleOnHit(SpellEffIndex /*effIndex*/)
    {
        auto caster = GetCaster();
        auto target = GetHitUnit();
        if (!caster || !target)
            return;

        if (GetSpell()->IsCritForTarget(target))
        {
            float mod = caster->CanPvPScalar() ? 0.75f : 0.5f;
            SetHitHeal(GetHitHeal() * mod);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_pri_atonement_heal::HandleOnHit, EFFECT_0, SPELL_EFFECT_HEAL);
    }
};

void AddSC_priest_spell_scripts()
{
    new spell_pri_spirit_shell();
    new spell_pri_halo_heal();
    new spell_pri_void_shift();
    new spell_pri_guardian_spirit();
    new spell_pri_voidform();
    new spell_pri_divine_hymn();
    new spell_pri_lights_wrath();
    new spell_pri_shadow_word_death();
    new spell_pri_shadow_mend();
    new spell_pri_clarity_of_will();
    new spell_pri_power_word_shield();
    new spell_pri_leap_of_faith();
    new spell_pri_desperate_prayer();
    new spell_pri_lingering_insanity();
    new spell_pri_purify();
    new spell_pri_ray_of_hopet();
    new spell_pri_shadow_mania();
    new spell_pri_premonition();
    new spell_pri_premonition_pvp();
    new spell_pri_mental_fortitude();
    new spell_pri_penance();
    new spell_pri_vampiric_touch();
    new spell_pri_penance_cast();
    new spell_pri_smite_dummy();
    new spell_pri_smite_absorb();
    new spell_pri_shadowform();
    RegisterSpellScript(spell_pri_atonement_heal);
}