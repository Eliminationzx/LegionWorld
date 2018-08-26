/*
    https://uwow.biz/
*/

#include "antorus.h"

enum Spells
{
    SPELL_OUTRO_CONV        = 249154,
};


struct boss_antoran : BossAI
{
    boss_antoran(Creature* creature) : BossAI(creature, DATA_ANTORAN) {}

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
        me->AddDelayedEvent(1000, [this]() -> void
        {
            me->CastSpell(me, SPELL_OUTRO_CONV);
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

void AddSC_boss_antoran()
{
    RegisterCreatureAI(boss_antoran);
}
