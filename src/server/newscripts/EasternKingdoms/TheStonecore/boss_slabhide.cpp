#include "the_stonecore.h"

enum Spells
{
    SPELL_SAND_BLAST            = 80807,
    SPELL_LAVA_FISSURE_DUM      = 80798,
    SPELL_ERUPTION              = 80800,
    SPELL_ERUPTION_AURA         = 80801,
    SPELL_STALACTITE_VISUAL     = 80654,
    SPELL_STALACTITE_MISSILE    = 80643,
    SPELL_STALACTITE            = 80656
};

enum Events
{
    EVENT_SAND_BLAST        = 1,
    EVENT_LAVA_FISSURE      = 2,
    EVENT_FLY               = 3,
    EVENT_GROUND            = 4,
    EVENT_ERUPTION          = 5,
    EVENT_STALACTITE_CAST   = 6
};

enum Adds
{
    NPC_LAVA_FISSURE                = 43242,
    NPC_STALACTITE_TRIGGER          = 43159,
    NPC_STALACTITE__GROUND_TRIGGER  = 43357
};

Position slabhidegroundPos = {1278.23f, 1212.27f, 247.28f, 0.0f};
Position slabhideflyPos = {1278.23f, 1212.27f, 257.28f, 0.0f};

class boss_slabhide : public CreatureScript
{
    public:
        boss_slabhide() : CreatureScript("boss_slabhide") {}

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_slabhideAI(creature);
        }

        struct boss_slabhideAI : public BossAI
        {
            boss_slabhideAI(Creature* pCreature) : BossAI(pCreature, DATA_SLABHIDE) {}

            EventMap events;

            void Reset() override
            {
                _Reset();

                me->SetCanFly(false);
                events.Reset();
            }

            void EnterCombat(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_FLY, 50000);
                events.ScheduleEvent(EVENT_SAND_BLAST, 10000);
                events.ScheduleEvent(EVENT_LAVA_FISSURE, urand(10000, 15000));
                instance->SetBossState(DATA_SLABHIDE, IN_PROGRESS);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                me->SetCanFly(false);
            }

            void MovementInform(uint32 type, uint32 id) override
            {
                if (type == POINT_MOTION_TYPE)
                {
                    switch (id)
                    {
                    case 1:
                        events.ScheduleEvent(EVENT_GROUND, 10000);
                        events.ScheduleEvent(EVENT_STALACTITE_CAST, 3000);
                        break;
                    case 2:
                        me->SetCanFly(false);
                        SetCombatMovement(true);
                        events.ScheduleEvent(EVENT_SAND_BLAST, 10000);
                        events.ScheduleEvent(EVENT_LAVA_FISSURE, urand(15000, 28000));
                        events.ScheduleEvent(EVENT_FLY, 50000);
                        if (auto victim = me->getVictim())
                            AttackStart(victim);
                        break;
                    }
                }
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
                        case EVENT_FLY:
                            events.Reset();
                            SetCombatMovement(false);
                            me->SetCanFly(true);
                            me->GetMotionMaster()->MovePoint(1, slabhideflyPos);
                            break;
                        case EVENT_GROUND:
                            events.Reset();
                            me->GetMotionMaster()->MovePoint(2, slabhidegroundPos);
                            break;
                        case EVENT_SAND_BLAST:
                            DoCast(SPELL_SAND_BLAST);
                            events.ScheduleEvent(EVENT_SAND_BLAST, 10000);
                            break;
                        case EVENT_LAVA_FISSURE:
                            if (auto pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                                me->SummonCreature(NPC_LAVA_FISSURE, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0.0f);
                            events.ScheduleEvent(EVENT_LAVA_FISSURE, urand(15000, 18000));
                            break;
                        case EVENT_STALACTITE_CAST:
                            DoCast(me, SPELL_STALACTITE, true);
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }
        };

};

class npc_lava_fissure : public CreatureScript
{
    public:
        npc_lava_fissure() : CreatureScript("npc_lava_fissure") {}

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_lava_fissureAI(pCreature);
        }

        struct npc_lava_fissureAI : public ScriptedAI
        {
            npc_lava_fissureAI(Creature *c) : ScriptedAI(c) {}

            EventMap events;
            uint32 uidespawnTimer;

            void Reset() override
            {
                uidespawnTimer = DUNGEON_MODE(15000, 33000);
                events.ScheduleEvent(EVENT_ERUPTION, DUNGEON_MODE(5000, 3000));
                DoCast(SPELL_LAVA_FISSURE_DUM);
            }

            void UpdateAI(uint32 diff) override
            {
                if (uidespawnTimer <= diff)
                    me->DespawnOrUnsummon();
                else
                    uidespawnTimer -= diff;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch(eventId)
                    {
                    case EVENT_ERUPTION:
                        me->RemoveAurasDueToSpell(SPELL_LAVA_FISSURE_DUM);
                        DoCast(SPELL_ERUPTION);
                        break;
                    }
                }
            }
        };
};

struct npc_stalactite_stalker : public ScriptedAI
{
    npc_stalactite_stalker(Creature * creature) : ScriptedAI(creature)
    {
        me->SetReactState(REACT_PASSIVE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
    }

    float x, y, z;
    bool done;
    bool visual;
    uint32 visualTimer;
    uint32 summonTimer;

    void Reset()
    {
        done = false;
        visual = false;
        visualTimer = 2000;
        summonTimer = 8000;
    }

    void IsSummonedBy(Unit* /*summoner*/)
    {
        me->GetPosition(x, y, z);
        me->GetMotionMaster()->Clear();
        me->NearTeleportTo(x, y, z + 50.0f, 0.0f);
        me->SendMovementFlagUpdate();
    }

    void UpdateAI(uint32 diff)
    {
        if (!visual)
        {
            if (visualTimer <= diff)
            {
                visual = true;
                me->CastSpell(x, y, z, SPELL_STALACTITE_VISUAL, true);
            }
            else visualTimer -= diff;
        }

        if (!done)
        {
            if (summonTimer <= diff)
            {
                done = true;
                me->CastSpell(x, y, z, SPELL_STALACTITE_MISSILE, false);
                me->DespawnOrUnsummon(30000);
            }
            else summonTimer -= diff;
        }
    }
};

void AddSC_boss_slabhide()
{
    new boss_slabhide();
    new npc_lava_fissure();
    RegisterCreatureAI(npc_stalactite_stalker);
}