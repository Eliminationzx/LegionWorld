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
 * Scripts for spells with MASTERY.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_mastery_".
 */

#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"

enum MasterySpells
{
    MASTERY_SPELL_LIGHTNING_BOLT        = 45284,
    MASTERY_SPELL_CHAIN_LIGHTNING       = 45297,
    MASTERY_SPELL_LAVA_BURST            = 77451,
    MASTERY_SPELL_ELEMENTAL_BLAST       = 120588,
};

// Called by 403 - Lightning Bolt, 421 - Chain Lightning, 51505 - Lava Burst and 117014 - Elemental Blast
// 168534 - Mastery : Elemental Overload
class spell_mastery_elemental_overload : public SpellScriptLoader
{
    public:
        spell_mastery_elemental_overload() : SpellScriptLoader("spell_mastery_elemental_overload") { }

        class spell_mastery_elemental_overload_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mastery_elemental_overload_SpellScript);

            void HandleAfterCast()
            {
                SpellInfo const* procSpell = GetSpellInfo();
                if (!procSpell)
                    return;

                Unit* caster = GetCaster();
                if (!caster || caster->GetTypeId() != TYPEID_PLAYER)
                    return;

                Unit* unitTarget = GetExplTargetUnit();
                if (!unitTarget)
                    return;

                // Mastery: Elemental Overload
                AuraEffect const* auraEff = caster->GetAuraEffect(168534, EFFECT_0);
                if (!auraEff)
                    return;

                int32 _chance = auraEff->GetAmount();
                if (AuraEffect const* auraEffTotem = caster->GetAuraEffect(210652, EFFECT_0)) // Storm Totem(only for 403)
                    _chance += auraEffTotem->GetAmount();

                if (!roll_chance_i(_chance))
                    return;

                /*// Every Lightning Bolt, Chain Lightning and Lava Burst spells have duplicate vs 75% damage and no cost
                switch (procSpell->Id)
                {
                    // case 51505:     // Lava Burst
                        // caster->CastSpell(unitTarget, MASTERY_SPELL_LAVA_BURST, true);
                        // break;
                    // case 403:       // Lightning Bolt
                        // caster->CastSpell(unitTarget, MASTERY_SPELL_LIGHTNING_BOLT, true);
                        // break;
                    // case 421:       // Chain Lightning
                        // caster->CastSpell(unitTarget, MASTERY_SPELL_CHAIN_LIGHTNING, true);
                        // break;
                    // case 117014:    // Elemental Blast
                        // caster->CastSpell(unitTarget, MASTERY_SPELL_ELEMENTAL_BLAST, true);
                        // caster->CastSpell(unitTarget, 118517, true); // Nature visual
                        // caster->CastSpell(unitTarget, 118515, true); // Frost visual
                        // break;
                    default:
                        break;
                }*/
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_mastery_elemental_overload_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mastery_elemental_overload_SpellScript();
        }
};

void AddSC_mastery_spell_scripts()
{
    new spell_mastery_elemental_overload();
}