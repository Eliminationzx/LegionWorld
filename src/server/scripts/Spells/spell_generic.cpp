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
 * Scripts for spells with SPELLFAMILY_GENERIC which cannot be included in AI script file
 * of creature using it or can't be bound to any player class.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_gen_"
 */

#include "BattlegroundMgr.h"
#include "Cell.h"
#include "CellImpl.h"
#include "Garrison.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Group.h"
#include "InstanceScript.h"
#include "LFGMgr.h"
#include "Packets/NPCPackets.h"
#include "PrecompiledHeaders/SpellPCH.h"
#include "QuestData.h"
#include "ScriptMgr.h"
#include "SkillDiscovery.h"
#include "SpellAuraEffects.h"
#include "SpellScript.h"
#include "Vehicle.h"

// Position const sumprpos[4] =
// {
//     {-13209.37f, 284.98f, 21.85f},
//     {-13213.52f, 278.17f, 21.85f},
//     {-13214.94f, 283.93f, 21.85f},
//     {-13207.78f, 278.98f, 21.85f},
// };
// 
// class spell_gen_protect : public SpellScriptLoader
// {
// public:
//     spell_gen_protect() : SpellScriptLoader("spell_gen_protect") { }
// 
//     class spell_gen_protect_SpellScript : public SpellScript
//     {
//         PrepareSpellScript(spell_gen_protect_SpellScript);
// 
//         void HandleAfterHit()
//         {
//             if (Unit* caster = GetCaster())
//                 if (Player* plr = caster->ToPlayer())
//                 {
//                     for (uint8 n = 0; n < 4; n++)
//                     {
//                         if (Creature* pr = plr->SummonCreature(48416, sumprpos[n]))
//                         {
//                             pr->SetObjectScale(0.1f);
//                             pr->SetFloatValue(UNIT_FIELD_COMBATREACH, 0.1f);
//                             pr->AI()->DoZoneInCombat(pr, 100.0f);
//                         }
//                     }
//                 }
//         }
// 
//         void Register()
//         {
//             AfterHit += SpellHitFn(spell_gen_protect_SpellScript::HandleAfterHit);
//         }
//     };
// 
//     SpellScript* GetSpellScript() const
//     {
//         return new spell_gen_protect_SpellScript();
//     }
// };

class spell_endurance_of_niuzao : public SpellScriptLoader
{
    public:
        spell_endurance_of_niuzao() : SpellScriptLoader("spell_endurance_of_niuzao") { }

        class spell_endurance_of_niuzao_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_endurance_of_niuzao_AuraScript);

            void AfterAbsorb(AuraEffect* aurEff, DamageInfo& dmgInfo, float& absorbAmount)
            {
                if (Unit* owner = GetUnitOwner())
                {
                    if (Player* plr = owner->ToPlayer())
                        if (dmgInfo.GetAbsorb() < owner->GetHealth() || owner->HasAura(148010) || !plr->isInTankSpec())
                        {
                            dmgInfo.AbsorbDamage(-(int32(absorbAmount)));
                            if (SpellInfo const* info = aurEff->GetSpellInfo())
                                if (aurEff->GetAmount() != info->Effects[EFFECT_0]->BasePoints)
                                    aurEff->SetAmount(info->Effects[EFFECT_0]->BasePoints);
                        }
                        else
                        {
                            owner->AddAura(148010, owner);
                            owner->AddAura(148958, owner);
                        }
                }
            }

            void Register() override
            {
                 AfterEffectAbsorb += AuraEffectAbsorbFn(spell_endurance_of_niuzao_AuraScript::AfterAbsorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_endurance_of_niuzao_AuraScript();
        }
};

class spell_gen_absorb0_hitlimit1 : public SpellScriptLoader
{
    public:
        spell_gen_absorb0_hitlimit1() : SpellScriptLoader("spell_gen_absorb0_hitlimit1") { }

        class spell_gen_absorb0_hitlimit1_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_absorb0_hitlimit1_AuraScript);

            float limit;

            bool Load() override
            {
                // Max absorb stored in 1 dummy effect
                limit = GetSpellInfo()->Effects[EFFECT_1]->CalcValue();
                return true;
            }

            void Absorb(AuraEffect* /*aurEff*/, DamageInfo& /*dmgInfo*/, float& absorbAmount)
            {
                absorbAmount = std::min(limit, absorbAmount);
            }

            void Register() override
            {
                 OnEffectAbsorb += AuraEffectAbsorbFn(spell_gen_absorb0_hitlimit1_AuraScript::Absorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_absorb0_hitlimit1_AuraScript();
        }
};

// 41337 Aura of Anger
class spell_gen_aura_of_anger : public SpellScriptLoader
{
    public:
        spell_gen_aura_of_anger() : SpellScriptLoader("spell_gen_aura_of_anger") { }

        class spell_gen_aura_of_anger_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_aura_of_anger_AuraScript);

            void HandleEffectPeriodicUpdate(AuraEffect* aurEff)
            {
                if (AuraEffect* aurEff1 = aurEff->GetBase()->GetEffect(EFFECT_1))
                    aurEff1->ChangeAmount(aurEff1->GetAmount() + 5);
                aurEff->SetAmount(100 * aurEff->GetTickNumber());
            }

            void Register() override
            {
                OnEffectUpdatePeriodic += AuraEffectUpdatePeriodicFn(spell_gen_aura_of_anger_AuraScript::HandleEffectPeriodicUpdate, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_aura_of_anger_AuraScript();
        }
};

class spell_gen_av_drekthar_presence : public SpellScriptLoader
{
    public:
        spell_gen_av_drekthar_presence() : SpellScriptLoader("spell_gen_av_drekthar_presence") { }

        class spell_gen_av_drekthar_presence_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_av_drekthar_presence_AuraScript);

            bool CheckAreaTarget(Unit* target)
            {
                switch (target->GetEntry())
                {
                    // alliance
                    case 14762: // Dun Baldar North Marshal
                    case 14763: // Dun Baldar South Marshal
                    case 14764: // Icewing Marshal
                    case 14765: // Stonehearth Marshal
                    case 11948: // Vandar Stormspike
                    // horde
                    case 14772: // East Frostwolf Warmaster
                    case 14776: // Tower Point Warmaster
                    case 14773: // Iceblood Warmaster
                    case 14777: // West Frostwolf Warmaster
                    case 11946: // Drek'thar
                        return true;
                    default:
                        return false;
                        break;
                }
            }
            void Register() override
            {
                DoCheckAreaTarget += AuraCheckAreaTargetFn(spell_gen_av_drekthar_presence_AuraScript::CheckAreaTarget);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_av_drekthar_presence_AuraScript();
        }
};

// 46394 Brutallus Burn
class spell_gen_burn_brutallus : public SpellScriptLoader
{
    public:
        spell_gen_burn_brutallus() : SpellScriptLoader("spell_gen_burn_brutallus") { }

        class spell_gen_burn_brutallus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_burn_brutallus_AuraScript);

            void HandleEffectPeriodicUpdate(AuraEffect* aurEff)
            {
                if (aurEff->GetTickNumber() % 11 == 0)
                    aurEff->SetAmount(aurEff->GetAmount() * 2);
            }

            void Register() override
            {
                OnEffectUpdatePeriodic += AuraEffectUpdatePeriodicFn(spell_gen_burn_brutallus_AuraScript::HandleEffectPeriodicUpdate, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_burn_brutallus_AuraScript();
        }
};

enum CannibalizeSpells
{
    SPELL_CANNIBALIZE_TRIGGERED = 20578,
};

class spell_gen_cannibalize : public SpellScriptLoader
{
    public:
        spell_gen_cannibalize() : SpellScriptLoader("spell_gen_cannibalize") { }

        class spell_gen_cannibalize_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_cannibalize_SpellScript);

            bool Validate(SpellInfo const* /*SpellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_CANNIBALIZE_TRIGGERED))
                    return false;
                return true;
            }

            SpellCastResult CheckIfCorpseNear()
            {
                Unit* caster = GetCaster();
                float max_range = GetSpellInfo()->GetMaxRange(false);
                WorldObject* result = NULL;
                // search for nearby enemy corpse in range
                Trinity::AnyDeadUnitSpellTargetInRangeCheck check(caster, max_range, GetSpellInfo(), TARGET_CHECK_ENEMY);
                Trinity::WorldObjectSearcher<Trinity::AnyDeadUnitSpellTargetInRangeCheck> searcher(caster, result, check);
                caster->GetMap()->VisitFirstFound(caster->m_positionX, caster->m_positionY, max_range, searcher);
                if (!result)
                    return SPELL_FAILED_NO_EDIBLE_CORPSES;
                return SPELL_CAST_OK;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                caster->CastSpell(caster, SPELL_CANNIBALIZE_TRIGGERED, false);
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_gen_cannibalize_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnCheckCast += SpellCheckCastFn(spell_gen_cannibalize_SpellScript::CheckIfCorpseNear);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_cannibalize_SpellScript();
        }
};

// Feast (pet ability) - 159953
class spell_gen_feast : public SpellScriptLoader
{
    public:
        spell_gen_feast() : SpellScriptLoader("spell_gen_feast") { }

        class spell_gen_feast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_feast_SpellScript);

            SpellCastResult CheckIfCorpseNear()
            {
                Unit* caster = GetCaster();
                float max_range = GetSpellInfo()->GetMaxRange(false);
                WorldObject* result = NULL;
                // search for nearby enemy corpse in range
                Trinity::AnyDeadUnitSpellTargetInRangeCheck check(caster, max_range, GetSpellInfo(), TARGET_CHECK_ENEMY);
                Trinity::WorldObjectSearcher<Trinity::AnyDeadUnitSpellTargetInRangeCheck> searcher(caster, result, check);
                caster->GetMap()->VisitFirstFound(caster->m_positionX, caster->m_positionY, max_range, searcher);
                if (!result)
                    return SPELL_FAILED_NO_EDIBLE_CORPSES;
                return SPELL_CAST_OK;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Unit* caster = GetCaster())
                    caster->CastSpell(caster, 159954, false);
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_feast_SpellScript::CheckIfCorpseNear);
                OnEffectHit += SpellEffectFn(spell_gen_feast_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_feast_SpellScript();
        }
};

class spell_gen_monk_crosswinds : public SpellScriptLoader
{
public:
    spell_gen_monk_crosswinds() : SpellScriptLoader("spell_gen_monk_crosswinds") {}

    class spell_gen_monk_crosswinds_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_monk_crosswinds_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            if (Unit* caster = GetCaster())
            {
                if (Unit* owner = caster->GetOwner())
                {
                    if (Unit* target = ObjectAccessor::GetUnit(*caster, caster->GetTargetGUID()))
                        owner->CastSpell(target, 196061, true);

                    caster->RemoveAurasDueToSpell(45204);
                    caster->RemoveAurasDueToSpell(196051);
                }
            }
        }

        void Register() override
        {
            OnEffectLaunch += SpellEffectFn(spell_gen_monk_crosswinds_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gen_monk_crosswinds_SpellScript();
    }
};

class spell_sha_cloud : public SpellScriptLoader
{
public:
    spell_sha_cloud() : SpellScriptLoader("spell_sha_cloud") { }

    class spell_sha_cloud_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_sha_cloud_SpellScript);

        void HandleOnCast()
        {
            if (Unit* caster = GetCaster())
                caster->GetMotionMaster()->MoveJump(1273.716f, 1039.498f, 434.867f, 20.0f, 15.0f, GetSpellInfo()->Id, 0.966f, 0); 
        }

        void Register() override
        {
            OnCast += SpellCastFn(spell_sha_cloud_SpellScript::HandleOnCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_sha_cloud_SpellScript();
    }
};

// 45472 Parachute
enum ParachuteSpells
{
    SPELL_PARACHUTE         = 45472,
    SPELL_PARACHUTE_BUFF    = 44795,
};

class spell_gen_parachute : public SpellScriptLoader
{
    public:
        spell_gen_parachute() : SpellScriptLoader("spell_gen_parachute") { }

        class spell_gen_parachute_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_parachute_AuraScript);

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PARACHUTE) || !sSpellMgr->GetSpellInfo(SPELL_PARACHUTE_BUFF))
                    return false;
                return true;
            }

            void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
            {
                if (Player* target = GetTarget()->ToPlayer())
                {
                    if (target->m_movementInfo.HasMovementFlag(MOVEMENTFLAG_FALLING_FAR))
                    {
                        target->RemoveAurasDueToSpell(SPELL_PARACHUTE);
                        target->CastSpell(target, SPELL_PARACHUTE_BUFF, true);
                    }
                    else if (target->m_movementInfo.HasMovementFlag(MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD | MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT)) // If we on land
                        target->RemoveAurasDueToSpell(SPELL_PARACHUTE);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_gen_parachute_AuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_parachute_AuraScript();
        }
};

enum NPCEntries
{
    NPC_DOOMGUARD   = 11859,
    NPC_INFERNAL    = 89,
    NPC_IMP         = 416,
};

class spell_gen_pet_summoned : public SpellScriptLoader
{
    public:
        spell_gen_pet_summoned() : SpellScriptLoader("spell_gen_pet_summoned") { }

        class spell_gen_pet_summoned_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_pet_summoned_SpellScript);

            bool Load() override
            {
                return GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Player* player = GetCaster()->ToPlayer();
                PetType newPetType = (player->getClass() == CLASS_HUNTER) ? HUNTER_PET : SUMMON_PET;

                Pet* newPet = new Pet(player, newPetType);
                if (newPet->LoadPetFromDB(player, 0, player->GetLastPetNumber()))
                {
                    // revive the pet if it is dead
                    if (newPet->getDeathState() == DEAD || newPet->getDeathState() == CORPSE)
                        newPet->setDeathState(ALIVE);

                    newPet->ClearUnitState(uint32(UNIT_STATE_ALL_STATE));
                    newPet->SetFullHealth();
                    newPet->SetPower(newPet->getPowerType(), newPet->GetMaxPower(newPet->getPowerType()));

                    switch (newPet->GetEntry())
                    {
                        case NPC_DOOMGUARD:
                        case NPC_INFERNAL:
                            newPet->SetEntry(NPC_IMP);
                            break;
                        default:
                            break;
                    }
                }
                else
                    delete newPet;
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_pet_summoned_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_pet_summoned_SpellScript();
        }
};

class spell_gen_remove_flight_auras : public SpellScriptLoader
{
    public:
        spell_gen_remove_flight_auras() : SpellScriptLoader("spell_gen_remove_flight_auras") {}

        class spell_gen_remove_flight_auras_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_remove_flight_auras_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Unit* target = GetHitUnit())
                {
                    target->RemoveAurasByType(SPELL_AURA_FLY);
                    target->RemoveAurasByType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_remove_flight_auras_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_remove_flight_auras_SpellScript();
        }
};

enum EluneCandle
{
    NPC_OMEN = 15467,

    SPELL_ELUNE_CANDLE_OMEN_HEAD   = 26622,
    SPELL_ELUNE_CANDLE_OMEN_CHEST  = 26624,
    SPELL_ELUNE_CANDLE_OMEN_HAND_R = 26625,
    SPELL_ELUNE_CANDLE_OMEN_HAND_L = 26649,
    SPELL_ELUNE_CANDLE_NORMAL      = 26636,
};

class spell_gen_elune_candle : public SpellScriptLoader
{
    public:
        spell_gen_elune_candle() : SpellScriptLoader("spell_gen_elune_candle") {}

        class spell_gen_elune_candle_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_elune_candle_SpellScript);
            bool Validate(SpellInfo const* /*SpellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_ELUNE_CANDLE_OMEN_HEAD))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_ELUNE_CANDLE_OMEN_CHEST))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_ELUNE_CANDLE_OMEN_HAND_R))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_ELUNE_CANDLE_OMEN_HAND_L))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_ELUNE_CANDLE_NORMAL))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                uint32 spellId = 0;

                if (GetHitUnit()->GetEntry() == NPC_OMEN)
                {
                    switch (urand(0, 3))
                    {
                        case 0: spellId = SPELL_ELUNE_CANDLE_OMEN_HEAD; break;
                        case 1: spellId = SPELL_ELUNE_CANDLE_OMEN_CHEST; break;
                        case 2: spellId = SPELL_ELUNE_CANDLE_OMEN_HAND_R; break;
                        case 3: spellId = SPELL_ELUNE_CANDLE_OMEN_HAND_L; break;
                    }
                }
                else
                    spellId = SPELL_ELUNE_CANDLE_NORMAL;

                GetCaster()->CastSpell(GetHitUnit(), spellId, true, NULL);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_elune_candle_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_elune_candle_SpellScript();
        }
};

// 24750 Trick
enum TrickSpells
{
    SPELL_PIRATE_COSTUME_MALE           = 24708,
    SPELL_PIRATE_COSTUME_FEMALE         = 24709,
    SPELL_NINJA_COSTUME_MALE            = 24710,
    SPELL_NINJA_COSTUME_FEMALE          = 24711,
    SPELL_LEPER_GNOME_COSTUME_MALE      = 24712,
    SPELL_LEPER_GNOME_COSTUME_FEMALE    = 24713,
    SPELL_SKELETON_COSTUME              = 24723,
    SPELL_GHOST_COSTUME_MALE            = 24735,
    SPELL_GHOST_COSTUME_FEMALE          = 24736,
    SPELL_TRICK_BUFF                    = 24753,
};

class spell_gen_trick : public SpellScriptLoader
{
    public:
        spell_gen_trick() : SpellScriptLoader("spell_gen_trick") {}

        class spell_gen_trick_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_trick_SpellScript);
            bool Validate(SpellInfo const* /*SpellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PIRATE_COSTUME_MALE) || !sSpellMgr->GetSpellInfo(SPELL_PIRATE_COSTUME_FEMALE) || !sSpellMgr->GetSpellInfo(SPELL_NINJA_COSTUME_MALE)
                    || !sSpellMgr->GetSpellInfo(SPELL_NINJA_COSTUME_FEMALE) || !sSpellMgr->GetSpellInfo(SPELL_LEPER_GNOME_COSTUME_MALE) || !sSpellMgr->GetSpellInfo(SPELL_LEPER_GNOME_COSTUME_FEMALE)
                    || !sSpellMgr->GetSpellInfo(SPELL_SKELETON_COSTUME) || !sSpellMgr->GetSpellInfo(SPELL_GHOST_COSTUME_MALE) || !sSpellMgr->GetSpellInfo(SPELL_GHOST_COSTUME_FEMALE) || !sSpellMgr->GetSpellInfo(SPELL_TRICK_BUFF))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (Player* target = GetHitPlayer())
                {
                    uint8 gender = target->getGender();
                    uint32 spellId = SPELL_TRICK_BUFF;
                    switch (urand(0, 5))
                    {
                        case 1:
                            spellId = gender ? SPELL_LEPER_GNOME_COSTUME_FEMALE : SPELL_LEPER_GNOME_COSTUME_MALE;
                            break;
                        case 2:
                            spellId = gender ? SPELL_PIRATE_COSTUME_FEMALE : SPELL_PIRATE_COSTUME_MALE;
                            break;
                        case 3:
                            spellId = gender ? SPELL_GHOST_COSTUME_FEMALE : SPELL_GHOST_COSTUME_MALE;
                            break;
                        case 4:
                            spellId = gender ? SPELL_NINJA_COSTUME_FEMALE : SPELL_NINJA_COSTUME_MALE;
                            break;
                        case 5:
                            spellId = SPELL_SKELETON_COSTUME;
                            break;
                        default:
                            break;
                    }

                    caster->CastSpell(target, spellId, true, NULL);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_trick_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_trick_SpellScript();
        }
};

// 24751 Trick or Treat
enum TrickOrTreatSpells
{
    SPELL_TRICK                 = 24714,
    SPELL_TREAT                 = 24715,
    SPELL_TRICKED_OR_TREATED    = 24755,
    SPELL_TRICKY_TREAT_SPEED    = 42919,
    SPELL_TRICKY_TREAT_TRIGGER  = 42965,
    SPELL_UPSET_TUMMY           = 42966
};

class spell_gen_trick_or_treat : public SpellScriptLoader
{
    public:
        spell_gen_trick_or_treat() : SpellScriptLoader("spell_gen_trick_or_treat") {}

        class spell_gen_trick_or_treat_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_trick_or_treat_SpellScript);

            bool Validate(SpellInfo const* /*SpellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_TRICK) || !sSpellMgr->GetSpellInfo(SPELL_TREAT) || !sSpellMgr->GetSpellInfo(SPELL_TRICKED_OR_TREATED))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (Player* target = GetHitPlayer())
                {
                    caster->CastSpell(target, roll_chance_i(50) ? SPELL_TRICK : SPELL_TREAT, true, NULL);
                    caster->CastSpell(target, SPELL_TRICKED_OR_TREATED, true, NULL);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_trick_or_treat_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_trick_or_treat_SpellScript();
        }
};

class spell_creature_permanent_feign_death : public SpellScriptLoader
{
    public:
        spell_creature_permanent_feign_death() : SpellScriptLoader("spell_creature_permanent_feign_death") { }

        class spell_creature_permanent_feign_death_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_creature_permanent_feign_death_AuraScript);

            void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();
                target->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);

                if (target->GetTypeId() == TYPEID_UNIT)
                    target->ToCreature()->SetReactState(REACT_PASSIVE);
            }

            void HandleEffectRemove(AuraEffect const * /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();
                target->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_creature_permanent_feign_death_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_creature_permanent_feign_death_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_creature_permanent_feign_death_AuraScript();
        }
};

enum PvPTrinketTriggeredSpells
{
    SPELL_WILL_OF_THE_FORSAKEN_COOLDOWN_TRIGGER         = 72752,
    SPELL_WILL_OF_THE_FORSAKEN_COOLDOWN_TRIGGER_WOTF    = 72757,
};

class spell_pvp_trinket_wotf_shared_cd : public SpellScriptLoader
{
    public:
        spell_pvp_trinket_wotf_shared_cd() : SpellScriptLoader("spell_pvp_trinket_wotf_shared_cd") {}

        class spell_pvp_trinket_wotf_shared_cd_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pvp_trinket_wotf_shared_cd_SpellScript);

            bool Load() override
            {
                return GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            bool Validate(SpellInfo const* /*SpellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_WILL_OF_THE_FORSAKEN_COOLDOWN_TRIGGER) || !sSpellMgr->GetSpellInfo(SPELL_WILL_OF_THE_FORSAKEN_COOLDOWN_TRIGGER_WOTF))
                    return false;
                return true;
            }

            void HandleScript()
            {
                // This is only needed because spells cast from spell_linked_spell are triggered by default
                // Spell::SendSpellCooldown() skips all spells with TRIGGERED_IGNORE_SPELL_AND_CATEGORY_CD
                GetCaster()->ToPlayer()->AddSpellAndCategoryCooldowns(GetSpellInfo(), GetCastItem() ? GetCastItem()->GetEntry() : 0, GetSpell());
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_pvp_trinket_wotf_shared_cd_SpellScript::HandleScript);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pvp_trinket_wotf_shared_cd_SpellScript();
        }
};

enum AnimalBloodPoolSpell
{
    SPELL_ANIMAL_BLOOD      = 46221,
    SPELL_SPAWN_BLOOD_POOL  = 63471,
};

class spell_gen_animal_blood : public SpellScriptLoader
{
    public:
        spell_gen_animal_blood() : SpellScriptLoader("spell_gen_animal_blood") { }

        class spell_gen_animal_blood_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_animal_blood_AuraScript);

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_SPAWN_BLOOD_POOL))
                    return false;
                return true;
            }

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                // Remove all auras with spell id 46221, except the one currently being applied
                while (Aura* aur = GetUnitOwner()->GetOwnedAura(SPELL_ANIMAL_BLOOD, ObjectGuid::Empty, ObjectGuid::Empty, 0, GetAura()))
                    GetUnitOwner()->RemoveOwnedAura(aur);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* owner = GetUnitOwner())
                    if (owner->IsInWater())
                        owner->CastSpell(owner, SPELL_SPAWN_BLOOD_POOL, true);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectRemoveFn(spell_gen_animal_blood_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_gen_animal_blood_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_animal_blood_AuraScript();
        }
};

class spell_gen_gunship_portal : public SpellScriptLoader
{
    public:
        spell_gen_gunship_portal() : SpellScriptLoader("spell_gen_gunship_portal") { }

        class spell_gen_gunship_portal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_gunship_portal_SpellScript);

            bool Load() override
            {
                return GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Player* caster = GetCaster()->ToPlayer();
                if (Battleground* bg = caster->GetBattleground())
                    if (bg->GetTypeID(true) == MS::Battlegrounds::BattlegroundTypeId::BattlegroundIsleOfConquest)
                        bg->DoAction(1, caster->GetGUID());
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_gunship_portal_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_gunship_portal_SpellScript();
        }
};

enum parachuteIC
{
    SPELL_PARACHUTE_IC = 66657,
};

class spell_gen_parachute_ic : public SpellScriptLoader
{
    public:
        spell_gen_parachute_ic() : SpellScriptLoader("spell_gen_parachute_ic") { }

        class spell_gen_parachute_ic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_parachute_ic_AuraScript)

            void HandleTriggerSpell(AuraEffect const* /*aurEff*/)
            {
                if (Player* target = GetTarget()->ToPlayer())
                    if (target->m_movementInfo.fall.fallTime > 2000)
                        target->CastSpell(target, SPELL_PARACHUTE_IC, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_gen_parachute_ic_AuraScript::HandleTriggerSpell, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_parachute_ic_AuraScript();
        }
};

class spell_gen_dungeon_credit : public SpellScriptLoader
{
    public:
        spell_gen_dungeon_credit() : SpellScriptLoader("spell_gen_dungeon_credit") { }

        class spell_gen_dungeon_credit_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_dungeon_credit_SpellScript);

            bool Load() override
            {
                _handled = false;
                return GetCaster()->GetTypeId() == TYPEID_UNIT;
            }

            void CreditEncounter()
            {
                // This hook is executed for every target, make sure we only credit instance once
                if (_handled)
                    return;

                _handled = true;
                Unit* caster = GetCaster();
                if (Map* instance = caster->GetMap())
                    instance->UpdateEncounterState(ENCOUNTER_CREDIT_CAST_SPELL, GetSpellInfo()->Id, caster, caster);
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_gen_dungeon_credit_SpellScript::CreditEncounter);
            }

            bool _handled;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_dungeon_credit_SpellScript();
        }
};

class spell_gen_profession_research : public SpellScriptLoader
{
    public:
        spell_gen_profession_research() : SpellScriptLoader("spell_gen_profession_research") {}

        class spell_gen_profession_research_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_profession_research_SpellScript);

            bool Load() override
            {
                return GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            SpellCastResult CheckRequirement()
            {
                if (HasDiscoveredAllSpells(GetSpellInfo()->Id, GetCaster()->ToPlayer()))
                {
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_NOTHING_TO_DISCOVER);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_profession_research_SpellScript::CheckRequirement);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_profession_research_SpellScript();
        }
};

class spell_generic_clone : public SpellScriptLoader
{
    public:
        spell_generic_clone() : SpellScriptLoader("spell_generic_clone") { }

        class spell_generic_clone_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_generic_clone_SpellScript);

            void HandleScriptEffect(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                uint32 spellId = uint32(GetSpellInfo()->Effects[effIndex]->CalcValue());
                GetHitUnit()->CastSpell(GetCaster(), spellId, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_generic_clone_SpellScript::HandleScriptEffect, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
                OnEffectHitTarget += SpellEffectFn(spell_generic_clone_SpellScript::HandleScriptEffect, EFFECT_2, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_generic_clone_SpellScript();
        }
};

enum CloneWeaponSpells
{
    SPELL_COPY_WEAPON_AURA       = 41055,
    SPELL_COPY_WEAPON_2_AURA     = 63416,
    SPELL_COPY_WEAPON_3_AURA     = 69891,

    SPELL_COPY_OFFHAND_AURA      = 45206,
    SPELL_COPY_OFFHAND_2_AURA    = 69892,

    SPELL_COPY_RANGED_AURA       = 57594
};

class spell_generic_clone_weapon : public SpellScriptLoader
{
    public:
        spell_generic_clone_weapon() : SpellScriptLoader("spell_generic_clone_weapon") { }

        class spell_generic_clone_weapon_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_generic_clone_weapon_SpellScript);

            void HandleScriptEffect(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                Unit* caster = GetCaster();

                if (Unit* target = GetHitUnit())
                {

                    uint32 spellId = uint32(GetSpellInfo()->Effects[EFFECT_0]->CalcValue());
                    caster->CastSpell(target, spellId, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_generic_clone_weapon_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_generic_clone_weapon_SpellScript();
        }
                   
};

class spell_gen_clone_weapon_aura : public SpellScriptLoader
{ 
    public:
        spell_gen_clone_weapon_aura() : SpellScriptLoader("spell_gen_clone_weapon_aura") { }

            class spell_gen_clone_weapon_auraScript : public AuraScript
            {
                PrepareAuraScript(spell_gen_clone_weapon_auraScript);

                uint32 prevItem;

                bool Validate(SpellInfo const* /*SpellInfo*/) override
                {
                    if (!sSpellMgr->GetSpellInfo(SPELL_COPY_WEAPON_AURA) || !sSpellMgr->GetSpellInfo(SPELL_COPY_WEAPON_2_AURA) || !sSpellMgr->GetSpellInfo(SPELL_COPY_WEAPON_3_AURA)
                        || !sSpellMgr->GetSpellInfo(SPELL_COPY_OFFHAND_AURA) || !sSpellMgr->GetSpellInfo(SPELL_COPY_OFFHAND_2_AURA) || !sSpellMgr->GetSpellInfo(SPELL_COPY_RANGED_AURA))
                        return false;
                    return true;
                }

                void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
                {
                    Unit* caster = GetCaster();
                    Unit* target = GetTarget();

                    if (!caster)
                        return;

                    switch (GetSpellInfo()->Id)
                    {
                        case SPELL_COPY_WEAPON_AURA:
                        case SPELL_COPY_WEAPON_2_AURA:
                        case SPELL_COPY_WEAPON_3_AURA:
                        {

                        prevItem = target->GetUInt32Value(UNIT_FIELD_VIRTUAL_ITEMS);
                        if (Player* player = caster->ToPlayer())

                        {
                            if (Item* mainItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND))
                                target->SetVirtualItem(0, mainItem->GetEntry());

                        }
                        else
                            target->SetVirtualItem(0, caster->GetUInt32Value(UNIT_FIELD_VIRTUAL_ITEMS));
                        break;
                    }
                    case SPELL_COPY_OFFHAND_AURA:
                    case SPELL_COPY_OFFHAND_2_AURA:
                    {
                        prevItem = target->GetUInt32Value(UNIT_FIELD_VIRTUAL_ITEMS) + 1;

                        if (Player* player = caster->ToPlayer())
                        {
                            if (Item* offItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND))
                                target->SetVirtualItem(1, offItem->GetEntry());
                        }
                        else
                            target->SetVirtualItem(1, caster->GetUInt32Value(UNIT_FIELD_VIRTUAL_ITEMS + 1));
                        break;
                    }
                    case SPELL_COPY_RANGED_AURA:
                    {
                        prevItem = target->GetUInt32Value(UNIT_FIELD_VIRTUAL_ITEMS) + 2;

                        if (Player* player = caster->ToPlayer())
                        {
                            if (Item* rangedItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED))
                                target->SetVirtualItem(2, rangedItem->GetEntry());
                        }
                        else 
                            target->SetVirtualItem(2, caster->GetUInt32Value(UNIT_FIELD_VIRTUAL_ITEMS + 2));
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            { 
                Unit* target = GetTarget();

                switch (GetSpellInfo()->Id)
                { 
                    case SPELL_COPY_WEAPON_AURA:
                    case SPELL_COPY_WEAPON_2_AURA:
                    case SPELL_COPY_WEAPON_3_AURA:
                       target->SetVirtualItem(0, prevItem);
                       break;
                    case SPELL_COPY_OFFHAND_AURA:
                    case SPELL_COPY_OFFHAND_2_AURA:
                        target->SetVirtualItem(1, prevItem);
                        break;
                    case SPELL_COPY_RANGED_AURA:
                        target->SetVirtualItem(2, prevItem);
                        break;
                    default:
                        break;
                }
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_gen_clone_weapon_auraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                OnEffectRemove += AuraEffectRemoveFn(spell_gen_clone_weapon_auraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }

        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_clone_weapon_auraScript();
        }
};

enum SeaforiumSpells
{
    SPELL_PLANT_CHARGES_CREDIT_ACHIEVEMENT = 60937,
};

class spell_gen_seaforium_blast : public SpellScriptLoader
{
    public:
        spell_gen_seaforium_blast() : SpellScriptLoader("spell_gen_seaforium_blast") {}

        class spell_gen_seaforium_blast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_seaforium_blast_SpellScript);

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PLANT_CHARGES_CREDIT_ACHIEVEMENT))
                    return false;
                return true;
            }

            bool Load() override
            {
                // OriginalCaster is always available in Spell::prepare
                return GetOriginalCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            void AchievementCredit(SpellEffIndex /*effIndex*/)
            {
                // but in effect handling OriginalCaster can become NULL
                if (Player* originalCaster = GetOriginalCaster()->ToPlayer())
                    if (GameObject* go = GetHitGObj())
                        if (go->GetGOInfo()->type == GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING)
                            originalCaster->UpdateAchievementCriteria(CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_PLANT_CHARGES_CREDIT_ACHIEVEMENT);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_seaforium_blast_SpellScript::AchievementCredit, EFFECT_1, SPELL_EFFECT_GAMEOBJECT_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_seaforium_blast_SpellScript();
        }
};

enum MagicRoosterSpells
{
    SPELL_MAGIC_ROOSTER_NORMAL          = 66122,
    SPELL_MAGIC_ROOSTER_DRAENEI_MALE    = 66123,
    SPELL_MAGIC_ROOSTER_TAUREN_MALE     = 66124,
};

class spell_gen_magic_rooster : public SpellScriptLoader
{
    public:
        spell_gen_magic_rooster() : SpellScriptLoader("spell_gen_magic_rooster") { }

        class spell_gen_magic_rooster_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_magic_rooster_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                if (Player* target = GetHitPlayer())
                {
                    // prevent client crashes from stacking mounts
                    target->RemoveAurasByType(SPELL_AURA_MOUNTED);

                    uint32 spellId = SPELL_MAGIC_ROOSTER_NORMAL;
                    switch (target->getRace())
                    {
                        case RACE_DRAENEI:
                            if (target->getGender() == GENDER_MALE)
                                spellId = SPELL_MAGIC_ROOSTER_DRAENEI_MALE;
                            break;
                        case RACE_TAUREN:
                            if (target->getGender() == GENDER_MALE)
                                spellId = SPELL_MAGIC_ROOSTER_TAUREN_MALE;
                            break;
                        default:
                            break;
                    }

                    target->CastSpell(target, spellId, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_magic_rooster_SpellScript::HandleScript, EFFECT_2, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_magic_rooster_SpellScript();
        }
};

class spell_gen_allow_cast_from_item_only : public SpellScriptLoader
{
    public:
        spell_gen_allow_cast_from_item_only() : SpellScriptLoader("spell_gen_allow_cast_from_item_only") { }

        class spell_gen_allow_cast_from_item_only_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_allow_cast_from_item_only_SpellScript);

            SpellCastResult CheckRequirement()
            {
                if (!GetCastItem())
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_allow_cast_from_item_only_SpellScript::CheckRequirement);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_allow_cast_from_item_only_SpellScript();
        }
};

enum Launch
{
    SPELL_LAUNCH_NO_FALLING_DAMAGE = 66251
};

class spell_gen_launch : public SpellScriptLoader
{
    public:
        spell_gen_launch() : SpellScriptLoader("spell_gen_launch") {}

        class spell_gen_launch_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_launch_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Player* player = GetHitPlayer())
                    player->AddAura(SPELL_LAUNCH_NO_FALLING_DAMAGE, player); // prevents falling damage
            }

            void Launch()
            {
                WorldLocation const* const position = GetExplTargetDest();

                if (Player* player = GetHitPlayer())
                {
                    player->ExitVehicle();

                    // A better research is needed
                    // There is no spell for this, the following calculation was based on void Spell::CalculateJumpSpeeds

                    float speedZ = 10.0f;
                    float dist = position->GetExactDist2d(player->GetPositionX(), player->GetPositionY());
                    float speedXY = dist;

                    player->GetMotionMaster()->MoveJump(position->GetPositionX(), position->GetPositionY(), position->GetPositionZ(), speedXY, speedZ);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_launch_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_FORCE_CAST);
                AfterHit += SpellHitFn(spell_gen_launch_SpellScript::Launch);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_launch_SpellScript();
        }
};

enum VehicleScaling
{
    SPELL_GEAR_SCALING      = 66668,
};

class spell_gen_vehicle_scaling : public SpellScriptLoader
{
    public:
        spell_gen_vehicle_scaling() : SpellScriptLoader("spell_gen_vehicle_scaling") { }

        class spell_gen_vehicle_scaling_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_vehicle_scaling_SpellScript);

            SpellCastResult CheckCast()
            {
                if (Unit* target = GetExplTargetUnit())
                    if (target->GetTypeId() == TYPEID_PLAYER)
                        return SPELL_FAILED_DONT_REPORT;

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_vehicle_scaling_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_vehicle_scaling_SpellScript();
        }

        class spell_gen_vehicle_scaling_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_vehicle_scaling_AuraScript);

            bool Load() override
            {
                return GetCaster() && GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, float& amount, bool& /*canBeRecalculated*/)
            {
                Unit* caster = GetCaster();
                float factor;
                uint16 baseItemLevel;

                // TODO: Reserach coeffs for different vehicles
                switch (GetId())
                {
                    case SPELL_GEAR_SCALING:
                        factor = 1.0f;
                        baseItemLevel = 405;
                        break;
                    default:
                        factor = 1.0f;
                        baseItemLevel = 405;
                        break;
                }

                float avgILvl = caster->ToPlayer()->GetAverageItemLevelTotal();
                if (avgILvl < baseItemLevel)
                    return;                     // TODO: Research possibility of scaling down

                amount = uint16((avgILvl - baseItemLevel) * factor);
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_vehicle_scaling_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_HEALING_PCT);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_vehicle_scaling_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_vehicle_scaling_AuraScript::CalculateAmount, EFFECT_2, SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_vehicle_scaling_AuraScript();
        }
};

class spell_gen_oracle_wolvar_reputation : public SpellScriptLoader
{
    public:
        spell_gen_oracle_wolvar_reputation() : SpellScriptLoader("spell_gen_oracle_wolvar_reputation") { }

        class spell_gen_oracle_wolvar_reputation_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_oracle_wolvar_reputation_SpellScript);

            bool Load() override
            {
                return GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            void HandleDummy(SpellEffIndex effIndex)
            {
                Player* player = GetCaster()->ToPlayer();
                uint32 factionId = GetSpellInfo()->Effects[effIndex]->CalcValue();
                int32  repChange =  GetSpellInfo()->Effects[EFFECT_1]->CalcValue();

                FactionEntry const* factionEntry = sFactionStore.LookupEntry(factionId);

                if (!factionEntry)
                    return;

                // Set rep to baserep + basepoints (expecting spillover for oposite faction -> become hated)
                // Not when player already has equal or higher rep with this faction
                if (player->GetReputationMgr().GetBaseReputation(factionEntry) < repChange)
                    player->GetReputationMgr().SetReputation(factionEntry, repChange);

                // EFFECT_INDEX_2 most likely update at war state, we already handle this in SetReputation
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_gen_oracle_wolvar_reputation_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_oracle_wolvar_reputation_SpellScript();
        }
};

class spell_gen_ds_flush_knockback : public SpellScriptLoader
{
    public:
        spell_gen_ds_flush_knockback() : SpellScriptLoader("spell_gen_ds_flush_knockback") { }

        class spell_gen_ds_flush_knockback_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_ds_flush_knockback_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                // Here the target is the water spout and determines the position where the player is knocked from
                if (Unit* target = GetHitUnit())
                {
                    if (Player* player = GetCaster()->ToPlayer())
                    {
                        if (player->GetDistance2d(target) > 35.f)
                            return;

                        float horizontalSpeed = 20.0f + (40.0f - GetCaster()->GetDistance(target));
                        float verticalSpeed = 8.0f;
                        // This method relies on the Dalaran Sewer map disposition and Water Spout position
                        // What we do is knock the player from a position exactly behind him and at the end of the pipe
                        player->KnockbackFrom(target->GetPositionX(), player->GetPositionY(), horizontalSpeed, verticalSpeed);
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_ds_flush_knockback_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_ds_flush_knockback_SpellScript();
        }
};

enum DummyTrigger
{
    SPELL_PERSISTANT_SHIELD_TRIGGERED       = 26470,
    SPELL_PERSISTANT_SHIELD                 = 26467,
};

class spell_gen_dummy_trigger : public SpellScriptLoader
{
    public:
        spell_gen_dummy_trigger() : SpellScriptLoader("spell_gen_dummy_trigger") { }

        class spell_gen_dummy_trigger_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_dummy_trigger_SpellScript);

            bool Validate(SpellInfo const* /*SpellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PERSISTANT_SHIELD_TRIGGERED) || !sSpellMgr->GetSpellInfo(SPELL_PERSISTANT_SHIELD))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                float damage = GetEffectValue();
                Unit* caster = GetCaster();
                if (Unit* target = GetHitUnit())
                    if (SpellInfo const* triggeredByAuraSpell = GetTriggeringSpell())
                        if (triggeredByAuraSpell->Id == SPELL_PERSISTANT_SHIELD_TRIGGERED)
                            caster->CastCustomSpell(target, SPELL_PERSISTANT_SHIELD_TRIGGERED, &damage, NULL, NULL, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_dummy_trigger_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_dummy_trigger_SpellScript();
        }

};

class spell_gen_spirit_healer_res : public SpellScriptLoader
{
    public:
        spell_gen_spirit_healer_res(): SpellScriptLoader("spell_gen_spirit_healer_res") { }

        class spell_gen_spirit_healer_res_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_spirit_healer_res_SpellScript);

            bool Load() override
            {
                return GetOriginalCaster() && GetOriginalCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                Player* originalCaster = GetOriginalCaster()->ToPlayer();
                if (Unit* target = GetHitUnit())
                {
                    WorldPackets::NPC::SpiritHealerConfirm packet;
                    packet.Unit = target->GetGUID();
                    originalCaster->SendDirectMessage(packet.Write());
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_spirit_healer_res_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_spirit_healer_res_SpellScript();
        }
};

enum TransporterBackfires
{
    SPELL_TRANSPORTER_MALFUNCTION_POLYMORPH     = 23444,
    SPELL_TRANSPORTER_EVIL_TWIN                 = 23445,
    SPELL_TRANSPORTER_MALFUNCTION_MISS          = 36902,
};

class spell_gen_gadgetzan_transporter_backfire : public SpellScriptLoader
{
    public:
        spell_gen_gadgetzan_transporter_backfire() : SpellScriptLoader("spell_gen_gadgetzan_transporter_backfire") { }

        class spell_gen_gadgetzan_transporter_backfire_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_gadgetzan_transporter_backfire_SpellScript)

            bool Validate(SpellInfo const* /*SpellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_TRANSPORTER_MALFUNCTION_POLYMORPH) || !sSpellMgr->GetSpellInfo(SPELL_TRANSPORTER_EVIL_TWIN)
                    || !sSpellMgr->GetSpellInfo(SPELL_TRANSPORTER_MALFUNCTION_MISS))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                Unit* caster = GetCaster();
                int32 r = irand(0, 119);
                if (r < 20)                           // Transporter Malfunction - 1/6 polymorph
                    caster->CastSpell(caster, SPELL_TRANSPORTER_MALFUNCTION_POLYMORPH, true);
                else if (r < 100)                     // Evil Twin               - 4/6 evil twin
                    caster->CastSpell(caster, SPELL_TRANSPORTER_EVIL_TWIN, true);
                else                                    // Transporter Malfunction - 1/6 miss the target
                    caster->CastSpell(caster, SPELL_TRANSPORTER_MALFUNCTION_MISS, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_gadgetzan_transporter_backfire_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_gadgetzan_transporter_backfire_SpellScript();
        }
};

enum GnomishTransporter
{
    SPELL_TRANSPORTER_SUCCESS                   = 23441,
    SPELL_TRANSPORTER_FAILURE                   = 23446,
};

class spell_gen_gnomish_transporter : public SpellScriptLoader
{
    public:
        spell_gen_gnomish_transporter() : SpellScriptLoader("spell_gen_gnomish_transporter") { }

        class spell_gen_gnomish_transporter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_gnomish_transporter_SpellScript)

            bool Validate(SpellInfo const* /*SpellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_TRANSPORTER_SUCCESS) || !sSpellMgr->GetSpellInfo(SPELL_TRANSPORTER_FAILURE))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                Unit* caster = GetCaster();
                caster->CastSpell(caster, roll_chance_i(50) ? SPELL_TRANSPORTER_SUCCESS : SPELL_TRANSPORTER_FAILURE , true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_gnomish_transporter_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_gnomish_transporter_SpellScript();
        }
};

enum DalaranDisguiseSpells
{
    SPELL_SUNREAVER_DISGUISE_TRIGGER       = 69672,
    SPELL_SUNREAVER_DISGUISE_FEMALE        = 70973,
    SPELL_SUNREAVER_DISGUISE_MALE          = 70974,

    SPELL_SILVER_COVENANT_DISGUISE_TRIGGER = 69673,
    SPELL_SILVER_COVENANT_DISGUISE_FEMALE  = 70971,
    SPELL_SILVER_COVENANT_DISGUISE_MALE    = 70972,
};

class spell_gen_dalaran_disguise : public SpellScriptLoader
{
    public:
        spell_gen_dalaran_disguise(const char* name) : SpellScriptLoader(name) {}

        class spell_gen_dalaran_disguise_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_dalaran_disguise_SpellScript);
            bool Validate(SpellInfo const* SpellInfo) override
            {
                switch (SpellInfo->Id)
                {
                    case SPELL_SUNREAVER_DISGUISE_TRIGGER:
                        if (!sSpellMgr->GetSpellInfo(SPELL_SUNREAVER_DISGUISE_FEMALE) || !sSpellMgr->GetSpellInfo(SPELL_SUNREAVER_DISGUISE_MALE))
                            return false;
                        break;
                    case SPELL_SILVER_COVENANT_DISGUISE_TRIGGER:
                        if (!sSpellMgr->GetSpellInfo(SPELL_SILVER_COVENANT_DISGUISE_FEMALE) || !sSpellMgr->GetSpellInfo(SPELL_SILVER_COVENANT_DISGUISE_MALE))
                            return false;
                        break;
                }
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Player* player = GetHitPlayer())
                {
                    uint8 gender = player->getGender();

                    uint32 spellId = GetSpellInfo()->Id;

                    switch (spellId)
                    {
                        case SPELL_SUNREAVER_DISGUISE_TRIGGER:
                            spellId = gender ? SPELL_SUNREAVER_DISGUISE_FEMALE : SPELL_SUNREAVER_DISGUISE_MALE;
                            break;
                        case SPELL_SILVER_COVENANT_DISGUISE_TRIGGER:
                            spellId = gender ? SPELL_SILVER_COVENANT_DISGUISE_FEMALE : SPELL_SILVER_COVENANT_DISGUISE_MALE;
                            break;
                        default:
                            break;
                    }
                    GetCaster()->CastSpell(player, spellId, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_dalaran_disguise_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_dalaran_disguise_SpellScript();
        }
};

/* DOCUMENTATION: Break-Shield spells
    Break-Shield spells can be classified in three groups:

        - Spells on vehicle bar used by players:
            + EFFECT_0: SCRIPT_EFFECT
            + EFFECT_1: NONE
            + EFFECT_2: NONE
        - Spells casted by players triggered by script:
            + EFFECT_0: SCHOOL_DAMAGE
            + EFFECT_1: SCRIPT_EFFECT
            + EFFECT_2: FORCE_CAST
        - Spells casted by NPCs on players:
            + EFFECT_0: SCHOOL_DAMAGE
            + EFFECT_1: SCRIPT_EFFECT
            + EFFECT_2: NONE

    In the following script we handle the SCRIPT_EFFECT for effIndex EFFECT_0 and EFFECT_1.
        - When handling EFFECT_0 we're in the "Spells on vehicle bar used by players" case
          and we'll trigger "Spells casted by players triggered by script"
        - When handling EFFECT_1 we're in the "Spells casted by players triggered by script"
          or "Spells casted by NPCs on players" so we'll search for the first defend layer and drop it.
*/

enum BreakShieldSpells
{
    SPELL_BREAK_SHIELD_DAMAGE_2K                 = 62626,
    SPELL_BREAK_SHIELD_DAMAGE_10K                = 64590,

    SPELL_BREAK_SHIELD_TRIGGER_FACTION_MOUNTS    = 62575, // Also on ToC5 mounts
    SPELL_BREAK_SHIELD_TRIGGER_CAMPAING_WARHORSE = 64595,
    SPELL_BREAK_SHIELD_TRIGGER_UNK               = 66480,
};

class spell_gen_break_shield: public SpellScriptLoader
{
    public:
        spell_gen_break_shield(const char* name) : SpellScriptLoader(name) {}

        class spell_gen_break_shield_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_break_shield_SpellScript)

            void HandleScriptEffect(SpellEffIndex effIndex)
            {
                Unit* target = GetHitUnit();

                switch (effIndex)
                {
                    case EFFECT_0: // On spells wich trigger the damaging spell (and also the visual)
                    {
                        uint32 spellId;

                        switch (GetSpellInfo()->Id)
                        {
                            case SPELL_BREAK_SHIELD_TRIGGER_UNK:
                            case SPELL_BREAK_SHIELD_TRIGGER_CAMPAING_WARHORSE:
                                spellId = SPELL_BREAK_SHIELD_DAMAGE_10K;
                                break;
                            case SPELL_BREAK_SHIELD_TRIGGER_FACTION_MOUNTS:
                                spellId = SPELL_BREAK_SHIELD_DAMAGE_2K;
                                break;
                            default:
                                return;
                        }

                        if (Unit* rider = GetCaster()->GetCharmer())
                            rider->CastSpell(target, spellId, false);
                        else
                            GetCaster()->CastSpell(target, spellId, false);
                        break;
                    }
                    case EFFECT_1: // On damaging spells, for removing a defend layer
                        {
                            Unit::AuraApplicationMap const& auras = target->GetAppliedAuras();
                            for (Unit::AuraApplicationMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
                            {
                                if (Aura* aura = itr->second->GetBase())
                                {
                                    if (aura->GetId() == 62552 || aura->GetId() == 62719 || aura->GetId() == 64100 || aura->GetId() == 66482)
                                    {
                                        aura->ModStackAmount(-1, AURA_REMOVE_BY_ENEMY_SPELL);
                                        // Remove dummys from rider (Necessary for updating visual shields)
                                        if (Unit* rider = target->GetCharmer())
                                            if (Aura* defend = rider->GetAura(aura->GetId()))
                                                defend->ModStackAmount(-1, AURA_REMOVE_BY_ENEMY_SPELL);
                                        break;
                                    }
                                }
                            }
                            break;
                        }
                    default:
                        break;
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_break_shield_SpellScript::HandleScriptEffect, EFFECT_FIRST_FOUND, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_break_shield_SpellScript();
        }
};

/* DOCUMENTATION: Charge spells
    Charge spells can be classified in four groups:

        - Spells on vehicle bar used by players:
            + EFFECT_0: SCRIPT_EFFECT
            + EFFECT_1: TRIGGER_SPELL
            + EFFECT_2: NONE
        - Spells casted by player's mounts triggered by script:
            + EFFECT_0: CHARGE
            + EFFECT_1: TRIGGER_SPELL
            + EFFECT_2: APPLY_AURA
        - Spells casted by players on the target triggered by script:
            + EFFECT_0: SCHOOL_DAMAGE
            + EFFECT_1: SCRIPT_EFFECT
            + EFFECT_2: NONE
        - Spells casted by NPCs on players:
            + EFFECT_0: SCHOOL_DAMAGE
            + EFFECT_1: CHARGE
            + EFFECT_2: SCRIPT_EFFECT

    In the following script we handle the SCRIPT_EFFECT and CHARGE
        - When handling SCRIPT_EFFECT:
            + EFFECT_0: Corresponds to "Spells on vehicle bar used by players" and we make player's mount cast
              the charge effect on the current target ("Spells casted by player's mounts triggered by script").
            + EFFECT_1 and EFFECT_2: Triggered when "Spells casted by player's mounts triggered by script" hits target,
              corresponding to "Spells casted by players on the target triggered by script" and "Spells casted by
              NPCs on players" and we check Defend layers and drop a charge of the first found.
        - When handling CHARGE:
            + Only launched for "Spells casted by player's mounts triggered by script", makes the player cast the
              damaging spell on target with a small chance of failing it.
*/

enum ChargeSpells
{
    SPELL_CHARGE_DAMAGE_8K5             = 62874,
    SPELL_CHARGE_DAMAGE_20K             = 68498,
    SPELL_CHARGE_DAMAGE_45K             = 64591,

    SPELL_CHARGE_CHARGING_EFFECT_8K5    = 63661,
    SPELL_CHARGE_CHARGING_EFFECT_20K_1  = 68284,
    SPELL_CHARGE_CHARGING_EFFECT_20K_2  = 68501,
    SPELL_CHARGE_CHARGING_EFFECT_45K_1  = 62563,
    SPELL_CHARGE_CHARGING_EFFECT_45K_2  = 66481,

    SPELL_CHARGE_TRIGGER_FACTION_MOUNTS = 62960,
    SPELL_CHARGE_TRIGGER_TRIAL_CHAMPION = 68282,

    SPELL_CHARGE_MISS_EFFECT            = 62977,
};

class spell_gen_mounted_charge: public SpellScriptLoader
{
    public:
        spell_gen_mounted_charge() : SpellScriptLoader("spell_gen_mounted_charge") { }

        class spell_gen_mounted_charge_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_mounted_charge_SpellScript)

            void HandleScriptEffect(SpellEffIndex effIndex)
            {
                Unit* target = GetHitUnit();

                switch (effIndex)
                {
                    case EFFECT_0: // On spells wich trigger the damaging spell (and also the visual)
                    {
                        uint32 spellId;

                        switch (GetSpellInfo()->Id)
                        {
                            case SPELL_CHARGE_TRIGGER_TRIAL_CHAMPION:
                                spellId = SPELL_CHARGE_CHARGING_EFFECT_20K_1;
                                break;
                            case SPELL_CHARGE_TRIGGER_FACTION_MOUNTS:
                                spellId = SPELL_CHARGE_CHARGING_EFFECT_8K5;
                                break;
                            default:
                                return;
                        }

                        // If target isn't a training dummy there's a chance of failing the charge
                        if (!target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_REMOVE_CLIENT_CONTROL) && roll_chance_f(12.5f))
                            spellId = SPELL_CHARGE_MISS_EFFECT;

                        if (Unit* vehicle = GetCaster()->GetVehicleBase())
                            vehicle->CastSpell(target, spellId, false);
                        else
                            GetCaster()->CastSpell(target, spellId, false);
                        break;
                    }
                    case EFFECT_1: // On damaging spells, for removing a defend layer
                    case EFFECT_2:
                        {
                            Unit::AuraApplicationMap const& auras = target->GetAppliedAuras();
                            for (Unit::AuraApplicationMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
                            {
                                if (Aura* aura = itr->second->GetBase())
                                {
                                    if (aura->GetId() == 62552 || aura->GetId() == 62719 || aura->GetId() == 64100 || aura->GetId() == 66482)
                                    {
                                        aura->ModStackAmount(-1, AURA_REMOVE_BY_ENEMY_SPELL);
                                        // Remove dummys from rider (Necessary for updating visual shields)
                                        if (Unit* rider = target->GetCharmer())
                                            if (Aura* defend = rider->GetAura(aura->GetId()))
                                                defend->ModStackAmount(-1, AURA_REMOVE_BY_ENEMY_SPELL);
                                        break;
                                    }
                                }
                            }
                            break;
                        }
                }
            }

            void HandleChargeEffect(SpellEffIndex /*effIndex*/)
            {
                uint32 spellId;

                switch (GetSpellInfo()->Id)
                {
                    case SPELL_CHARGE_CHARGING_EFFECT_8K5:
                        spellId = SPELL_CHARGE_DAMAGE_8K5;
                        break;
                    case SPELL_CHARGE_CHARGING_EFFECT_20K_1:
                    case SPELL_CHARGE_CHARGING_EFFECT_20K_2:
                        spellId = SPELL_CHARGE_DAMAGE_20K;
                        break;
                    case SPELL_CHARGE_CHARGING_EFFECT_45K_1:
                    case SPELL_CHARGE_CHARGING_EFFECT_45K_2:
                        spellId = SPELL_CHARGE_DAMAGE_45K;
                        break;
                    default:
                        return;
                }

                if (Unit* rider = GetCaster()->GetCharmer())
                    rider->CastSpell(GetHitUnit(), spellId, false);
                else
                    GetCaster()->CastSpell(GetHitUnit(), spellId, false);
            }

            void Register() override
            {
                SpellInfo const* spell = sSpellMgr->GetSpellInfo(m_scriptSpellId);

                if (spell->HasEffect(SPELL_EFFECT_SCRIPT_EFFECT))
                    OnEffectHitTarget += SpellEffectFn(spell_gen_mounted_charge_SpellScript::HandleScriptEffect, EFFECT_FIRST_FOUND, SPELL_EFFECT_SCRIPT_EFFECT);

                if (spell->Effects[EFFECT_0]->Effect == SPELL_EFFECT_CHARGE)
                    OnEffectHitTarget += SpellEffectFn(spell_gen_mounted_charge_SpellScript::HandleChargeEffect, EFFECT_0, SPELL_EFFECT_CHARGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_mounted_charge_SpellScript();
        }
};

enum DefendVisuals
{
    SPELL_VISUAL_SHIELD_1 = 63130,
    SPELL_VISUAL_SHIELD_2 = 63131,
    SPELL_VISUAL_SHIELD_3 = 63132,
};

class spell_gen_defend : public SpellScriptLoader
{
    public:
        spell_gen_defend() : SpellScriptLoader("spell_gen_defend") { }

        class spell_gen_defend_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_defend_AuraScript);

            bool Validate(SpellInfo const* /*SpellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_VISUAL_SHIELD_1))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_VISUAL_SHIELD_2))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_VISUAL_SHIELD_3))
                    return false;
                return true;
            }

            void RefreshVisualShields(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (GetCaster())
                {
                    Unit* target = GetTarget();

                    for (uint8 i = 0; i < GetSpellInfo()->AuraOptions.CumulativeAura; ++i)
                        target->RemoveAurasDueToSpell(SPELL_VISUAL_SHIELD_1 + i);

                    target->CastSpell(target, SPELL_VISUAL_SHIELD_1 + GetAura()->GetStackAmount() - 1, true, NULL, aurEff);
                }
                else
                    GetTarget()->RemoveAurasDueToSpell(GetId());
            }

            void RemoveVisualShields(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                for (uint8 i = 0; i < GetSpellInfo()->AuraOptions.CumulativeAura; ++i)
                    GetTarget()->RemoveAurasDueToSpell(SPELL_VISUAL_SHIELD_1 + i);
            }

            void RemoveDummyFromDriver(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                    if (TempSummon* vehicle = caster->ToTempSummon())
                        if (Unit* rider = vehicle->GetSummoner())
                            rider->RemoveAurasDueToSpell(GetId());
            }

            void Register() override
            {
                SpellInfo const* spell = sSpellMgr->GetSpellInfo(m_scriptSpellId);

                // Defend spells casted by NPCs (add visuals)
                if (spell->Effects[EFFECT_0]->ApplyAuraName == SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN)
                {
                    AfterEffectApply += AuraEffectApplyFn(spell_gen_defend_AuraScript::RefreshVisualShields, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                    OnEffectRemove += AuraEffectRemoveFn(spell_gen_defend_AuraScript::RemoveVisualShields, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK);
                }

                // Remove Defend spell from player when he dismounts
                if (spell->Effects[EFFECT_2]->ApplyAuraName == SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN)
                    OnEffectRemove += AuraEffectRemoveFn(spell_gen_defend_AuraScript::RemoveDummyFromDriver, EFFECT_2, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL);

                // Defend spells casted by players (add/remove visuals)
                if (spell->Effects[EFFECT_1]->ApplyAuraName == SPELL_AURA_DUMMY)
                {
                    AfterEffectApply += AuraEffectApplyFn(spell_gen_defend_AuraScript::RefreshVisualShields, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                    OnEffectRemove += AuraEffectRemoveFn(spell_gen_defend_AuraScript::RemoveVisualShields, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK);
                }
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_defend_AuraScript();
        }
};

enum MountedDuelSpells
{
    SPELL_ON_TOURNAMENT_MOUNT = 63034,
    SPELL_MOUNTED_DUEL        = 62875,
};

class spell_gen_tournament_duel : public SpellScriptLoader
{
    public:
        spell_gen_tournament_duel() : SpellScriptLoader("spell_gen_tournament_duel") { }

        class spell_gen_tournament_duel_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_tournament_duel_SpellScript);

            bool Validate(SpellInfo const* /*SpellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_ON_TOURNAMENT_MOUNT))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_MOUNTED_DUEL))
                    return false;
                return true;
            }

            void HandleScriptEffect(SpellEffIndex /*effIndex*/)
            {
                if (Unit* rider = GetCaster()->GetCharmer())
                {
                    if (Player* plrTarget = GetHitPlayer())
                    {
                        if (plrTarget->HasAura(SPELL_ON_TOURNAMENT_MOUNT) && plrTarget->GetVehicleBase())
                            rider->CastSpell(plrTarget, SPELL_MOUNTED_DUEL, true);
                    }
                    else if (Unit* unitTarget = GetHitUnit())
                    {
                        if (unitTarget->GetCharmer() && unitTarget->GetCharmer()->GetTypeId() == TYPEID_PLAYER && unitTarget->GetCharmer()->HasAura(SPELL_ON_TOURNAMENT_MOUNT))
                            rider->CastSpell(unitTarget->GetCharmer(), SPELL_MOUNTED_DUEL, true);
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_tournament_duel_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_tournament_duel_SpellScript();
        }
};

enum TournamentMountsSpells
{
    SPELL_LANCE_EQUIPPED = 62853,
};

class spell_gen_summon_tournament_mount : public SpellScriptLoader
{
    public:
        spell_gen_summon_tournament_mount() : SpellScriptLoader("spell_gen_summon_tournament_mount") { }

        class spell_gen_summon_tournament_mount_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_summon_tournament_mount_SpellScript);

            bool Validate(SpellInfo const* /*SpellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_LANCE_EQUIPPED))
                    return false;
                return true;
            }

            SpellCastResult CheckIfLanceEquiped()
            {
                if (GetCaster()->IsInDisallowedMountForm())
                    GetCaster()->RemoveAurasByType(SPELL_AURA_MOD_SHAPESHIFT);

                if (!GetCaster()->HasAura(SPELL_LANCE_EQUIPPED))
                {
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_MUST_HAVE_LANCE_EQUIPPED);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_summon_tournament_mount_SpellScript::CheckIfLanceEquiped);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_summon_tournament_mount_SpellScript();
        }
};

enum TournamentPennantSpells
{
    SPELL_PENNANT_STORMWIND_ASPIRANT      = 62595,
    SPELL_PENNANT_STORMWIND_VALIANT       = 62596,
    SPELL_PENNANT_STORMWIND_CHAMPION      = 62594,
    SPELL_PENNANT_GNOMEREGAN_ASPIRANT     = 63394,
    SPELL_PENNANT_GNOMEREGAN_VALIANT      = 63395,
    SPELL_PENNANT_GNOMEREGAN_CHAMPION     = 63396,
    SPELL_PENNANT_SEN_JIN_ASPIRANT        = 63397,
    SPELL_PENNANT_SEN_JIN_VALIANT         = 63398,
    SPELL_PENNANT_SEN_JIN_CHAMPION        = 63399,
    SPELL_PENNANT_SILVERMOON_ASPIRANT     = 63401,
    SPELL_PENNANT_SILVERMOON_VALIANT      = 63402,
    SPELL_PENNANT_SILVERMOON_CHAMPION     = 63403,
    SPELL_PENNANT_DARNASSUS_ASPIRANT      = 63404,
    SPELL_PENNANT_DARNASSUS_VALIANT       = 63405,
    SPELL_PENNANT_DARNASSUS_CHAMPION      = 63406,
    SPELL_PENNANT_EXODAR_ASPIRANT         = 63421,
    SPELL_PENNANT_EXODAR_VALIANT          = 63422,
    SPELL_PENNANT_EXODAR_CHAMPION         = 63423,
    SPELL_PENNANT_IRONFORGE_ASPIRANT      = 63425,
    SPELL_PENNANT_IRONFORGE_VALIANT       = 63426,
    SPELL_PENNANT_IRONFORGE_CHAMPION      = 63427,
    SPELL_PENNANT_UNDERCITY_ASPIRANT      = 63428,
    SPELL_PENNANT_UNDERCITY_VALIANT       = 63429,
    SPELL_PENNANT_UNDERCITY_CHAMPION      = 63430,
    SPELL_PENNANT_ORGRIMMAR_ASPIRANT      = 63431,
    SPELL_PENNANT_ORGRIMMAR_VALIANT       = 63432,
    SPELL_PENNANT_ORGRIMMAR_CHAMPION      = 63433,
    SPELL_PENNANT_THUNDER_BLUFF_ASPIRANT  = 63434,
    SPELL_PENNANT_THUNDER_BLUFF_VALIANT   = 63435,
    SPELL_PENNANT_THUNDER_BLUFF_CHAMPION  = 63436,
    SPELL_PENNANT_ARGENT_CRUSADE_ASPIRANT = 63606,
    SPELL_PENNANT_ARGENT_CRUSADE_VALIANT  = 63500,
    SPELL_PENNANT_ARGENT_CRUSADE_CHAMPION = 63501,
    SPELL_PENNANT_EBON_BLADE_ASPIRANT     = 63607,
    SPELL_PENNANT_EBON_BLADE_VALIANT      = 63608,
    SPELL_PENNANT_EBON_BLADE_CHAMPION     = 63609,
};

enum TournamentMounts
{
    NPC_STORMWIND_STEED             = 33217,
    NPC_IRONFORGE_RAM               = 33316,
    NPC_GNOMEREGAN_MECHANOSTRIDER   = 33317,
    NPC_EXODAR_ELEKK                = 33318,
    NPC_DARNASSIAN_NIGHTSABER       = 33319,
    NPC_ORGRIMMAR_WOLF              = 33320,
    NPC_DARK_SPEAR_RAPTOR           = 33321,
    NPC_THUNDER_BLUFF_KODO          = 33322,
    NPC_SILVERMOON_HAWKSTRIDER      = 33323,
    NPC_FORSAKEN_WARHORSE           = 33324,
    NPC_ARGENT_WARHORSE             = 33782,
    NPC_ARGENT_STEED_ASPIRANT       = 33845,
    NPC_ARGENT_HAWKSTRIDER_ASPIRANT = 33844,
};

enum TournamentQuestsAchievements
{
    ACHIEVEMENT_CHAMPION_STORMWIND     = 2781,
    ACHIEVEMENT_CHAMPION_DARNASSUS     = 2777,
    ACHIEVEMENT_CHAMPION_IRONFORGE     = 2780,
    ACHIEVEMENT_CHAMPION_GNOMEREGAN    = 2779,
    ACHIEVEMENT_CHAMPION_THE_EXODAR    = 2778,
    ACHIEVEMENT_CHAMPION_ORGRIMMAR     = 2783,
    ACHIEVEMENT_CHAMPION_SEN_JIN       = 2784,
    ACHIEVEMENT_CHAMPION_THUNDER_BLUFF = 2786,
    ACHIEVEMENT_CHAMPION_UNDERCITY     = 2787,
    ACHIEVEMENT_CHAMPION_SILVERMOON    = 2785,
    ACHIEVEMENT_ARGENT_VALOR           = 2758,
    ACHIEVEMENT_CHAMPION_ALLIANCE      = 2782,
    ACHIEVEMENT_CHAMPION_HORDE         = 2788,

    QUEST_VALIANT_OF_STORMWIND         = 13593,
    QUEST_A_VALIANT_OF_STORMWIND       = 13684,
    QUEST_VALIANT_OF_DARNASSUS         = 13706,
    QUEST_A_VALIANT_OF_DARNASSUS       = 13689,
    QUEST_VALIANT_OF_IRONFORGE         = 13703,
    QUEST_A_VALIANT_OF_IRONFORGE       = 13685,
    QUEST_VALIANT_OF_GNOMEREGAN        = 13704,
    QUEST_A_VALIANT_OF_GNOMEREGAN      = 13688,
    QUEST_VALIANT_OF_THE_EXODAR        = 13705,
    QUEST_A_VALIANT_OF_THE_EXODAR      = 13690,
    QUEST_VALIANT_OF_ORGRIMMAR         = 13707,
    QUEST_A_VALIANT_OF_ORGRIMMAR       = 13691,
    QUEST_VALIANT_OF_SEN_JIN           = 13708,
    QUEST_A_VALIANT_OF_SEN_JIN         = 13693,
    QUEST_VALIANT_OF_THUNDER_BLUFF     = 13709,
    QUEST_A_VALIANT_OF_THUNDER_BLUFF   = 13694,
    QUEST_VALIANT_OF_UNDERCITY         = 13710,
    QUEST_A_VALIANT_OF_UNDERCITY       = 13695,
    QUEST_VALIANT_OF_SILVERMOON        = 13711,
    QUEST_A_VALIANT_OF_SILVERMOON      = 13696,
};

class spell_gen_on_tournament_mount : public SpellScriptLoader
{
    public:
        spell_gen_on_tournament_mount() : SpellScriptLoader("spell_gen_on_tournament_mount") { }

        class spell_gen_on_tournament_mount_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_on_tournament_mount_AuraScript);

            uint32 _pennantSpellId;

            bool Load() override
            {
                _pennantSpellId = 0;
                return GetCaster() && GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            void HandleApplyEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                {
                    if (Unit* vehicle = caster->GetVehicleBase())
                    {
                        _pennantSpellId = GetPennatSpellId(caster->ToPlayer(), vehicle);
                        caster->CastSpell(caster, _pennantSpellId, true);
                    }
                }
            }

            void HandleRemoveEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                    caster->RemoveAurasDueToSpell(_pennantSpellId);
            }

            uint32 GetPennatSpellId(Player* player, Unit* mount)
            {
                switch (mount->GetEntry())
                {
                    case NPC_ARGENT_STEED_ASPIRANT:
                    case NPC_STORMWIND_STEED:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_STORMWIND))
                            return SPELL_PENNANT_STORMWIND_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_STORMWIND) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_STORMWIND))
                            return SPELL_PENNANT_STORMWIND_VALIANT;
                        else
                            return SPELL_PENNANT_STORMWIND_ASPIRANT;
                    }
                    case NPC_GNOMEREGAN_MECHANOSTRIDER:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_GNOMEREGAN))
                            return SPELL_PENNANT_GNOMEREGAN_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_GNOMEREGAN) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_GNOMEREGAN))
                            return SPELL_PENNANT_GNOMEREGAN_VALIANT;
                        else
                            return SPELL_PENNANT_GNOMEREGAN_ASPIRANT;
                    }
                    case NPC_DARK_SPEAR_RAPTOR:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_SEN_JIN))
                            return SPELL_PENNANT_SEN_JIN_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_SEN_JIN) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_SEN_JIN))
                            return SPELL_PENNANT_SEN_JIN_VALIANT;
                        else
                            return SPELL_PENNANT_SEN_JIN_ASPIRANT;
                    }
                    case NPC_ARGENT_HAWKSTRIDER_ASPIRANT:
                    case NPC_SILVERMOON_HAWKSTRIDER:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_SILVERMOON))
                            return SPELL_PENNANT_SILVERMOON_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_SILVERMOON) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_SILVERMOON))
                            return SPELL_PENNANT_SILVERMOON_VALIANT;
                        else
                            return SPELL_PENNANT_SILVERMOON_ASPIRANT;
                    }
                    case NPC_DARNASSIAN_NIGHTSABER:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_DARNASSUS))
                            return SPELL_PENNANT_DARNASSUS_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_DARNASSUS) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_DARNASSUS))
                            return SPELL_PENNANT_DARNASSUS_VALIANT;
                        else
                            return SPELL_PENNANT_DARNASSUS_ASPIRANT;
                    }
                    case NPC_EXODAR_ELEKK:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_THE_EXODAR))
                            return SPELL_PENNANT_EXODAR_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_THE_EXODAR) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_THE_EXODAR))
                            return SPELL_PENNANT_EXODAR_VALIANT;
                        else
                            return SPELL_PENNANT_EXODAR_ASPIRANT;
                    }
                    case NPC_IRONFORGE_RAM:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_IRONFORGE))
                            return SPELL_PENNANT_IRONFORGE_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_IRONFORGE) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_IRONFORGE))
                            return SPELL_PENNANT_IRONFORGE_VALIANT;
                        else
                            return SPELL_PENNANT_IRONFORGE_ASPIRANT;
                    }
                    case NPC_FORSAKEN_WARHORSE:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_UNDERCITY))
                            return SPELL_PENNANT_UNDERCITY_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_UNDERCITY) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_UNDERCITY))
                            return SPELL_PENNANT_UNDERCITY_VALIANT;
                        else
                            return SPELL_PENNANT_UNDERCITY_ASPIRANT;
                    }
                    case NPC_ORGRIMMAR_WOLF:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_ORGRIMMAR))
                            return SPELL_PENNANT_ORGRIMMAR_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_ORGRIMMAR) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_ORGRIMMAR))
                            return SPELL_PENNANT_ORGRIMMAR_VALIANT;
                        else
                            return SPELL_PENNANT_ORGRIMMAR_ASPIRANT;
                    }
                    case NPC_THUNDER_BLUFF_KODO:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_THUNDER_BLUFF))
                            return SPELL_PENNANT_THUNDER_BLUFF_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_THUNDER_BLUFF) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_THUNDER_BLUFF))
                            return SPELL_PENNANT_THUNDER_BLUFF_VALIANT;
                        else
                            return SPELL_PENNANT_THUNDER_BLUFF_ASPIRANT;
                    }
                    case NPC_ARGENT_WARHORSE:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_ALLIANCE) || player->HasAchieved(ACHIEVEMENT_CHAMPION_HORDE))
                            return player->getClass() == CLASS_DEATH_KNIGHT ? SPELL_PENNANT_EBON_BLADE_CHAMPION : SPELL_PENNANT_ARGENT_CRUSADE_CHAMPION;
                        else if (player->HasAchieved(ACHIEVEMENT_ARGENT_VALOR))
                            return player->getClass() == CLASS_DEATH_KNIGHT ? SPELL_PENNANT_EBON_BLADE_VALIANT : SPELL_PENNANT_ARGENT_CRUSADE_VALIANT;
                        else
                            return player->getClass() == CLASS_DEATH_KNIGHT ? SPELL_PENNANT_EBON_BLADE_ASPIRANT : SPELL_PENNANT_ARGENT_CRUSADE_ASPIRANT;
                    }
                    default:
                        return 0;
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_gen_on_tournament_mount_AuraScript::HandleApplyEffect, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                OnEffectRemove += AuraEffectRemoveFn(spell_gen_on_tournament_mount_AuraScript::HandleRemoveEffect, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_on_tournament_mount_AuraScript();
        }
};

class spell_gen_tournament_pennant : public SpellScriptLoader
{
    public:
        spell_gen_tournament_pennant() : SpellScriptLoader("spell_gen_tournament_pennant") { }

        class spell_gen_tournament_pennant_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_tournament_pennant_AuraScript);

            bool Load() override
            {
                return GetCaster() && GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            void HandleApplyEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                    if (!caster->GetVehicleBase())
                        caster->RemoveAurasDueToSpell(GetId());
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_gen_tournament_pennant_AuraScript::HandleApplyEffect, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_tournament_pennant_AuraScript();
        }
};

enum ChaosBlast
{
    SPELL_CHAOS_BLAST   = 37675,
};

class spell_gen_chaos_blast : public SpellScriptLoader
{
    public:
        spell_gen_chaos_blast() : SpellScriptLoader("spell_gen_chaos_blast") { }

        class spell_gen_chaos_blast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_chaos_blast_SpellScript)

            bool Validate(SpellInfo const* /*SpellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_CHAOS_BLAST))
                    return false;
                return true;
            }
            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                float basepoints0 = 100.f;
                Unit* caster = GetCaster();
                if (Unit* target = GetHitUnit())
                    caster->CastCustomSpell(target, SPELL_CHAOS_BLAST, &basepoints0, NULL, NULL, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_chaos_blast_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_chaos_blast_SpellScript();
        }

};

class spell_gen_wg_water : public SpellScriptLoader
{
    public:
        spell_gen_wg_water() : SpellScriptLoader("spell_gen_wg_water") {}

        class spell_gen_wg_water_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_wg_water_SpellScript);

            SpellCastResult CheckCast()
            {
                if (!GetSpellInfo()->CheckTargetCreatureType(GetCaster()))
                    return SPELL_FAILED_DONT_REPORT;
                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_wg_water_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_wg_water_SpellScript();
        }
};

class spell_gen_count_pct_from_max_hp : public SpellScriptLoader
{
    public:
        spell_gen_count_pct_from_max_hp(char const* name, int32 damagePct = 0) : SpellScriptLoader(name), _damagePct(damagePct) { }

        class spell_gen_count_pct_from_max_hp_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_count_pct_from_max_hp_SpellScript)

        public:
            spell_gen_count_pct_from_max_hp_SpellScript(int32 damagePct) : SpellScript(), _damagePct(damagePct) { }

            void RecalculateDamage()
            {
                if (!_damagePct)
                    _damagePct = GetHitDamage();

                SetHitDamage(GetHitUnit()->CountPctFromMaxHealth(_damagePct));
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_gen_count_pct_from_max_hp_SpellScript::RecalculateDamage);
            }

        private:
            int32 _damagePct;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_count_pct_from_max_hp_SpellScript(_damagePct);
        }

    private:
        int32 _damagePct;
};

class spell_gen_despawn_self : public SpellScriptLoader
{
public:
    spell_gen_despawn_self() : SpellScriptLoader("spell_gen_despawn_self") { }

    class spell_gen_despawn_self_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_despawn_self_SpellScript);

        bool Load() override
        {
            return GetCaster()->GetTypeId() == TYPEID_UNIT;
        }

        void HandleDummy(SpellEffIndex effIndex)
        {
            if (GetSpellInfo()->Effects[effIndex]->Effect == SPELL_EFFECT_DUMMY || GetSpellInfo()->Effects[effIndex]->Effect == SPELL_EFFECT_SCRIPT_EFFECT)
                GetCaster()->ToCreature()->DespawnOrUnsummon();
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_despawn_self_SpellScript::HandleDummy, EFFECT_ALL, SPELL_EFFECT_ANY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gen_despawn_self_SpellScript();
    }
};

class spell_gen_touch_the_nightmare : public SpellScriptLoader
{
public:
    spell_gen_touch_the_nightmare() : SpellScriptLoader("spell_gen_touch_the_nightmare") { }

    class spell_gen_touch_the_nightmare_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_touch_the_nightmare_SpellScript);

        void HandleDamageCalc(SpellEffIndex /*effIndex*/)
        {
            uint32 bp = GetCaster()->GetMaxHealth() * 0.3f;
            SetHitDamage(bp);
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_touch_the_nightmare_SpellScript::HandleDamageCalc, EFFECT_2, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gen_touch_the_nightmare_SpellScript();
    }
};

class spell_gen_dream_funnel: public SpellScriptLoader
{
public:
    spell_gen_dream_funnel() : SpellScriptLoader("spell_gen_dream_funnel") { }

    class spell_gen_dream_funnel_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_gen_dream_funnel_AuraScript);

        void HandleEffectCalcAmount(AuraEffect const* /*aurEff*/, float& amount, bool& canBeRecalculated)
        {
            if (GetCaster())
                amount = GetCaster()->GetMaxHealth() * 0.05f;

            canBeRecalculated = false;
        }

        void Register() override
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_dream_funnel_AuraScript::HandleEffectCalcAmount, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_dream_funnel_AuraScript::HandleEffectCalcAmount, EFFECT_2, SPELL_AURA_PERIODIC_DAMAGE);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_gen_dream_funnel_AuraScript();
    }
};

enum GenericBandage
{
    SPELL_RECENTLY_BANDAGED = 11196,
};

class spell_gen_bandage : public SpellScriptLoader
{
    public:
        spell_gen_bandage() : SpellScriptLoader("spell_gen_bandage") { }

        class spell_gen_bandage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_bandage_SpellScript);

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_RECENTLY_BANDAGED))
                    return false;
                return true;
            }

            SpellCastResult CheckCast()
            {
                if (Unit* target = GetExplTargetUnit())
                {
                    if (target->HasAura(SPELL_RECENTLY_BANDAGED))
                        return SPELL_FAILED_TARGET_AURASTATE;
                }
                return SPELL_CAST_OK;
            }

            void HandleScript()
            {
                if (Unit* target = GetHitUnit())
                    GetCaster()->CastSpell(target, SPELL_RECENTLY_BANDAGED, true);
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_bandage_SpellScript::CheckCast);
                AfterHit += SpellHitFn(spell_gen_bandage_SpellScript::HandleScript);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_bandage_SpellScript();
        }
};

enum GenericLifebloom
{
    SPELL_HEXLORD_MALACRASS_LIFEBLOOM_FINAL_HEAL        = 43422,
    SPELL_TUR_RAGEPAW_LIFEBLOOM_FINAL_HEAL              = 52552,
    SPELL_CENARION_SCOUT_LIFEBLOOM_FINAL_HEAL           = 53692,
    SPELL_TWISTED_VISAGE_LIFEBLOOM_FINAL_HEAL           = 57763,
    SPELL_FACTION_CHAMPIONS_DRU_LIFEBLOOM_FINAL_HEAL    = 66094,
};

class spell_gen_lifebloom : public SpellScriptLoader
{
    public:
        spell_gen_lifebloom(const char* name, uint32 spellId) : SpellScriptLoader(name), _spellId(spellId) { }

        class spell_gen_lifebloom_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_lifebloom_AuraScript);

        public:
            spell_gen_lifebloom_AuraScript(uint32 spellId) : AuraScript(), _spellId(spellId) { }

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(_spellId))
                    return false;
                return true;
            }

            void AfterRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                // Final heal only on duration end
                if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_EXPIRE && GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_ENEMY_SPELL)
                    return;

                // final heal
                GetTarget()->CastSpell(GetTarget(), _spellId, true, NULL, aurEff, GetCasterGUID());
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_gen_lifebloom_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL);
            }

        private:
            uint32 _spellId;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_lifebloom_AuraScript(_spellId);
        }

    private:
        uint32 _spellId;
};

enum SummonElemental
{
    SPELL_SUMMON_FIRE_ELEMENTAL  = 8985,
    SPELL_SUMMON_EARTH_ELEMENTAL = 19704
};

class spell_gen_summon_elemental : public SpellScriptLoader
{
    public:
        spell_gen_summon_elemental(const char* name, uint32 spellId) : SpellScriptLoader(name), _spellId(spellId) { }

        class spell_gen_summon_elemental_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_summon_elemental_AuraScript);

        public:
            spell_gen_summon_elemental_AuraScript(uint32 spellId) : AuraScript(), _spellId(spellId) { }

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(_spellId))
                    return false;
                return true;
            }

            void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetCaster())
                    if (Unit* owner = GetCaster()->GetOwner())
                        owner->CastSpell(owner, _spellId, true);
            }

            void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetCaster())
                    if (Unit* owner = GetCaster()->GetOwner())
                        if (owner->GetTypeId() == TYPEID_PLAYER) // todo: this check is maybe wrong
                            owner->ToPlayer()->RemovePet(NULL);
            }

            void Register() override
            {
                 AfterEffectApply += AuraEffectApplyFn(spell_gen_summon_elemental_AuraScript::AfterApply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                 AfterEffectRemove += AuraEffectRemoveFn(spell_gen_summon_elemental_AuraScript::AfterRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }

        private:
            uint32 _spellId;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_summon_elemental_AuraScript(_spellId);
        }

    private:
        uint32 _spellId;
};

enum Mounts
{
    SPELL_COLD_WEATHER_FLYING           = 54197,

    // Magic Broom
    SPELL_MAGIC_BROOM_60                = 42680,
    SPELL_MAGIC_BROOM_100               = 42683,
    SPELL_MAGIC_BROOM_150               = 42667,
    SPELL_MAGIC_BROOM_280               = 42668,

    // Headless Horseman's Mount
    SPELL_HEADLESS_HORSEMAN_MOUNT_60    = 51621,
    SPELL_HEADLESS_HORSEMAN_MOUNT_100   = 48024,
    SPELL_HEADLESS_HORSEMAN_MOUNT_150   = 51617,
    SPELL_HEADLESS_HORSEMAN_MOUNT_280   = 48023,

    // Winged Steed of the Ebon Blade
    SPELL_WINGED_STEED_150              = 54726,
    SPELL_WINGED_STEED_280              = 54727,

    // Big Love Rocket
    SPELL_BIG_LOVE_ROCKET_0             = 71343,
    SPELL_BIG_LOVE_ROCKET_60            = 71344,
    SPELL_BIG_LOVE_ROCKET_100           = 71345,
    SPELL_BIG_LOVE_ROCKET_150           = 71346,
    SPELL_BIG_LOVE_ROCKET_310           = 71347,

    // Invincible
    SPELL_INVINCIBLE_60                 = 72281,
    SPELL_INVINCIBLE_100                = 72282,
    SPELL_INVINCIBLE_150                = 72283,
    SPELL_INVINCIBLE_310                = 72284,

    // Blazing Hippogryph
    SPELL_BLAZING_HIPPOGRYPH_150        = 74854,
    SPELL_BLAZING_HIPPOGRYPH_280        = 74855,

    // Celestial Steed
    SPELL_CELESTIAL_STEED_60            = 75619,
    SPELL_CELESTIAL_STEED_100           = 75620,
    SPELL_CELESTIAL_STEED_150           = 75617,
    SPELL_CELESTIAL_STEED_280           = 75618,
    SPELL_CELESTIAL_STEED_310           = 76153,

    // X-53 Touring Rocket
    SPELL_X53_TOURING_ROCKET_150        = 75957,
    SPELL_X53_TOURING_ROCKET_280        = 75972,
    SPELL_X53_TOURING_ROCKET_310        = 76154,
};

class spell_gen_mount : public SpellScriptLoader
{
    public:
        spell_gen_mount(const char* name, uint32 mount0 = 0, uint32 mount60 = 0, uint32 mount100 = 0, uint32 mount150 = 0, uint32 mount280 = 0, uint32 mount310 = 0) : SpellScriptLoader(name),
            _mount0(mount0), _mount60(mount60), _mount100(mount100), _mount150(mount150), _mount280(mount280), _mount310(mount310) { }

        class spell_gen_mount_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_mount_SpellScript);

        public:
            spell_gen_mount_SpellScript(uint32 mount0, uint32 mount60, uint32 mount100, uint32 mount150, uint32 mount280, uint32 mount310) : SpellScript(),
                _mount0(mount0), _mount60(mount60), _mount100(mount100), _mount150(mount150), _mount280(mount280), _mount310(mount310) { }

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (_mount0 && !sSpellMgr->GetSpellInfo(_mount0))
                    return false;
                if (_mount60 && !sSpellMgr->GetSpellInfo(_mount60))
                    return false;
                if (_mount100 && !sSpellMgr->GetSpellInfo(_mount100))
                    return false;
                if (_mount150 && !sSpellMgr->GetSpellInfo(_mount150))
                    return false;
                if (_mount280 && !sSpellMgr->GetSpellInfo(_mount280))
                    return false;
                if (_mount310 && !sSpellMgr->GetSpellInfo(_mount310))
                    return false;
                return true;
            }

            void HandleMount(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);

                if (Player* target = GetHitPlayer())
                {
                    // Prevent stacking of mounts and client crashes upon dismounting
                    target->RemoveAurasByType(SPELL_AURA_MOUNTED, ObjectGuid::Empty, GetHitAura());

                    // Triggered spell id dependent on riding skill and zone
                    bool canFly = false;
                    uint32 map = sDB2Manager.GetVirtualMapForMapAndZone(target->GetMapId(), target->GetZoneId());
                    if (map == 530 || (map == 571 && target->HasSpell(SPELL_COLD_WEATHER_FLYING)))
                        canFly = true;

                    float x, y, z;
                    target->GetPosition(x, y, z);
                    uint32 areaFlag = 0; //target->GetBaseMap()->GetAreaFlag(x, y, z);
                    AreaTableEntry const* area = sAreaTableStore.LookupEntry(areaFlag);
                    if (!area || (canFly && (area->Flags[0] & AREA_FLAG_NO_FLY_ZONE)))
                        canFly = false;

                    uint32 mount = 0;
                    switch (target->GetBaseSkillValue(SKILL_RIDING))
                    {
                        case 0:
                            mount = _mount0;
                            break;
                        case 75:
                            mount = _mount60;
                            break;
                        case 150:
                            mount = _mount100;
                            break;
                        case 225:
                            if (canFly)
                                mount = _mount150;
                            else
                                mount = _mount100;
                            break;
                        case 300:
                            if (canFly)
                                mount = _mount280;
                            else
                                mount = _mount100;
                            break;
                        case 375:
                            if (canFly)
                                mount = _mount310;
                            else
                                mount = _mount100;
                            break;
                        default:
                            break;
                    }

                    if (mount)
                    {
                        PreventHitAura();
                        target->CastSpell(target, mount, true);
                    }
                }
            }

            void Register() override
            {
                 OnEffectHitTarget += SpellEffectFn(spell_gen_mount_SpellScript::HandleMount, EFFECT_2, SPELL_EFFECT_SCRIPT_EFFECT);
            }

        private:
            uint32 _mount0;
            uint32 _mount60;
            uint32 _mount100;
            uint32 _mount150;
            uint32 _mount280;
            uint32 _mount310;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_mount_SpellScript(_mount0, _mount60, _mount100, _mount150, _mount280, _mount310);
        }

    private:
        uint32 _mount0;
        uint32 _mount60;
        uint32 _mount100;
        uint32 _mount150;
        uint32 _mount280;
        uint32 _mount310;
};

enum FoamSword
{
    ITEM_FOAM_SWORD_GREEN = 45061,
    ITEM_FOAM_SWORD_PINK = 45176,
    ITEM_FOAM_SWORD_BLUE = 45177,
    ITEM_FOAM_SWORD_RED = 45178,
    ITEM_FOAM_SWORD_YELLOW = 45179,

    SPELL_BONKED = 62991,
    SPELL_FOAM_SWORD_DEFEAT = 62994,
    SPELL_ON_GUARD = 62972,
};

class spell_gen_upper_deck_create_foam_sword : public SpellScriptLoader
{
public:
    spell_gen_upper_deck_create_foam_sword() : SpellScriptLoader("spell_gen_upper_deck_create_foam_sword") { }

    class spell_gen_upper_deck_create_foam_sword_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_upper_deck_create_foam_sword_SpellScript);

        void HandleScript(SpellEffIndex effIndex)
        {
            if (Player* player = GetHitPlayer())
            {
                static uint32 const itemId[5] = { ITEM_FOAM_SWORD_GREEN, ITEM_FOAM_SWORD_PINK, ITEM_FOAM_SWORD_BLUE, ITEM_FOAM_SWORD_RED, ITEM_FOAM_SWORD_YELLOW };
                // player can only have one of these items
                for (uint8 i = 0; i < 5; ++i)
                {
                    if (player->HasItemCount(itemId[i], 1, true))
                        return;
                }

                CreateItem(effIndex, itemId[urand(0, 4)]);
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_upper_deck_create_foam_sword_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gen_upper_deck_create_foam_sword_SpellScript();
    }
};

class spell_gen_bonked : public SpellScriptLoader
{
public:
    spell_gen_bonked() : SpellScriptLoader("spell_gen_bonked") { }

    class spell_gen_bonked_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_bonked_SpellScript);

        void HandleScript(SpellEffIndex /*effIndex*/)
        {
            if (Player* target = GetHitPlayer())
            {
                Aura const* aura = GetHitAura();
                if (!(aura && aura->GetStackAmount() == 3))
                    return;

                target->CastSpell(target, SPELL_FOAM_SWORD_DEFEAT, true);
                target->RemoveAurasDueToSpell(SPELL_BONKED);

                if (Aura const* aura = target->GetAura(SPELL_ON_GUARD))
                {
                    if (Item* item = target->GetItemByGuid(aura->GetCastItemGUID()))
                        target->DestroyItemCount(item->GetEntry(), 1, true);
                }
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_bonked_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gen_bonked_SpellScript();
    }
};

enum RunningWildMountIds
{
    RUNNING_WILD_MODEL     = 73200,
    SPELL_ALTERED_FORM     = 97709,
};

class spell_gen_running_wild : public SpellScriptLoader
{
    public:
        spell_gen_running_wild() : SpellScriptLoader("spell_gen_running_wild") { }

        class spell_gen_running_wild_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_running_wild_AuraScript);

            void HandleMount(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();
                PreventDefaultAction();

                target->Mount(RUNNING_WILD_MODEL, 0, 0);

                // cast speed aura
                if (MountCapabilityEntry const* mountCapability = sMountCapabilityStore.LookupEntry(aurEff->GetAmount()))
                    target->CastSpell(target, mountCapability->ModSpellAuraID, TRIGGERED_FULL_MASK);
            }

            void HandleEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (auto target = GetTarget())
                    if (MountCapabilityEntry const* mountCapability = sMountCapabilityStore.LookupEntry(aurEff->GetAmount()))
                        target->RemoveAurasDueToSpell(mountCapability->ModSpellAuraID);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_gen_running_wild_AuraScript::HandleMount, EFFECT_1, SPELL_AURA_MOUNTED, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_gen_running_wild_AuraScript::HandleEffectRemove, EFFECT_1, SPELL_AURA_MOUNTED, AURA_EFFECT_HANDLE_REAL);
            }
        };

        class spell_gen_running_wild_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_running_wild_SpellScript);

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_ALTERED_FORM))
                    return false;
                return true;
            }

            bool Load() override
            {
                // Definitely not a good thing, but currently the only way to do something at cast start
                // Should be replaced as soon as possible with a new hook: BeforeCastStart
                GetCaster()->CastSpell(GetCaster(), SPELL_ALTERED_FORM, TRIGGERED_FULL_MASK);
                return false;
            }

            void Register() override
            {
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_running_wild_AuraScript();
        }

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_running_wild_SpellScript();
        }
};

class spell_gen_two_forms : public SpellScriptLoader
{
    public:
        spell_gen_two_forms() : SpellScriptLoader("spell_gen_two_forms") { }

        class spell_gen_two_forms_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_two_forms_SpellScript);

            SpellCastResult CheckCast()
            {
                if (GetCaster()->isInCombat())
                {
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_CANT_TRANSFORM);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                // Player cannot transform to human form if he is forced to be worgen for some reason (Darkflight)
                if (GetCaster()->GetAuraTypeCount(SPELL_AURA_WORGEN_ALTERED_FORM) > 1)
                {
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_CANT_TRANSFORM);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                return SPELL_CAST_OK;
            }

            void HandleTransform(SpellEffIndex effIndex)
            {
                Unit* target = GetHitUnit();
                PreventHitDefaultEffect(effIndex);
                if (target->HasAuraType(SPELL_AURA_WORGEN_ALTERED_FORM))
                    target->RemoveAurasByType(SPELL_AURA_WORGEN_ALTERED_FORM);
                else    // Basepoints 1 for this aura control whether to trigger transform transition animation or not.
                    target->CastCustomSpell(SPELL_ALTERED_FORM, SPELLVALUE_BASE_POINT0, 1, target, true);
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_two_forms_SpellScript::CheckCast);
                OnEffectHitTarget += SpellEffectFn(spell_gen_two_forms_SpellScript::HandleTransform, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_two_forms_SpellScript();
        }
};

class spell_gen_darkflight : public SpellScriptLoader
{
    public:
        spell_gen_darkflight() : SpellScriptLoader("spell_gen_darkflight") { }

        class spell_gen_darkflight_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_darkflight_SpellScript);

            void TriggerTransform()
            {
                GetCaster()->CastSpell(GetCaster(), SPELL_ALTERED_FORM, TRIGGERED_FULL_MASK);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_gen_darkflight_SpellScript::TriggerTransform);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_darkflight_SpellScript();
        }
};

#define GOBELING_GUMBO_BURP 42755

// Gobelin Gumbo - 42760
class spell_gen_gobelin_gumbo : public SpellScriptLoader
{
    public:
        spell_gen_gobelin_gumbo() : SpellScriptLoader("spell_gen_gobelin_gumbo") { }

        class spell_gen_gobelin_gumbo_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_gobelin_gumbo_AuraScript);

            uint32 burpTimer;

            bool Load() override
            {
                burpTimer = 10000;
                return true;
            }

            void OnUpdate(uint32 diff, AuraEffect* aurEff)
            {
                if (GetCaster())
                {
                    if (burpTimer <= diff)
                    {
                        burpTimer = 10000;
                        if (roll_chance_i(30))
                            GetCaster()->CastSpell(GetCaster(), GOBELING_GUMBO_BURP, true);
                    }
                    else
                        burpTimer -= diff;
                }
            }

            void Register() override
            {
                OnEffectUpdate += AuraEffectUpdateFn(spell_gen_gobelin_gumbo_AuraScript::OnUpdate, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_gobelin_gumbo_AuraScript();
        }
};

enum eBrewfestSpeedSpells
{
    SPELL_BREWFEST_RAM          = 43880,
    SPELL_RAM_FATIGUE           = 43052,
    SPELL_SPEED_RAM_GALLOP      = 42994,
    SPELL_SPEED_RAM_CANTER      = 42993,
    SPELL_SPEED_RAM_TROT        = 42992,
    SPELL_SPEED_RAM_NORMAL      = 43310,
    SPELL_SPEED_RAM_EXHAUSED    = 43332
};

class spell_brewfest_speed : public SpellScriptLoader
{
public:
    spell_brewfest_speed() : SpellScriptLoader("spell_brewfest_speed") {}

    class spell_brewfest_speed_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_brewfest_speed_AuraScript)
        bool Validate(SpellInfo const * /*SpellInfo*/) override
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_RAM_FATIGUE))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_BREWFEST_RAM))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_SPEED_RAM_GALLOP))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_SPEED_RAM_CANTER))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_SPEED_RAM_TROT))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_SPEED_RAM_NORMAL))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_SPEED_RAM_GALLOP))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_SPEED_RAM_EXHAUSED))
                return false;
            return true;
        }

        void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
        {
            if (GetId() == SPELL_SPEED_RAM_EXHAUSED)
                return;
            Player* pCaster = GetCaster()->ToPlayer();
            if (!pCaster)
                return;
            int i;
            switch (GetId())
            {
                case SPELL_SPEED_RAM_GALLOP:
                    for (i = 0; i < 5; i++)
                       pCaster->AddAura(SPELL_RAM_FATIGUE,pCaster);
                    break;
                case SPELL_SPEED_RAM_CANTER:
                    pCaster->AddAura(SPELL_RAM_FATIGUE,pCaster);
                    break;
                case SPELL_SPEED_RAM_TROT:
                    if (pCaster->HasAura(SPELL_RAM_FATIGUE))
                        if (pCaster->GetAura(SPELL_RAM_FATIGUE)->GetStackAmount() <= 2)
                            pCaster->RemoveAura(SPELL_RAM_FATIGUE);
                        else
                            pCaster->GetAura(SPELL_RAM_FATIGUE)->ModStackAmount(-2);
                    break;
                case SPELL_SPEED_RAM_NORMAL:
                    if (pCaster->HasAura(SPELL_RAM_FATIGUE))
                        if (pCaster->GetAura(SPELL_RAM_FATIGUE)->GetStackAmount() <= 4)
                            pCaster->RemoveAura(SPELL_RAM_FATIGUE);
                        else
                            pCaster->GetAura(SPELL_RAM_FATIGUE)->ModStackAmount(-4);
                    break;
            }
            if (pCaster->HasAura(SPELL_RAM_FATIGUE))
                if (pCaster->GetAura(SPELL_RAM_FATIGUE)->GetStackAmount() >= 100)
                    pCaster->CastSpell(pCaster,SPELL_SPEED_RAM_EXHAUSED, false);
        }

        void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Player* pCaster = GetCaster()->ToPlayer();
            if (!pCaster)
                return;
            if (!pCaster->HasAura(SPELL_BREWFEST_RAM))
                return;
            if (GetId() == SPELL_SPEED_RAM_EXHAUSED) 
            {
                if (pCaster->HasAura(SPELL_RAM_FATIGUE))
                    pCaster->GetAura(SPELL_RAM_FATIGUE)->ModStackAmount(-15);
            } else if (!pCaster->HasAura(SPELL_RAM_FATIGUE) || pCaster->GetAura(SPELL_RAM_FATIGUE)->GetStackAmount() < 100)
                switch (GetId())
                {
                    case SPELL_SPEED_RAM_GALLOP:
                        if (!pCaster->HasAura(SPELL_SPEED_RAM_EXHAUSED))
                            pCaster->CastSpell(pCaster,SPELL_SPEED_RAM_CANTER, false);
                        break;
                    case SPELL_SPEED_RAM_CANTER:
                        if (!pCaster->HasAura(SPELL_SPEED_RAM_GALLOP))
                            pCaster->CastSpell(pCaster,SPELL_SPEED_RAM_TROT, false);
                        break;
                    case SPELL_SPEED_RAM_TROT:
                        if (!pCaster->HasAura(SPELL_SPEED_RAM_CANTER))
                            pCaster->CastSpell(pCaster,SPELL_SPEED_RAM_NORMAL, false);
                        break;
                }
        }

        void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Player* pCaster = GetCaster()->ToPlayer();
            if (!pCaster)
                return;
            switch (GetId())
            {
                case SPELL_SPEED_RAM_GALLOP:
                    pCaster->GetAura(SPELL_SPEED_RAM_GALLOP)->SetDuration(4000);
                    break;
                case SPELL_SPEED_RAM_CANTER:
                    pCaster->GetAura(SPELL_SPEED_RAM_CANTER)->SetDuration(4000);
                    break;
                case SPELL_SPEED_RAM_TROT:
                    pCaster->GetAura(SPELL_SPEED_RAM_TROT)->SetDuration(4000);
                    break;
            }
        }

        void Register() override
        {
            OnEffectApply += AuraEffectApplyFn(spell_brewfest_speed_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_MOD_INCREASE_MOUNTED_SPEED, AURA_EFFECT_HANDLE_REAL);
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_brewfest_speed_AuraScript::HandleEffectPeriodic,EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            OnEffectRemove += AuraEffectRemoveFn(spell_brewfest_speed_AuraScript::HandleEffectRemove, EFFECT_2, SPELL_AURA_USE_NORMAL_MOVEMENT_SPEED, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_brewfest_speed_AuraScript();
    }
};

class spell_gen_tricky_treat : public SpellScriptLoader
{
    public:
        spell_gen_tricky_treat() : SpellScriptLoader("spell_gen_tricky_treat") {}

        class spell_gen_tricky_treat_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_tricky_treat_SpellScript);

            bool Validate(SpellInfo const* /*SpellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_TRICKY_TREAT_SPEED))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_TRICKY_TREAT_TRIGGER))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_UPSET_TUMMY))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Unit* caster = GetCaster())
                    if (caster->HasAura(SPELL_TRICKY_TREAT_TRIGGER) && caster->GetAuraCount(SPELL_TRICKY_TREAT_SPEED) > 3 && roll_chance_i(33))
                        caster->CastSpell(caster, SPELL_UPSET_TUMMY, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_tricky_treat_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_tricky_treat_SpellScript();
        }
};

class spell_gen_leviroth_self_impale : public SpellScriptLoader
{
public:
    spell_gen_leviroth_self_impale() : SpellScriptLoader("spell_gen_leviroth_self_impale") { }

    class spell_gen_leviroth_self_impale_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_leviroth_self_impale_SpellScript);

        void HandleDamageCalc(SpellEffIndex /*effIndex*/)
        {
            uint32 bp = GetCaster()->CountPctFromCurHealth(93);
            SetHitDamage(bp);
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_leviroth_self_impale_SpellScript::HandleDamageCalc, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gen_leviroth_self_impale_SpellScript();
    }
};

// spell 147655 - Токсин хваткой лягушки
class spell_gulp_frog_toxin : public SpellScriptLoader
{
    public:
        spell_gulp_frog_toxin() : SpellScriptLoader("spell_gulp_frog_toxin") { }

        class spell_gulp_frog_toxinAuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gulp_frog_toxinAuraScript);

            uint32 lastStack;

            bool Load() override
            {
                lastStack = 0;
                return true;
            }

            void OnStackChange(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* target = GetTarget())
                    if(lastStack == 10)
                        target->CastSpell(target, 147656, false);

                lastStack = GetStackAmount();
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_gulp_frog_toxinAuraScript::OnStackChange, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        // function which creates AuraScript
        AuraScript* GetAuraScript() const override
        {
            return new spell_gulp_frog_toxinAuraScript();
        }
};

// spell 147280 - Time-Lost Wisdom
class spell_time_lost_wisdom : public SpellScriptLoader
{
    public:
        spell_time_lost_wisdom() : SpellScriptLoader("spell_time_lost_wisdom") { }

        class spell_time_lost_wisdom_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_time_lost_wisdom_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Unit* target = GetHitUnit())
                {
                    switch (urand(0, 4))
                    {
                        case 0: target->CastSpell(target, 147281, true); break;
                        case 1: target->CastSpell(target, 147282, true); break;
                        case 2: target->CastSpell(target, 147283, true); break;
                        case 3: target->CastSpell(target, 147284, true); break;
                        case 4: target->CastSpell(target, 147285, true); break;
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_time_lost_wisdom_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_time_lost_wisdom_SpellScript();
        }
};

// spell  98507 - Brutal Assault
class spell_gen_brutal_assault : public SpellScriptLoader
{
    public:
        spell_gen_brutal_assault() : SpellScriptLoader("spell_gen_brutal_assault") { }

        class spell_gen_brutal_assaultAuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_brutal_assaultAuraScript);

            void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                int32 amount = aurEff->GetOldBaseAmount() + aurEff->GetAmount();
                if(AuraEffect* aurEffSelf = GetEffect(EFFECT_0))
                    aurEffSelf->SetAmount(amount);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_gen_brutal_assaultAuraScript::OnApply, EFFECT_0, SPELL_AURA_USE_NORMAL_MOVEMENT_SPEED, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        // function which creates AuraScript
        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_brutal_assaultAuraScript();
        }
};

// spell  110310 - Dampening for arena
class spell_gen_dampening : public SpellScriptLoader
{
    public:
        spell_gen_dampening() : SpellScriptLoader("spell_gen_dampening") { }

        class spell_gen_dampeningAuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_dampeningAuraScript);

            int32 amount = 0;

            void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (Player* player = GetCaster()->ToPlayer())
                {
                    if (player->GetBattleground() && player->GetBattleground()->GetJoinType() == MS::Battlegrounds::JoinType::Arena2v2)
                    {
                        amount = 20;
                        const_cast<AuraEffect*>(aurEff)->ChangeAmount(amount);
                    }
                }
            }

            void HandlePeriodicTick(AuraEffect const* aurEff)
            {
                Player* player = GetCaster()->ToPlayer();
                if (!player)
                    return;

                if (AuraEffect* aurEff0 = aurEff->GetBase()->GetEffect(EFFECT_0))
                {
                    amount = aurEff0->GetAmount() + 1;
                    if (amount >= 100)
                        return;

                    aurEff0->ChangeAmount(amount);

                    float bp0 = -amount;
                    player->CastCustomSpell(player, 74410, &bp0, &bp0, &bp0, true);
                }
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_gen_dampeningAuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_gen_dampeningAuraScript::HandlePeriodicTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_dampeningAuraScript();
        }
};

// spell  121176 - Orb of Power
class spell_gen_orb_of_power : public SpellScriptLoader
{
    public:
        spell_gen_orb_of_power() : SpellScriptLoader("spell_gen_orb_of_power") { }

        class spell_gen_orb_of_powerAuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_orb_of_powerAuraScript);

            uint8 ticks = 0;

            void HandleEffectPeriodicUpdate(AuraEffect* aurEff)
            {
                if (AuraEffect* aurEff0 = aurEff->GetBase()->GetEffect(EFFECT_0))
                    aurEff0->SetAmount(aurEff0->GetAmount() + aurEff0->GetBaseAmount());
                if (AuraEffect* aurEff1 = aurEff->GetBase()->GetEffect(EFFECT_1))
                    aurEff1->SetAmount(aurEff1->GetAmount() + aurEff1->GetBaseAmount());
                if (AuraEffect* aurEff2 = aurEff->GetBase()->GetEffect(EFFECT_2))
                {
                    int32 amount = aurEff2->GetAmount() + aurEff2->GetBaseAmount();

                    if (AuraApplication * aurApp = GetAura()->GetApplicationOfTarget(GetCasterGUID()))
                    {
                        aurEff2->HandleEffect(aurApp, AURA_EFFECT_HANDLE_REAL, false);
                        aurEff2->SetAmount(amount);
                        aurEff2->HandleEffect(aurApp, AURA_EFFECT_HANDLE_REAL, true);
                    }
                }
                
                if (Player* caster = GetCaster()->ToPlayer())
                {
                    if (caster->GetPositionX() > 1749 && caster->GetPositionX() < 1816 && caster->GetPositionY() > 1287 && caster->GetPositionY() < 1379)
                    {
                        ticks++;

                        if (ticks == 6)
                            caster->UpdateAchievementCriteria(CRITERIA_TYPE_BE_SPELL_TARGET, 128768); // Credit Achieve - Powerball
                    }
                    else
                        ticks = 0;
                }
            }

            void Register() override
            {
                OnEffectUpdatePeriodic += AuraEffectUpdatePeriodicFn(spell_gen_orb_of_powerAuraScript::HandleEffectPeriodicUpdate, EFFECT_3, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        // function which creates AuraScript
        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_orb_of_powerAuraScript();
        }
};

enum
{
    SPELL_MAGE_TEMPORAL_DISPLACEMENT             = 80354,
    HUNTER_SPELL_INSANITY                        = 95809,
    HUNTER_SPELL_FATIGUED                        = 160455,
    SPELL_SHAMAN_SATED                           = 57724,
    SPELL_SHAMAN_EXHAUSTED                       = 57723,

};
// Drums of Rage - 146555, 178207
class spell_gen_drums_of_rage : public SpellScriptLoader
{
    public:
        spell_gen_drums_of_rage() : SpellScriptLoader("spell_gen_drums_of_rage") { }

        class spell_gen_drums_of_rage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_drums_of_rage_SpellScript);

            void RemoveInvalidTargets(std::list<WorldObject*>& targets)
            {
                std::list<WorldObject*> removeList;

                for (auto itr : targets)
                {
                    if (Unit* unit = itr->ToUnit())
                    {
                        if (unit->HasAura(207970)) // Shard of the Exodar
                            continue;

                        if (unit->HasAura(HUNTER_SPELL_INSANITY) || unit->HasAura(HUNTER_SPELL_FATIGUED) || unit->HasAura(SPELL_SHAMAN_EXHAUSTED) || unit->HasAura(SPELL_SHAMAN_SATED) || unit->HasAura(SPELL_MAGE_TEMPORAL_DISPLACEMENT))
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
                if (Player* caster = GetCaster()->ToPlayer())
                {
                    if (Unit* target = GetHitUnit())
                    {
                        if (caster->GetTeam() == HORDE)
                            target->CastSpell(target, SPELL_SHAMAN_SATED, true);
                        else
                            target->CastSpell(target, SPELL_SHAMAN_EXHAUSTED, true);
                    }
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_gen_drums_of_rage_SpellScript::RemoveInvalidTargets, EFFECT_0, TARGET_UNIT_CASTER_AREA_RAID);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_gen_drums_of_rage_SpellScript::RemoveInvalidTargets, EFFECT_1, TARGET_UNIT_CASTER_AREA_RAID);
                AfterHit += SpellHitFn(spell_gen_drums_of_rage_SpellScript::ApplyDebuff);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_drums_of_rage_SpellScript();
        }
};

// Guild Battle Standard
class spell_gen_battle_guild_standart : public SpellScriptLoader
{
    public:
        spell_gen_battle_guild_standart() : SpellScriptLoader("spell_gen_battle_guild_standart") { }

        class spell_gen_battle_guild_standart_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_battle_guild_standart_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                //Updated by CyberBrest
                if (Creature* pStandart = GetCaster()->ToCreature())
                {
                    Unit* owner = pStandart->GetOwner();
                    if (!owner)
                    {
                        targets.clear();
                        return;
                    }

                    Player* player = owner->ToPlayer();
                    if (!player)
                    {
                        targets.clear();
                        return;
                    }

                    ObjectGuid guildID = player->GetGuildGUID();
                    if (!guildID)
                    {
                        targets.clear();
                        return;
                    }

                    targets.remove_if(GuildCheck(pStandart->GetGUID(), guildID));
                }
                else
                    targets.clear();
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_gen_battle_guild_standart_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_gen_battle_guild_standart_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ALLY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_gen_battle_guild_standart_SpellScript::FilterTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ALLY);
            }

        private:
            class GuildCheck
            {
                public:
                    GuildCheck(ObjectGuid casterGUID, ObjectGuid guildId) : _casterGUID(casterGUID), _guildId(guildId) {}

                    bool operator()(WorldObject* unit)
                    {
                        if (!_guildId)
                            return true;

                        if (unit->GetTypeId() != TYPEID_PLAYER)
                            return true;

                        if (unit->ToPlayer()->GetGuildGUID() != _guildId)
                            return true;

                        if (Aura* const aur = unit->ToPlayer()->GetAura(90216))
                            if (aur->GetCasterGUID() != _casterGUID)
                                return true;

                        if (Aura* const aur = unit->ToPlayer()->GetAura(90708))
                            if (aur->GetCasterGUID() != _casterGUID)
                                return true;

                        return false;
                    }

                private:
                    ObjectGuid _casterGUID;
                    ObjectGuid _guildId;
            };
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_battle_guild_standart_SpellScript();
        }

        class spell_gen_battle_guild_standart_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_battle_guild_standart_AuraScript);


            void HandleTriggerSpell(AuraEffect const* /*aurEff*/)
            {
                if (Unit* pStandart = GetCaster())
                {                                 
                    float bp0 = 0.f;
                    float bp1 = 0.f;
                         
                    switch (pStandart->GetEntry())
                    {                        
                        case 48636: // 5%, horde
                            bp0 = 5;
                            break;
                        case 48637: // 10%, horde
                            bp0 = 10;
                            break;
                        case 48638: // 15%, horde
                            bp0 = 15;
                            break;
                        case 48115: // 5%, alliance
                            bp1 = 5;
                            break;
                        case 48633: // 10%, alliance
                            bp1 = 10;
                            break;
                        case 48634: // 15%, alliance
                            bp1 = 15;
                            break;
                    }
                    if (bp0)
                        pStandart->CastCustomSpell(pStandart, 90708, &bp0, &bp0, &bp0, true);
                    
                    if (bp1)
                        pStandart->CastCustomSpell(pStandart, 90216, &bp1, &bp1, &bp1, true);
                }
            }

            void Register() override
            {  
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_gen_battle_guild_standart_AuraScript::HandleTriggerSpell, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_battle_guild_standart_AuraScript();
        }
};

enum MineSpells
{
    SPELL_ACHIEV_MINE_KNOCKBACK = 54402,
    SPELL_ACHIEV_MINE_STACK     = 57099,
    SPELL_ACHIEV_MINE_CREDIT    = 57064,
};
// Achiev Mine Sweeper http://www.wowhead.com/achievement=1428
// http://www.wowhead.com/spell=54355
class spell_gen_landmine_knockback : public SpellScriptLoader
{
    public:
        spell_gen_landmine_knockback() : SpellScriptLoader("spell_gen_landmine_knockback") { }

        class spell_gen_landmine_knockback_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_landmine_knockback_SpellScript);

            void HandleOnHit()
            {
                Unit* caster = GetCaster();
                if (!caster)
                    return;

                Unit* target = GetHitUnit();
                if (!target)
                    return;

                caster->CastSpell(target, SPELL_ACHIEV_MINE_KNOCKBACK, true);

                if (Aura* aur = target->GetAura(SPELL_ACHIEV_MINE_STACK))
                    if (aur->GetStackAmount() == 10)
                        caster->CastSpell(target, SPELL_ACHIEV_MINE_CREDIT, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_gen_landmine_knockback_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_landmine_knockback_SpellScript();
        }
};

enum ICSeaforiumSpells
{
    SPELL_BOMB_CREDIT_1 = 68366,
    SPELL_BOMB_CREDIT_2 = 68367, // huge
};
//Isle of Conquest: A-bomb-inable
class spell_gen_ic_seaforium_blast : public SpellScriptLoader
{
    public:
        spell_gen_ic_seaforium_blast() : SpellScriptLoader("spell_gen_ic_seaforium_blast") {}

        class spell_gen_ic_seaforium_blast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_ic_seaforium_blast_SpellScript);

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_BOMB_CREDIT_1))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_BOMB_CREDIT_2))
                    return false;
                return true;
            }

            bool Load() override
            {
                return GetOriginalCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            void AchievementCredit(SpellEffIndex /*effIndex*/)
            {
                if (Unit* originalCaster = GetOriginalCaster())
                    if (GameObject* go = GetHitGObj())
                        if (go->GetGOInfo()->type == GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING)
                        {
                            if (m_scriptSpellId == 66676 || m_scriptSpellId == 67814)
                                originalCaster->CastSpell(originalCaster, SPELL_BOMB_CREDIT_1, true);
                            else if (m_scriptSpellId == 66672 || m_scriptSpellId ==  67813)
                                originalCaster->CastSpell(originalCaster, SPELL_BOMB_CREDIT_2, true);
                        }

            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_ic_seaforium_blast_SpellScript::AchievementCredit, EFFECT_1, SPELL_EFFECT_GAMEOBJECT_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_ic_seaforium_blast_SpellScript();
        }
};

class spell_gen_cooking_way : public SpellScriptLoader
{
    public:
        spell_gen_cooking_way() : SpellScriptLoader("spell_gen_cooking_way") { }

        class spell_gen_cooking_way_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_cooking_way_SpellScript);

            void HandleAfterCast()
            {
                Player* caster = GetCaster()->ToPlayer();
                if (!caster)
                    return;

                uint32 skillid = GetSpellInfo()->Effects[1]->MiscValue;
                caster->SetSkill(skillid, GetSpellInfo()->Effects[1]->CalcValue(), 525, 600);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_gen_cooking_way_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_cooking_way_SpellScript();
        }
};

//43681
class spell_gen_bg_inactive : public SpellScriptLoader
{
    public:
        spell_gen_bg_inactive() : SpellScriptLoader("spell_gen_bg_inactive") {}

        class spell_gen_bg_inactive_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_bg_inactive_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Player* player = GetCaster()->ToPlayer())
                {
                    if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_EXPIRE)
                        return;

                    if (player->GetMap()->IsBattleground())
                        player->LeaveBattleground();
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_gen_bg_inactive_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_SPELL_AURA_MOD_HONOR_POINTS_GAIN, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_bg_inactive_AuraScript();
        }
};

class spell_gen_bounce_achievement : public SpellScriptLoader
{
    public:
        spell_gen_bounce_achievement() : SpellScriptLoader("spell_gen_bounce_achievement") { }

        class spell_gen_bounce_achievement_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_bounce_achievement_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Player* pCaster = GetCaster()->ToPlayer())
                    pCaster->GetAchievementMgr().ResetAchievementCriteria(CRITERIA_TYPE_BE_SPELL_TARGET, CRITERIA_CONDITION_NOT_LOSE_AURA, 95529);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_gen_bounce_achievement_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_bounce_achievement_AuraScript();
        }
};

//193378, 193379, 193380, 193381, 193382, 193383, 193384, 205139, 205695, 205712, 205713, 205714
class spell_gen_herbalism_trap : public SpellScriptLoader
{
public:
    spell_gen_herbalism_trap() : SpellScriptLoader("spell_gen_herbalism_trap") { }

    class spell_gen_herbalism_trap_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_herbalism_trap_SpellScript);

        void HandleDummy(SpellEffIndex effIndex)
        {
            Player* caster = GetCaster()->ToPlayer();
            if (!caster)
                return;

            switch (RoundingFloatValue(GetSpellInfo()->Effects[effIndex]->BasePoints))
            {
                case 201702: //Legion Herbalist Learning
                {
                    if (!caster->HasSpell(195114)) //Check Legion Herbalist skill
                        caster->CastSpell(caster, GetSpellInfo()->Effects[effIndex]->BasePoints, true);
                    break;
                }
                default:
                    break;
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_herbalism_trap_SpellScript::HandleDummy, EFFECT_ALL, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gen_herbalism_trap_SpellScript();
    }
};

//185496, 185528, 185529, 185768, 185769, 185770, 204918, 204981, 205009, 205060, 205802, 205804, 205805
class spell_gen_mining_trap : public SpellScriptLoader
{
public:
    spell_gen_mining_trap() : SpellScriptLoader("spell_gen_mining_trap") { }

    class spell_gen_mining_trap_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_mining_trap_SpellScript);

        void HandleDummy(SpellEffIndex effIndex)
        {
            Player* caster = GetCaster()->ToPlayer();
            if (!caster)
                return;

            switch (RoundingFloatValue(GetSpellInfo()->Effects[effIndex]->BasePoints))
            {
                case 201706: //Legion Mining Learning
                {
                    if (!caster->HasSpell(195122)) //Check Legion Mining skill
                        caster->CastSpell(caster, GetSpellInfo()->Effects[effIndex]->BasePoints, true);
                    break;
                }
                default:
                    break;
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_mining_trap_SpellScript::HandleDummy, EFFECT_ALL, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gen_mining_trap_SpellScript();
    }
};

//8613
class spell_gen_learn_legion_skinning : public SpellScriptLoader
{
public:
    spell_gen_learn_legion_skinning() : SpellScriptLoader("spell_gen_learn_legion_skinning") { }

    class spell_gen_learn_legion_skinning_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_learn_legion_skinning_SpellScript);

        void HandleDummy(SpellEffIndex effIndex)
        {
            Player* caster = GetCaster()->ToPlayer();
            if (!caster)
                return;

            if (!caster->HasSpell(195125)) //Check Legion Skinning skill
            {
                if (Creature* creature = GetHitUnit()->ToCreature())
                    if (creature->GetCreatureTemplate()->RequiredExpansion == EXPANSION_LEGION)
                        caster->CastSpell(caster, 201707, true);
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_learn_legion_skinning_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_SKINNING);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gen_learn_legion_skinning_SpellScript();
    }
};

//! 192190
//! triggered from spells: 210908, 192190 at quest complete.
//! for example SELECT * FROM `quest_template` WHERE `RewardDisplaySpell` = 210908
class spell_gen_create_artefact : public SpellScriptLoader
{
public:
    spell_gen_create_artefact() : SpellScriptLoader("spell_gen_create_artefact") { }

    class spell_gen_create_artefact_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_create_artefact_SpellScript);

        enum sp
        {
            __HORDE = 192191,
            __ALLIANCE = 185506,
        };
        void HandleDummy(SpellEffIndex effIndex)
        {
            Player* caster = GetCaster()->ToPlayer();
            if (!caster)
                return;
            
            caster->CastSpell(caster, caster->GetTeam() == HORDE ? __HORDE : __ALLIANCE, true);
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_create_artefact_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gen_create_artefact_SpellScript();
    }
};

// SPELL_PRINCIPLES_OF_WAR - 197912
class spell_principles_of_war : public SpellScriptLoader
{
public:
    spell_principles_of_war() : SpellScriptLoader("spell_principles_of_war") {}

    class spell_principles_of_war_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_principles_of_war_AuraScript);

        void CalculateAmount(AuraEffect const* /*aurEff*/, float& amount, bool& canBeRecalculated)
        {
            auto caster = GetCaster();
            if (!caster)
                return;

            auto player = caster->ToPlayer();
            if (!player)
                return;

            canBeRecalculated = true;

            float itemLevel = player->GetFloatValue(PLAYER_FIELD_AVG_ITEM_LEVEL);
            if (itemLevel > 1000.f)
                itemLevel = 1000.f;
            
            if (itemLevel < 800.f)
            {
                amount = 0;
                return;
            }

            amount = RoundingFloatValue((itemLevel - 800.f) * 0.1f);
        }

        void Register() override
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_principles_of_war_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_principles_of_war_AuraScript();
    }
};

// Blessing of the Light - 220058, 220887, 220792, 220110
class spell_gen_absorb_if_health : public SpellScriptLoader
{
    public:
        spell_gen_absorb_if_health() : SpellScriptLoader("spell_gen_absorb_if_health") { }

        class spell_gen_absorb_if_health_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_absorb_if_health_AuraScript);

            void CalculateAmount(AuraEffect const* /*AuraEffect**/, float& amount, bool& /*canBeRecalculated*/)
            {
                amount = -1;
            }

            void Absorb(AuraEffect* /*aurEff*/, DamageInfo & dmgInfo, float & absorbAmount)
            {
                Unit* target = GetCaster();

                if (dmgInfo.GetDamage() < target->GetHealth())
                    return;
                int32 health = GetSpellInfo()->Id == 220110 ? 220113 : 220073;
                int32 knock = GetSpellInfo()->Id == 220110 ? 220111 : 220103;

                target->CastSpell(target, health, true);
                target->CastSpell(target, knock, true);
                absorbAmount = dmgInfo.GetDamage();
                GetAura()->SetDuration(250);
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_absorb_if_health_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_gen_absorb_if_health_AuraScript::Absorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_absorb_if_health_AuraScript();
        }
};

//51640
class spell_gen_taunt_flag_targeting : public SpellScriptLoader
{
    public:
        spell_gen_taunt_flag_targeting() : SpellScriptLoader("spell_gen_taunt_flag_targeting") { }

        class spell_gen_taunt_flag_targeting_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_taunt_flag_targeting_SpellScript);

            SpellCastResult CheckIfCorpseNear()
            {
                Unit* caster = GetCaster();
                float max_range = GetSpellInfo()->GetMaxRange(false);
                WorldObject* result = nullptr;
                // search for nearby enemy corpse in range
                Trinity::AnyDeadUnitSpellTargetInRangeCheck check(caster, max_range, GetSpellInfo(), TARGET_CHECK_ENEMY);
                Trinity::WorldObjectSearcher<Trinity::AnyDeadUnitSpellTargetInRangeCheck> searcher(caster, result, check);
                caster->GetMap()->VisitFirstFound(caster->m_positionX, caster->m_positionY, max_range, searcher);
                if (!result || !result->ToPlayer())
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
                return SPELL_CAST_OK;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                GetCaster()->CastSpell(GetHitUnit(), 51657, true);
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_taunt_flag_targeting_SpellScript::CheckIfCorpseNear);
                OnEffectHitTarget += SpellEffectFn(spell_gen_taunt_flag_targeting_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_taunt_flag_targeting_SpellScript();
        }
};

//52410 BG: Strand of the Ancients
class spell_sa_place_seaforium_charge : public SpellScriptLoader
{
    public:
        spell_sa_place_seaforium_charge() : SpellScriptLoader("spell_sa_place_seaforium_charge") {}

        class spell_sa_place_seaforium_charge_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sa_place_seaforium_charge_SpellScript);

            void HandleOnHit()
            {
                if (Player* player = GetCaster()->ToPlayer())
                {
                    if (player->GetTeam() == HORDE)
                        player->CastSpell(player, 226090, true);
                    else
                        player->CastSpell(player, 226094, true);
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_sa_place_seaforium_charge_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sa_place_seaforium_charge_SpellScript();
        }
};

//Hidden Appearance Unlock
class spell_hidden_appearance_unlock : public SpellScriptLoader
{
    public:
        spell_hidden_appearance_unlock() : SpellScriptLoader("spell_hidden_appearance_unlock") {}

        class spell_hidden_appearance_unlock_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hidden_appearance_unlock_SpellScript);

            void HandleOnHit()
            {
                if (Player* player = GetCaster()->ToPlayer())
                {
                    Quest const* quest = sQuestDataStore->GetQuestTemplate(34905);
                    if (!quest)
                        return;

                    uint16 logSlot = player->FindQuestSlot(34905);
                    if (logSlot < MAX_QUEST_LOG_SIZE)
                        player->AreaExploredOrEventHappens(34905);
                    else if (player->CanTakeQuest(quest, false))    // never rewarded before
                        player->CompleteQuest(34905);             // quest not in log - for internal use
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_hidden_appearance_unlock_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hidden_appearance_unlock_SpellScript();
        }
};

//! 213170 spell
class spell_class_hall_panel : public SpellScriptLoader
{
public:
    spell_class_hall_panel() : SpellScriptLoader("spell_class_hall_panel") { }

    class spell_class_hall_panel_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_class_hall_panel_AuraScript);

        void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Unit* target = GetTarget();
            if (target->GetTypeId() != TYPEID_PLAYER)
                return;

            Player* player = target->ToPlayer();
            if (!player)
                return;

            GarrisonPtr garrison = player->GetGarrisonPtr();
            if ((!garrison || !garrison->HasGarrison(GARRISON_TYPE_CLASS_ORDER)) && player->getLevel() >= 100)
            {
                if (player->GetTeam() == HORDE)
                {
                    // if (GetQuestRewardStatus(40518))
                    player->CastSpell(player, 192191, true);
                }
                else // if (GetQuestRewardStatus(42740))
                    player->CastSpell(player, 185506, true);
            }

            if (target->GetTypeId() == TYPEID_UNIT)
                target->ToCreature()->SetReactState(REACT_PASSIVE);
        }

        void HandleEffectRemove(AuraEffect const * /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Unit* target = GetTarget();
            target->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);
        }

        void Register() override
        {
            OnEffectApply += AuraEffectApplyFn(spell_class_hall_panel_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_class_hall_panel_AuraScript();
    }
};

// Level of Craven - 158263
class spell_gen_craven : public SpellScriptLoader
{
public:
    spell_gen_craven() : SpellScriptLoader("spell_gen_craven") {}

    class spell_gen_craven_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_gen_craven_AuraScript);

        void CalculateMaxDuration(int32& duration)
        {
            if (Unit* caster = GetCaster())
            {
                if (AuraEffect* auraEff = caster->GetAuraEffect(SPELL_BG_LEVEL_OF_CRAVEN, EFFECT_0))
                {
                    duration = auraEff->GetAmount() * MINUTE * IN_MILLISECONDS;
                }
            }
        }

        void Register() override
        {
            DoCalcMaxDuration += AuraCalcMaxDurationFn(spell_gen_craven_AuraScript::CalculateMaxDuration);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_gen_craven_AuraScript();
    }
};

// Artificial Stamina - 211309
class spell_gen_artificial_stamina : public SpellScriptLoader
{
    public:
        spell_gen_artificial_stamina() : SpellScriptLoader("spell_gen_artificial_stamina") { }

        class spell_gen_artificial_stamina_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_artificial_stamina_AuraScript);

            void CalculateAmount(AuraEffect const* /*aurEff*/, float& amount, bool& /*canBeRecalculated*/)
            {
                Player* _player = GetCaster()->ToPlayer();
                if (!_player)
                    return;

                if (_player->HasPvpStatsScalingEnabled())
                {
                    amount = 0;
                    return;
                }

                Item* item = _player->GetItemByGuid(GetAura()->GetCastItemGUID());
                if (!item || !item->IsEquipped())
                    return;

                amount = item->GetTotalPurchasedArtifactPowers() * 0.75f;
                if (amount > 101)
                    amount = 101;
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_artificial_stamina_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_TOTAL_STAT_PERCENTAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_artificial_stamina_AuraScript();
        }
};
static uint32 const _effects[] = 
{
    ATTACK_POWER_FOR_ATTACKER,
    NULL,
    MOD_DAMAGE_DONE_OVERRIDE,
    MOD_POWER_REGEN_OVERRIDE,
    STRENGTH_MULTIPLICATIVE,
    AGILITY_MULTIPLICATIVE,
    INTELLECT_MULTIPLICATIVE,
    STAMINA_MULTIPLICATIVE,
    ARMOR_MULTIPLICATIVE,
    MASTERY_MULTIPLICATIVE,
    HASTE_MULTIPLICATIVE,
    VERSATILITY_1_2_MULTIPLICATIVE,
    VERSATILITY_2_2_MULTIPLICATIVE,
    CRITICAL_STRIKE_MULTIPLICATIVE
};

// PvP Stats - Spec Stat Template - All Specs - 198439
class spell_gen_spec_stat_template : public SpellScriptLoader
{
public:
    spell_gen_spec_stat_template() : SpellScriptLoader("spell_gen_spec_stat_template") {}

    class spell_gen_spec_stat_template_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_gen_spec_stat_template_AuraScript);

        void CalculateAmount(AuraEffect const* aurEff, float& amount, bool& /*canBeRecalculated*/)
        {
            if (Unit* owner = GetUnitOwner())
            {
                if (Player* plr = owner->ToPlayer())
                {
                    uint32 specId = plr->GetUInt32Value(PLAYER_FIELD_CURRENT_SPEC_ID);
                    float statMod = 0.f;

                    if (!plr->GetCustomPvPMods(statMod, _effects[aurEff->GetEffIndex()], specId))
                        statMod = sDB2Manager.GetPvpScalingValueByEffectType(_effects[aurEff->GetEffIndex()], specId);

                    switch (aurEff->GetEffIndex())
                    {
                        case EFFECT_0: 
                        {
                            amount *= statMod;
                            break;
                        }
                        case EFFECT_2: case EFFECT_3:
                        {
                            amount = statMod;
                            break;
                        }
                        case EFFECT_4: case EFFECT_5: case EFFECT_6:
                        {
                            amount = (plr->GetCreateStat(Stats(aurEff->GetMiscValue())) + MainStatValue) * statMod;
                            break;
                        }
                        case EFFECT_7:
                        {
                            amount = (plr->GetCreateStat(Stats(aurEff->GetMiscValue())) + StaminaStatValue) * statMod;
                            break;
                        }
                        case EFFECT_8:
                        {
                            amount = plr->GetPvpArmorTemplate(specId);
                            float val = amount * statMod;
                            amount = val - amount;
                            break;
                        }
                        default:
                        {
                            amount = SecondaryStatValue;
                            float val = amount * statMod;
                            amount = val - amount; 
                            break;
                        }
                    }
                }
            }
        }

        void Register() override
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_spec_stat_template_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MELEE_ATTACK_POWER_ATTACKER_BONUS);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_spec_stat_template_AuraScript::CalculateAmount, EFFECT_2, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_spec_stat_template_AuraScript::CalculateAmount, EFFECT_3, SPELL_AURA_MOD_POWER_REGEN);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_spec_stat_template_AuraScript::CalculateAmount, EFFECT_4, SPELL_AURA_MOD_STAT);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_spec_stat_template_AuraScript::CalculateAmount, EFFECT_5, SPELL_AURA_MOD_STAT);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_spec_stat_template_AuraScript::CalculateAmount, EFFECT_6, SPELL_AURA_MOD_STAT);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_spec_stat_template_AuraScript::CalculateAmount, EFFECT_7, SPELL_AURA_MOD_STAT);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_spec_stat_template_AuraScript::CalculateAmount, EFFECT_8, SPELL_AURA_MOD_BASE_RESISTANCE);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_spec_stat_template_AuraScript::CalculateAmount, EFFECT_9, SPELL_AURA_MOD_RATING);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_spec_stat_template_AuraScript::CalculateAmount, EFFECT_10, SPELL_AURA_MOD_RATING);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_spec_stat_template_AuraScript::CalculateAmount, EFFECT_11, SPELL_AURA_MOD_RATING);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_spec_stat_template_AuraScript::CalculateAmount, EFFECT_12, SPELL_AURA_MOD_RATING);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_spec_stat_template_AuraScript::CalculateAmount, EFFECT_13, SPELL_AURA_MOD_RATING);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_gen_spec_stat_template_AuraScript();
    }
};

// Artificial Damage - 226829, 219655, 213428
class spell_gen_artificial_damage : public SpellScriptLoader
{
    public:
        spell_gen_artificial_damage() : SpellScriptLoader("spell_gen_artificial_damage") { }

        class spell_gen_artificial_damage_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_artificial_damage_AuraScript);

            void CalculateAmount(AuraEffect const* /*aurEff*/, float& amount, bool& /*canBeRecalculated*/)
            {
                if (Unit* caster = GetCaster())
                {
                    if (Player* _player = caster->ToPlayer())
                    {
                        if (Item* item = _player->GetItemByGuid(GetAura()->GetCastItemGUID()))
                        {
                            if (!item->IsEquipped())
                                return;

                            uint32 artLvl = item->GetTotalPurchasedArtifactPowers();

                            if (artLvl > 101)
                                artLvl = 101;

                            switch (GetSpellInfo()->Id)
                            {
                                case 226829: // tank specs
                                case 219655: // discipline priest
                                {
                                    amount = artLvl * 0.5f;
                                    break;
                                }
                                case 213428: // other healer specs
                                {
                                    amount = artLvl;
                                    break;
                                }
                                default:
                                {
                                    amount = 0;
                                    break;
                                }
                            }
                        }
                    }
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_artificial_damage_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_artificial_damage_AuraScript();
        }
};

// 220124 - Ratstallion Harness
class spell_gen_ratstallion_harness : public SpellScriptLoader
{
    public:
        spell_gen_ratstallion_harness() : SpellScriptLoader("spell_gen_ratstallion_harness") { }

        class spell_gen_ratstallion_harness_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_ratstallion_harness_AuraScript);

            uint32 update = 0;

            void OnUpdate(uint32 diff, AuraEffect* aurEff)
            {
                update += diff;

                if (update >= 250)
                {
                    if (!GetCaster())
                        return;

                    if (Unit* vehicle = GetCaster()->GetVehicleBase())
                    {
                        if (vehicle->isInCombat())
                            GetAura()->Remove();
                        else if (GetCaster()->isInCombat())
                            GetAura()->Remove();
                    }
                    update = 0;
                }
            }

            void Register() override
            {
                OnEffectUpdate += AuraEffectUpdateFn(spell_gen_ratstallion_harness_AuraScript::OnUpdate, EFFECT_0, SPELL_AURA_CONTROL_VEHICLE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_ratstallion_harness_AuraScript();
        }
};

// 235765 - Artifact Knowledge Research
class spell_gen_artifact_knowledge_research : public SpellScriptLoader
{
    public:
        spell_gen_artifact_knowledge_research() : SpellScriptLoader("spell_gen_artifact_knowledge_research") {}

        class spell_gen_artifact_knowledge_research_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_artifact_knowledge_research_SpellScript);

            void HandleOnCast()
            {
                if (Unit* caster = GetCaster())
                {
                    if (Player* plr = caster->ToPlayer())
                    {
                        uint32 artifactItemCount = plr->GetItemCount(146745, true);
                        uint32 artifactKnowledgeLevel = plr->GetCurrency(CURRENCY_TYPE_ARTIFACT_KNOWLEDGE) + artifactItemCount;
                        if (artifactKnowledgeLevel > 25)
                            if (artifactItemCount > 0)
                                plr->DestroyItemCount(146745, 1, true);
                    }
                }
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_gen_artifact_knowledge_research_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_artifact_knowledge_research_SpellScript();
        }
};

// 236488, 236489, 236477, 236490, 243176, 243177, 243178, 243182, 243183 - Increase Artifact Knowledge
class spell_gen_increase_artifact_knowledge : public SpellScriptLoader
{
    public:
        spell_gen_increase_artifact_knowledge() : SpellScriptLoader("spell_gen_increase_artifact_knowledge") {}

        class spell_gen_increase_artifact_knowledge_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_increase_artifact_knowledge_SpellScript);

            void HandleScriptEffect(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);

                if (Unit* caster = GetCaster())
                {
                    if (Player* plr = caster->ToPlayer())
                    {
                        uint32 knowledgeLevelAdd = GetSpellInfo()->Effects[effIndex]->CalcValue(caster);
                        uint32 knowledgeLevel = plr->GetCurrency(CURRENCY_TYPE_ARTIFACT_KNOWLEDGE);
                        if (knowledgeLevel < knowledgeLevelAdd)
                            plr->ModifyCurrency(CURRENCY_TYPE_ARTIFACT_KNOWLEDGE, (knowledgeLevelAdd - knowledgeLevel) * sDB2Manager.GetCurrencyPrecision(CURRENCY_TYPE_ARTIFACT_KNOWLEDGE));

                        plr->DestroyItemCount(146745, 50, true);
                        plr->DestroyItemCount(139390, 50, true);
                    }
                }
            }

            SpellCastResult CheckCast()
            {
                if (Unit* caster = GetCaster())
                {
                    if (Player* plr = caster->ToPlayer())
                    {
                        uint32 knowledgeLevelAdd = GetSpellInfo()->Effects[EFFECT_0]->CalcValue(caster);
                        uint32 knowledgeLevel = plr->GetCurrency(CURRENCY_TYPE_ARTIFACT_KNOWLEDGE);
                        if (knowledgeLevel >= knowledgeLevelAdd)
                        {
                            SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_LEARNED_ALL_THAT_YOU_CAN_ABOUT_YOUR_ARTIFACT);
                            return SPELL_FAILED_CUSTOM_ERROR;
                        }
                    }
                }

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_increase_artifact_knowledge_SpellScript::CheckCast);
                OnEffectHitTarget += SpellEffectFn(spell_gen_increase_artifact_knowledge_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_increase_artifact_knowledge_SpellScript();
        }
};


// 65785, 77050, 79737, 73690, 115318, 143553 - Generic Eject All Passengers - Always Allow
class spell_gen_eject_all_passengers : public SpellScriptLoader
{
    public:
        spell_gen_eject_all_passengers() : SpellScriptLoader("spell_gen_eject_all_passengers") {}

        class spell_gen_eject_all_passengers_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_eject_all_passengers_SpellScript);

            void HandleOnCast()
            {
                if (Unit* caster = GetCaster())
                {
                    if (Vehicle* vehicle = caster->GetVehicle())
                        vehicle->RemoveAllPassengers();
                    else if (Vehicle* vehicle = caster->GetVehicleKit())
                        vehicle->RemoveAllPassengers();
                }
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_gen_eject_all_passengers_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_eject_all_passengers_SpellScript();
        }
};

//195558
class spell_gen_hammer_of_supreme_justice : public SpellScriptLoader
{
    public:
        spell_gen_hammer_of_supreme_justice() : SpellScriptLoader("spell_gen_hammer_of_supreme_justice") {}

        class spell_gen_hammer_of_supreme_justice_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_hammer_of_supreme_justice_AuraScript);

            uint16 m_checkTimer = 3000;

            void OnUpdate(uint32 diff, AuraEffect* /*aurEff*/)
            {
                if (m_checkTimer)
                {
                    if (m_checkTimer <= diff)
                    {
                        m_checkTimer = 0;

                        if (GetCaster())
                            GetCaster()->NearTeleportTo(2281.12f, -5320.15f, 88.82f, 2.32f);
                    }
                    else
                        m_checkTimer -= diff;
                }
            }

            void Register()
            {
                OnEffectUpdate += AuraEffectUpdateFn(spell_gen_hammer_of_supreme_justice_AuraScript::OnUpdate, EFFECT_0, SPELL_AURA_MOD_STUN);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_hammer_of_supreme_justice_AuraScript();
        }
};

// 193472, 193475
class spell_mercenary_contract : public SpellScriptLoader
{
public:
    spell_mercenary_contract() : SpellScriptLoader("spell_mercenary_contract") { }

    class spell_mercenary_contract_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_mercenary_contract_AuraScript);
        
        uint32 GetSpellForMorph(uint32 race, uint32 gender)
        {
            uint32 other_Race = 0;
            switch(race)
            {
                case RACE_HUMAN:
                    other_Race = (gender == GENDER_MALE ?  97097 : 97096);
                    break;
                case RACE_ORC:
                    other_Race = (gender == GENDER_MALE ?  97061 : 97059);
                    break;

                case RACE_DWARF:
                    other_Race = (gender == GENDER_MALE ?  97113 : 97112);
                    break;
                case RACE_UNDEAD_PLAYER:
                    other_Race = (gender == GENDER_MALE ?  97054 : 97035);
                    break;

                case RACE_NIGHTELF:
                    other_Race = (gender == GENDER_MALE ?  97108 : 97107);
                    break;
                case RACE_TAUREN:
                    other_Race = (gender == GENDER_MALE ?  97063 : 97062);
                    break;

                case RACE_GNOME:
                    other_Race = (gender == GENDER_MALE ?  97111 : 97109);
                    break;
                case RACE_TROLL:
                    other_Race = (gender == GENDER_MALE ?  97056 : 97055);
                    break;

                case RACE_DRAENEI:
                    other_Race = (gender == GENDER_MALE ?  97030 : 97026);
                    break;
                case RACE_BLOODELF:
                    other_Race = (gender == GENDER_MALE ?  97034 : 97033);
                    break;

                case RACE_WORGEN:
                    other_Race = (gender == GENDER_MALE ?  97058 : 97057);
                    break;
                case RACE_GOBLIN:
                    other_Race = (gender == GENDER_MALE ?  97116 : 97115);
                    break;

                case RACE_PANDAREN_NEUTRAL:
                case RACE_PANDAREN_ALLIANCE:
                case RACE_PANDAREN_HORDE:
                    return 0;  // not needed

            } 
            
            return other_Race;
        }

        void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Unit* target = GetTarget();
            if (Player* player = target->ToPlayer())
            {
                if (!player->InBattleground())
                    return;

                if (sWorld->getBoolConfig(CONFIG_CROSSFACTIONBG) && !player->GetBattleground()->IsRBG())
                {
                    if(GetSpellInfo()->Id == 193472 && player->GetTeamId() == TEAM_HORDE) // spell morph to horde, but it's horde
                        return; 
                    else if(GetSpellInfo()->Id == 193475 && player->GetTeamId() == TEAM_ALLIANCE) // spell morph to alliance, but it's alliance
                        return;
                        
                    uint32 other_Race = GetSpellForMorph(player->getRace(), player->getGender());
                         
                    if (other_Race)
                        player->CastSpell(player, other_Race, true);

                }
            }
        }

        void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Unit* target = GetTarget();
            if (Player* player = target->ToPlayer())
            {
                uint32 other_Race = GetSpellForMorph(player->getRace(), player->getGender());
                if (other_Race)
                    player->RemoveAurasDueToSpell(other_Race);
            }
        }

        void Register() override
        {
            AfterEffectApply += AuraEffectApplyFn(spell_mercenary_contract_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            AfterEffectRemove += AuraEffectRemoveFn(spell_mercenary_contract_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_mercenary_contract_AuraScript();
    }
};

class spell_gen_relearn_enchanting_quests : public SpellScriptLoader
{
public:
    spell_gen_relearn_enchanting_quests() : SpellScriptLoader("spell_gen_relearn_enchanting_quests") { }

    class spell_gen_relearn_enchanting_quests_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_relearn_enchanting_quests_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Unit* caster = GetCaster();
            if (!caster)
                return;
            if (Player* player = caster->ToPlayer())
            {
                if (player->GetQuestRewardStatus(39874))
                {
                    caster->CastSpell(caster, 201698, true);
                }
                if (player->GetQuestRewardStatus(39875))
                {
                    caster->CastSpell(caster, 211528, true);
                }
                if (player->GetQuestRewardStatus(39905))
                {
                    caster->CastSpell(caster, 208718, true);
                }
                if (player->GetQuestRewardStatus(39883))
                {
                    caster->CastSpell(caster, 208686, true);
                }
                if (player->GetQuestRewardStatus(39882))
                {
                    caster->CastSpell(caster, 190954, true);
                }
                if (player->GetQuestRewardStatus(39904))
                {
                    caster->CastSpell(caster, 212398, true);
                }
                if (player->GetQuestRewardStatus(39918))
                {
                    caster->CastSpell(caster, 208898, true);
                }
                if (player->GetQuestRewardStatus(39910))
                {
                    caster->CastSpell(caster, 208896, true);
                }
                if (player->GetQuestRewardStatus(39914))
                {
                    caster->CastSpell(caster, 208897, true);
                }
                if (player->GetQuestRewardStatus(39923))
                {
                    caster->CastSpell(caster, 209015, true);
                }
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_relearn_enchanting_quests_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gen_relearn_enchanting_quests_SpellScript();
    }
};

class spell_gen_relearn_inscription_quests : public SpellScriptLoader
{
public:
    spell_gen_relearn_inscription_quests() : SpellScriptLoader("spell_gen_relearn_inscription_quests") { }

    class spell_gen_relearn_inscription_quests_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_relearn_inscription_quests_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Unit* caster = GetCaster();
            if (!caster)
                return;
            if (Player* player = caster->ToPlayer())
            {
                if (player->GetQuestRewardStatus(39847))
                {
                    caster->CastSpell(caster, 201703, true);
                }
                if (player->GetQuestRewardStatus(39931))
                {
                    caster->CastSpell(caster, 192962, true);
                }
                if (player->GetQuestRewardStatus(39933))
                {
                    caster->CastSpell(caster, 193005, true);
                }
                if (player->GetQuestRewardStatus(39950))
                {
                    caster->CastSpell(caster, 193386, true);
                }
                if (player->GetQuestRewardStatus(39957))
                {
                    caster->CastSpell(caster, 193479, true);
                }
                if (player->GetQuestRewardStatus(40057))
                {
                    caster->CastSpell(caster, 193050, true);
                }
                if (player->GetQuestRewardStatus(39940))
                {
                    caster->CastSpell(caster, 210344, true);
                }
                if (player->GetQuestRewardStatus(40060))
                {
                    caster->CastSpell(caster, 193049, true);
                }
                if (player->GetQuestRewardStatus(39943))
                {
                    caster->CastSpell(caster, 193748, true);
                }
                if (player->GetQuestRewardStatus(40052))
                {
                    caster->CastSpell(caster, 193363, true);
                }
                if (player->GetQuestRewardStatus(39954))
                {
                    caster->CastSpell(caster, 210810, true);
                }
                if (player->GetQuestRewardStatus(39960))
                {
                    caster->CastSpell(caster, 193542, true);
                }
                if (player->GetQuestRewardStatus(39956))
                {
                    caster->CastSpell(caster, 193542, true);
                }
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_relearn_inscription_quests_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gen_relearn_inscription_quests_SpellScript();
    }
};

class spell_gen_relearn_tailoring_quests : public SpellScriptLoader
{
public:
    spell_gen_relearn_tailoring_quests() : SpellScriptLoader("spell_gen_relearn_tailoring_quests") { }

    class spell_gen_relearn_tailoring_quests_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_relearn_tailoring_quests_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Unit* caster = GetCaster();
            if (!caster)
                return;
            if (Player* player = caster->ToPlayer())
            {
                if (player->GetQuestRewardStatus(38944))
                {
                    caster->CastSpell(caster, 201708, true);
                }
                if (player->GetQuestRewardStatus(38945))
                {
                    caster->CastSpell(caster, 186017, true);
                }
                if (player->GetQuestRewardStatus(38949))
                {
                    caster->CastSpell(caster, 186018, true);
                }
                if (player->GetQuestRewardStatus(38953))
                {
                    caster->CastSpell(caster, 186019, true);
                }
                if (player->GetQuestRewardStatus(38955))
                {
                    caster->CastSpell(caster, 186390, true);
                }
                if (player->GetQuestRewardStatus(38957))
                {
                    caster->CastSpell(caster, 186020, true);
                }
                if (player->GetQuestRewardStatus(38958))
                {
                    caster->CastSpell(caster, 186021, true);
                }
                if (player->GetQuestRewardStatus(38963))
                {
                    caster->CastSpell(caster, 186023, true);
                }
                if (player->GetQuestRewardStatus(38961))
                {
                    caster->CastSpell(caster, 186022, true);
                }
                if (player->GetQuestRewardStatus(38966))
                {
                    caster->CastSpell(caster, 186025, true);
                }
                if (player->GetQuestRewardStatus(38970))
                {
                    caster->CastSpell(caster, 186028, true);
                }
                if (player->GetQuestRewardStatus(38974))
                {
                    caster->CastSpell(caster, 186611, true);
                }
                if (player->GetQuestRewardStatus(38971))
                {
                    caster->CastSpell(caster, 186603, true);
                }
                if (player->GetQuestRewardStatus(38975))
                {
                    caster->CastSpell(caster, 186623, true);
                }
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_relearn_tailoring_quests_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gen_relearn_tailoring_quests_SpellScript();
    }
};

class spell_gen_relearn_leatherworking_quests : public SpellScriptLoader
{
public:
    spell_gen_relearn_leatherworking_quests() : SpellScriptLoader("spell_gen_relearn_leatherworking_quests") { }

    class spell_gen_relearn_leatherworking_quests_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_relearn_leatherworking_quests_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Unit* caster = GetCaster();
            if (!caster)
                return;
            if (Player* player = caster->ToPlayer())
            {
                if (player->GetQuestRewardStatus(39958))
                {
                    caster->CastSpell(caster, 201705, true);
                }
                if (player->GetQuestRewardStatus(40183))
                {
                    caster->CastSpell(caster, 194898, true);
                }
                if (player->GetQuestRewardStatus(41889))
                {
                    caster->CastSpell(caster, 196574, true);
                }
                if (player->GetQuestRewardStatus(40201))
                {
                    caster->CastSpell(caster, 196443, true);
                }
                if (player->GetQuestRewardStatus(40179))
                {
                    caster->CastSpell(caster, 194862, true);
                }
                if (player->GetQuestRewardStatus(40178))
                {
                    caster->CastSpell(caster, 194830, true);
                }
                if (player->GetQuestRewardStatus(40181))
                {
                    caster->CastSpell(caster, 194891, true);
                }
                if (player->GetQuestRewardStatus(40182))
                {
                    caster->CastSpell(caster, 194892, true);
                }
                if (player->GetQuestRewardStatus(40176))
                {
                    caster->CastSpell(caster, 194813, true);
                }
                if (player->GetQuestRewardStatus(40186))
                {
                    caster->CastSpell(caster, 194952, true);
                }
                if (player->GetQuestRewardStatus(40185))
                {
                    caster->CastSpell(caster, 194948, true);
                }
                if (player->GetQuestRewardStatus(40192))
                {
                    caster->CastSpell(caster, 194997, true);
                }
                if (player->GetQuestRewardStatus(40191))
                {
                    caster->CastSpell(caster, 194976, true);
                }
                if (player->GetQuestRewardStatus(40198))
                {
                    caster->CastSpell(caster, 195098, true);
                }
                if (player->GetQuestRewardStatus(40205))
                {
                    caster->CastSpell(caster, 216233, true);
                }
                if (player->GetQuestRewardStatus(40203))
                {
                    caster->CastSpell(caster, 216231, true);
                }
                if (player->GetQuestRewardStatus(40204))
                {
                    caster->CastSpell(caster, 216232, true);
                }
                if (player->GetQuestRewardStatus(40189))
                {
                    caster->CastSpell(caster, 194965, true);
                }
                if (player->GetQuestRewardStatus(40327))
                {
                    caster->CastSpell(caster, 196065, true);
                }
                if (player->GetQuestRewardStatus(40194))
                {
                    caster->CastSpell(caster, 195003, true);
                }
                if (player->GetQuestRewardStatus(40188))
                {
                    caster->CastSpell(caster, 194959, true);
                }
                if (player->GetQuestRewardStatus(40199))
                {
                    caster->CastSpell(caster, 195101, true);
                }
                if (player->GetQuestRewardStatus(40209))
                {
                    caster->CastSpell(caster, 216236, true);
                }
                if (player->GetQuestRewardStatus(40207))
                {
                    caster->CastSpell(caster, 216234, true);
                }
                if (player->GetQuestRewardStatus(40208))
                {
                    caster->CastSpell(caster, 216235, true);
                }
                if (player->GetQuestRewardStatus(40215))
                {
                    caster->CastSpell(caster, 195314, true);
                }
                if (player->GetQuestRewardStatus(40214))
                {
                    caster->CastSpell(caster, 195326, true);
                }
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_relearn_leatherworking_quests_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gen_relearn_leatherworking_quests_SpellScript();
    }
};

class spell_gen_relearn_herbalism_quests : public SpellScriptLoader
{
public:
    spell_gen_relearn_herbalism_quests() : SpellScriptLoader("spell_gen_relearn_herbalism_quests") { }

    class spell_gen_relearn_herbalism_quests_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_relearn_herbalism_quests_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Unit* caster = GetCaster();
            if (!caster)
                return;
            if (Player* player = caster->ToPlayer())
            {
                if (player->GetQuestRewardStatus(40014))
                {
                    caster->CastSpell(caster, 193415, true);
                }
                if (player->GetQuestRewardStatus(40016))
                {
                    caster->CastSpell(caster, 193416, true);
                }
                if (player->GetQuestRewardStatus(40017))
                {
                    caster->CastSpell(caster, 193417, true);
                }
                if (player->GetQuestRewardStatus(40019))
                {
                    caster->CastSpell(caster, 193418, true);
                }
                if (player->GetQuestRewardStatus(40021))
                {
                    caster->CastSpell(caster, 193419, true);
                }
                if (player->GetQuestRewardStatus(40023))
                {
                    caster->CastSpell(caster, 193420, true);
                }
                if (player->GetQuestRewardStatus(40040))
                {
                    caster->CastSpell(caster, 193430, true);
                    caster->CastSpell(caster, 201702, true);
                }
                if (player->GetQuestRewardStatus(40041))
                {
                    caster->CastSpell(caster, 193431, true);
                }
                if (player->GetQuestRewardStatus(40042))
                {
                    caster->CastSpell(caster, 193432, true);
                }
                if (player->GetQuestRewardStatus(40029))
                {
                    caster->CastSpell(caster, 193424, true);
                    caster->CastSpell(caster, 201702, true);
                }
                if (player->GetQuestRewardStatus(40031))
                {
                    caster->CastSpell(caster, 193425, true);
                }
                if (player->GetQuestRewardStatus(40033))
                {
                    caster->CastSpell(caster, 193426, true);
                }
                if (player->GetQuestRewardStatus(40024))
                {
                    caster->CastSpell(caster, 193421, true);
                    caster->CastSpell(caster, 201702, true);
                }
                if (player->GetQuestRewardStatus(40026))
                {
                    caster->CastSpell(caster, 193422, true);
                }
                if (player->GetQuestRewardStatus(40028))
                {
                    caster->CastSpell(caster, 193423, true);
                }
                if (player->GetQuestRewardStatus(40035))
                {
                    caster->CastSpell(caster, 193427, true);
                }
                if (player->GetQuestRewardStatus(40037))
                {
                    caster->CastSpell(caster, 193428, true);
                }
                if (player->GetQuestRewardStatus(40039))
                {
                    caster->CastSpell(caster, 193429, true);
                }
                if (player->GetQuestRewardStatus(40018))
                {
                    caster->CastSpell(caster, 201702, true);
                }
                if (player->GetQuestRewardStatus(40034))
                {
                    caster->CastSpell(caster, 201702, true);
                }
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_relearn_herbalism_quests_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gen_relearn_herbalism_quests_SpellScript();
    }
};

class spell_gen_relearn_mining_quests : public SpellScriptLoader
{
public:
    spell_gen_relearn_mining_quests() : SpellScriptLoader("spell_gen_relearn_mining_quests") { }

    class spell_gen_relearn_mining_quests_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_relearn_mining_quests_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Unit* caster = GetCaster();
            if (!caster)
                return;
            if (Player* player = caster->ToPlayer())
            {
                if (player->GetQuestRewardStatus(38785))
                {
                    caster->CastSpell(caster, 191945, true);
                    caster->CastSpell(caster, 201706, true);
                }
                if (player->GetQuestRewardStatus(38784))
                {
                    caster->CastSpell(caster, 191944, true);
                    caster->CastSpell(caster, 201706, true);
                }
                if (player->GetQuestRewardStatus(38777))
                {
                    caster->CastSpell(caster, 191943, true);
                    caster->CastSpell(caster, 201706, true);
                }
                if (player->GetQuestRewardStatus(38790))
                {
                    caster->CastSpell(caster, 191951, true);
                }
                if (player->GetQuestRewardStatus(38789))
                {
                    caster->CastSpell(caster, 191947, true);
                }
                if (player->GetQuestRewardStatus(38791))
                {
                    caster->CastSpell(caster, 191953, true);
                }
                if (player->GetQuestRewardStatus(38792))
                {
                    caster->CastSpell(caster, 191950, true);
                }
                if (player->GetQuestRewardStatus(38793))
                {
                    caster->CastSpell(caster, 191952, true);
                }
                if (player->GetQuestRewardStatus(38794))
                {
                    caster->CastSpell(caster, 191954, true);
                }
                if (player->GetQuestRewardStatus(38795))
                {
                    caster->CastSpell(caster, 191956, true);
                }
                if (player->GetQuestRewardStatus(38796))
                {
                    caster->CastSpell(caster, 191959, true);
                }
                if (player->GetQuestRewardStatus(38797))
                {
                    caster->CastSpell(caster, 191964, true);
                }
                if (player->GetQuestRewardStatus(38802))
                {
                    caster->CastSpell(caster, 191965, true);
                }
                if (player->GetQuestRewardStatus(38801))
                {
                    caster->CastSpell(caster, 191961, true);
                }
                if (player->GetQuestRewardStatus(38800))
                {
                    caster->CastSpell(caster, 191957, true);
                }
                if (player->GetQuestRewardStatus(38805))
                {
                    caster->CastSpell(caster, 191966, true);
                }
                if (player->GetQuestRewardStatus(38804))
                {
                    caster->CastSpell(caster, 191963, true);
                }
                if (player->GetQuestRewardStatus(38803))
                {
                    caster->CastSpell(caster, 191958, true);
                }
                if (player->GetQuestRewardStatus(38806))
                {
                    caster->CastSpell(caster, 191968, true);
                }
                if (player->GetQuestRewardStatus(38807))
                {
                    caster->CastSpell(caster, 191969, true);
                }
                if (player->GetQuestRewardStatus(39830))
                {
                    caster->CastSpell(caster, 191971, true);
                }
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_relearn_mining_quests_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gen_relearn_mining_quests_SpellScript();
    }
};

class spell_gen_relearn_skinning_quests : public SpellScriptLoader
{
public:
    spell_gen_relearn_skinning_quests() : SpellScriptLoader("spell_gen_relearn_skinning_quests") { }

    class spell_gen_relearn_skinning_quests_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_relearn_skinning_quests_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Unit* caster = GetCaster();
            if (!caster)
                return;
            if (Player* player = caster->ToPlayer())
            {
                if (player->GetQuestRewardStatus(40142))
                {
                    caster->CastSpell(caster, 194820, true);
                }
                if (player->GetQuestRewardStatus(40145))
                {
                    caster->CastSpell(caster, 194821, true);
                }
                if (player->GetQuestRewardStatus(40155))
                {
                    caster->CastSpell(caster, 194822, true);
                }
                if (player->GetQuestRewardStatus(40132))
                {
                    caster->CastSpell(caster, 194817, true);
                }
                if (player->GetQuestRewardStatus(40135))
                {
                    caster->CastSpell(caster, 194818, true);
                }
                if (player->GetQuestRewardStatus(40140))
                {
                    caster->CastSpell(caster, 194819, true);
                }
                if (player->GetQuestRewardStatus(40156))
                {
                    caster->CastSpell(caster, 194823, true);
                }
                if (player->GetQuestRewardStatus(40158))
                {
                    caster->CastSpell(caster, 194824, true);
                }
                if (player->GetQuestRewardStatus(40159))
                {
                    caster->CastSpell(caster, 194825, true);
                }
                if (player->GetQuestRewardStatus(40131))
                {
                    caster->CastSpell(caster, 201707, true);
                }
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_relearn_skinning_quests_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gen_relearn_skinning_quests_SpellScript();
    }
};

class spell_gen_relearn_blacksmithing_quests : public SpellScriptLoader
{
public:
    spell_gen_relearn_blacksmithing_quests() : SpellScriptLoader("spell_gen_relearn_blacksmithing_quests") { }

    class spell_gen_relearn_blacksmithing_quests_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_relearn_blacksmithing_quests_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Unit* caster = GetCaster();
            if (!caster)
                return;
            if (Player* player = caster->ToPlayer())
            {
                if (player->GetQuestRewardStatus(39681))
                {
                    caster->CastSpell(caster, 191237, true);
                }
                if (player->GetQuestRewardStatus(38501))
                {
                    caster->CastSpell(caster, 191243, true);
                }
                if (player->GetQuestRewardStatus(38499))
                {
                    caster->CastSpell(caster, 201699, true);
                }
                if (player->GetQuestRewardStatus(38500))
                {
                    caster->CastSpell(caster, 191338, true);
                }
                if (player->GetQuestRewardStatus(39699))
                {
                    caster->CastSpell(caster, 191462, true);
                }
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_relearn_blacksmithing_quests_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gen_relearn_blacksmithing_quests_SpellScript();
    }
};

class spell_gen_relearn_engineering_quests : public SpellScriptLoader
{
public:
    spell_gen_relearn_engineering_quests() : SpellScriptLoader("spell_gen_relearn_engineering_quests") { }

    class spell_gen_relearn_engineering_quests_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_relearn_engineering_quests_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Unit* caster = GetCaster();
            if (!caster)
                return;
            if (Player* player = caster->ToPlayer())
            {
                if (player->GetQuestRewardStatus(40545))
                {
                    caster->CastSpell(caster, 201700, true);
                }
                if (player->GetQuestRewardStatus(40859))
                {
                    caster->CastSpell(caster, 201585, true);
                }
                if (player->GetQuestRewardStatus(40858))
                {
                    caster->CastSpell(caster, 201584, true);
                }
                if (player->GetQuestRewardStatus(40863))
                {
                    caster->CastSpell(caster, 201590, true);
                }
                if (player->GetQuestRewardStatus(40864))
                {
                    caster->CastSpell(caster, 201593, true);
                }
                if (player->GetQuestRewardStatus(40870))
                {
                    caster->CastSpell(caster, 201605, true);
                }
                if (player->GetQuestRewardStatus(40869))
                {
                    caster->CastSpell(caster, 201603, true);
                }
                if (player->GetQuestRewardStatus(40866))
                {
                    caster->CastSpell(caster, 201596, true);
                }
                if (player->GetQuestRewardStatus(40868))
                {
                    caster->CastSpell(caster, 201602, true);
                }
                if (player->GetQuestRewardStatus(40872))
                {
                    caster->CastSpell(caster, 201606, true);
                }
                if (player->GetQuestRewardStatus(40873))
                {
                    caster->CastSpell(caster, 201607, true);
                }
                if (player->GetQuestRewardStatus(40875))
                {
                    caster->CastSpell(caster, 201608, true);
                }
                if (player->GetQuestRewardStatus(40876))
                {
                    caster->CastSpell(caster, 201611, true);
                }
                if (player->GetQuestRewardStatus(40877))
                {
                    caster->CastSpell(caster, 201612, true);
                }
                if (player->GetQuestRewardStatus(40878))
                {
                    caster->CastSpell(caster, 201613, true);
                }
                if (player->GetQuestRewardStatus(40880))
                {
                    caster->CastSpell(caster, 201615, true);
                }
                if (player->GetQuestRewardStatus(40879))
                {
                    caster->CastSpell(caster, 201614, true);
                }
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_relearn_engineering_quests_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gen_relearn_engineering_quests_SpellScript();
    }
};

class spell_gen_relearn_alchemy_quests : public SpellScriptLoader
{
public:
    spell_gen_relearn_alchemy_quests() : SpellScriptLoader("spell_gen_relearn_alchemy_quests") { }

    class spell_gen_relearn_alchemy_quests_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_relearn_alchemy_quests_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Unit* caster = GetCaster();
            if (!caster)
                return;
            if (Player* player = caster->ToPlayer())
            {
                if (player->GetQuestRewardStatus(39325))
                {
                    caster->CastSpell(caster, 201697, true);
                }
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_relearn_alchemy_quests_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gen_relearn_alchemy_quests_SpellScript();
    }
};

class spell_gen_relearn_jewelcrafting_quests : public SpellScriptLoader
{
public:
    spell_gen_relearn_jewelcrafting_quests() : SpellScriptLoader("spell_gen_relearn_jewelcrafting_quests") { }

    class spell_gen_relearn_jewelcrafting_quests_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_relearn_jewelcrafting_quests_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Unit* caster = GetCaster();
            if (!caster)
                return;
            if (Player* player = caster->ToPlayer())
            {
                if (player->GetQuestRewardStatus(40523))
                {
                    caster->CastSpell(caster, 201704, true);
                }
                if (player->GetQuestRewardStatus(40529))
                {
                    caster->CastSpell(caster, 197931, true);
                }
                if (player->GetQuestRewardStatus(40538))
                {
                    caster->CastSpell(caster, 198797, true);
                }
                if (player->GetQuestRewardStatus(40539))
                {
                    caster->CastSpell(caster, 200151, true);
                }
                if (player->GetQuestRewardStatus(40546))
                {
                    caster->CastSpell(caster, 199526, true);
                }
                if (player->GetQuestRewardStatus(40542))
                {
                    caster->CastSpell(caster, 199417, true);
                }
                if (player->GetQuestRewardStatus(40561))
                {
                    caster->CastSpell(caster, 199785, true);
                }
                if (player->GetQuestRewardStatus(40560))
                {
                    caster->CastSpell(caster, 199784, true);
                }
                if (player->GetQuestRewardStatus(40559))
                {
                    caster->CastSpell(caster, 199783, true);
                }
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_relearn_jewelcrafting_quests_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gen_relearn_jewelcrafting_quests_SpellScript();
    }
};

class spell_gen_love_seat : public SpellScriptLoader
{
public:
    spell_gen_love_seat() : SpellScriptLoader("spell_gen_love_seat") { }

    class spell_gen_love_seat_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_love_seat_SpellScript);

        void HandleDummy(SpellEffIndex)
        {
            Unit* caster = GetCaster();
            if (!caster)
                return;

            if (Unit* target = GetHitUnit())
            {
                if (target->HasAura(56687))
                {
                    caster->CastSpell(target, 63345, true);
                }
                else if (target->HasAura(63345))
                {
                    caster->CastSpell(target, 56687, true);
                }
                if (!target->HasAura(56687))
                {
                    caster->CastSpell(target, 63345, true);
                }
                else if (!target->HasAura(63345))
                {
                    caster->CastSpell(target, 56687, true);
                }
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_love_seat_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gen_love_seat_SpellScript();
    }
};
// Reckless Stomp - 250863
class spell_gen_reckless_stomp : public SpellScriptLoader
{
    public:
    spell_gen_reckless_stomp() : SpellScriptLoader("spell_gen_reckless_stomp") {}

    class spell_gen_reckless_stomp_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_reckless_stomp_SpellScript);

        void HandleDummy(SpellEffIndex Index)
        {
            if (Unit* caster = GetCaster())
            {
                uint32 spellId = GetSpellInfo()->Effects[Index]->BasePoints;

                for (uint8 i = 0; i < 10; i++)
                    caster->CastSpell(caster, spellId, true);

                caster->AddDelayedEvent(50, [caster, spellId]() -> void
                {
                    for (uint8 i = 0; i < 10; i++)
                        caster->CastSpell(caster, spellId, true);
                });
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_reckless_stomp_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gen_reckless_stomp_SpellScript();
    }
};

// 257040 - Spatial Rift (Racial)
class spell_gen_spatial_rift : public SpellScriptLoader
{
    public:
        spell_gen_spatial_rift() : SpellScriptLoader("spell_gen_spatial_rift") { }

        class spell_gen_spatial_rift_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_spatial_rift_SpellScript);

            void HandleOnCast()
            {
                if (Unit* caster = GetCaster())
                {
                    std::list<AreaTrigger*> list;
                    caster->GetAreaObjectList(list, 256948);
                    if (!list.empty())
                    {
                        for (std::list<AreaTrigger*>::iterator itr = list.begin(); itr != list.end(); ++itr)
                        {
                            if (AreaTrigger* areaObj = (*itr))
                            {
                                caster->NearTeleportTo(areaObj->GetPositionX(), areaObj->GetPositionY(), areaObj->GetPositionZ(), areaObj->GetOrientation(), true);
                                areaObj->CastAction();
                                return;
                            }
                        }
                    }
                }
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_gen_spatial_rift_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_spatial_rift_SpellScript();
        }
};

// 253322 - Fire Mines
class spell_gen_fire_mines : public SpellScriptLoader
{
    public:
        spell_gen_fire_mines() : SpellScriptLoader("spell_gen_fire_mines") { }

        class spell_gen_fire_mines_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_fire_mines_SpellScript);

            void HandleOnCast()
            {
                if (Unit* caster = GetCaster())
                {
                    std::list<AreaTrigger*> list;
                    caster->GetAreaObjectList(list, 253320);
                    if (!list.empty())
                    {
                        for (std::list<AreaTrigger*>::iterator itr = list.begin(); itr != list.end(); ++itr)
                        {
                            if (AreaTrigger* areaObj = (*itr))
                            {
                                if (caster->GetDistance(areaObj) <= 40.f)
                                    areaObj->CastAction();
                            }
                        }
                    }
                }
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_gen_fire_mines_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_fire_mines_SpellScript();
        }
};

//239056
class spell_gen_share_health_239056 : public AuraScript
{
    PrepareAuraScript(spell_gen_share_health_239056);

    void OnProc(AuraEffect const* /*auraEffect*/, ProcEventInfo& eventInfo)
    {
        if (auto target = GetTarget())
        {
            if (auto attacker = eventInfo.GetActor())
            {
                std::list<Creature*> creatureList;
                target->GetAliveCreatureListWithEntryInGrid(creatureList, 116484, 80.0f);
                target->GetAliveCreatureListWithEntryInGrid(creatureList, 116496, 80.0f);
                target->GetAliveCreatureListWithEntryInGrid(creatureList, 116499, 80.0f);
                for (auto creature : creatureList)
                {
                    if (creature->GetEntry() != target->GetEntry())
                        attacker->DealDamage(creature, eventInfo.GetDamageInfo()->GetDamage());
                }
            }
        }
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_gen_share_health_239056::OnProc, EFFECT_1, SPELL_AURA_DUMMY);
    }
};

// 232661
class spell_storm_comet : public SpellScript
{
    PrepareSpellScript(spell_storm_comet);

    void HandleDummy(SpellEffIndex Index)
    {
        Unit* target = GetHitUnit();
        if (!target)
            return;
        uint8 count = urand(23, 27);
        for (uint8 i = 1; i <= count; ++i)
        {
            target->AddDelayedEvent(10 * i, [target]() -> void
            {
                Position pos(target->GetPosition());
                target->MovePosition(pos, frand(4, 7), static_cast<float>(rand_norm()) * static_cast<float>(2 * M_PI));
                target->SummonCreature(116806, pos, TEMPSUMMON_TIMED_DESPAWN, 20000);
            });
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_storm_comet::HandleDummy, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Call from proc Adaptation (PvP Talent) - 195845
class spell_gen_adaptation_dummy : public AuraScript
{
    PrepareAuraScript(spell_gen_adaptation_dummy);

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (auto caster = GetCaster())
        {
            caster->CastSpell(caster, 42292, true);
            caster->CastSpell(caster, 195901, true);
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_gen_adaptation_dummy::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Rogue Shadow Step - 36554, Druid Wild Charge - 49376
class spell_shadow_step_dru_rog : public SpellScript
{
    PrepareSpellScript(spell_shadow_step_dru_rog);

    SpellCastResult CheckPosition()
    {
        auto caster = GetCaster();
        auto target = GetExplTargetUnit();

        if (!caster || !target)
            return SPELL_FAILED_BAD_TARGETS;

        float delta_z = fabs(target->GetPositionZ()) - fabs(caster->GetPositionZ());
        if (caster->GetMap()->IsBattleground())
        {
            if (delta_z > 12.0f || target->GetEntry() == 27894)
                return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
            else if (target->FindNearestCreature(27894, 4.0f) && delta_z > 4.0f)
                return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
        }

        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_shadow_step_dru_rog::CheckPosition);
    }
};

void AddSC_generic_spell_scripts()
{
//    new spell_gen_protect();
    new spell_sha_cloud();
    new spell_endurance_of_niuzao();
    new spell_gen_absorb0_hitlimit1();
    new spell_gen_aura_of_anger();
    new spell_gen_av_drekthar_presence();
    new spell_gen_burn_brutallus();
    new spell_gen_cannibalize();
    new spell_gen_feast();
    new spell_gen_parachute();
    new spell_gen_pet_summoned();
    new spell_gen_remove_flight_auras();
    new spell_gen_trick();
    new spell_gen_trick_or_treat();
    new spell_creature_permanent_feign_death();
    new spell_pvp_trinket_wotf_shared_cd();
    new spell_gen_animal_blood();
    new spell_gen_parachute_ic();
    new spell_gen_gunship_portal();
    new spell_gen_dungeon_credit();
    new spell_gen_profession_research();
    new spell_generic_clone();
    new spell_generic_clone_weapon();
    new spell_gen_clone_weapon_aura();
    new spell_gen_seaforium_blast();
    new spell_gen_magic_rooster();
    new spell_gen_allow_cast_from_item_only();
    new spell_gen_launch();
    new spell_gen_vehicle_scaling();
    new spell_gen_oracle_wolvar_reputation();
    new spell_gen_dummy_trigger();
    new spell_gen_spirit_healer_res();
    new spell_gen_gadgetzan_transporter_backfire();
    new spell_gen_gnomish_transporter();
    new spell_gen_dalaran_disguise("spell_gen_sunreaver_disguise");
    new spell_gen_dalaran_disguise("spell_gen_silver_covenant_disguise");
    new spell_gen_elune_candle();
    new spell_gen_break_shield("spell_gen_break_shield");
    new spell_gen_break_shield("spell_gen_tournament_counterattack");
    new spell_gen_mounted_charge();
    new spell_gen_defend();
    new spell_gen_tournament_duel();
    new spell_gen_summon_tournament_mount();
    new spell_gen_on_tournament_mount();
    new spell_gen_tournament_pennant();
    new spell_gen_chaos_blast();
    new spell_gen_wg_water();
    new spell_gen_despawn_self();
    new spell_gen_touch_the_nightmare();
    new spell_gen_dream_funnel();
    new spell_gen_bandage();
    new spell_gen_lifebloom("spell_hexlord_lifebloom", SPELL_HEXLORD_MALACRASS_LIFEBLOOM_FINAL_HEAL);
    new spell_gen_lifebloom("spell_tur_ragepaw_lifebloom", SPELL_TUR_RAGEPAW_LIFEBLOOM_FINAL_HEAL);
    new spell_gen_lifebloom("spell_cenarion_scout_lifebloom", SPELL_CENARION_SCOUT_LIFEBLOOM_FINAL_HEAL);
    new spell_gen_lifebloom("spell_twisted_visage_lifebloom", SPELL_TWISTED_VISAGE_LIFEBLOOM_FINAL_HEAL);
    new spell_gen_lifebloom("spell_faction_champion_dru_lifebloom", SPELL_FACTION_CHAMPIONS_DRU_LIFEBLOOM_FINAL_HEAL);
    new spell_gen_summon_elemental("spell_gen_summon_fire_elemental", SPELL_SUMMON_FIRE_ELEMENTAL);
    new spell_gen_summon_elemental("spell_gen_summon_earth_elemental", SPELL_SUMMON_EARTH_ELEMENTAL);
    new spell_gen_mount("spell_magic_broom", 0, SPELL_MAGIC_BROOM_60, SPELL_MAGIC_BROOM_100, SPELL_MAGIC_BROOM_150, SPELL_MAGIC_BROOM_280);
    new spell_gen_mount("spell_headless_horseman_mount", 0, SPELL_HEADLESS_HORSEMAN_MOUNT_60, SPELL_HEADLESS_HORSEMAN_MOUNT_100, SPELL_HEADLESS_HORSEMAN_MOUNT_150, SPELL_HEADLESS_HORSEMAN_MOUNT_280);
    new spell_gen_mount("spell_winged_steed_of_the_ebon_blade", 0, 0, 0, SPELL_WINGED_STEED_150, SPELL_WINGED_STEED_280);
    new spell_gen_mount("spell_big_love_rocket", SPELL_BIG_LOVE_ROCKET_0, SPELL_BIG_LOVE_ROCKET_60, SPELL_BIG_LOVE_ROCKET_100, SPELL_BIG_LOVE_ROCKET_150, SPELL_BIG_LOVE_ROCKET_310);
    new spell_gen_mount("spell_invincible", 0, SPELL_INVINCIBLE_60, SPELL_INVINCIBLE_100, SPELL_INVINCIBLE_150, SPELL_INVINCIBLE_310);
    new spell_gen_mount("spell_blazing_hippogryph", 0, 0, 0, SPELL_BLAZING_HIPPOGRYPH_150, SPELL_BLAZING_HIPPOGRYPH_280);
    new spell_gen_mount("spell_celestial_steed", 0, SPELL_CELESTIAL_STEED_60, SPELL_CELESTIAL_STEED_100, SPELL_CELESTIAL_STEED_150, SPELL_CELESTIAL_STEED_280, SPELL_CELESTIAL_STEED_310);
    new spell_gen_mount("spell_x53_touring_rocket", 0, 0, 0, SPELL_X53_TOURING_ROCKET_150, SPELL_X53_TOURING_ROCKET_280, SPELL_X53_TOURING_ROCKET_310);
    new spell_gen_upper_deck_create_foam_sword();
    new spell_gen_bonked();
    new spell_gen_running_wild();
    new spell_gen_two_forms();
    new spell_gen_darkflight();
    new spell_gen_gobelin_gumbo();
    new spell_gen_ds_flush_knockback();
    new spell_brewfest_speed();
    new spell_gen_tricky_treat();
    new spell_gen_leviroth_self_impale();
    new spell_gulp_frog_toxin();
    new spell_time_lost_wisdom();
    new spell_gen_brutal_assault();
    new spell_gen_dampening();
    new spell_gen_orb_of_power();
    new spell_gen_drums_of_rage();
    new spell_gen_battle_guild_standart();
    new spell_gen_landmine_knockback();
    new spell_gen_ic_seaforium_blast();
    new spell_gen_cooking_way();
    new spell_gen_bg_inactive();
    new spell_gen_bounce_achievement();
    new spell_gen_herbalism_trap();
    new spell_gen_mining_trap();
    new spell_gen_learn_legion_skinning();
    new spell_gen_create_artefact();
    new spell_principles_of_war();
    new spell_gen_absorb_if_health();
    new spell_gen_taunt_flag_targeting();
    new spell_sa_place_seaforium_charge();
    new spell_hidden_appearance_unlock();
    new spell_class_hall_panel();
    new spell_gen_artificial_stamina();
    new spell_gen_artificial_damage();
    new spell_gen_ratstallion_harness();
    new spell_gen_artifact_knowledge_research();
    new spell_gen_increase_artifact_knowledge();
    new spell_gen_eject_all_passengers();
    new spell_gen_hammer_of_supreme_justice();
    new spell_mercenary_contract();
    new spell_gen_spec_stat_template();
    new spell_gen_monk_crosswinds();
    new spell_gen_relearn_mining_quests();
    new spell_gen_relearn_jewelcrafting_quests();
    new spell_gen_relearn_alchemy_quests();
    new spell_gen_relearn_engineering_quests();
    new spell_gen_relearn_blacksmithing_quests();
    new spell_gen_relearn_skinning_quests();
    new spell_gen_relearn_herbalism_quests();
    new spell_gen_relearn_leatherworking_quests();
    new spell_gen_relearn_tailoring_quests();
    new spell_gen_relearn_inscription_quests();
    new spell_gen_relearn_enchanting_quests();
    new spell_gen_craven();
    new spell_gen_love_seat();
    new spell_gen_spatial_rift();
    new spell_gen_fire_mines();
    new spell_gen_reckless_stomp();
    RegisterAuraScript(spell_gen_share_health_239056);
    RegisterSpellScript(spell_storm_comet);
    RegisterSpellScript(spell_shadow_step_dru_rog);
    RegisterAuraScript(spell_gen_adaptation_dummy);
}
