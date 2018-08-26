/*
    https://uwow.biz/
*/

#include "antorus.h"

enum Spells
{
    SPELL_OUTRO                 = 257606,
};


struct boss_argus : BossAI
{
    boss_argus(Creature* creature) : BossAI(creature, DATA_ARGUS) {}

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
            const auto& players = me->GetMap()->GetPlayers();
            for (const auto& player : players)
                player.getSource()->CastSpell(player.getSource(), SPELL_OUTRO);

            // to-do: Teleport and other
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

void AddSC_boss_argus()
{
    RegisterCreatureAI(boss_argus);
}
