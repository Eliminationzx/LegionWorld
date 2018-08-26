/*
    https://uwow.biz/
*/

#include "antorus.h"

enum Spells
{
    SPELL_OUTRO_CONV        = 251475,
};


struct boss_imonar : BossAI
{
    boss_imonar(Creature* creature) : BossAI(creature, DATA_IMONAR) {}

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
            instance->instance->LoadGrid(-10608.08f, 9002.58f);
            instance->instance->LoadGrid(-10574.31f, 8488.62f);
            me->CastSpell(me, SPELL_OUTRO_CONV);
            me->AddDelayedEvent(13000, [this] () -> void
            {
                if (GameObject* go = instance->instance->GetGameObject(instance->GetGuidData(GO_ARCANE_BARRIER_VISUAL)))
                    go->SetAnimKitId(6132, true);
                    
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

void AddSC_boss_imonar()
{
    RegisterCreatureAI(boss_imonar);
}
