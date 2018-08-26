/*
    http://uwow.biz
*/

#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "Spell.h"
#include "ScriptedCreature.h"

enum eSpells
{
    SPELL_VOLUNTEER_CITY_GUARD_HORDE = 231163,
    SPELL_VOLUNTEER_CITY_GUARD_ALLIANCE = 231193
};

struct npc_volunteer_guard_day : public ScriptedAI
{
    npc_volunteer_guard_day(Creature* creature) : ScriptedAI(creature) {}

    void ReceiveEmote(Player* player, uint32 textEmote) override
    {
        if (textEmote == TEXT_EMOTE_SALUTE)
        {
            if (sGameEventMgr->IsActiveEvent(70))
            {
                if (player->GetTeam() == HORDE)
                {
                    DoCast(player, SPELL_VOLUNTEER_CITY_GUARD_HORDE);
                    me->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
                }
                else
                {
                    DoCast(player, SPELL_VOLUNTEER_CITY_GUARD_ALLIANCE);
                    me->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
                }
            }
        }
    }
};

void AddSC_VolunteerGuardDay()
{
    RegisterCreatureAI(npc_volunteer_guard_day);
}