/*
    http://uwow.biz
    Dungeon : Black Rook Hold Dungeon 100-110
    Encounter: Illysanna Ravencrest
    Normal: 100%, Heroic: 100%, Mythic: 100%
*/

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "black_rook_hold_dungeon.h"
#include "Packets/UpdatePackets.h"

/* enum Says
{
    SAY_AGGRO           = ,
    SAY_DEATH           = ,
}; */

enum Spells
{
    SPELL_FURY_POWER_OVERRIDE   = 197367,
    SPELL_REGEN_POWER           = 197394,
    SPELL_BRUTAL_GLAIVE         = 197546,
    SPELL_VENGEFUL_SHEAR        = 197418,
    SPELL_DARK_RUSH_FILTER      = 197478,
    SPELL_DARK_RUSH_CHARGE      = 197484,
    SPELL_PHASE_2_JUMP          = 197622,
    SPELL_EYE_BEAMS             = 197674,
    SPELL_EYE_BEAMS_AURA        = 197696,
    SPELL_EYE_BEAMS_AT          = 197703,
    SPELL_FIXATE_BEAM           = 197687,
};

enum eEvents
{
    EVENT_BRUTAL_GLAIVE         = 1,
    EVENT_VENGEFUL_SHEAR        = 2,
    EVENT_DARK_RUSH_1           = 3,
    EVENT_DARK_RUSH_2           = 4,
    EVENT_SUMMON_ADDS           = 5,
    EVENT_EYE_BEAMS             = 6,
    EVENT_PHASE_1               = 7,
};

Position const summonsPos[2] =
{
    {3105.11f, 7296.94f, 103.28f, 3.14f}, //NPC_RISEN_ARCANIST
    {3083.07f, 7313.35f, 103.28f, 4.92f}  //NPC_SOUL_TORN_VANGUARD
};

//98696
class boss_illysanna_ravencrest : public CreatureScript
{
public:
    boss_illysanna_ravencrest() : CreatureScript("boss_illysanna_ravencrest") {}

    struct boss_illysanna_ravencrestAI : public BossAI
    {
        boss_illysanna_ravencrestAI(Creature* creature) : BossAI(creature, DATA_ILLYSANNA) {}

        bool introDone = false;
        bool glaiveDubleCast;
        bool phaseTwo;

        void Reset() override
        {
            _Reset();
            glaiveDubleCast = true;
            me->SetPower(POWER_ENERGY, 100);
            DoCast(me, SPELL_FURY_POWER_OVERRIDE, true);
            DoCast(me, SPELL_REGEN_POWER, true);
            me->SetReactState(REACT_AGGRESSIVE);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            phaseTwo = false;
            SetAnim(0);
            SetFlyMode(false);
        }

        void EnterCombat(Unit* /*who*/) override
        {
            _EnterCombat();
            DefaultEvents();
        }

        void JustDied(Unit* /*killer*/) override
        {
            _JustDied();
            instance->DoUpdateAchievementCriteria(CRITERIA_TYPE_COMPLETE_DUNGEON_ENCOUNTER, 1833);
        }

        void DefaultEvents()
        {
            events.ScheduleEvent(EVENT_BRUTAL_GLAIVE, 6000);  //07:41, 07:57
            events.ScheduleEvent(EVENT_VENGEFUL_SHEAR, 9000); //07:44, 07:59, 08:11
            events.ScheduleEvent(EVENT_DARK_RUSH_1, 12000);   //07:47, 08:18
        }

        void SetAnim(uint8 Id)
        {
            WorldPackets::Update::SetAnimTimer anim;
            anim.Tier = Id;
            anim.Unit = me->GetGUID();
            me->SendMessageToSet(anim.Write(), true);
        }

        void MoveInLineOfSight(Unit* who) override
        {  
            if (who->GetTypeId() != TYPEID_PLAYER)
                return;

            if (!introDone && me->IsWithinDistInMap(who, 57.0f))
            {
                if (Creature* cap = me->FindNearestCreature(98706, 100.0f, true))
                    cap->AI()->DoAction(true);

                introDone = true;
            }
        }

        void SpellHitTarget(Unit* target, const SpellInfo* spell) override
        {
            switch (spell->Id)
            {
                case SPELL_VENGEFUL_SHEAR:
                    if (!target->IsActiveMitigation())
                        DoCast(target, 197429, true); //debuff: mod damage
                    break;
                case SPELL_DARK_RUSH_CHARGE:
                    me->GetMotionMaster()->MoveCharge(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 50.0f, SPELL_DARK_RUSH_CHARGE);
                    break;
                case SPELL_EYE_BEAMS:
                {
                    float x = target->GetPositionX();
                    float y = target->GetPositionY();
                    float z = target->GetPositionZ();
                    if (Creature* stalker = me->SummonCreature(NPC_EYE_BEAM_STALKER, x, y, z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 12000))
                        DoCast(stalker, SPELL_EYE_BEAMS_AURA, true);
                    break;
                }
                default:
                    break;
            }
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type == EFFECT_MOTION_TYPE)
            {
                if (id == SPELL_PHASE_2_JUMP)
                {
                    me->GetMotionMaster()->Clear(false);
                    me->SetFacingTo(4.05f);
                    SetAnim(3);
                    events.ScheduleEvent(EVENT_SUMMON_ADDS, 1000);
                    events.ScheduleEvent(EVENT_EYE_BEAMS, 2000);
                }
                if (id == 1)
                {
                    SetFlyMode(false);
                    SetAnim(0);
                    me->SetReactState(REACT_AGGRESSIVE);
                    DoCast(me, SPELL_REGEN_POWER, true);
                    phaseTwo = false;
                }
            }

            if (type == POINT_MOTION_TYPE)
            {
                if (id == SPELL_DARK_RUSH_CHARGE)
                    events.ScheduleEvent(EVENT_DARK_RUSH_2, 500);
            }
        }

        void DoAction(int32 const action) override
        {
            if (!phaseTwo)
            {
                phaseTwo = true;
                events.Reset();
                me->StopAttack();
                me->RemoveAurasDueToSpell(SPELL_REGEN_POWER);
                SetFlyMode(true);
                DoCast(me, SPELL_PHASE_2_JUMP, true);
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (CheckHomeDistToEvade(diff, 21.0f))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_BRUTAL_GLAIVE:
                        if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 60.0f, true, -SPELL_BRUTAL_GLAIVE))
                            DoCast(pTarget, SPELL_BRUTAL_GLAIVE);
                        if (glaiveDubleCast)
                            glaiveDubleCast = false;
                        else
                            glaiveDubleCast = true;
                        events.ScheduleEvent(EVENT_BRUTAL_GLAIVE, !glaiveDubleCast ? 0 : 16000);
                        break;
                    case EVENT_VENGEFUL_SHEAR:
                        DoCastVictim(SPELL_VENGEFUL_SHEAR);
                        events.ScheduleEvent(EVENT_VENGEFUL_SHEAR, 12000);
                        break;
                    case EVENT_DARK_RUSH_1:
                        DoCast(SPELL_DARK_RUSH_FILTER);
                        events.ScheduleEvent(EVENT_DARK_RUSH_1, 30000);
                        events.ScheduleEvent(EVENT_DARK_RUSH_2, 3000);
                        break;
                    case EVENT_DARK_RUSH_2:
                        DoCast(me, SPELL_DARK_RUSH_CHARGE, true);
                        break;
                    case EVENT_SUMMON_ADDS:
                        if (GetDifficultyID() != DIFFICULTY_NORMAL)
                            me->SummonCreature(NPC_RISEN_ARCANIST, summonsPos[0]);
                        me->SummonCreature(NPC_SOUL_TORN_VANGUARD, summonsPos[1]);
                        break;
                    case EVENT_EYE_BEAMS:
                        me->SetPower(POWER_ENERGY, me->GetPower(POWER_ENERGY) - 33);
                        DoCast(SPELL_EYE_BEAMS);
                        if (me->GetPower(POWER_ENERGY) <= 0)
                            events.ScheduleEvent(EVENT_PHASE_1, 14000);
                        else
                            events.ScheduleEvent(EVENT_EYE_BEAMS, 12000);
                        break;
                    case EVENT_PHASE_1:
                        DefaultEvents();
                        DoCast(me, SPELL_FURY_POWER_OVERRIDE, true);
                        me->GetMotionMaster()->MoveJump(3089.76f, 7299.66f, 103.53f, 25, 15, 1); //HomePos
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_illysanna_ravencrestAI (creature);
    }
};

//100436
class npc_illysanna_eye_beam_stalker : public CreatureScript
{
public:
    npc_illysanna_eye_beam_stalker() : CreatureScript("npc_illysanna_eye_beam_stalker") {}

    struct npc_illysanna_eye_beam_stalkerAI : public ScriptedAI
    {
        npc_illysanna_eye_beam_stalkerAI(Creature* creature) : ScriptedAI(creature) 
        {
            me->SetReactState(REACT_PASSIVE);
        }

        EventMap events;
        ObjectGuid targetGuid;

        void Reset() override {}

        void IsSummonedBy(Unit* summoner) override
        {
            events.ScheduleEvent(EVENT_1, 500);
            events.ScheduleEvent(EVENT_2, 1000);
        }

        void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType dmgType) override
        {
            damage = 0;
        }

        void UpdateAI(uint32 diff) override
        {
            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_1:
                    {
                    if (Unit* owner = me->GetAnyOwner())
                        if (Creature* summoner = owner->ToCreature())
                            if (Unit* target = summoner->AI()->SelectTarget(SELECT_TARGET_RANDOM, 0, 60.0f, true))
                            {
                                targetGuid = target->GetGUID();
                                me->CastSpell(target, SPELL_FIXATE_BEAM, true);
                            }
                        events.ScheduleEvent(EVENT_1, 6000);
                        break;
                    }
                    case EVENT_2:
                        if (Unit* plrTarget = ObjectAccessor::GetUnit(*me, targetGuid))
                            me->GetMotionMaster()->MovePoint(1, plrTarget->GetPositionX(), plrTarget->GetPositionY(), plrTarget->GetPositionZ());
                        events.ScheduleEvent(EVENT_2, 500);
                        break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_illysanna_eye_beam_stalkerAI(creature);
    }
};

// commandir 98706
class npc_illysanna_commandir : public CreatureScript
{
public:
    npc_illysanna_commandir() : CreatureScript("npc_illysanna_commandir") {}

    struct npc_illysanna_commandirAI : public ScriptedAI
    {
        npc_illysanna_commandirAI(Creature* creature) : ScriptedAI(creature) 
        {
            me->SetReactState(REACT_PASSIVE);
            instance = me->GetInstanceScript();
            if (me->isAlive())
                instance->SetData(DATA_ILLYSANNA_INTRO, IN_PROGRESS);
        }

        EventMap events;
        InstanceScript* instance;
        bool wait;
        std::list<Creature*> trashList;

        void Reset() override
        {
            wait = false;
        }

        void DoAction(int32 const action) override
        {
            events.ScheduleEvent(EVENT_1, 9000);
        }

        void EnterCombat(Unit* /*who*/) override
        //07:35
        {
            events.ScheduleEvent(EVENT_3, 16000);
        }        
        
        void JustDied(Unit* /*killer*/) override
        {
            instance->SetData(DATA_ILLYSANNA_INTRO, DONE);
        }
        
        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim() && wait)
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_1:
                       Talk(0);
                       events.ScheduleEvent(EVENT_2, 3000);
                       break;
                    case EVENT_2:
                        me->SetReactState(REACT_AGGRESSIVE);
                        GetCreatureListWithEntryInGrid(trashList, me, 98275, 70.0f);
                        GetCreatureListWithEntryInGrid(trashList, me, 98691, 70.0f);
                        if (Unit* target = me->SelectNearestPlayer(50.0f))
                        {
                           for (std::list<Creature*>::iterator itr = trashList.begin(); itr != trashList.end(); ++itr)
                              if ((*itr))    
                                 (*itr)->AI()->AttackStart(target);
                           me->AI()->AttackStart(target);                              
                        }
                        wait = true;
                        break;   
                    case EVENT_3:
                        DoCast(200261);
                        events.ScheduleEvent(EVENT_3, 16000);
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_illysanna_commandirAI(creature);
    }
};

//197394
class spell_illysanna_periodic_energize : public SpellScriptLoader
{
public:
    spell_illysanna_periodic_energize() : SpellScriptLoader("spell_illysanna_periodic_energize") { }

    class spell_illysanna_periodic_energize_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_illysanna_periodic_energize_AuraScript);

        uint8 PowerCount = 0;

        void OnTick(AuraEffect const* aurEff)
        {
            Creature* caster = GetCaster()->ToCreature();
            if (!caster || !caster->isInCombat())
                return;

            PowerCount = caster->GetPower(POWER_ENERGY);

            if (PowerCount < 100)
                caster->SetPower(POWER_ENERGY, PowerCount + 1);
            else
                caster->AI()->DoAction(true);
        }

        void Register() override
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_illysanna_periodic_energize_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_illysanna_periodic_energize_AuraScript();
    }
};

//197696
class spell_illysanna_eye_beams : public SpellScriptLoader
{
public:
    spell_illysanna_eye_beams() : SpellScriptLoader("spell_illysanna_eye_beams") { }

    class spell_illysanna_eye_beams_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_illysanna_eye_beams_AuraScript);

        void OnTick(AuraEffect const* aurEff)
        {
            Unit* target = GetTarget();
            if (!target)
                return;

            target->CastSpell(target, SPELL_EYE_BEAMS_AT, true);
        }

        void Register() override
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_illysanna_eye_beams_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_illysanna_eye_beams_AuraScript();
    }
};

void AddSC_boss_illysanna_ravencrest()
{
    new boss_illysanna_ravencrest();
    new npc_illysanna_eye_beam_stalker();
    new npc_illysanna_commandir();
    new spell_illysanna_periodic_energize();
    new spell_illysanna_eye_beams();
}