/*
    https://uwow.biz/
*/

#include "antorus.h"

enum Spells
{
    SPELL_OUTRO_CONV        = 250806,
};


struct boss_coven_shivarres : BossAI
{
    boss_coven_shivarres(Creature* creature) : BossAI(creature, DATA_COVEN) {}

    void Reset() override
    {
        _Reset();
    }

    void DoAction(int32 const action)
    {
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
            me->CastSpell(me, SPELL_OUTRO_CONV);  // To-DO. Check, that only 1 shivarr cast it
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

void AddSC_boss_coven_shivarres()
{
    RegisterCreatureAI(boss_coven_shivarres);
}
