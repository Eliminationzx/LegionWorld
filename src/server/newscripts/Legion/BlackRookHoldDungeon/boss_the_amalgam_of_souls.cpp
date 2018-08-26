/*
    http://uwow.biz
    Dungeon : Black Rook Hold Dungeon 100-110
    Encounter: The Amalgam of Souls
    Normal: 100%, Heroic: 100%, Mythic: 100%
*/

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "black_rook_hold_dungeon.h"

enum Says
{
    SAY_AGGRO           = 0,
    SAY_SCYTHE          = 1,
    SAY_SOUL            = 2,
    SAY_REAP            = 3,
    SAY_DEATH           = 4,
};

enum Spells
{
    SPELL_SWIRLING_SCYTHE       = 195254,
    SPELL_SOUL_ECHOES           = 194966,
    SPELL_REAP_SOUL             = 194956,

    //Heroic
    SPELL_CALL_SOULS            = 196078,
    SPELL_CALL_SOULS_AT         = 196925,
    SPELL_SOUL_BURST            = 196587,
    SPELL_SOULGORGE             = 196930,

    //Trash
    SPELL_SOUL_ECHOES_CLONE_PLR = 194981,
    SPELL_SOUL_ECHOES_DMG       = 194960,

    SPELL_SUMMON_MINIBOSS_A     = 196619, //Суммонятся что бы открыть двери, после смерти босса
    SPELL_SUMMON_MINIBOSS_B     = 196620,
    SPELL_SUMMON_MINIBOSS_C     = 196646,
    SPELL_SUMMON_CONVERSATION   = 197075,
};

enum eEvents
{
    EVENT_SWIRLING_SCYTHE       = 1,
    EVENT_SOUL_ECHOES           = 2,
    EVENT_REAP_SOUL             = 3,
    EVENT_MOVE_CENTR_POS        = 4,
    EVENT_CALL_SOULS            = 5,
    EVENT_CHECK_SOULS           = 6,
    EVENT_REMOVE_SOULS          = 7,
};

enum eAnim //SMSG_PLAY_ONE_SHOT_ANIM_KIT
{
    STALKER_ANIM_1 = 9038,
    STALKER_ANIM_2 = 9039,
    STALKER_ANIM_3 = 9176
};

uint32 AnimRand[3] =
{
    STALKER_ANIM_1,
    STALKER_ANIM_2,
    STALKER_ANIM_3
};

Position const centrPos = {3251.35f, 7582.79f, 12.75f};

Position const soulsPos[7] =
{
    {3288.37f, 7593.30f, 14.10f, 3.45f},
    {3259.17f, 7620.32f, 14.10f, 4.53f},
    {3239.09f, 7618.44f, 14.10f, 5.05f},
    {3213.97f, 7587.99f, 14.10f, 6.12f},
    {3215.85f, 7567.98f, 14.10f, 0.36f},
    {3245.75f, 7541.42f, 14.10f, 1.41f},
    {3265.50f, 7543.86f, 14.10f, 1.90f}
};

//98542
class boss_the_amalgam_of_souls : public CreatureScript
{
public:
    boss_the_amalgam_of_souls() : CreatureScript("boss_the_amalgam_of_souls")  {}

    struct boss_the_amalgam_of_soulsAI : public BossAI
    {
        boss_the_amalgam_of_soulsAI(Creature* creature) : BossAI(creature, DATA_AMALGAM) 
        {
            if (me->isAlive())
                instance->SetData(DATA_AMALGAM_OUTRO, IN_PROGRESS);   
        }

        bool heroicEvent;

        void Reset() override
        {
            _Reset();
            me->RemoveAurasDueToSpell(SPELL_CALL_SOULS_AT);
            me->SetReactState(REACT_AGGRESSIVE);
            heroicEvent = false;
        }

        void EnterCombat(Unit* /*who*/) override
        //58:07
        {
            Talk(SAY_AGGRO); //Consume! Devour!
            _EnterCombat();
            DefaultEvents();
        }

        void DefaultEvents()
        {
            events.ScheduleEvent(EVENT_SWIRLING_SCYTHE, 8000); //58:15, 58:36, 58:59
            events.ScheduleEvent(EVENT_SOUL_ECHOES, 14000); //58:22, 58:50, 59:18, 59:45
            events.ScheduleEvent(EVENT_REAP_SOUL, 20000); //58:27, 58:40, 58:54, 59:07
        }

        void JustDied(Unit* /*killer*/) override
        {
            _JustDied();
            DoCast(me, SPELL_SUMMON_MINIBOSS_C, true);
            DoCast(me, SPELL_SUMMON_MINIBOSS_B, true);
            DoCast(me, SPELL_SUMMON_MINIBOSS_A, true);
            instance->DoUpdateAchievementCriteria(CRITERIA_TYPE_COMPLETE_DUNGEON_ENCOUNTER, 1832);
        }

        void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType dmgType) override
        {
            if (me->HealthBelowPct(50) && !heroicEvent)
            {
                heroicEvent = true;
                if (GetDifficultyID() != DIFFICULTY_NORMAL)
                {
                    me->StopAttack();
                    events.Reset();
                    events.ScheduleEvent(EVENT_MOVE_CENTR_POS, 500);
                }
            }
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type != POINT_MOTION_TYPE || id != 1)
                return;

            events.ScheduleEvent(EVENT_CALL_SOULS, 1000);
        }

        void SpellHit(Unit* caster, const SpellInfo* spell) override
        {
            if (spell->Id == SPELL_CALL_SOULS)
            {
                DoCast(me, SPELL_CALL_SOULS_AT, true);

                for (uint8 i = 0; i < 7; i++)
                    me->SummonCreature(NPC_RESTLESS_SOUL, soulsPos[i]);

                events.ScheduleEvent(EVENT_CHECK_SOULS, 2000);
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (CheckHomeDistToEvade(diff, 15.0f))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_SWIRLING_SCYTHE:
                        Talk(SAY_SCYTHE);
                        if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 60.0f, true))
                            DoCast(pTarget, SPELL_SWIRLING_SCYTHE);
                        events.ScheduleEvent(EVENT_SWIRLING_SCYTHE, 20000);
                        break;
                    case EVENT_SOUL_ECHOES:
                        Talk(SAY_SOUL);
                        if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 60.0f, true))
                            DoCast(pTarget, SPELL_SOUL_ECHOES);
                        events.ScheduleEvent(EVENT_SOUL_ECHOES, 28000);
                        break;
                    case EVENT_REAP_SOUL:
                        Talk(SAY_REAP);
                        DoCastVictim(SPELL_REAP_SOUL);
                        events.ScheduleEvent(EVENT_REAP_SOUL, 13000);
                        break;
                    case EVENT_MOVE_CENTR_POS:
                        me->GetMotionMaster()->MovePoint(1, me->GetHomePosition());
                        break;
                    case EVENT_CALL_SOULS:
                        DoCast(SPELL_CALL_SOULS);
                        break;
                    case EVENT_CHECK_SOULS:
                        if (Creature* soul = me->FindNearestCreature(NPC_RESTLESS_SOUL, 100.0f))
                            events.ScheduleEvent(EVENT_CHECK_SOULS, 2000);
                        else
                        {
                            me->RemoveAurasDueToSpell(SPELL_CALL_SOULS_AT);
                            DoCast(SPELL_SOUL_BURST);
                            me->SetReactState(REACT_AGGRESSIVE);
                            DefaultEvents();
                            events.ScheduleEvent(EVENT_REMOVE_SOULS, 6000);
                        }
                        break;
                    case EVENT_REMOVE_SOULS:
                        me->RemoveAurasDueToSpell(SPELL_SOULGORGE);
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_the_amalgam_of_soulsAI(creature);
    }
};

//99090
class npc_soul_echoes_stalker : public CreatureScript
{
public:
    npc_soul_echoes_stalker() : CreatureScript("npc_soul_echoes_stalker") {}

    struct npc_soul_echoes_stalkerAI : public ScriptedAI
    {
        npc_soul_echoes_stalkerAI(Creature* creature) : ScriptedAI(creature) 
        {
            me->SetReactState(REACT_PASSIVE);
        }

        EventMap events;

        void Reset() override {}

        void IsSummonedBy(Unit* summoner) override
        {
            summoner->CastSpell(me, SPELL_SOUL_ECHOES_CLONE_PLR, true);
            me->PlayOneShotAnimKit(AnimRand[urand(STALKER_ANIM_1, STALKER_ANIM_3)]);
            events.ScheduleEvent(EVENT_1, 4000);
        }

        void SpellHitTarget(Unit* target, const SpellInfo* spell) override
        {
            if (spell->Id == SPELL_SOUL_ECHOES_DMG)
                if (me->GetMap()->GetDifficultyID() != DIFFICULTY_NORMAL)
                    DoCast(target, 204246, true); // Возможно указал неверный спелл
        }

        void UpdateAI(uint32 diff) override
        {
            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                if (eventId == EVENT_1)
                    DoCast(me, SPELL_SOUL_ECHOES_DMG, true);
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_soul_echoes_stalkerAI(creature);
    }
};

//
class npc_soul_echoes_outro : public CreatureScript
{
public:
    npc_soul_echoes_outro() : CreatureScript("npc_soul_echoes_outro") {}

    struct npc_soul_echoes_outroAI : public ScriptedAI
    {
        npc_soul_echoes_outroAI(Creature* creature) : ScriptedAI(creature)
        {
            first = false;
            two = false;
            three = false;
            instance = me->GetInstanceScript();
            instance->SetData(DATA_AMALGAM_OUTRO, IN_PROGRESS);
            outro = false;
        }

        EventMap events;
        InstanceScript* instance;
        bool first;
        bool two;
        bool three;
        bool outro;

        void IsSummonedBy(Unit* summoner) override
        {
            events.ScheduleEvent(EVENT_3, 10);
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type != POINT_MOTION_TYPE)
                return;

            switch (id)
            {
                case 1:
                    instance->SetData(DATA_AMALGAM_OUTRO, 0);
                    Talk(0);
                    break;
                case 2:
                    instance->SetData(DATA_AMALGAM_OUTRO, 0);
                    break;
                case 3:
                    instance->SetData(DATA_AMALGAM_OUTRO, 0);
                    break;
            }
        }

        void UpdateAI(uint32 diff) override
        {
            events.Update(diff);

            if (instance->GetData(DATA_AMALGAM_OUTRO) >= 3 && !outro)
            {
                outro = true;
                if (me->GetEntry() == 99857)
                    me->CastSpell(me, 205212);
                if (me->GetEntry() == 99426)
                    me->CastSpell(me, 205210);
                if (me->GetEntry() == 99858)
                {
                    std::list<Creature*> list;
                    list.clear();
                    me->GetCreatureListWithEntryInGrid(list, 103662, 30.0f);
                    if (!list.empty())
                        for (std::list<Creature*>::const_iterator itr = list.begin(); itr != list.end(); ++itr)
                            if (!(*itr)->HasAura(205210) && !(*itr)->HasAura(205212))
                                me->CastSpell((*itr), 205211, true);
                }
                events.ScheduleEvent(EVENT_2, 5000);
            }

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                   case EVENT_1:
                        if (me->GetEntry() == 99857)
                            me->GetMotionMaster()->MovePoint(1, 3226.46f, 7553.97f, 15.26f);
                        if (me->GetEntry() == 99858)
                            me->GetMotionMaster()->MovePoint(2, 3228.56f, 7548.91f, 14.85f);
                        if (me->GetEntry() == 99426)
                            me->GetMotionMaster()->MovePoint(3, 3233.58f, 7548.09f, 15.04f);
                        break;
                   case EVENT_2:
                        me->CastStop();
                        instance->SetData(DATA_AMALGAM_OUTRO, DONE);
                        me->DespawnOrUnsummon(2000);
                        break;          
                   case EVENT_3:
                        if (me->GetEntry() == 99857)
                            DoCast(SPELL_SUMMON_CONVERSATION);        
                        events.ScheduleEvent(EVENT_1, 7000);
                        break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_soul_echoes_outroAI(creature);
    }
};

//99664
class npc_amalgam_restless_soul : public CreatureScript
{
public:
    npc_amalgam_restless_soul() : CreatureScript("npc_amalgam_restless_soul") {}

    struct npc_amalgam_restless_soulAI : public ScriptedAI
    {
        npc_amalgam_restless_soulAI(Creature* creature) : ScriptedAI(creature) 
        {
            me->SetReactState(REACT_PASSIVE);
            me->SetSpeed(MOVE_FLIGHT, 0.3f);
            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISTRACT, true);
            me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
        }

        uint16 moveTimer;

        void Reset() override {}

        void IsSummonedBy(Unit* summoner) override
        {
            moveTimer = 1000;
        }

        void UpdateAI(uint32 diff) override
        {
            if (moveTimer)
            {
                if (moveTimer <= diff)
                {
                    moveTimer = 1000;

                    if (me->GetDistance(centrPos) < 4.0f)
                    {
                        moveTimer = 0;
                        me->DespawnOrUnsummon(4000);
                        DoCast(me, SPELL_SOULGORGE, true);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                        me->SetVisible(false);
                    }
                    else
                    {
                        if (!me->HasUnitState(UNIT_STATE_STUNNED))
                            me->GetMotionMaster()->MovePoint(1, centrPos);
                    }
                }
                else
                    moveTimer -= diff;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_amalgam_restless_soulAI(creature);
    }
};

void AddSC_boss_the_amalgam_of_souls()
{
    new boss_the_amalgam_of_souls();
    new npc_soul_echoes_stalker();
    new npc_soul_echoes_outro();
    new npc_amalgam_restless_soul();
}