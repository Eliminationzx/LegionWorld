#include "the_stonecore.h"

enum ScriptTexts
{
    SAY_AGGRO   = 0,
    SAY_KILL    = 1,
    SAY_SKILL   = 2,
    SAY_DEATH   = 3
};
enum Spells
{
    SPELL_GROUND_SLAM       = 78903,
    SPELL_GROUND_SLAM_H     = 92410,
    SPELL_SHATTER           = 78807,
    SPELL_PARALYZE          = 92426,
    SPELL_PARALYZE_DMG      = 94661,
    SPELL_ENRAGE            = 80467,
    SPELL_SHIELD            = 78835,
    SPELL_BULWARK           = 78939
};

enum Events
{
    EVENT_GROUND_SLAM   = 1,
    EVENT_SHATTER       = 2,
    EVENT_PARALYZE      = 3,
    EVENT_SHIELD        = 4,
    EVENT_BULWARK       = 5
};

class boss_ozruk : public CreatureScript
{
    public:
        boss_ozruk() : CreatureScript("boss_ozruk") {}

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_ozrukAI(creature);
        }

        struct boss_ozrukAI : public BossAI
        {
            boss_ozrukAI(Creature* pCreature) : BossAI(pCreature, DATA_OZRUK) {}

            EventMap events;
            bool bEnrage;

            void Reset() override
            {
                _Reset();
                bEnrage = false;
                events.Reset();
            }

            void EnterCombat(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_SHATTER, 21000);
                events.ScheduleEvent(EVENT_GROUND_SLAM, 7000);
                events.ScheduleEvent(EVENT_BULWARK, 80000);
                events.ScheduleEvent(EVENT_SHIELD, 3000);
                Talk(SAY_AGGRO);
                instance->SetBossState(DATA_OZRUK, IN_PROGRESS);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                Talk(SAY_DEATH);
            }

            void KilledUnit(Unit* victim) override
            {
                if (!victim)
                    return;

                if (victim->IsPlayer())
                    Talk(SAY_KILL);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (me->HealthBelowPct(20) && !bEnrage)
                {
                    DoCast(SPELL_ENRAGE);
                    bEnrage = true;
                }

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                    case EVENT_GROUND_SLAM:
                        DoCast(SPELL_GROUND_SLAM);
                        events.ScheduleEvent(EVENT_GROUND_SLAM, 15000);
                        break;
                    case EVENT_SHATTER:
                        if (IsHeroic())
                        {
                            events.RescheduleEvent(EVENT_GROUND_SLAM, 15000);
                            DoCast(SPELL_PARALYZE);
                        }
                        else
                            DoCast(SPELL_SHATTER);
                        events.ScheduleEvent(EVENT_SHATTER, 20000);
                        break;
                    case EVENT_SHIELD:
                        DoCast(SPELL_SHIELD);
                        events.ScheduleEvent(EVENT_SHIELD, 15000);
                        break;
                    case EVENT_BULWARK:
                        DoCast(SPELL_BULWARK);
                        events.ScheduleEvent(EVENT_BULWARK, 15000);
                        break;
                    }
                }
                DoMeleeAttackIfReady();
            }
        };
};

void AddSC_boss_ozruk()
{
    new boss_ozruk();
}