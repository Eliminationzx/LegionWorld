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

#include "sunwell_plateau.h"

enum Quotes
{
 YELL_INTRO                 =   -1580017,
 YELL_INTRO_BREAK_ICE       =   -1580018,
 YELL_INTRO_CHARGE          =   -1580019,
 YELL_INTRO_KILL_MADRIGOSA  =   -1580020,
 YELL_INTRO_TAUNT           =   -1580021,

 YELL_MADR_ICE_BARRIER      =   -1580031,
 YELL_MADR_INTRO            =   -1580032,
 YELL_MADR_ICE_BLOCK        =   -1580033,
 YELL_MADR_TRAP             =   -1580034,
 YELL_MADR_DEATH            =   -1580035,

 YELL_AGGRO                 =   -1580022,
 YELL_KILL1                 =   -1580023,
 YELL_KILL2                 =   -1580024,
 YELL_KILL3                 =   -1580025,
 YELL_LOVE1                 =   -1580026,
 YELL_LOVE2                 =   -1580027,
 YELL_LOVE3                 =   -1580028,
 YELL_BERSERK               =   -1580029,
 YELL_DEATH                 =   -1580030
};

enum Spells
{
    SPELL_METEOR_SLASH                 =   45150,
    SPELL_BURN                         =   46394,
    SPELL_STOMP                        =   45185,
    SPELL_BERSERK                      =   26662,
    SPELL_DUAL_WIELD                   =   42459,

    SPELL_INTRO_FROST_BLAST            =   45203,
    SPELL_INTRO_FROSTBOLT              =   44843,
    SPELL_INTRO_ENCAPSULATE            =   45665,
    SPELL_INTRO_ENCAPSULATE_CHANELLING =   45661
};

enum sEvents
{
     EVENT_SLASH          = 1,
     EVENT_STOMP          = 2,
     EVENT_BURN           = 3,
     EVENT_BERSERK        = 4,
};

class boss_brutallus : public CreatureScript
{
public:
    boss_brutallus() : CreatureScript("boss_brutallus") { }

    struct boss_brutallusAI : public BossAI
    {
        boss_brutallusAI(Creature* creature) : BossAI(creature, DATA_BRUTALLUS_EVENT)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        void Reset()
        {
            _Reset();
            DoCast(me, SPELL_DUAL_WIELD, true);
        }

        void EnterCombat(Unit* /*who*/)
        {
            _EnterCombat();
            DoScriptText(YELL_AGGRO, me);
            events.ScheduleEvent(EVENT_SLASH,                11000);
            events.ScheduleEvent(EVENT_STOMP,                30000);
            events.ScheduleEvent(EVENT_BURN,  urand(60000, 180000));
            events.ScheduleEvent(EVENT_BERSERK,             360000);
        }

        void KilledUnit(Unit* victim)
        {
            if (victim->GetTypeId() == TYPEID_PLAYER)
                DoScriptText(RAND(YELL_KILL1, YELL_KILL2, YELL_KILL3), me);
        }

        void JustDied(Unit* /*killer*/)
        {
            DoScriptText(YELL_DEATH, me);
            _JustDied();
        }

        void UpdateAI(uint32 diff)
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_SLASH:
                    DoCastVictim(SPELL_METEOR_SLASH);
                    events.ScheduleEvent(EVENT_SLASH, 11000);
                    break;
                case EVENT_STOMP:
                    DoScriptText(RAND(YELL_LOVE1, YELL_LOVE2, YELL_LOVE3), me);
                    DoCastVictim(SPELL_STOMP);
                    events.ScheduleEvent(EVENT_STOMP, 30000);
                    break;
                case EVENT_BURN:
                    {
                        Map::PlayerList const &players = me->GetMap()->GetPlayers();
                        for (Map::PlayerList::const_iterator i = players.begin(); i != players.end(); ++i)
                        {
                            if (Player* pl = i->getSource())
                                if (pl->isAlive())
                                {
                                    if (!pl->HasAura(SPELL_BURN))
                                    {
                                        pl->CastSpell(pl, SPELL_BURN, true);
                                        break;
                                    }
                                }
                        }
                        events.ScheduleEvent(EVENT_BURN, urand(60000, 180000));
                        break;
                    }
                case EVENT_BERSERK:
                    DoScriptText(YELL_BERSERK, me);
                    DoCast(me, SPELL_BERSERK);
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_brutallusAI (creature);
    }
};

void AddSC_boss_brutallus()
{
    new boss_brutallus();
}
