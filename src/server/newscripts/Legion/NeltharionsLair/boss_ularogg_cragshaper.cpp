/*
    http://uwow.biz
    Dungeon : Neltharions Lair 100-110
    Encounter: Ularogg Cragshaper
    Normal: 100%, Heroic: 100%, Mythic: 100%
*/

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "neltharions_lair.h"

enum Says
{
    SAY_AGGRO           = 0,
    SAY_STANCE_EMOTE    = 1,
    SAY_DEATH           = 2,
};

enum Spells
{
    SPELL_SUNDER                    = 198496,
    SPELL_SUNDER_CALL               = 198823, //Conversation
    SPELL_STRIKE_MOUNTAIN           = 198428,
    SPELL_STRIKE_MOUNTAIN_2         = 216290,
    SPELL_STRIKE_MOUNTAIN_AT        = 216292,
    SPELL_MOUNTAIN_CALL             = 198825, //Conversation
    SPELL_BELLOW_DEEPS_1            = 193375,
    SPELL_BELLOW_DEEPS_2            = 193376,
    SPELL_BELLOW_DEEPS_CALL         = 198824,
    //SPELL_CALL                    = 198826, //Conversation
    SPELL_STANCE_MOUNTAIN_JUMP      = 198509,
    SPELL_STANCE_MOUNTAIN_SUM_1     = 198564,
    SPELL_STANCE_MOUNTAIN_SUM_2     = 198565,
    SPELL_STANCE_MOUNTAIN_SUM_3     = 216249, //Heroic+
    SPELL_STANCE_MOUNTAIN_SUM_4     = 216250, //Heroic+
    SPELL_STANCE_MOUNTAIN_MORPH     = 198510,
    SPELL_STANCE_MOUNTAIN_TICK      = 198617,
    SPELL_STANCE_MOUNTAIN_FILTER    = 198619,
    SPELL_STANCE_MOUNTAIN_TELEPORT  = 198630,
    SPELL_FALLING_DEBRIS_ULAROGG    = 198719, //boss
    SPELL_STANCE_MOUNTAIN_END       = 198631,

    SPELL_FALLING_DEBRIS            = 193267, //npc 98081
    SPELL_FALLING_DEBRIS_2          = 198717, //npc 100818
    SPELL_STANCE_MOUNTAIN_MOVE      = 198616,
};

enum eEvents
{
    EVENT_SUNDER                = 1,
    EVENT_STRIKE_MOUNTAIN       = 2,
    EVENT_BELLOW_DEEPS_1        = 3,
    EVENT_BELLOW_DEEPS_2        = 4,
    EVENT_STANCE_MOUNTAIN_1     = 5,
    EVENT_STANCE_MOUNTAIN_2     = 6,
    EVENT_STANCE_MOUNTAIN_3     = 7,
    EVENT_STANCE_MOUNTAIN_4     = 8
};

Position const trashPos[4] =
{
    {2864.54f, 1703.48f, -40.72f, 3.35f}, //90997
    {2868.39f, 1655.84f, -40.74f, 4.51f}, //90998
    {2861.02f, 1696.18f, -40.74f, 1.39f}, //91008
    {2854.72f, 1706.34f, -40.74f, 5.67f}  //91008
};

//91004
class boss_ularogg_cragshaper : public CreatureScript
{
public:
    boss_ularogg_cragshaper() : CreatureScript("boss_ularogg_cragshaper") { }

    struct boss_ularogg_cragshaperAI : public BossAI
    {
        boss_ularogg_cragshaperAI(Creature* creature) : BossAI(creature, DATA_ULAROGG)
        {
            IntroSpawn();
        }

        ObjectGuid stanceGUID;
        uint8 eventDiesCount = 0;

        void Reset() override
        {
            _Reset();

            me->SetReactState(REACT_DEFENSIVE);

            if (eventDiesCount)
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                me->RemoveAurasDueToSpell(SPELL_STANCE_MOUNTAIN_MORPH);
                me->RemoveAurasDueToSpell(SPELL_STANCE_MOUNTAIN_TICK);
            }
        }

        void IntroSpawn()
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
            me->SummonCreature(NPC_MIGHTSTONE_BREAKER, trashPos[0]);
            me->SummonCreature(NPC_BLIGHTSHARD_SHAPER, trashPos[1]);
            me->SummonCreature(NPC_ROCKBOUND_PELTER,   trashPos[2]);
            me->SummonCreature(NPC_ROCKBOUND_PELTER,   trashPos[3]);
        }

        void EnterCombat(Unit* /*who*/) override
        {
            Talk(SAY_AGGRO); //Pay attention, Navarogg. I want you to see your heroes die.
            _EnterCombat();
            DefaultEvent(true);
        }

        void DefaultEvent(bool enterCombat)
        {
            events.ScheduleEvent(EVENT_SUNDER, 8000);
            events.ScheduleEvent(EVENT_STRIKE_MOUNTAIN, 16000);
            events.ScheduleEvent(EVENT_BELLOW_DEEPS_1, 20000);
            events.ScheduleEvent(EVENT_STANCE_MOUNTAIN_1, enterCombat ? 50000 : 120000);
        }

        void JustDied(Unit* /*killer*/) override
        {
            Talk(SAY_DEATH);
            _JustDied();
        }

        void SpellFinishCast(const SpellInfo* spell)
        {
            switch (spell->Id)
            {
                case SPELL_STANCE_MOUNTAIN_SUM_1:
                    DoCast(SPELL_STANCE_MOUNTAIN_SUM_2);
                    break;
                case SPELL_STANCE_MOUNTAIN_SUM_2:
                    if (GetDifficultyID() == DIFFICULTY_NORMAL)
                        events.ScheduleEvent(EVENT_STANCE_MOUNTAIN_2, 2000);
                    else
                        DoCast(SPELL_STANCE_MOUNTAIN_SUM_3);
                    break;
                case SPELL_STANCE_MOUNTAIN_SUM_3:
                    DoCast(SPELL_STANCE_MOUNTAIN_SUM_4);
                    break;
                case SPELL_STANCE_MOUNTAIN_SUM_4:
                    events.ScheduleEvent(EVENT_STANCE_MOUNTAIN_2, 2000);
                    break;
            }
        }

        void SpellHitTarget(Unit* target, const SpellInfo* spell) override
        {
            switch (spell->Id)
            {
                case SPELL_STANCE_MOUNTAIN_FILTER:
                {
                    Position pos;
                    me->GetRandomNearPosition(pos, 30.0f);
                    target->CastSpell(pos, SPELL_STANCE_MOUNTAIN_MOVE, true);
                    break;
                }
                case SPELL_STRIKE_MOUNTAIN_2:
                {
                    Position pos;
                    float angle = M_PI;
                    for (uint8 i = 0; i < 4; ++i)
                    {
                        target->GetNearPosition(pos, 5.0f, angle);
                        target->CastSpell(pos, SPELL_STRIKE_MOUNTAIN_AT, true);
                        angle += M_PI / 2.0f;
                    }
                    break;
                }
            }
        }

        void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
        {
            if (summon->GetGUID() == stanceGUID)
            {
                me->CastSpell(summon->GetPosition(), SPELL_STANCE_MOUNTAIN_TELEPORT, true);
                me->RemoveAurasDueToSpell(SPELL_STANCE_MOUNTAIN_MORPH);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                me->SetReactState(REACT_AGGRESSIVE);
                DoCast(me, SPELL_STANCE_MOUNTAIN_END, true);
                DefaultEvent(false);
            }

            if (eventDiesCount > 3)
                return;

            switch (summon->GetEntry())
            {
                case NPC_MIGHTSTONE_BREAKER:
                case NPC_BLIGHTSHARD_SHAPER:
                case NPC_ROCKBOUND_PELTER:
                    eventDiesCount++;
                    if (eventDiesCount == 4)
                    {
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                        me->SetHomePosition(2838.71f, 1668.18f, -40.64f, 3.75f);
                        me->GetMotionMaster()->MoveJump(2838.71f, 1668.18f, -40.64f, 30.0f, 10.0f);
                    }
                    break;
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (CheckHomeDistToEvade(diff, 40.0f))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_SUNDER:
                        if (me->getVictim())
                        {
                            DoCast(me, SPELL_SUNDER_CALL, true);
                            DoCastVictim(SPELL_SUNDER);
                        }
                        events.ScheduleEvent(EVENT_SUNDER, 10000);
                        break;
                    case EVENT_STRIKE_MOUNTAIN:
                        DoCast(SPELL_STRIKE_MOUNTAIN);        
                        events.ScheduleEvent(EVENT_STRIKE_MOUNTAIN, 16000);
                        break;
                    case EVENT_BELLOW_DEEPS_1:
                        DoCast(SPELL_BELLOW_DEEPS_1);
                        events.ScheduleEvent(EVENT_BELLOW_DEEPS_1, 32000);
                        events.ScheduleEvent(EVENT_BELLOW_DEEPS_2, 3000);
                        break;
                    case EVENT_BELLOW_DEEPS_2:
                        DoCast(me, SPELL_BELLOW_DEEPS_CALL, true);
                        DoCast(SPELL_BELLOW_DEEPS_2);
                        break;
                    case EVENT_STANCE_MOUNTAIN_1:
                        events.Reset();
                        me->StopAttack();
                        me->GetMotionMaster()->Clear();
                        me->CastSpell(me, SPELL_STANCE_MOUNTAIN_JUMP, TriggerCastFlags(TRIGGERED_IGNORE_POWER_AND_REAGENT_COST));
                        Talk(SAY_STANCE_EMOTE);
                        break;
                    case EVENT_STANCE_MOUNTAIN_2:
                        me->RemoveAurasAllDots();
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                        DoCast(SPELL_STANCE_MOUNTAIN_MORPH);
                        if (Creature* summon = me->SummonCreature(NPC_BELLOWING_IDOL_2, me->GetPosition()))
                            stanceGUID = summon->GetGUID();
                        events.ScheduleEvent(EVENT_STANCE_MOUNTAIN_3, 2000);
                        break;
                    case EVENT_STANCE_MOUNTAIN_3:
                        me->AddAura(SPELL_STANCE_MOUNTAIN_TICK, me);
                        events.ScheduleEvent(EVENT_STANCE_MOUNTAIN_4, 10000);
                        break;
                    case EVENT_STANCE_MOUNTAIN_4:
                    {
                        EntryCheckPredicate pred(NPC_BELLOWING_IDOL_2);
                        summons.DoAction(ACTION_1, pred);
                        me->AddAura(SPELL_FALLING_DEBRIS_ULAROGG, me);
                        break;
                    }
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_ularogg_cragshaperAI (creature);
    }
};

//98081, 100818
class npc_ularogg_bellowing_idols : public CreatureScript
{
public:
    npc_ularogg_bellowing_idols() : CreatureScript("npc_ularogg_bellowing_idols") {}

    struct npc_ularogg_bellowing_idolsAI : public ScriptedAI
    {
        npc_ularogg_bellowing_idolsAI(Creature* creature) : ScriptedAI(creature) 
        {
            me->SetReactState(REACT_PASSIVE);
        }

        void Reset() override {}

        void IsSummonedBy(Unit* summoner) override
        {
            if (me->GetEntry() == NPC_BELLOWING_IDOL)
                DoCast(me, SPELL_FALLING_DEBRIS, true);
            else
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                DoCast(me, 198569, true); //Visual Spawn
            }
        }

        void DoAction(int32 const action) override
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
            DoCast(me, SPELL_FALLING_DEBRIS_2, true);
        }

        void UpdateAI(uint32 diff) override {}
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_ularogg_bellowing_idolsAI(creature);
    }
};

//trash 102228
class npc_stonedark_slave : public CreatureScript
{
public:
    npc_stonedark_slave() : CreatureScript("npc_stonedark_slave") {}

    struct npc_stonedark_slaveAI : public ScriptedAI
    {
        npc_stonedark_slaveAI(Creature* creature) : ScriptedAI(creature) 
        {
            _endBarrel = false;
            _intro = false;
        }

        bool _endBarrel;
        bool _intro;

        void MoveInLineOfSight(Unit* who) override
        {  
            if (who->GetTypeId() != TYPEID_PLAYER)
                return;

            if (!_endBarrel && me->IsWithinDistInMap(who, 90.0f))
            {
                // who->CastSpell(who, 209531, true);  пока офф до 7.0.3, крашит
                _endBarrel = true;
            }            
            if (!_intro && me->IsWithinDistInMap(who, 40.0f))
            {
                who->CastSpell(who, 209536, true);
                _intro = true;
            }
        }

    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_stonedark_slaveAI(creature);
    }
};

//92473
class npc_ularogg_empty_barrel : public CreatureScript
{
public:
    npc_ularogg_empty_barrel() : CreatureScript("npc_ularogg_empty_barrel") {}

    struct npc_ularogg_empty_barrelAI : public ScriptedAI
    {
        npc_ularogg_empty_barrelAI(Creature* creature) : ScriptedAI(creature) 
        {
            me->SetReactState(REACT_PASSIVE);
        }

        void Reset() override {}

        void OnSpellClick(Unit* clicker) override
        {
            //Start WP player
            if (!clicker->HasAura(183213))
                clicker->CastSpell(clicker, 183213, true);
        }

        void UpdateAI(uint32 diff) override {}
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_ularogg_empty_barrelAI(creature);
    }
};

//183213
class spell_barrel_ride_plr_move : public SpellScriptLoader
{
    public:
        spell_barrel_ride_plr_move() : SpellScriptLoader("spell_barrel_ride_plr_move") {}

        class spell_barrel_ride_plr_move_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_barrel_ride_plr_move_AuraScript);

            void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (Player* player = GetTarget()->ToPlayer())
                {
                    player->GetMotionMaster()->MoveIdle();
                    player->GetMotionMaster()->MovePath(9100400, false);
                }
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_barrel_ride_plr_move_AuraScript::OnApply, EFFECT_2, SPELL_AURA_MOD_NO_ACTIONS, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_barrel_ride_plr_move_AuraScript();
        }
};

// trash 183088
class spell_avalanche : public SpellScriptLoader
{
    public:
        spell_avalanche() : SpellScriptLoader("spell_avalanche") { }

        class spell_avalanche_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_avalanche_SpellScript);
            
            void HandleScript(SpellEffIndex effIndex)
            {
                Unit* caster = GetCaster();
                if (!caster)
                    return;
                std::list<Player*> targets;
                GetPlayerListInGrid(targets, caster, 40);
                Trinity::Containers::RandomResizeList(targets, 2);
                for (std::list<Player*>::iterator itr = targets.begin(); itr != targets.end(); ++itr)
                    if (!targets.empty())
                        caster->CastSpell((*itr), 183095, true);                 
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_avalanche_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_avalanche_SpellScript();
        }
};

void AddSC_boss_ularogg_cragshaper()
{
    new boss_ularogg_cragshaper();
    new npc_ularogg_bellowing_idols();
    new npc_stonedark_slave();
    new npc_ularogg_empty_barrel();
    new spell_barrel_ride_plr_move();
    new spell_avalanche();
}