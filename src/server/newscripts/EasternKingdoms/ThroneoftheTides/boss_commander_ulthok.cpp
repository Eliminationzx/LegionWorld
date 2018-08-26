#include "throne_of_the_tides.h"

enum Spells
{
    SPELL_DARK_FISSURE          = 76047,
    SPELL_SQUEEZE               = 76026,
    SPELL_SQUEEZE_VEHICLE       = 76028,
    SPELL_ENRAGE                = 76100,
    SPELL_CURSE_OF_FATIGUE      = 76094,
    SPELL_DARK_FISSURE_AURA     = 76066,
    SPELL_DARK_FISSURE_DMG      = 76085,
    SPELL_ULTHOK_INTRO          = 82960
};

enum Events
{
    EVENT_DARK_FISSURE      = 1,
    EVENT_SQUEEZE           = 2,
    EVENT_CURSE_OF_FATIGUE  = 3,
    EVENT_ENRAGE            = 4
};

enum Actions
{
    ACTION_COMMANDER_ULTHOK_START_EVENT = 2
};

enum Adds
{
    NPC_DARK_FISSURE = 40784
};

class boss_commander_ulthok : public CreatureScript
{
    public:
        boss_commander_ulthok() : CreatureScript("boss_commander_ulthok") {}

        CreatureAI* GetAI(Creature *pCreature) const
        {
            return new boss_commander_ulthokAI (pCreature);
        }

        struct boss_commander_ulthokAI : public BossAI
        {
            boss_commander_ulthokAI(Creature* pCreature) : BossAI(pCreature, DATA_COMMANDER_ULTHOK) {}

            void Reset() override
            {
                _Reset();
                me->AddAura(76017, me);
            }

            void DoAction(const int32 action) override
            {
                if (action == ACTION_COMMANDER_ULTHOK_START_EVENT)
                {
                    me->SetPhaseMask(PHASEMASK_NORMAL, true);
                    DoCast(SPELL_ULTHOK_INTRO);
                    if (auto pCorales = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(DATA_CORALES)))
                    {
                        pCorales->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                        pCorales->SetPhaseMask(2, true);
                    }
                }
            }

            void EnterCombat(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_DARK_FISSURE, urand(5000, 8000));
                events.ScheduleEvent(EVENT_ENRAGE, urand(20000, 25000));
                events.ScheduleEvent(EVENT_CURSE_OF_FATIGUE, urand(9000, 15000));
                events.ScheduleEvent(EVENT_SQUEEZE, urand(14000, 20000));
                instance->SetBossState(DATA_COMMANDER_ULTHOK, IN_PROGRESS);
                me->RemoveAura(76017);
            }

            void JustDied(Unit* /*pKiller*/) override
            {
                _JustDied();
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
                    case EVENT_DARK_FISSURE:
                        DoCast(SPELL_DARK_FISSURE);
                        events.ScheduleEvent(EVENT_DARK_FISSURE, urand(20000, 22000));
                        break;
                    case EVENT_ENRAGE:
                        DoCast(SPELL_ENRAGE);
                        events.ScheduleEvent(EVENT_ENRAGE, urand(20000, 25000));
                        break;
                    case EVENT_SQUEEZE:
                        if (auto pTarget = SelectTarget(SELECT_TARGET_RANDOM, 1, 100.0f, true))
                            DoCast(pTarget, SPELL_SQUEEZE, false);
                        events.ScheduleEvent(EVENT_SQUEEZE, urand(29000, 31000));
                        break;
                    case EVENT_CURSE_OF_FATIGUE:
                        if (auto pTarget = SelectTarget(SELECT_TARGET_RANDOM, 1, 100.0f, true))
                            DoCast(pTarget, SPELL_CURSE_OF_FATIGUE, false);
                        events.ScheduleEvent(EVENT_CURSE_OF_FATIGUE, urand(13000, 15000));
                        break;
                    }
                }
                DoMeleeAttackIfReady();
            }
        };

};

class npc_ulthok_dark_fissure : public CreatureScript
{
    public:
        npc_ulthok_dark_fissure() : CreatureScript("npc_ulthok_dark_fissure") {}

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return GetInstanceAI<npc_ulthok_dark_fissureAI>(pCreature);
        }

        struct npc_ulthok_dark_fissureAI : public Scripted_NoMovementAI
        {
            npc_ulthok_dark_fissureAI(Creature* creature) : Scripted_NoMovementAI(creature)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                me->SetReactState(REACT_PASSIVE);
            }

            void Reset() override
            {
                DoCast(me, SPELL_DARK_FISSURE_AURA, true);
            }
        };
};

class at_tott_commander_ulthok : public AreaTriggerScript
{
    public:
        at_tott_commander_ulthok() : AreaTriggerScript("at_tott_commander_ulthok") {}

        bool OnTrigger(Player* pPlayer, const AreaTriggerEntry* /*pAt*/, bool /*enter*/)
        {
            if (InstanceScript* instance = pPlayer->GetInstanceScript())
            {
                if (instance->GetData(DATA_COMMANDER_ULTHOK_EVENT) != DONE && instance->GetBossState(DATA_LADY_NAZJAR) == DONE)
                {
                    instance->SetData(DATA_COMMANDER_ULTHOK_EVENT, DONE);
                    if (auto pUlthok = ObjectAccessor::GetCreature(*pPlayer, instance->GetGuidData(DATA_COMMANDER_ULTHOK)))
                        pUlthok->AI()->DoAction(ACTION_COMMANDER_ULTHOK_START_EVENT);
                }
            }
            return true;
        }
};

void AddSC_boss_commander_ulthok()
{
    new boss_commander_ulthok();
    new npc_ulthok_dark_fissure();
    new at_tott_commander_ulthok();
}