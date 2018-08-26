/*
    http://uwow.biz
    Dungeon : Black Rook Hold Dungeon 100-110
    Encounter: Lord Kurtalos Ravencrest
    Normal: 100%, Heroic: 100%, Mythic: 100%
*/

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "black_rook_hold_dungeon.h"

enum Says
{
    SAY_AGGRO           = 0,
};

enum Spells
{
//Kurtalos
    //Phase 1
    SPELL_UNERRING_SHEAR        = 198635,
    SPELL_WHIRLING_BLADE        = 198641,
    SPELL_WHIRLING_BLADE_AT     = 198782,
    SPELL_SUICIDE               = 117624,
    //Phase 2
    SPELL_KURTALOS_GHOST_VISUAL = 199243,
    SPELL_LEGACY_RAVENCREST     = 199368,

//Latosius
    //Phase 1
    SPELL_TELEPORT_1            = 198835,
    SPELL_TELEPORT_2            = 199058,
    SPELL_SHADOW_BOLT           = 198833,
    SPELL_DARK_BLAST            = 198820,
    SPELL_DARK_OBLITERATION     = 199567,
    //Phase 2
    SPELL_CONVERSATION          = 199239, // Dreadlord Conversation
    SPELL_SAP_SOUL              = 198961,
    SPELL_TRANSFORM             = 199064,
    SPELL_SHADOW_BOLT_VOLLEY    = 202019,
    SPELL_CLOUD_OF_HYPNOSIS     = 199143,
    SPELL_DREADLORDS_GUILE      = 199193,
    SPELL_BREAK_PLR_TARGETTING  = 140562,
    //Heroic
    SPELL_STINGING_SWARM        = 201733,
};

enum eEvents
{
//Kurtalos
    //Phase 1
    EVENT_UNERRING_SHEAR        = 1,
    EVENT_WHIRLING_BLADE        = 2,

//Latosius
    //Phase 1
    EVENT_LATOSIUS_TP           = 1,
    EVENT_SHADOW_BOLT           = 2,
    EVENT_DARK_BLAST            = 3,
    //Phase 2
    EVENT_LATOSIUS_TP_2         = 4,
    EVENT_SAP_SOUL              = 5,
    EVENT_TRANSFORM             = 6,
    EVENT_SHADOW_BOLT_VOLLEY    = 7,
    EVENT_CLOUD_OF_HYPNOSIS     = 8,
    EVENT_DREADLORDS_GUILE      = 9,
    EVENT_SUM_IMAGE             = 10,
    EVENT_IMAGE_END             = 11,
    EVENT_STINGING_SWARM        = 12, //Heroic
};

enum ePhase
{
    PHASE_1     = 0,
    PHASE_2     = 1,
};

Position const tpPos[12] =
{
    {3169.09f, 7432.38f, 272.10f, 5.42f},
    {3165.53f, 7421.75f, 272.10f, 5.85f},
    {3158.29f, 7414.00f, 272.10f, 0.0f },
    {3164.16f, 7395.63f, 272.10f, 0.62f},
    {3173.76f, 7387.44f, 272.10f, 1.08f},
    {3184.47f, 7383.66f, 272.10f, 1.49f},
    {3200.61f, 7390.41f, 272.10f, 2.14f},
    {3205.70f, 7400.89f, 272.10f, 2.61f},
    {3210.50f, 7409.22f, 272.10f, 3.02f},
    {3204.84f, 7427.28f, 272.10f, 3.84f},
    {3195.91f, 7430.39f, 272.10f, 4.24f},
    {3186.87f, 7436.26f, 272.10f, 4.70f}
};

//98970
class boss_latosius : public CreatureScript
{
public:
    boss_latosius() : CreatureScript("boss_latosius") {}

    struct boss_latosiusAI : public BossAI
    {
        boss_latosiusAI(Creature* creature) : BossAI(creature, DATA_KURTALOS) {}

        bool secondPhase;
        uint8 imageSumCount;

        void Reset() override
        {
            if (instance->GetData(DATA_KURTALOS_STATE) != PHASE_1)
                return;

            _Reset();
            secondPhase = false;
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1);
            SetEquipmentSlots(false, 65483);
            me->SummonCreature(NPC_KURTALOS, 3191.72f, 7423.69f, 270.462f, 0.57f);
            RemoveAuras();
        }

        void EnterCombat(Unit* /*who*/) override
        //22:08
        {
            if (instance->GetData(DATA_KURTALOS_STATE) != PHASE_1)
                return;

            _EnterCombat();
            if (Creature* kurtalos = me->FindNearestCreature(NPC_KURTALOS, 30.0f))
                kurtalos->AI()->DoZoneInCombat(kurtalos, 100.0f);

            events.ScheduleEvent(EVENT_LATOSIUS_TP, 0); //22:08
            events.ScheduleEvent(EVENT_DARK_BLAST, 9000);
        }

        void EnterEvadeMode() override
        {
            if (instance->GetData(DATA_KURTALOS_STATE) != PHASE_1)
                instance->SetData(DATA_KURTALOS_STATE, PHASE_1);

            me->RemoveAurasDueToSpell(SPELL_TRANSFORM);
            me->NearTeleportTo(3196.04f, 7425.02f, 270.373f, 3.47f); //Go Home
            me->UpdateEntry(NPC_LATOSIUS);
            me->SetVisible(true);

            BossAI::EnterEvadeMode();
        }

        void JustDied(Unit* /*killer*/) override
        {
            _JustDied();
            RemoveAuras();
            instance->DoUpdateAchievementCriteria(CRITERIA_TYPE_COMPLETE_DUNGEON_ENCOUNTER, 1835);
            
            // for quest need "kill"
            Map::PlayerList const& players = me->GetMap()->GetPlayers();
            if (!players.isEmpty())
            {
                for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                {
                    if (Player* pPlayer = itr->getSource())
                    {
                        pPlayer->KilledMonsterCredit(99611);
                    }
                }
            }
        }

        void RemoveAuras()
        {
            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNERRING_SHEAR);
            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_LEGACY_RAVENCREST);
            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_STINGING_SWARM);
        }

        void DoAction(int32 const action) override
        {
            if (action == ACTION_1 && instance->GetData(DATA_KURTALOS_STATE) != PHASE_2)
            {
                instance->SetData(DATA_KURTALOS_STATE, PHASE_2);
                events.Reset();
                events.ScheduleEvent(EVENT_LATOSIUS_TP_2, 1000);
            }
        }

        void SpellHit(Unit* caster, const SpellInfo* spell) override
        {
            switch (spell->Id)
            {
                case SPELL_DREADLORDS_GUILE:
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1);
                    me->SetVisible(false);
                    DoCast(me, SPELL_BREAK_PLR_TARGETTING, true);
                    break;
                default:
                    break;
            }
        }

        void SpellHitTarget(Unit* target, const SpellInfo* spell) override
        {
            if (spell->Id == SPELL_SAP_SOUL)
                events.ScheduleEvent(EVENT_TRANSFORM, 8000);

            if (spell->Id == SPELL_STINGING_SWARM)
            {
                if (Creature* sum = target->SummonCreature(NPC_STINGING_SWARM, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ()))
                    sum->EnterVehicle(target);
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (secondPhase && CheckHomeDistToEvade(diff, 41.0f))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                if (me->HasAura(SPELL_DREADLORDS_GUILE) && eventId != EVENT_SUM_IMAGE && eventId != EVENT_IMAGE_END)
                    return;

                switch (eventId)
                {
                    case EVENT_LATOSIUS_TP:
                        DoCast(me, SPELL_TELEPORT_1, true);
                        events.ScheduleEvent(EVENT_SHADOW_BOLT, 500);
                        break;
                    case EVENT_SHADOW_BOLT:
                        DoCast(SPELL_SHADOW_BOLT);
                        events.ScheduleEvent(EVENT_SHADOW_BOLT, 3000);
                        break;
                    case EVENT_DARK_BLAST:
                        DoCast(SPELL_DARK_BLAST);
                        events.CancelEvent(EVENT_SHADOW_BOLT);
                        events.ScheduleEvent(EVENT_LATOSIUS_TP, 5000);
                        events.ScheduleEvent(EVENT_DARK_BLAST, 10000);
                        break;
                    case EVENT_LATOSIUS_TP_2:
                        DoCast(me, SPELL_TELEPORT_2, true);
                        DoCast(me, SPELL_CONVERSATION, true);
                        events.ScheduleEvent(EVENT_SAP_SOUL, 1000);
                        break;
                    case EVENT_SAP_SOUL:
                        DoCast(me, SPELL_SAP_SOUL, true);
                        break;
                    case EVENT_TRANSFORM: //22:39
                        me->UpdateEntry(NPC_DANTALIONAX);
                        secondPhase = true;
                        SetEquipmentSlots(false, 0, 0, 0);
                        DoCast(me, SPELL_TRANSFORM, true);
                        me->SetReactState(REACT_AGGRESSIVE);
                        events.ScheduleEvent(EVENT_SHADOW_BOLT_VOLLEY, 3000); //22:42, 22:56, 23:37
                        events.ScheduleEvent(EVENT_CLOUD_OF_HYPNOSIS, 13000); //22:52
                        events.ScheduleEvent(EVENT_DREADLORDS_GUILE, 28000); //23:08
                        if (GetDifficultyID() != DIFFICULTY_NORMAL)
                            events.ScheduleEvent(EVENT_STINGING_SWARM, 7000);
                        DoZoneInCombat(me, 100.0f);
                        break;
                    case EVENT_SHADOW_BOLT_VOLLEY:
                        DoCast(SPELL_SHADOW_BOLT_VOLLEY);
                        events.ScheduleEvent(EVENT_SHADOW_BOLT_VOLLEY, 14000);
                        break;
                    case EVENT_CLOUD_OF_HYPNOSIS:
                        if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f, true))
                            DoCast(pTarget, SPELL_CLOUD_OF_HYPNOSIS);
                        events.ScheduleEvent(EVENT_CLOUD_OF_HYPNOSIS, 32000);
                        break;
                    case EVENT_DREADLORDS_GUILE:
                        me->StopAttack();
                        imageSumCount = 0;
                        DoCast(SPELL_DREADLORDS_GUILE);
                        events.ScheduleEvent(EVENT_DREADLORDS_GUILE, 62000);
                        events.ScheduleEvent(EVENT_SUM_IMAGE, 4000);
                        break;
                    case EVENT_SUM_IMAGE:
                        me->SummonCreature(NPC_IMAGE_OF_LATOSIUS, tpPos[imageSumCount]);
                        imageSumCount++;
                        if (imageSumCount > 10)
                        {
                            events.ScheduleEvent(EVENT_IMAGE_END, 3000);
                            break;
                        }
                        events.ScheduleEvent(EVENT_SUM_IMAGE, 2000);
                        break;
                    case EVENT_IMAGE_END:
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1);
                        me->SetVisible(true);
                        me->RemoveAurasDueToSpell(SPELL_DREADLORDS_GUILE);
                        me->SetReactState(REACT_AGGRESSIVE);
                        break;
                    case EVENT_STINGING_SWARM:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 90.0f, true, -SPELL_STINGING_SWARM))
                            DoCast(target, SPELL_STINGING_SWARM);
                        events.ScheduleEvent(EVENT_STINGING_SWARM, 17000);
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_latosiusAI(creature);
    }
};

//98965
class npc_kurtalos_ravencrest : public CreatureScript
{
public:
    npc_kurtalos_ravencrest() : CreatureScript("npc_kurtalos_ravencrest") { }

    struct npc_kurtalos_ravencrestAI : public ScriptedAI
    {
        npc_kurtalos_ravencrestAI(Creature* creature) : ScriptedAI(creature) 
        {
            instance = me->GetInstanceScript();
        }

        InstanceScript* instance;
        EventMap events;
        bool secondPhase;

        void Reset() override
        {
            secondPhase = false;
        }

        void EnterCombat(Unit* /*who*/) override
        //22:08
        {
            Talk(SAY_AGGRO); //Fiends, you shall never have our world!

            if (Creature* latosius = me->FindNearestCreature(NPC_LATOSIUS, 30.0f))
                latosius->AI()->DoZoneInCombat(latosius, 100.0f);

            events.ScheduleEvent(EVENT_UNERRING_SHEAR, 6000);  //22:14
            events.ScheduleEvent(EVENT_WHIRLING_BLADE, 10000); //22:18
        }

        void EnterEvadeMode() override
        {
            if (instance->GetData(DATA_KURTALOS_STATE) != PHASE_1)
                instance->SetData(DATA_KURTALOS_STATE, PHASE_1);

            if (Creature* latosius = me->FindNearestCreature(NPC_LATOSIUS, 30.0f))
                latosius->AI()->EnterEvadeMode();

            ScriptedAI::EnterEvadeMode();
        }

        void JustDied(Unit* /*killer*/) override
        {
            me->SummonCreature(NPC_SOUL_KURTALOS, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
        }

        void SpellHitTarget(Unit* target, const SpellInfo* spell) override
        {
            if (spell->Id == SPELL_WHIRLING_BLADE)
                if (Creature* blade = me->SummonCreature(NPC_KURTALOS_BLADE_TRIGGER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ()))
                    blade->GetMotionMaster()->MovePoint(1, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ());
        }

        void SpellHit(Unit* caster, const SpellInfo* spell) override
        {
            if (spell->Id == SPELL_SAP_SOUL)
                me->m_Events.AddEvent(new DelayCastEvent(*me, me->GetGUID(), SPELL_SUICIDE, true), me->m_Events.CalculateTime(8000));
        }

        void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType dmgType) override
        {
            if (damage >= me->GetHealth() || me->HealthBelowPct(20))
            {
                if (!secondPhase)
                {
                    secondPhase = true;
                    me->StopAttack();
                    events.Reset();
                    if (Unit* owner = me->GetAnyOwner())
                        if (Creature* summoner = owner->ToCreature())
                            summoner->AI()->DoAction(ACTION_1);
                }
                if (attacker != me)
                    damage = 0;
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (CheckHomeDistToEvade(diff, 41.0f))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_UNERRING_SHEAR:
                        DoCastVictim(SPELL_UNERRING_SHEAR);
                        events.ScheduleEvent(EVENT_UNERRING_SHEAR, 13000);
                        break;
                    case EVENT_WHIRLING_BLADE:
                        if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f, true))
                            DoCast(pTarget, SPELL_WHIRLING_BLADE);
                        events.ScheduleEvent(EVENT_WHIRLING_BLADE, 10000);
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_kurtalos_ravencrestAI (creature);
    }
};

//101054 - Soul, 100861 - Blade Trigger, 101028 - Image of Latosius
class npc_kurtalos_trigger : public CreatureScript
{
public:
    npc_kurtalos_trigger() : CreatureScript("npc_kurtalos_trigger") {}

    struct npc_kurtalos_triggerAI : public ScriptedAI
    {
        npc_kurtalos_triggerAI(Creature* creature) : ScriptedAI(creature) 
        {
            instance = me->GetInstanceScript();
            me->setFaction(14);
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
        }

        InstanceScript* instance;
        EventMap events;

        void Reset() override {}

        void IsSummonedBy(Unit* summoner) override
        {
            if (instance->GetBossState(DATA_KURTALOS) != IN_PROGRESS)
            {
                me->DespawnOrUnsummon();
                return;
            }

            switch(me->GetEntry())
            {
                case NPC_KURTALOS_BLADE_TRIGGER:
                    DoCast(me, SPELL_WHIRLING_BLADE_AT, true);
                    break;
                case NPC_SOUL_KURTALOS:
                    me->setFaction(35);
                    DoCast(me, SPELL_KURTALOS_GHOST_VISUAL, true);
                    events.ScheduleEvent(EVENT_2, 10000);
                    break;
                case NPC_IMAGE_OF_LATOSIUS:
                    me->DespawnOrUnsummon(10000);
                    DoCast(SPELL_DARK_OBLITERATION);
                    break;
                default:
                    break;
            }
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (id == 1)
                events.ScheduleEvent(EVENT_1, 1000);

            if (id == 2)
                me->DespawnOrUnsummon();
        }

        void UpdateAI(uint32 diff) override
        {
            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_1:
                        if (me->GetAnyOwner())
                            if (Creature* summoner = me->GetAnyOwner()->ToCreature())
                                me->GetMotionMaster()->MovePoint(2, summoner->GetPositionX(), summoner->GetPositionY(), summoner->GetPositionZ());
                        break;
                    case EVENT_2:
                        if (instance->GetBossState(DATA_KURTALOS) == IN_PROGRESS)
                            me->CastSpell(me, SPELL_LEGACY_RAVENCREST, true);
                        me->DespawnOrUnsummon(2000);
                        break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_kurtalos_triggerAI(creature);
    }
};

//101008
class npc_kurtalos_stinging_swarm : public CreatureScript
{
public:
    npc_kurtalos_stinging_swarm() : CreatureScript("npc_kurtalos_stinging_swarm") {}

    struct npc_kurtalos_stinging_swarmAI : public ScriptedAI
    {
        npc_kurtalos_stinging_swarmAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = me->GetInstanceScript();
            me->SetReactState(REACT_PASSIVE);
        }

        InstanceScript* instance;
        uint16 checkTimer;

        void Reset() override
        {
            checkTimer = 2000;
        }

        void JustDied(Unit* /*killer*/) override
        {
            if (me->ToTempSummon())
                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                    summoner->RemoveAurasDueToSpell(SPELL_STINGING_SWARM);
        }

        void UpdateAI(uint32 diff) override
        {
            if (checkTimer)
            {
                if (checkTimer <= diff)
                {
                    if (instance->GetBossState(DATA_KURTALOS) != IN_PROGRESS)
                    {
                        me->DespawnOrUnsummon();
                        return;
                    }
                    checkTimer = 2000;
                }
                else
                    checkTimer -= diff;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_kurtalos_stinging_swarmAI(creature);
    }
};

//198835
class spell_latosius_random_teleport : public SpellScriptLoader
{
public:
    spell_latosius_random_teleport() : SpellScriptLoader("spell_latosius_random_teleport") { }

    class spell_latosius_random_teleport_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_latosius_random_teleport_SpellScript);

        uint8 rand;

        void HandleScriptEffect(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);
            
            Unit* caster = GetCaster();
            if (!caster)
                return;

            rand = urand(0,11);
            caster->NearTeleportTo(tpPos[rand].GetPositionX(), tpPos[rand].GetPositionY(), tpPos[rand].GetPositionZ(), tpPos[rand].GetOrientation());
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_latosius_random_teleport_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_TELEPORT_L);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_latosius_random_teleport_SpellScript();
    }
};

void AddSC_boss_lord_kurtalos_ravencrest()
{
    new boss_latosius();
    new npc_kurtalos_ravencrest();
    new npc_kurtalos_trigger();
    new npc_kurtalos_stinging_swarm();
    new spell_latosius_random_teleport();
}