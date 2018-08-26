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
 * Scripts for spells with SPELLFAMILY_MAGE and SPELLFAMILY_GENERIC spells used by mage players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_mage_".
 */

#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "ScriptedCreature.h"
#include "GridNotifiers.h"

// Cauterize - 86949
class spell_mage_cauterize : public SpellScriptLoader
{
    public:
        spell_mage_cauterize() : SpellScriptLoader("spell_mage_cauterize") { }

        class spell_mage_cauterize_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_cauterize_AuraScript);

            uint32 healtPct;

            bool Load() override
            {
                healtPct = GetSpellInfo()->Effects[EFFECT_1]->CalcValue(GetCaster());
                return GetUnitOwner()->ToPlayer();
            }

            void CalculateAmount(AuraEffect const* /*AuraEffect**/, float& amount, bool& /*canBeRecalculated*/)
            {
                amount = -1;
            }

            void Absorb(AuraEffect* /*AuraEffect**/, DamageInfo& dmgInfo, float& absorbAmount)
            {
                Unit* target = GetCaster();

                if (dmgInfo.GetDamage() < target->GetHealth())
                    return;

                if (target->ToPlayer()->HasAura(87024))
                    return;

                float bp1 = target->CountPctFromMaxHealth(healtPct);
                target->CastCustomSpell(target, 87023, NULL, &bp1, NULL, true);
                target->CastCustomSpell(target, 87024, NULL, &bp1, NULL, true);

                absorbAmount = dmgInfo.GetDamage();
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_cauterize_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_mage_cauterize_AuraScript::Absorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_cauterize_AuraScript();
        }
};

class CheckArcaneBarrageImpactPredicate
{
    public:
        CheckArcaneBarrageImpactPredicate(Unit* caster, Unit* mainTarget) : _caster(caster), _mainTarget(mainTarget) {}

        bool operator()(Unit* target)
        {
            if (!_caster || !_mainTarget)
                return true;

            if (!_caster->IsValidAttackTarget(target))
                return true;

            if (!target->IsWithinLOSInMap(_caster))
                return true;

            if (!_caster->isInFront(target))
                return true;

            if (target->GetGUID() == _caster->GetGUID())
                return true;

            if (target->GetGUID() == _mainTarget->GetGUID())
                return true;

            return false;
        }

    private:
        Unit* _caster;
        Unit* _mainTarget;
};

// Arcane Barrage - 44425
class spell_mage_arcane_barrage : public SpellScriptLoader
{
    public:
        spell_mage_arcane_barrage() : SpellScriptLoader("spell_mage_arcane_barrage") { }

        class spell_mage_arcane_barrage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_arcane_barrage_SpellScript);

            void HandleEffectHit(SpellEffIndex /*effIndex*/)
            {
                if (Player* _player = GetCaster()->ToPlayer())
                {
                    if (Unit* target = GetHitUnit())
                    {
                        if (_player->GetSelectedUnit() != target)
                        {
                            int32 perc = GetSpellInfo()->Effects[EFFECT_1]->BasePoints;
                            SetHitDamage(CalculatePct(GetHitDamage(), perc));
                        }
                        if (AuraEffect const* aurEff = _player->GetAuraEffect(205028, EFFECT_0))
                            SetHitDamage(GetHitDamage() + CalculatePct(GetHitDamage(), aurEff->GetAmount() * GetSpell()->GetTargetCount()));
                    }
                    _player->ModifyPower(POWER_ARCANE_CHARGES, -_player->GetPower(POWER_ARCANE_CHARGES));
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_mage_arcane_barrage_SpellScript::HandleEffectHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_arcane_barrage_SpellScript();
        }
};

// Flurry - 44614
class spell_mage_flurry : public SpellScriptLoader
{
public:
    spell_mage_flurry() : SpellScriptLoader("spell_mage_flurry") {}

    class spell_mage_flurry_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_mage_flurry_SpellScript);

        void HandleLaunchTarget(SpellEffIndex /*effIndex*/)
        {
            if (Unit* caster = GetCaster())
            {
                if (Unit* target = GetHitUnit())
                {
                    if (SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(228354))
                    {
                        float damage = 1.f;
                        std::vector<uint32> ExcludeAuraList;
                        damage = caster->SpellDamageBonusDone(target, spellInfo, uint32(damage), SPELL_DIRECT_DAMAGE, ExcludeAuraList, EFFECT_1);
                        if (AuraEffect const* aurEff = caster->GetAuraEffect(251859, EFFECT_0)) // Item - Mage T21 Frost 2P Bonus
                            AddPct(damage, aurEff->GetAmount());

                        if (Aura* aura = caster->GetAura(231584))
                        {
                            if (AuraEffect* eff = aura->GetEffect(EFFECT_0))
                            {
                                if (caster->HasAura(190446))
                                {
                                    eff->SetAmount(1.f);
                                }
                                else
                                {
                                    eff->SetAmount(0.f);
                                }
                            }
                        }

                        caster->CastCustomSpell(target, 228596, &damage, NULL, NULL, true);

                        ObjectGuid targetGUID = target->GetGUID();
                        caster->AddDelayedEvent(300, [caster, targetGUID, damage]() -> void
                        {
                            Unit* target = ObjectAccessor::GetUnit(*caster, targetGUID);
                            if (!target)
                                return;

                            caster->CastCustomSpell(target, 228596, &damage, NULL, NULL, true);
                        });

                        caster->AddDelayedEvent(600, [caster, targetGUID, damage]() -> void
                        {
                            Unit* target = ObjectAccessor::GetUnit(*caster, targetGUID);
                            if (!target)
                                return;

                            caster->CastCustomSpell(target, 228596, &damage, NULL, NULL, true);
                        });
                        
                    }
                }
            }
        }

        void Register() override
        {
            OnEffectLaunchTarget += SpellEffectFn(spell_mage_flurry_SpellScript::HandleLaunchTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_mage_flurry_SpellScript();
    }
};

class CheckNetherImpactPredicate
{
    public:
        CheckNetherImpactPredicate(Unit* caster, Unit* mainTarget) : _caster(caster), _mainTarget(mainTarget) {}

        bool operator()(Unit* target)
        {
            if (!_caster || !_mainTarget)
                return true;

            if (!_caster->IsValidAttackTarget(target))
                return true;

            if (!target->IsWithinLOSInMap(_caster))
                return true;

            if (!_caster->isInFront(target))
                return true;

            if (target->GetGUID() == _caster->GetGUID())
                return true;

            if (target->GetGUID() == _mainTarget->GetGUID())
                return true;

            return false;
        }

    private:
        Unit* _caster;
        Unit* _mainTarget;
};

class CheckInfernoBlastImpactPredicate
{
    public:
        CheckInfernoBlastImpactPredicate(Unit* caster, Unit* mainTarget) : _caster(caster), _mainTarget(mainTarget) {}

        bool operator()(Unit* target)
        {
            if (!_caster || !_mainTarget)
                return true;

            if (!_caster->IsValidAttackTarget(target))
                return true;

            if (!target->IsWithinLOSInMap(_caster))
                return true;

            if (!_caster->isInFront(target))
                return true;

            if (target->GetGUID() == _caster->GetGUID())
                return true;

            if (target->GetGUID() == _mainTarget->GetGUID())
                return true;

            return false;
        }

    private:
        Unit* _caster;
        Unit* _mainTarget;
};

// Conjure Refreshment - 42955, 190336
class spell_mage_conjure_refreshment : public SpellScriptLoader
{
    public:
        spell_mage_conjure_refreshment() : SpellScriptLoader("spell_mage_conjure_refreshment") { }

        class spell_mage_conjure_refreshment_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_conjure_refreshment_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Player* _player = GetCaster()->ToPlayer())
                {
                    Group* group = _player->GetGroup();
                    if (group)
                    {
                        if (_player->getLevel() < 80)
                            _player->CastSpell(_player, 120056, true);  // Conjure Refreshment Table
                        else if (_player->getLevel() < 85)
                            _player->CastSpell(_player, 120055, true);  // Conjure Refreshment Table
                        else if (_player->getLevel() < 90)
                            _player->CastSpell(_player, 120054, true);  // Conjure Refreshment Table
                        else
                            _player->CastSpell(_player, 120053, true);  // Conjure Refreshment Table
                    }
                    else
                    {
                        if (_player->getLevel() < 44)
                            _player->CastSpell(_player, 92739, true);
                        else if (_player->getLevel() < 54)
                            _player->CastSpell(_player, 92799, true);
                        else if (_player->getLevel() < 64)
                            _player->CastSpell(_player, 92802, true);
                        else if (_player->getLevel() < 74)
                            _player->CastSpell(_player, 92805, true);
                        else if (_player->getLevel() < 80)
                            _player->CastSpell(_player, 74625, true);
                        else if (_player->getLevel() < 85)
                            _player->CastSpell(_player, 42956, true);
                        else if (_player->getLevel() < 90)
                            _player->CastSpell(_player, 92727, true);
                        else
                            _player->CastSpell(_player, 116130, true);
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_mage_conjure_refreshment_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_conjure_refreshment_SpellScript();
        }
};

// Time Warp - 80353
class spell_mage_time_warp : public SpellScriptLoader
{
    public:
        spell_mage_time_warp() : SpellScriptLoader("spell_mage_time_warp") { }

        class spell_mage_time_warp_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_time_warp_SpellScript);

            void RemoveInvalidTargets(std::list<WorldObject*>& targets)
            {
                std::list<WorldObject*> removeList;

                for (auto itr : targets)
                {
                    if (Unit* unit = itr->ToUnit())
                    {
                        if (unit->HasAura(207970)) // Shard of the Exodar
                            continue;

                        if (unit->HasAura(95809) || unit->HasAura(57723) || unit->HasAura(57724) || unit->HasAura(80354) || unit->HasAura(160455))
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
                    if (!target->HasAura(207970)) // Shard of the Exodar
                        target->CastSpell(target, 80354, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mage_time_warp_SpellScript::RemoveInvalidTargets, EFFECT_0, TARGET_UNIT_CASTER_AREA_RAID);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mage_time_warp_SpellScript::RemoveInvalidTargets, EFFECT_1, TARGET_UNIT_CASTER_AREA_RAID);
                AfterHit += SpellHitFn(spell_mage_time_warp_SpellScript::ApplyDebuff);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_time_warp_SpellScript();
        }
};

// Alter Time - 127140 (overrided)
class spell_mage_alter_time_overrided : public SpellScriptLoader
{
    public:
        spell_mage_alter_time_overrided() : SpellScriptLoader("spell_mage_alter_time_overrided") { }

        class spell_mage_alter_time_overrided_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_alter_time_overrided_SpellScript);

            bool Validate(SpellInfo const* /*SpellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(127140))
                    return false;
                return true;
            }

            void HandleAfterCast()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                    _player->RemoveAurasDueToSpell(110909);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_mage_alter_time_overrided_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_alter_time_overrided_SpellScript();
        }
};

struct auraData
{
    auraData(uint32 id, int32 duration) : m_id(id), m_duration(duration) {}
    uint32 m_id;
    int32 m_duration;
};

// Alter Time - 110909
class spell_mage_alter_time : public SpellScriptLoader
{
    public:
        spell_mage_alter_time() : SpellScriptLoader("spell_mage_alter_time") { }

        class spell_mage_alter_time_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_alter_time_AuraScript);

            int32 mana;
            int32 health;
            float posX;
            float posY;
            float posZ;
            float orientation;
            uint32 map;
            std::set<auraData*> auras;

        public:
            spell_mage_alter_time_AuraScript() : AuraScript()
            {
                map = uint32(-1);
            }

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Player* _player = GetTarget()->ToPlayer())
                {
                    posX = _player->GetPositionX();
                    posY = _player->GetPositionY();
                    posZ = _player->GetPositionZ();
                    orientation = _player->GetOrientation();
                    map = _player->GetMapId();
                    Unit::AuraApplicationMap const& appliedAuras = _player->GetAppliedAuras();
                    for (Unit::AuraApplicationMap::const_iterator itr = appliedAuras.begin(); itr != appliedAuras.end(); ++itr)
                    {
                        if (Aura* aura = itr->second->GetBase())
                        {
                            SpellInfo const* auraInfo = aura->GetSpellInfo();

                            if (auraInfo->HasAttribute(SPELL_ATTR0_HIDDEN_CLIENTSIDE))
                                continue;

                            if (auraInfo->HasAttribute(SPELL_ATTR0_PASSIVE))
                                continue;

                            if (aura->IsArea() && aura->GetCasterGUID() != _player->GetGUID())
                                continue;

                            if (auraInfo->Id == 110909 || auraInfo->Id == 116014)
                                continue;

                            auras.insert(new auraData(auraInfo->Id, aura->GetDuration()));
                        }
                    }
                    mana = _player->GetPower(POWER_MANA);
                    health = _player->GetHealth();
                }
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (map == uint32(-1))
                    return;

                if (Player* _player = GetTarget()->ToPlayer())
                {
                    if (_player->GetMapId() != map)
                        return;

                    for (std::set<auraData*>::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
                    {
                        Aura* aura = !_player->HasAura((*itr)->m_id) ? _player->AddAura((*itr)->m_id, _player) : _player->GetAura((*itr)->m_id);
                        if (aura)
                        {
                            aura->SetDuration((*itr)->m_duration);
                            aura->SetNeedClientUpdateForTargets();
                        }

                        delete *itr;
                    }

                    auras.clear();

                    _player->SetPower(POWER_MANA, mana);
                    _player->SetHealth(health);
                    _player->TeleportTo(map, posX, posY, posZ, orientation);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_mage_alter_time_AuraScript::OnApply, EFFECT_0, SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_mage_alter_time_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_alter_time_AuraScript();
        }
};

enum MageSpells
{
    SPELL_MAGE_SQUIRREL_FORM                     = 32813,
    SPELL_MAGE_GIRAFFE_FORM                      = 32816,
    SPELL_MAGE_SERPENT_FORM                      = 32817,
    SPELL_MAGE_DRAGONHAWK_FORM                   = 32818,
    SPELL_MAGE_WORGEN_FORM                       = 32819,
    SPELL_MAGE_SHEEP_FORM                        = 32820,
};

enum SilvermoonPolymorph
{
    NPC_AUROSALIA   = 18744,
};

// TODO: move out of here and rename - not a mage spell
class spell_mage_polymorph_cast_visual : public SpellScriptLoader
{
    public:
        spell_mage_polymorph_cast_visual() : SpellScriptLoader("spell_mage_polymorph_visual") { }

        class spell_mage_polymorph_cast_visual_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_polymorph_cast_visual_SpellScript);

            static const uint32 PolymorhForms[6];

            bool Validate(SpellInfo const* /*SpellInfo*/) override
            {
                // check if spell ids exist in dbc
                for (uint32 i = 0; i < 6; i++)
                    if (!sSpellMgr->GetSpellInfo(PolymorhForms[i]))
                        return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Unit* target = GetCaster()->FindNearestCreature(NPC_AUROSALIA, 30.0f))
                    if (target->GetTypeId() == TYPEID_UNIT)
                        target->CastSpell(target, PolymorhForms[urand(0, 5)], true);
            }

            void Register() override
            {
                // add dummy effect spell handler to Polymorph visual
                OnEffectHitTarget += SpellEffectFn(spell_mage_polymorph_cast_visual_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_polymorph_cast_visual_SpellScript();
        }
};

const uint32 spell_mage_polymorph_cast_visual::spell_mage_polymorph_cast_visual_SpellScript::PolymorhForms[6] =
{
    SPELL_MAGE_SQUIRREL_FORM,
    SPELL_MAGE_GIRAFFE_FORM,
    SPELL_MAGE_SERPENT_FORM,
    SPELL_MAGE_DRAGONHAWK_FORM,
    SPELL_MAGE_WORGEN_FORM,
    SPELL_MAGE_SHEEP_FORM
};

// Greater Invisibility - 110960
class spell_mage_greater_invisibility : public SpellScriptLoader
{
    public:
        spell_mage_greater_invisibility() : SpellScriptLoader("spell_mage_greater_invisibility") { }

        class spell_mage_greater_invisibility_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_greater_invisibility_AuraScript);

            void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                {
                    int32 count = 0;
                    if (Unit::AuraEffectList const* mPeriodic = caster->GetAuraEffectsByType(SPELL_AURA_PERIODIC_DAMAGE))
                    {
                        for (Unit::AuraEffectList::const_iterator iter = mPeriodic->begin(); iter != mPeriodic->end(); ++iter)
                        {
                            if (!(*iter)) // prevent crash
                                continue;
                            Aura* aura = (*iter)->GetBase();
                            aura->Remove();
                            count++;
                            if(count > 1)
                                return;
                        }
                    }
                    caster->CastSpell(GetTarget(), 113862, true, NULL, aurEff);
                }
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                    if (Aura* aura = GetTarget()->GetAura(113862, caster->GetGUID()))
                    {
                        aura->SetDuration(3000);
                        aura->SetMaxDuration(3000);
                    }
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_mage_greater_invisibility_AuraScript::OnApply, EFFECT_1, SPELL_AURA_MOD_INVISIBILITY, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_mage_greater_invisibility_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_MOD_INVISIBILITY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_greater_invisibility_AuraScript();
        }
};

// Invisibility - 32612
// Greater Invisibility - 110960
class spell_elem_invisibility : public SpellScriptLoader
{
    public:
        spell_elem_invisibility() : SpellScriptLoader("spell_elem_invisibility") { }

        class spell_elem_invisibility_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_elem_invisibility_AuraScript);

        public:
 
            void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();
                if (target->GetOwnerGUID())
                    return;

                if (Guardian* pet = target->GetGuardianPet())
                    pet->CastSpell(pet, 96243, true);
            }

            void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();
                if (target->GetOwnerGUID())
                    return;

                if (Guardian* pet = target->GetGuardianPet())
                    pet->RemoveAurasDueToSpell(96243);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_elem_invisibility_AuraScript::OnApply, EFFECT_1, SPELL_AURA_MOD_INVISIBILITY, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_elem_invisibility_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_MOD_INVISIBILITY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_elem_invisibility_AuraScript();
        }
};

// Arcane Blast - 30451
class spell_mage_arcane_blast : public SpellScriptLoader
{
    public:
        spell_mage_arcane_blast() : SpellScriptLoader("spell_mage_arcane_blast") { }

        class spell_mage_arcane_blast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_arcane_blast_SpellScript);

            void HandleAfterHit()
            {
                Unit* caster = GetCaster();
                if (!caster)
                    return;

                caster->CastSpell(caster, GetSpellInfo()->Effects[1]->TriggerSpell, true);
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_mage_arcane_blast_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_arcane_blast_SpellScript();
        }
};

// Icicle - 148023 (periodic dummy)
class spell_mage_icicle : public SpellScriptLoader
{
    public:
        spell_mage_icicle() : SpellScriptLoader("spell_mage_icicle") { }

        class spell_mage_icicle_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_icicle_AuraScript);

            void OnTick(AuraEffect const* aurEff)
            {
                if (Unit* caster = GetCaster())
                if (Player* _player = caster->ToPlayer())
                if (Unit* target = _player->GetSelectedUnit())
                {
                    int32 addPctDamage = 0;
                    if (roll_chance_i(20))
                        if (AuraEffect const* aurEffPct = caster->GetAuraEffect(195615, EFFECT_0)) // Black Ice
                            addPctDamage += aurEffPct->GetAmount();

                    if (Aura* aur = caster->GetAura(205473))
                        aur->ModStackAmount(-1);

                    switch(aurEff->GetTickNumber())
                    {
                        case 1:
                        {
                            if (Aura* icicle = caster->GetAura(148012))
                            {
                                float tickamount = icicle->GetEffect(EFFECT_0)->GetAmount();
                                tickamount += CalculatePct(tickamount, addPctDamage);
                                caster->CastSpell(target, 148017, true);
                                caster->CastCustomSpell(target, 148022, &tickamount, NULL, NULL, true);
                                icicle->Remove();
                            }
                            break;
                        }
                        case 2:
                        {
                            if (Aura* icicle = caster->GetAura(148013))
                            {
                                float tickamount = icicle->GetEffect(EFFECT_0)->GetAmount();
                                tickamount += CalculatePct(tickamount, addPctDamage);
                                caster->CastSpell(target, 148018, true);
                                caster->CastCustomSpell(target, 148022, &tickamount, NULL, NULL, true);
                                icicle->Remove();
                            }
                            break;
                        }
                        case 3:
                        {
                            if (Aura* icicle = caster->GetAura(148014))
                            {
                                float tickamount = icicle->GetEffect(EFFECT_0)->GetAmount();
                                tickamount += CalculatePct(tickamount, addPctDamage);
                                caster->CastSpell(target, 148019, true);
                                caster->CastCustomSpell(target, 148022, &tickamount, NULL, NULL, true);
                                icicle->Remove();
                            }
                            break;
                        }
                        case 4:
                        {
                            if (Aura* icicle = caster->GetAura(148015))
                            {
                                float tickamount = icicle->GetEffect(EFFECT_0)->GetAmount();
                                tickamount += CalculatePct(tickamount, addPctDamage);
                                caster->CastSpell(target, 148020, true);
                                caster->CastCustomSpell(target, 148022, &tickamount, NULL, NULL, true);
                                icicle->Remove();
                            }
                            break;
                        }
                        case 5:
                        {
                            if (Aura* icicle = caster->GetAura(148016))
                            {
                                float tickamount = icicle->GetEffect(EFFECT_0)->GetAmount();
                                tickamount += CalculatePct(tickamount, addPctDamage);
                                caster->CastSpell(target, 148021, true);
                                caster->CastCustomSpell(target, 148022, &tickamount, NULL, NULL, true);
                                icicle->Remove();
                            }
                            break;
                        }
                        default:
                            GetAura()->Remove();
                            break;
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_mage_icicle_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_icicle_AuraScript();
        }
};

//Illusion glyph - 131784
class spell_mage_illusion : public SpellScriptLoader
{
    public:
        spell_mage_illusion() : SpellScriptLoader("spell_mage_illusion") { }

        class spell_mage_illusion_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_illusion_SpellScript)

            void HandleOnHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                {
                    /*if(Unit* target = _player->GetSelectedUnit())
                    {
                        if (target->GetTypeId() == TYPEID_PLAYER && target != GetCaster())
                        {
                            target->CastSpell(_player, 80396, true);
                            return;
                        }
                    }*/
                    _player->CastSpell(_player, 94632, true);
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_mage_illusion_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_illusion_SpellScript();
        }
};

// Flameglow - 140468
class spell_mage_flameglow : public SpellScriptLoader
{
    public:
        spell_mage_flameglow() : SpellScriptLoader("spell_mage_flameglow") { }

        class spell_mage_flameglow_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_flameglow_AuraScript);

            uint32 absorb;
            uint32 LimitAbsorb;
            uint32 TakenDamage;

            bool Load() override
            {
                absorb = 0;
                LimitAbsorb = 0;
                TakenDamage = 0;
                return true;
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, float & amount, bool & /*canBeRecalculated*/)
            {
                amount = -1;                
            }

            void Absorb(AuraEffect* /*aurEff*/, DamageInfo & dmgInfo, float & absorbAmount)
            {
                absorb = GetCaster()->GetSpellPowerDamage() * GetSpellInfo()->Effects[EFFECT_1]->BasePoints / 100;
                LimitAbsorb = GetSpellInfo()->Effects[EFFECT_2]->BasePoints;
                
                absorbAmount = CalculatePct(dmgInfo.GetDamage(), LimitAbsorb);
                if (absorbAmount > absorb)
                    absorbAmount = absorb;
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_flameglow_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_mage_flameglow_AuraScript::Absorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_flameglow_AuraScript();
        }
};

// Glyph of Conjure Familiar - 126748
enum CreateFamiliarStone
{
    ITEM_ICY_FAMILIAR_STONE_1 = 87259,
    ITEM_FIERY_FAMILIAR_STONE_2 = 87258,
    ITEM_ARCANE_FAMILIAR_STONE_3 = 87257,
};

class spell_mage_glyph_of_conjure_familiar : public SpellScriptLoader
{
    public:
        spell_mage_glyph_of_conjure_familiar() : SpellScriptLoader("spell_mage_glyph_of_conjure_familiar") { }

        class spell_mage_glyph_of_conjure_familiar_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_glyph_of_conjure_familiar_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                if (Player* target = GetHitPlayer())
                {
                    static const uint32 items[] = {ITEM_ICY_FAMILIAR_STONE_1, ITEM_FIERY_FAMILIAR_STONE_2, ITEM_ARCANE_FAMILIAR_STONE_3};
                    target->AddItem(items[urand(0, 2)], 1);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_mage_glyph_of_conjure_familiar_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_CREATE_ITEM_2);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_glyph_of_conjure_familiar_SpellScript();
        }
};

// Icicle - 148022
class spell_mage_icicle_damage : public SpellScriptLoader
{
    public:
        spell_mage_icicle_damage() : SpellScriptLoader("spell_mage_icicle_damage") { }

        class spell_mage_icicle_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_icicle_damage_SpellScript)

            void HandleDamage(SpellEffIndex /*effIndex*/)
            {
                if (Unit* target = GetHitUnit())
                {
                    if (target != GetExplTargetUnit())
                        SetHitDamage(int32(GetHitDamage() * 0.5f));
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_mage_icicle_damage_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_icicle_damage_SpellScript();
        }
};

// Supernova - 157980, Blast Wave - 157981, Ice Nova - 157997
class spell_mage_supernova : public SpellScriptLoader
{
    public:
        spell_mage_supernova() : SpellScriptLoader("spell_mage_supernova") { }

        class spell_mage_supernova_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_supernova_SpellScript);

            void HandleDamage(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (!caster)
                    return;

                if (Unit* target = GetHitUnit())
                {
                    if (target == GetExplTargetUnit())
                        SetHitDamage(GetHitDamage() * 2);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_mage_supernova_SpellScript::HandleDamage, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_supernova_SpellScript();
        }
};

// Incanter's Flow - 1463
class spell_mage_incanters_flow : public SpellScriptLoader
{
    public:
        spell_mage_incanters_flow() : SpellScriptLoader("spell_mage_incanters_flow") { }

        class spell_mage_incanters_flow_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_incanters_flow_AuraScript);

            bool direction = false;
            void OnTick(AuraEffect const* aurEff)
            {
                Unit* caster = GetCaster();
                if (!caster)
                    return;
                if (caster->isInCombat())
                {
                    if (Aura* aura = caster->GetAura(116267))
                    {
                        if (!direction)
                        {
                            if(aura->GetStackAmount() < 5)
                                aura->ModStackAmount(1);
                            else
                            {
                                direction = true;
                                aura->ModStackAmount(-1);
                            }
                        }
                        else if(direction)
                        {
                            if(aura->GetStackAmount() > 1)
                                aura->ModStackAmount(-1);
                            else
                            {
                                direction = false;
                                aura->ModStackAmount(1);
                            }
                        }
                    }
                    else
                        caster->CastSpell(caster, 116267, true);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_mage_incanters_flow_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_incanters_flow_AuraScript();
        }
};

// Erosion - 210154
class spell_mage_erosion : public SpellScriptLoader
{
    public:
        spell_mage_erosion() : SpellScriptLoader("spell_mage_erosion") { }

        class spell_mage_erosion_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_erosion_AuraScript);

            bool wait = false;

            void OnTick(AuraEffect const* aurEff)
            {
                Unit* caster = GetCaster();
                if (!caster)
                    return;

                if (Unit* target = GetUnitOwner())
                {
                    if (!wait && aurEff->GetTickNumber() == 3)
                    {
                        GetAura()->SetMaxDuration(11000);
                        GetAura()->SetDuration(11000);
                        wait = true;
                    }
                    else if (wait)
                    {
                        if (Aura* aura = target->GetAura(210134, caster->GetGUID()))
                            aura->ModStackAmount(-1);
                    }
                }
            }

            void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                wait = false;
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_mage_erosion_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                OnEffectApply += AuraEffectApplyFn(spell_mage_erosion_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_erosion_AuraScript();
        }
};

// Displacement - 195676, 212801
class spell_mage_displacement : public SpellScriptLoader
{
    public:
        spell_mage_displacement() : SpellScriptLoader("spell_mage_displacement") { }

        class spell_mage_displacement_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_displacement_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Unit* caster = GetCaster())
                {
                    std::list<AreaTrigger*> list;
                    caster->GetAreaObjectList(list, 212799);
                    if(!list.empty())
                    {
                        for (std::list<AreaTrigger*>::iterator itr = list.begin(); itr != list.end(); ++itr)
                        {
                            if(AreaTrigger* areaObj = (*itr))
                            {
                                caster->NearTeleportTo(areaObj->GetPositionX(), areaObj->GetPositionY(), areaObj->GetPositionZ(), areaObj->GetOrientation(), true);
                                areaObj->Despawn();
                                caster->RemoveAurasDueToSpell(212799);
                                return;
                            }
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_mage_displacement_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_displacement_SpellScript();
        }
};

// Combustion - 190319
class spell_mage_combustion : public SpellScriptLoader
{
    public:
        spell_mage_combustion() : SpellScriptLoader("spell_mage_combustion") { }

        class spell_mage_combustion_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_combustion_AuraScript);

            int32 lastData = 0;

            void CalculateAmount(AuraEffect const* aurEff, float & amount, bool & /*canBeRecalculated*/)
            {
                Unit* caster = GetCaster();
                if(!caster)
                    return;

                if (Player* _player = caster->ToPlayer())
                    lastData = amount = CalculatePct(_player->GetUInt32Value(PLAYER_FIELD_COMBAT_RATINGS + CR_CRIT_SPELL), 50);
            }

            void OnTick(AuraEffect const* aurEff)
            {
                Unit* caster = GetCaster();
                if(!caster)
                    return;

                if (Player* _player = caster->ToPlayer())
                {
                    uint32 crit = _player->GetUInt32Value(PLAYER_FIELD_COMBAT_RATINGS + CR_CRIT_SPELL);
                    int32 critData = CalculatePct(crit, 50); // 50%
                    if (lastData != critData)
                    {
                        lastData = critData;
                        if (Aura* aura = const_cast<Aura*>(aurEff->GetBase()))
                            if (AuraEffect* aurEff1 = aura->GetEffect(EFFECT_1))
                                aurEff1->ChangeAmount(lastData);
                    }
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_combustion_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_MOD_RATING);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_mage_combustion_AuraScript::OnTick, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_combustion_AuraScript();
        }
};

// Living Bomb - 217694
class spell_mage_living_bomb : public SpellScriptLoader
{
    public:
        spell_mage_living_bomb() : SpellScriptLoader("spell_mage_living_bomb") { }

        class spell_mage_living_bomb_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_living_bomb_AuraScript);

            void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                Unit* caster = GetCaster();
                Unit* target = GetUnitOwner();
                if (!caster || !target)
                    return;

                caster->CastSpell(target, 44461, true);
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_mage_living_bomb_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_living_bomb_AuraScript();
        }
};

// Living Bomb - 44461
class spell_mage_living_bomb_damage : public SpellScriptLoader
{
    public:
        spell_mage_living_bomb_damage() : SpellScriptLoader("spell_mage_living_bomb_damage") { }

        class spell_mage_living_bomb_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_living_bomb_damage_SpellScript);

            void HandleEffectHit(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                Unit* target = GetHitUnit();
                if (!caster || !target)
                    return;

                caster->CastSpell(target, 244813, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_mage_living_bomb_damage_SpellScript::HandleEffectHit, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_living_bomb_damage_SpellScript();
        }
};

// Frostbolt 228597 - for 76613 Mastery: Icicles
class spell_mage_frostbolt : public SpellScriptLoader
{
    public:
        spell_mage_frostbolt() : SpellScriptLoader("spell_mage_frostbolt") { }

        class spell_mage_frostbolt_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_frostbolt_SpellScript)

            void HandleBeforeHit()
            {
                if (Unit* caster = GetCaster())
                {
                    if (Unit* target = GetHitUnit())
                    {
                        // Icicle
                        if (caster->HasAura(148016))
                        {
                            int32 visual = 148017;
                            Aura* icicle2 = caster->GetAura(148012);
                            Aura* icicle3 = caster->GetAura(148013);
                            Aura* icicle4 = caster->GetAura(148014);
                            Aura* icicle5 = caster->GetAura(148015);
                            Aura* icicle6 = caster->GetAura(148016);

                            if(icicle2 && icicle3 && icicle4 && icicle5 && icicle6)
                            {
                                Aura* icicle = icicle2;
                                if(icicle->GetDuration() > icicle3->GetDuration())
                                {
                                    icicle = icicle3;
                                    visual = 148018;
                                }
                                if(icicle->GetDuration() > icicle4->GetDuration())
                                {
                                    icicle = icicle4;
                                    visual = 148019;
                                }
                                if(icicle->GetDuration() > icicle5->GetDuration())
                                {
                                    icicle = icicle5;
                                    visual = 148020;
                                }
                                if(icicle->GetDuration() > icicle6->GetDuration())
                                {
                                    icicle = icicle6;
                                    visual = 148021;
                                }

                                int32 addPctDamage = 0;
                                if (roll_chance_i(20))
                                    if (AuraEffect const* aurEffPct = caster->GetAuraEffect(195615, EFFECT_0)) // Black Ice
                                        addPctDamage += aurEffPct->GetAmount();

                                float tickamount = icicle->GetEffect(EFFECT_0)->GetAmount();
                                tickamount += CalculatePct(tickamount, addPctDamage);
                                caster->CastSpell(target, visual, true);
                                caster->CastCustomSpell(target, 148022, &tickamount, NULL, NULL, true);
                                icicle->Remove();
                            }
                        }
                    }
                }
            }

            void HandleAfterHit()
            {
                if (Unit* caster = GetCaster())
                {
                    if (Unit* target = GetHitUnit())
                    {
                        // Ice Nine
                        if (caster->HasAura(214664) && roll_chance_i(10))
                        {
                            int32 damagePct = 0;
                            if (AuraEffect const* aurEffPct = caster->GetAuraEffect(76613, EFFECT_0))
                                damagePct += aurEffPct->GetAmount();
                            float damage = CalculatePct(GetHitDamage(), damagePct);

                            if (caster->GetAura(148016))
                            {
                                if (Aura* icicle = caster->GetAura(148012))
                                {
                                    int32 addPctDamage = 0;
                                    if (roll_chance_i(20))
                                        if (AuraEffect const* aurEffPct = caster->GetAuraEffect(195615, EFFECT_0)) // Black Ice
                                            addPctDamage += aurEffPct->GetAmount();

                                    float tickamount = icicle->GetEffect(EFFECT_0)->GetAmount();
                                    tickamount += CalculatePct(tickamount, addPctDamage);
                                    caster->CastSpell(target, 148018, true);
                                    caster->CastSpell(caster, 205473, true);
                                    caster->CastCustomSpell(target, 148022, &tickamount, NULL, NULL, true);
                                    icicle->Remove();
                                }
                                caster->CastCustomSpell(target, 148012, &damage, NULL, NULL, true);
                            }
                            else if (caster->GetAura(148015))
                            {
                                caster->CastSpell(caster, 205473, true);
                                caster->CastCustomSpell(target, 148016, &damage, NULL, NULL, true);
                            }
                            else if (caster->GetAura(148014))
                            {
                                caster->CastSpell(caster, 205473, true);
                                caster->CastCustomSpell(target, 148015, &damage, NULL, NULL, true);
                            }
                            else if (caster->GetAura(148013))
                            {
                                caster->CastSpell(caster, 205473, true);
                                caster->CastCustomSpell(target, 148014, &damage, NULL, NULL, true);
                            }
                            else if (caster->GetAura(148012))
                            {
                                caster->CastSpell(caster, 205473, true);
                                caster->CastCustomSpell(target, 148013, &damage, NULL, NULL, true);
                            }
                        }
                    }
                }
            }

            void Register() override
            {
                BeforeHit += SpellHitFn(spell_mage_frostbolt_SpellScript::HandleBeforeHit);
                AfterHit += SpellHitFn(spell_mage_frostbolt_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_frostbolt_SpellScript();
        }
};

// Shimmer - 212653
class spell_mage_shimmer : public SpellScriptLoader
{
    public:
        spell_mage_shimmer() : SpellScriptLoader("spell_mage_shimmer") { }

        class spell_mage_shimmer_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_shimmer_SpellScript);

            SpellCastResult CheckCast()
            {
                if(Unit* caster = GetCaster())
                {
                    if (caster->HasUnitState(UNIT_STATE_CONFUSED))
                        return SPELL_FAILED_CONFUSED;
                    if (caster->HasUnitState(UNIT_STATE_FLEEING))
                        return SPELL_FAILED_FLEEING;
                    if (caster->HasUnitState(UNIT_STATE_STUNNED))
                        return SPELL_FAILED_STUNNED;
                }

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_mage_shimmer_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_shimmer_SpellScript();
        }
};

// Ice Lance - 228598
class spell_mage_ice_lance : public SpellScriptLoader
{
    public:
        spell_mage_ice_lance() : SpellScriptLoader("spell_mage_ice_lance") { }

        class spell_mage_ice_lance_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_ice_lance_SpellScript)

            void HandleEffectHit(SpellEffIndex /*effIndex*/)
            {
                if (Player* _player = GetCaster()->ToPlayer())
                {
                    if (Unit* target = GetHitUnit())
                    {
                        if (_player->GetSelectedUnit() != target)
                        {
                            if (SpellInfo const* spellinfo =  sSpellMgr->GetSpellInfo(56377))
                                SetHitDamage(CalculatePct(GetHitDamage(), spellinfo->Effects[EFFECT_1]->BasePoints));
                        }

                        if (_player->HasSpell(155149))
                        {
                            if (target->HasAuraState(AURA_STATE_FROZEN, GetSpellInfo(), _player))
                                if (Aura* aura = _player->GetAura(12472))
                                    aura->SetDuration(aura->GetDuration() + 1000);
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_mage_ice_lance_SpellScript::HandleEffectHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_ice_lance_SpellScript();
        }
};

// Glacial Spike - 199786
class spell_mage_glacial_spike : public SpellScriptLoader
{
    public:
        spell_mage_glacial_spike() : SpellScriptLoader("spell_mage_glacial_spike") { }

        class spell_mage_glacial_spike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_glacial_spike_SpellScript)

            void HandleOnCast()
            {
                if (Unit* caster = GetCaster())
                {
                    float damage = 0.f;
                    for (auto const spellID : {148012, 148013, 148014, 148015, 148016})
                    {
                        if (Aura* icicle = caster->GetAura(spellID))
                        {
                            if (icicle->GetEffect(EFFECT_0))
                                damage += icicle->GetEffect(EFFECT_0)->GetAmount();
                            icicle->Remove();
                        }
                    }
                    for (auto const visual : {148017, 148018, 148019, 148020, 148021})
                        if (Aura* icicle = caster->GetAura(visual))
                            icicle->Remove();

                    caster->CastCustomSpell(caster, 214325, &damage, NULL, NULL, true);
                }
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_mage_glacial_spike_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_glacial_spike_SpellScript();
        }
};

// Glacial Spike - 228600
class spell_mage_glacial_spike_damage : public SpellScriptLoader
{
    public:
        spell_mage_glacial_spike_damage() : SpellScriptLoader("spell_mage_glacial_spike_damage") { }

        class spell_mage_glacial_spike_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_glacial_spike_damage_SpellScript)

            void HandleEffectHit(SpellEffIndex /*effIndex*/)
            {
                if (Unit* caster = GetCaster())
                {
                    int32 damage = GetHitDamage();
                    if (Aura* aura = caster->GetAura(214325))
                    {
                        if (aura->GetEffect(EFFECT_0))
                            damage += aura->GetEffect(EFFECT_0)->GetAmount();
                        aura->Remove();
                    }
                    SetHitDamage(damage);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_mage_glacial_spike_damage_SpellScript::HandleEffectHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_glacial_spike_damage_SpellScript();
        }
};

// Ice barrier - 11426 , Prismatic barrier - 235450
class spell_mage_barriers : public SpellScriptLoader
{
    public:
        spell_mage_barriers() : SpellScriptLoader("spell_mage_barriers") { }

        class spell_mage_barriers_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_barriers_AuraScript);

            void CalculateAmount(AuraEffect const* aurEff, float& amount, bool& /*canBeRecalculated*/)
            {
                if (Unit* caster = GetCaster())
                {
                    if (AuraEffect const* aurEff = caster->GetAuraEffect(195354, EFFECT_0))
                        amount += CalculatePct(amount, aurEff->GetAmount());
                    if (AuraEffect const* aurEff = caster->GetAuraEffect(210716, EFFECT_0))
                        amount += CalculatePct(amount, aurEff->GetAmount());
                }
            }

            void Absorb(AuraEffect* aurEff, DamageInfo & dmgInfo, float & absorbAmount)
            {
                if (Unit* caster = GetCaster())
                {
                    if (AuraEffect* aurEff = caster->GetAuraEffect(235463, EFFECT_0)) // Mana Shield
                    {
                        float bp0 = CalculatePct(absorbAmount, aurEff->GetAmount());
                        caster->CastCustomSpell(caster, 235470, &bp0, NULL, NULL, true);
                    }
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_barriers_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                AfterEffectAbsorb += AuraEffectAbsorbFn(spell_mage_barriers_AuraScript::Absorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_barriers_AuraScript();
        }
};

// Dampened Magic (PvP talent) - 236788
class spell_mage_dampened_magic : public SpellScriptLoader
{
    public:
        spell_mage_dampened_magic() : SpellScriptLoader("spell_mage_dampened_magic") { }

        class spell_mage_dampened_magic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_dampened_magic_AuraScript);

            void CalculateAmount(AuraEffect const* aurEff, float & amount, bool & canBeRecalculated)
            {
                // Set absorbtion amount to unlimited
                amount = -1;
            }

            void Absorb(AuraEffect* aurEff, DamageInfo& dmgInfo, float& absorbAmount)
            {
                absorbAmount = 0;
                if (dmgInfo.GetDamageType() == DOT)
                    absorbAmount = CalculatePct(dmgInfo.GetDamage(), GetSpellInfo()->Effects[EFFECT_1]->BasePoints);
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_dampened_magic_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_mage_dampened_magic_AuraScript::Absorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_dampened_magic_AuraScript();
        }
};

// Temporal Shield (Honor Talent) - 198111
class spell_mage_temporal_shield : public SpellScriptLoader
{
    public:
        spell_mage_temporal_shield() : SpellScriptLoader("spell_mage_temporal_shield") { }

        class spell_mage_temporal_shield_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_temporal_shield_AuraScript);

            void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                AuraRemoveMode removeMode = GetTargetApplication()->GetRemoveMode();
                if (removeMode != AURA_REMOVE_BY_EXPIRE && removeMode != AURA_REMOVE_BY_CANCEL)
                    return;

                if (Unit* owner = GetUnitOwner())
                {
                    if (float bp0 = aurEff->GetAmount())
                        owner->CastCustomSpell(owner, 198116, &bp0, NULL, NULL, true);
                }
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_mage_temporal_shield_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_temporal_shield_AuraScript();
        }
};

// Touch of the Magi - 210824
class spell_mage_touch_of_the_magi : public SpellScriptLoader
{
    public:
        spell_mage_touch_of_the_magi() : SpellScriptLoader("spell_mage_touch_of_the_magi") { }

        class spell_mage_touch_of_the_magi_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_touch_of_the_magi_AuraScript);

            void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                {
                    float bp0 = aurEff->GetAmount();
                    if (Unit* target = GetUnitOwner())
                        caster->CastCustomSpell(target, 210833, &bp0, NULL, NULL, true);
                }
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_mage_touch_of_the_magi_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_touch_of_the_magi_AuraScript();
        }
};

// Aegwynn's Ascendance - 210847
class spell_mage_aegwynns_ascendance : public SpellScriptLoader
{
    public:
        spell_mage_aegwynns_ascendance() : SpellScriptLoader("spell_mage_aegwynns_ascendance") { }

        class spell_mage_aegwynns_ascendance_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_aegwynns_ascendance_AuraScript);

            void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                {
                    int32 perc = 25;
                    if (AuraEffect* aurEff2 = caster->GetAuraEffect(187680, EFFECT_0)) // Aegwynn's Ascendance
                        perc = aurEff2->GetAmount();
                    float bp0 = CalculatePct(aurEff->GetAmount(), perc);
                    caster->CastCustomSpell(caster, 187677, &bp0, NULL, NULL, true);
                }
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_mage_aegwynns_ascendance_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_aegwynns_ascendance_AuraScript();
        }
};

// Belo'vir's Final Stand - 207283
class spell_mage_belovirs_final_stand : public SpellScriptLoader
{
    public:
        spell_mage_belovirs_final_stand() : SpellScriptLoader("spell_mage_belovirs_final_stand") { }

        class spell_mage_belovirs_final_stand_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_belovirs_final_stand_AuraScript);

            void CalculateAmount(AuraEffect const* aurEff, float & amount, bool & canBeRecalculated)
            {
                if (Unit* caster = GetCaster())
                    if (AuraEffect* aurEff2 = caster->GetAuraEffect(207277, EFFECT_0)) // Belo'vir's Final Stand
                        amount = caster->CountPctFromMaxHealth(aurEff2->GetAmount());
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_belovirs_final_stand_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_belovirs_final_stand_AuraScript();
        }
};

// 208141 - Ray of Frost
class spell_mage_ray_of_frost : public SpellScriptLoader
{
    public:
        spell_mage_ray_of_frost() : SpellScriptLoader("spell_mage_ray_of_frost") { }

        class spell_mage_ray_of_frostAuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_ray_of_frostAuraScript);

            void OnStackChange(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                    if (!caster->HasAura(208166))
                        caster->InterruptSpell(CURRENT_CHANNELED_SPELL, true, true);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_mage_ray_of_frostAuraScript::OnStackChange, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_ray_of_frostAuraScript();
        }
};

// Arcane Missiles - 79683
class spell_mage_arcane_missiles : public SpellScriptLoader
{
    public:
        spell_mage_arcane_missiles() : SpellScriptLoader("spell_mage_arcane_missiles") { }

        class spell_mage_arcane_missiles_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_arcane_missiles_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                auto caster = GetCaster();
                if (!caster)
                    return;

                if (Aura* aur = caster->GetAura(GetSpellInfo()->Id))
                {
                    switch (aur->GetStackAmount())
                    {
                        case 1:
                            caster->RemoveAurasDueToSpell(79808);
                            caster->RemoveAurasDueToSpell(170572);
                            caster->CastSpell(caster, 170571, true);
                            break;
                        case 2:
                            caster->RemoveAurasDueToSpell(79808);
                            caster->RemoveAurasDueToSpell(170571);
                            caster->RemoveAurasDueToSpell(170572);
                            caster->CastSpell(caster, 79808, true);
                            break;
                        case 3:
                            caster->RemoveAurasDueToSpell(79808);
                            caster->CastSpell(caster, 170572, true);
                            break;
                    }
                }
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                auto caster = GetCaster();
                if (!caster)
                    return;

                Aura* aur = caster->GetAura(GetSpellInfo()->Id);
                if (!aur)
                {
                    caster->RemoveAurasDueToSpell(79808);
                    caster->RemoveAurasDueToSpell(170571);
                    caster->RemoveAurasDueToSpell(170572);
                }
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectApplyFn(spell_mage_arcane_missiles_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectApply += AuraEffectApplyFn(spell_mage_arcane_missiles_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_arcane_missiles_AuraScript();
        }
};

void AddSC_mage_spell_scripts()
{
    new spell_mage_cauterize();
    new spell_mage_arcane_barrage();
    new spell_mage_conjure_refreshment();
    new spell_mage_time_warp();
    new spell_mage_alter_time_overrided();
    new spell_mage_alter_time();
    new spell_mage_polymorph_cast_visual();
    new spell_mage_greater_invisibility();
    new spell_elem_invisibility();
    new spell_mage_arcane_blast();
    new spell_mage_icicle();
    new spell_mage_illusion();
    new spell_mage_flameglow();
    new spell_mage_glyph_of_conjure_familiar();
    new spell_mage_icicle_damage();
    new spell_mage_supernova();
    new spell_mage_incanters_flow();
    new spell_mage_erosion();
    new spell_mage_displacement();
    new spell_mage_combustion();
    new spell_mage_living_bomb();
    new spell_mage_living_bomb_damage();
    new spell_mage_frostbolt();
    new spell_mage_shimmer();
    new spell_mage_ice_lance();
    new spell_mage_glacial_spike();
    new spell_mage_glacial_spike_damage();
    new spell_mage_barriers();
    new spell_mage_dampened_magic();
    new spell_mage_temporal_shield();
    new spell_mage_touch_of_the_magi();
    new spell_mage_aegwynns_ascendance();
    new spell_mage_belovirs_final_stand();
    new spell_mage_ray_of_frost();
    new spell_mage_flurry();
    new spell_mage_arcane_missiles();
}
