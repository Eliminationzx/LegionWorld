#include "throne_of_the_tides.h"

enum ScriptTextsErunak
{
    SAY_FREED   = 0,
    SAY_VICTORY = 1
};

enum ScriptTextsGhursha
{
    SAY_PHASE   = 0,
    SAY_KILL    = 1,
    SAY_DEATH   = 2,
    SAY_ENCLAVE = 3,
    SAY_FOG     = 4
};

enum Spells
{
    // Erunak Stonespeaker
    SPELL_EARTH_SHARDS              = 84931,
    SPELL_EARTH_SHARDS_AURA         = 84935,
    SPELL_EARTH_SHARDS_DMG          = 84945,
    SPELL_EMBERSTRIKE               = 76165,
    SPELL_LAVA_BOLT                 = 76171,
    SPELL_MAGMA_SPLASH              = 76170,

    // Mindbender Ghur'sha
    SPELL_ENSLAVE                   = 76207,
    SPELL_ENSLAVE_BUFF              = 76213,
    SPELL_ABSORB_MAGIC              = 76307,
    SPELL_MIND_FOG                  = 76234,
    SPELL_MIND_FOG_AURA             = 76230,
    SPELL_MIND_FOG_VISUAL           = 76231,
    SPELL_UNRELENTING_AGONY         = 76339,
    SPELL_UNRELENTING_AGONY_DMG     = 76341
};

enum Events
{
    EVENT_EARTH_SHARDS      = 1,
    EVENT_EMBERSTRIKE       = 2,
    EVENT_LAVA_BOLT         = 3,
    EVENT_MAGMA_SPLASH      = 4,
    EVENT_ENSLAVE           = 5,
    EVENT_ABSORB_MAGIC      = 6,
    EVENT_MIND_FOG          = 7,
    EVENT_UNRELENTING_AGONY = 8
};

enum Actions
{
    ACTION_GHURSHA_START    = 1
};

enum Adds
{
    NPC_EARTH_SHARDS    = 45469,
    NPC_MIND_FOG        = 40861
};

class boss_erunak_stonespeaker : public CreatureScript
{
    public:
        boss_erunak_stonespeaker() : CreatureScript("boss_erunak_stonespeaker") {}

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetInstanceAI<boss_erunak_stonespeakerAI>(creature);
        }

        struct boss_erunak_stonespeakerAI : public ScriptedAI
        {
            boss_erunak_stonespeakerAI(Creature* creature) : ScriptedAI(creature), summons(me)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            EventMap events;
            SummonList summons;

            bool bPhase;

            void Reset() override
            {
                bPhase = false;
                events.Reset();
                summons.DespawnAll();
                if (instance)
                {
                    if (instance->GetBossState(DATA_MINDBENDER_GHURSHA) == DONE || bPhase)
                    {
                        me->setFaction(35);
                        me->RemoveAllAuras();
                    }
                }
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                if (instance)
                    if (auto pGhursha = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_MINDBENDER_GHURSHA)))
                        pGhursha->AI()->Talk(SAY_KILL);
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell) override
            {
                if (me->GetCurrentSpell(CURRENT_GENERIC_SPELL))
                    if (me->GetCurrentSpell(CURRENT_GENERIC_SPELL)->m_spellInfo->Id == SPELL_LAVA_BOLT)
                        for (uint8 i = 0; i < 3; ++i)
                            if (spell->Effects[i]->Effect == SPELL_EFFECT_INTERRUPT_CAST)
                                me->InterruptSpell(CURRENT_GENERIC_SPELL);
            }

            void EnterCombat(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_EARTH_SHARDS, 8000);
                events.ScheduleEvent(EVENT_EMBERSTRIKE, 11000);
                events.ScheduleEvent(EVENT_LAVA_BOLT, 13000);
                events.ScheduleEvent(EVENT_MAGMA_SPLASH, 6000);
                if (instance)
                    instance->SetBossState(DATA_MINDBENDER_GHURSHA, IN_PROGRESS);
            }

            void JustSummoned(Creature* summon) override
            {
                if (me->isInCombat())
                    summon->SetInCombatWithZone();
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->GetHealthPct() < 50 && !bPhase)
                {
                    bPhase = true;
                    events.Reset();
                    me->setFaction(35);
                    EnterEvadeMode();
                    if (instance)
                    {
                        if (auto pGhursha = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_MINDBENDER_GHURSHA)))
                            pGhursha->AI()->DoAction(ACTION_GHURSHA_START);
                    }
                    return;
                }

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                    case EVENT_EARTH_SHARDS:
                        if (auto pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                            DoCast(pTarget, SPELL_EARTH_SHARDS, false);
                        events.ScheduleEvent(EVENT_EARTH_SHARDS, 20000);
                        break;
                    case EVENT_EMBERSTRIKE:
                        if (auto victim = me->getVictim())
                            DoCast(victim, SPELL_EMBERSTRIKE);
                        events.ScheduleEvent(EVENT_EMBERSTRIKE, 11000);
                        break;
                    case EVENT_LAVA_BOLT:
                        if (auto pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                            DoCast(pTarget, SPELL_LAVA_BOLT, false);
                        events.ScheduleEvent(EVENT_LAVA_BOLT, 14000);
                        break;
                    case EVENT_MAGMA_SPLASH:
                        DoCast(SPELL_MAGMA_SPLASH);
                        events.ScheduleEvent(EVENT_MAGMA_SPLASH, 13000);
                        break;
                    }
                }            
                DoMeleeAttackIfReady();
            }
        };
};

class boss_mindbender_ghursha : public CreatureScript
{
    public:
        boss_mindbender_ghursha() : CreatureScript("boss_mindbender_ghursha") {}

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return GetInstanceAI<boss_mindbender_ghurshaAI>(pCreature);
        }

        struct boss_mindbender_ghurshaAI : public BossAI
        {
            boss_mindbender_ghurshaAI(Creature* pCreature) : BossAI(pCreature, DATA_MINDBENDER_GHURSHA) {}

            void Reset() override
            {
                _Reset();

                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                events.Reset();
                if (auto pErunak = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ERUNAK_STONESPEAKER)))
                    pErunak->AI()->EnterEvadeMode();
            }

            void DoAction(const int32 action) override
            {
                if (action == ACTION_GHURSHA_START)
                {
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    if (auto pTarget = me->SelectNearestTarget(100.0f))
                        me->GetMotionMaster()->MoveChase(pTarget);
                }
            }

            void EnterCombat(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_ENSLAVE, 13000);
                events.ScheduleEvent(EVENT_ABSORB_MAGIC, 20000);
                events.ScheduleEvent(EVENT_MIND_FOG, urand(6000,12000));
                events.ScheduleEvent(EVENT_UNRELENTING_AGONY, 10000);
                Talk(SAY_PHASE);
                instance->SetBossState(DATA_MINDBENDER_GHURSHA, IN_PROGRESS);
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
                    case EVENT_ENSLAVE:
                        Talk(SAY_ENCLAVE);
                        events.ScheduleEvent(EVENT_ENSLAVE, 31000);
                        break;
                    case EVENT_ABSORB_MAGIC:
                        DoCast(SPELL_ABSORB_MAGIC);
                        events.ScheduleEvent(EVENT_ABSORB_MAGIC, 15000);
                        break;
                    case EVENT_MIND_FOG:
                        DoCast(SPELL_MIND_FOG);
                        events.ScheduleEvent(EVENT_MIND_FOG, urand(23000,25000));
                        break;
                    case EVENT_UNRELENTING_AGONY:
                        DoCast(SPELL_UNRELENTING_AGONY);
                        events.ScheduleEvent(EVENT_UNRELENTING_AGONY, 30000);
                        break;
                    }
                }        
                DoMeleeAttackIfReady();
            }

            void KilledUnit(Unit* victim) override
            {
                if (!victim)
                    return;

                if (victim->IsPlayer())
                    Talk(SAY_KILL);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                Talk(SAY_DEATH);
                if (auto pErunak = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ERUNAK_STONESPEAKER)))
                {
                    pErunak->AI()->EnterEvadeMode();
                    pErunak->AI()->Talk(SAY_VICTORY);
                }
            }
        };
};

class npc_erunak_earth_shards : public CreatureScript
{
    public:
        npc_erunak_earth_shards() : CreatureScript("npc_erunak_earth_shards") {}

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return GetInstanceAI<npc_erunak_earth_shardsAI>(pCreature);
        }

        struct npc_erunak_earth_shardsAI : public Scripted_NoMovementAI
        {
            npc_erunak_earth_shardsAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                me->SetReactState(REACT_PASSIVE);
            }

            uint32 uiDespawnTimer;

            void Reset() override
            {
                uiDespawnTimer = 5000;
                DoCast(SPELL_EARTH_SHARDS_AURA);
            }

            void UpdateAI(uint32 diff) override
            {
                if (uiDespawnTimer <= diff)
                    me->DespawnOrUnsummon();
                else
                    uiDespawnTimer -= diff;
            }
        };
};

class npc_ghursha_mind_fog : public CreatureScript
{
    public:
        npc_ghursha_mind_fog() : CreatureScript("npc_ghursha_mind_fog") {}

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return GetInstanceAI<npc_ghursha_mind_fogAI>(pCreature);
        }

        struct npc_ghursha_mind_fogAI : public Scripted_NoMovementAI
        {
            npc_ghursha_mind_fogAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                me->SetReactState(REACT_PASSIVE);
            }

            void Reset() override
            {
                DoCast(me, SPELL_MIND_FOG_AURA, true);
                DoCast(me, SPELL_MIND_FOG_VISUAL, true);
            }
        };
};

void AddSC_boss_erunak_stonespeaker()
{
    new boss_erunak_stonespeaker();
    new boss_mindbender_ghursha();
    new npc_erunak_earth_shards();
    new npc_ghursha_mind_fog();
}