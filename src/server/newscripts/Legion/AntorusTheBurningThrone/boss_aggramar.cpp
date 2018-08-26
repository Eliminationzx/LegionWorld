/*
    https://uwow.biz/
*/

#include "antorus.h"

enum Spells
{
    SPELL_OUTRO_CONV        = 254315,
};


struct boss_aggramar : BossAI
{
    boss_aggramar(Creature* creature) : BossAI(creature, DATA_AGGRAMAR) {}

    void Reset() override
    {
        _Reset();
    }

    void EnterCombat(Unit* /*who*/) override
    {
        _EnterCombat();

        //  Talk(SAY_AGGRO);
    }

    void JustDied(Unit* /*killer*/) override
    {
        // Talk(SAY_DEATH);
        _JustDied();
        me->AddDelayedEvent(3000, [this]() -> void
        {
            uint32 const entries[4]{ 128169, 129876, 125683, 125682 };

            Position const pos[4]{
                {-12646.00f, -2292.10f, 2514.26f, 1.44f},
                {-12639.39f, -2293.25f, 2514.26f, 1.44f},
                {-12630.38f, -2293.25f, 2514.26f, 1.60f},
                {-12621.95f, -2293.79f, 2514.26f, 1.66f}
            };
            for (uint8 i = 0; i < 4; ++i)
                me->SummonCreature(entries[i], pos[i]);

            me->AddDelayedEvent(1000, [this]() -> void
            {
                me->CastSpell(me, SPELL_OUTRO_CONV);
            });
        });
    }

    void UpdateAI(uint32 diff) override
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

            }
        }
        DoMeleeAttackIfReady();
    }
};

void AddSC_boss_aggramar()
{
    RegisterCreatureAI(boss_aggramar);
}
