/*
    https://uwow.biz/
*/

#include "the_emerald_nightmare.h"
#include "QuestData.h"

enum Says
{
    //SAY_1           = ,
};

enum Spells
{
    //SPELL_    = ,
    //SPELL_    = ,
    //SPELL_    = ,
    //SPELL_    = ,
};

enum eEvents
{
    //EVENT_    = 1,
    //EVENT_    = 2,
    //EVENT_    = 3,
    //EVENT_    = 4,
};

class spell_vantus_rune_the_emerald_nightmare : public SpellScriptLoader
{
public:
    spell_vantus_rune_the_emerald_nightmare() : SpellScriptLoader("spell_vantus_rune_the_emerald_nightmare") {}

    class spell_vantus_rune_the_emerald_nightmare_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_vantus_rune_the_emerald_nightmare_AuraScript);

        uint16 checkOnProc;
        uint16 checkOnRemove;

        bool Load()
        {
            checkOnProc = 1000;
            checkOnRemove = 1000;
            return true;
        }

        void OnUpdate(uint32 diff, AuraEffect* /*aurEff*/)
        {
            Unit* player = GetCaster();
            if (!player)
                return;

            InstanceScript* instance = player->GetInstanceScript();
            if (!instance)
                return;

            if (checkOnProc <= diff)
            {
                if (instance->GetBossState(DATA_NYTHENDRA) == IN_PROGRESS)
                {
                    if (!player->HasAura(208844) && player->HasAura(192761))
                    {
                        player->CastSpell(player, 208844, false);
                    }
                }
                if (instance->GetBossState(DATA_RENFERAL) == IN_PROGRESS)
                {
                    if (!player->HasAura(208848) && player->HasAura(192765))
                    {
                        player->CastSpell(player, 208848, false);
                    }
                }
                if (instance->GetBossState(DATA_ILGYNOTH) == IN_PROGRESS)
                {
                    if (!player->HasAura(208845) && player->HasAura(192762))
                    {
                        player->CastSpell(player, 208852, false);
                    }
                }
                if (instance->GetBossState(DATA_URSOC) == IN_PROGRESS)
                {
                    if (!player->HasAura(208843) && player->HasAura(191464))
                    {
                        player->CastSpell(player, 208843, false);
                    }
                }
                if (instance->GetBossState(DATA_DRAGON_NIGHTMARE) == IN_PROGRESS)
                {
                    if (!player->HasAura(208846) && player->HasAura(192763))
                    {
                        player->CastSpell(player, 208846, false);
                    }
                }
                if (instance->GetBossState(DATA_CENARIUS) == IN_PROGRESS)
                {
                    if (!player->HasAura(208849) && player->HasAura(192766))
                    {
                        player->CastSpell(player, 208849, false);
                    }
                }
                if (instance->GetBossState(DATA_XAVIUS) == IN_PROGRESS)
                {
                    if (!player->HasAura(208847) && player->HasAura(192764))
                    {
                        player->CastSpell(player, 208847, false);
                    }
                }
            }
            else checkOnProc -= diff;

            if (checkOnRemove <= diff)
            {
                if (player->HasAura(208844) && player->HasAura(192761))
                {
                    if (instance->GetBossState(DATA_NYTHENDRA) == DONE || instance->GetBossState(DATA_NYTHENDRA) == NOT_STARTED)
                    {
                        player->RemoveAura(208844);
                        checkOnProc = 1000;
                        checkOnRemove = 1000;
                    }
                }
                if (player->HasAura(208848) && player->HasAura(192765))
                {
                    if (instance->GetBossState(DATA_RENFERAL) == DONE || instance->GetBossState(DATA_RENFERAL) == NOT_STARTED)
                    {
                        player->RemoveAura(208848);
                        checkOnProc = 1000;
                        checkOnRemove = 1000;
                    }
                }
                if (player->HasAura(208846) && player->HasAura(192762))
                {
                    if (instance->GetBossState(DATA_ILGYNOTH) == DONE || instance->GetBossState(DATA_ILGYNOTH) == NOT_STARTED)
                    {
                        player->RemoveAura(208846);
                        checkOnProc = 1000;
                        checkOnRemove = 1000;
                    }
                }
                if (player->HasAura(208843) && player->HasAura(191464))
                {
                    if (instance->GetBossState(DATA_URSOC) == DONE || instance->GetBossState(DATA_URSOC) == NOT_STARTED)
                    {
                        player->RemoveAura(208843);
                        checkOnProc = 1000;
                        checkOnRemove = 1000;
                    }
                }
                if (player->HasAura(208856) && player->HasAura(192763))
                {
                    if (instance->GetBossState(DATA_DRAGON_NIGHTMARE) == DONE || instance->GetBossState(DATA_DRAGON_NIGHTMARE) == NOT_STARTED)
                    {
                        player->RemoveAura(208856);
                        checkOnProc = 1000;
                        checkOnRemove = 1000;
                    }
                }
                if (player->HasAura(208849) && player->HasAura(192766))
                {
                    if (instance->GetBossState(DATA_CENARIUS) == DONE || instance->GetBossState(DATA_CENARIUS) == NOT_STARTED)
                    {
                        player->RemoveAura(208849);
                        checkOnProc = 1000;
                        checkOnRemove = 1000;
                    }
                }
                if (player->HasAura(208847) && player->HasAura(192764))
                {
                    if (instance->GetBossState(DATA_XAVIUS) == DONE || instance->GetBossState(DATA_XAVIUS) == NOT_STARTED)
                    {
                        player->RemoveAura(208847);
                        checkOnProc = 1000;
                        checkOnRemove = 1000;
                    }
                }
            }
            else checkOnRemove -= diff;
        }

        void Register() override
        {
            OnEffectUpdate += AuraEffectUpdateFn(spell_vantus_rune_the_emerald_nightmare_AuraScript::OnUpdate, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_vantus_rune_the_emerald_nightmare_AuraScript();
    }

    class spell_vantus_rune_the_emerald_nightmare_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_vantus_rune_the_emerald_nightmare_SpellScript);

        SpellCastResult CheckCast()
        {
            Unit* caster = GetCaster();
            if (Player* player = caster->ToPlayer())
            {
                if (!player->GetQuestRewardStatus(39695))
                    return SPELL_CAST_OK;
            }
            SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_YOU_ALREADY_USED_VANTUS_RUNE);
            return SPELL_FAILED_CUSTOM_ERROR;
        }

        void HandleOnHit()
        {
            if (!GetCaster())
                return;

            if (Player* player = GetCaster()->ToPlayer())
            {
                uint32 questId = 39695;
                if (questId)
                {
                    Quest const* quest = sQuestDataStore->GetQuestTemplate(questId);
                    if (!quest)
                        return;

                    if (player->CanTakeQuest(quest, false))
                        player->CompleteQuest(questId);
                }
            }
        }

        void Register() override
        {
            OnCheckCast += SpellCheckCastFn(spell_vantus_rune_the_emerald_nightmare_SpellScript::CheckCast);
            OnHit += SpellHitFn(spell_vantus_rune_the_emerald_nightmare_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_vantus_rune_the_emerald_nightmare_SpellScript();
    }
};

void AddSC_the_emerald_nightmare()
{
    new spell_vantus_rune_the_emerald_nightmare();
}