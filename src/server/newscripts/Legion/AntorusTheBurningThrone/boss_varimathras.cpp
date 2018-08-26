/*
    https://uwow.biz/
*/

#include "antorus.h"

enum Spells
{
    SPELL_INTRO_CONV        = 250026,
};


struct boss_varimathras : BossAI
{
    boss_varimathras(Creature* creature) : BossAI(creature, DATA_VARIMATHRAS) {}

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
        
        me->AddDelayedCombat(3000, [this]() -> void
        {
            me->CastSpell(me, SPELL_INTRO_CONV);
        });
    }

    void JustDied(Unit* /*killer*/) override
    {
        // Talk(SAY_DEATH);
        _JustDied();
        me->AddDelayedEvent(1000, [this]() -> void
        {
            if (Creature* cre = instance->instance->GetCreature(instance->GetGuidData(NPC_AZARA)))
                cre->AI()->Talk(0);
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

void AddSC_boss_varimathras()
{
    RegisterCreatureAI(boss_varimathras);
}
