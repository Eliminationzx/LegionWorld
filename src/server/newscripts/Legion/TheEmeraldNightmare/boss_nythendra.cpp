/*
    https://uwow.biz/
*/

#include "the_emerald_nightmare.h"

enum Says
{
    SAY_AGGRO           = 0,
    SAY_ROT             = 1,
    SAY_BREATH_EMOTE    = 2,
    SAY_BREATH          = 3,
    SAY_PHASE           = 4,
    SAY_DEATH           = 5,
};

enum Spells
{
    SPELL_ENERGIZE                  = 220952,
    SPELL_ALLOW_ROT_CAST            = 204755,
    SPELL_HEART_SWARM               = 203552,
    SPELL_ROT_FILTER                = 203095,
    SPELL_ROT_DOT                   = 203096,
    SPELL_VOLATILE_ROT              = 204463,
    SPELL_INFESTED_GROUND_AT        = 204377,
    SPELL_INFESTED_GROUND_VIS       = 204476,
    SPELL_INFESTED_BREATH_FILTER    = 202968,
    SPELL_INFESTED_BREATH_CHANNEL   = 202977,
    SPELL_INFESTED_BREATH_DMG       = 202978,
    SPELL_TRANSFORM_BUGS            = 204894,

    //Heroic
    SPELL_TARGETING_AURA            = 220189,
    SPELL_INFESTED                  = 204504,

    //Mythic
    SPELL_INFESTED_MIND_FILTER      = 225943,
    SPELL_INFESTED_MIND             = 205043,
    SPELL_SPREAD_INFESTATION        = 205070,

//!Trash
    //Gelatinized Decay - 111004
    SPELL_TRANSFORM_INSECT_AURA     = 220978,
    SPELL_TRANSFORM_INSECT          = 220976,
    SPELL_WAVE_OF_DECAY             = 221059,
    SPELL_UNSTABLE_DECAY            = 221030,
    SPELL_PUTRID_SWARM              = 221033, //Aura summons Vermin

    //Corrupted Vermin - 111005
    SPELL_HEART_SWARM_SCALE         = 203647,
    SPELL_PUTRID_SWARM_DMG          = 221036,

    //Corrupted Vermin - 102998
    SPELL_SCALE                     = 196285,
    SPELL_BURST_OF_CORRUPTION       = 203646,
};

enum eEvents
{
    EVENT_CHECK_POWER           = 1,
    EVENT_ALLOW_ROT_CAST        = 2,
    EVENT_ROT                   = 3,
    EVENT_VOLATILE_ROT          = 4,
    EVENT_INFESTED_BREATH       = 5,
    EVENT_HEART_SWARM_END       = 6,
    EVENT_INFESTED_MIND         = 7, //Mythic
};

enum ePhases
{
    PHASE_BATTLE                = 1,
    PHASE_SWARM                 = 2
};

enum eOther
{
    DATA_SWARM_COUNTER          = 1,
};

Position const decayPos[3] =
{
    {1910.29f, 1332.19f, 369.50f},
    {1866.0f,  1332.56f, 369.50f},
    {1851.89f, 1295.29f, 369.50f}
};

//102672
class boss_nythendra : public CreatureScript
{
public:
    boss_nythendra() : CreatureScript("boss_nythendra") {}

    struct boss_nythendraAI : public BossAI
    {
        boss_nythendraAI(Creature* creature) : BossAI(creature, DATA_NYTHENDRA)
        {
            intro = true;
        }

        bool intro;
        uint8 swarmCount;
        uint8 rotCastCount;
        uint8 decayDiedCount;

        void Intro()
        {
            intro = false;
            decayDiedCount = 0;
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
            me->SetStandState(UNIT_STAND_STATE_SLEEP);

            for (uint8 i = 0; i < 3; i++)
                me->SummonCreature(NPC_GELATINIZED_DECAY, decayPos[i]);
        }

        void Reset() override
        {
            _Reset();
            RemoveBugs();
            me->SetPower(POWER_ENERGY, 100);

            if (intro)
                Intro();
            else
                me->SetReactState(REACT_DEFENSIVE);

            uint8 playerCount = me->GetMap()->GetPlayersCountExceptGMs();
            if (playerCount < 15)
                swarmCount = 1;
            else if (playerCount >= 15 && playerCount < 25)
                swarmCount = 2;
            else
                swarmCount = 3;
        }

        void EnterCombat(Unit* /*who*/) override
        //12:45(N)
        {
            Talk(SAY_AGGRO);
            _EnterCombat();

            events.ScheduleEvent(EVENT_CHECK_POWER, 5000);
            events.ScheduleEvent(EVENT_ALLOW_ROT_CAST, 1000);
            DefaultEvents(true);

            Map::PlayerList const& players = me->GetMap()->GetPlayers();
            for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                if (Player* player = itr->getSource())
                    if (!player->HasAura(SPELL_TARGETING_AURA))
                        player->CastSpell(player, SPELL_TARGETING_AURA, true);
        }

        void DefaultEvents(bool enterCombat)
        {
            events.ScheduleEvent(EVENT_ROT, enterCombat ? 6000 : 14000);
            events.ScheduleEvent(EVENT_VOLATILE_ROT, enterCombat ? 24000 : 35000);
            events.ScheduleEvent(EVENT_INFESTED_BREATH, (enterCombat ? urand(35, 40) : 47) * IN_MILLISECONDS);
        }

        void JustDied(Unit* /*killer*/) override
        {
            _JustDied();
            RemoveBugs();
            Talk(SAY_DEATH);
            Conversation* conversation = new Conversation;
            if (!conversation->CreateConversation(sObjectMgr->GetGenerator<HighGuid::Conversation>()->Generate(), 3601, me, nullptr, me->GetPosition()))
                delete conversation;
        }

        void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
        {
            if (summon->GetEntry() == NPC_GELATINIZED_DECAY)
            {
                decayDiedCount++;
                if (decayDiedCount >= 3)
                {
                    me->SetReactState(REACT_DEFENSIVE);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                    me->SetStandState(UNIT_STAND_STATE_STAND);
                }
            }
        }

        void SpellHitTarget(Unit* target, const SpellInfo* spell) override
        {
            switch (spell->Id)
            {
                case SPELL_INFESTED_BREATH_FILTER:
                {
                    Position pos;
                    me->GetNearPosition(pos, 10.0f, me->GetRelativeAngle(target));
                    if (Creature* stalker = me->SummonCreature(NPC_BREATH_STALKER, pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 10000))
                    {
                        for (uint8 i = 0; i < 5; i++)
                            me->m_Events.AddEvent(new DelayCastEvent(*me, me->GetGUID(), SPELL_INFESTED_BREATH_DMG, true), me->m_Events.CalculateTime((i + 3) * 1000));
                        me->SetFacingToObject(stalker);
                        DoCast(stalker, SPELL_INFESTED_BREATH_CHANNEL);
                    }
                    break;
                }
                case SPELL_TRANSFORM_BUGS:
                    target->SetVisible(true);
                    break;
                case SPELL_INFESTED_MIND_FILTER:
                {
                    if (Aura* aura = target->GetAura(SPELL_INFESTED))
                        if (aura->GetStackAmount() >= 10)
                            DoCast(target, SPELL_INFESTED_MIND, true);
                    break;
                }
            }
        }

        void RemoveBugs()
        {
            std::list<Creature*> bugslist;
            me->GetCreatureListWithEntryInGrid(bugslist, NPC_CORRUPTED_VERMIN, 150.0f);
            if (!bugslist.empty())
                for (auto bugs : bugslist)
                {
                    bugs->RemoveAurasDueToSpell(SPELL_TRANSFORM_BUGS);
                    bugs->SetVisible(false);
                }

            if (GetDifficultyID() == DIFFICULTY_HEROIC_RAID)
            {
                std::list<HostileReference*> threatlist = me->getThreatManager().getThreatList();
                if (!threatlist.empty())
                {
                    for (std::list<HostileReference*>::const_iterator itr = threatlist.begin(); itr != threatlist.end(); ++itr)
                        if (Player* player = me->GetPlayer(*me, (*itr)->getUnitGuid()))
                            if (player->HasAura(SPELL_INFESTED))
                                player->RemoveAurasDueToSpell(SPELL_INFESTED);
                }
            }
        }

        uint32 GetData(uint32 type) const override
        {
            switch (type)
            {
                case DATA_SWARM_COUNTER:
                    return swarmCount;
            }
            return 0;
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
                    case EVENT_CHECK_POWER:
                        if (me->GetPower(POWER_ENERGY) == 0)
                        {
                            swarmCount++;
                            me->StopAttack();
                            events.Reset();
                            Talk(SAY_PHASE);
                            DoCast(me, SPELL_TRANSFORM_BUGS, true);
                            DoCast(SPELL_HEART_SWARM);
                            events.ScheduleEvent(EVENT_HEART_SWARM_END, 20000);
                        }
                        events.ScheduleEvent(EVENT_CHECK_POWER, 10000);
                        break;
                    case EVENT_ALLOW_ROT_CAST:
                        DoCast(me, SPELL_ALLOW_ROT_CAST, true);
                        events.ScheduleEvent(EVENT_ALLOW_ROT_CAST, 120000);
                        break;
                    case EVENT_ROT:
                        if (me->GetPower(POWER_ENERGY) > 0)
                        {
                            DoCast(SPELL_ROT_FILTER);
                            Talk(SAY_ROT);
                        }
                        events.ScheduleEvent(EVENT_ROT, 16000);
                        break;
                    case EVENT_VOLATILE_ROT:
                        if (me->GetPower(POWER_ENERGY) > 0)
                        {
                            DoCastVictim(SPELL_VOLATILE_ROT);
                        }
                        events.ScheduleEvent(EVENT_VOLATILE_ROT, 25000);
                        break;
                    case EVENT_INFESTED_BREATH:
                        DoCast(SPELL_INFESTED_BREATH_FILTER);
                        Talk(SAY_BREATH_EMOTE);
                        Talk(SAY_BREATH);
                        events.ScheduleEvent(EVENT_INFESTED_BREATH, 36000);
                        if (GetDifficultyID() == DIFFICULTY_MYTHIC_RAID)
                            events.ScheduleEvent(EVENT_INFESTED_MIND, 1000);
                        break;
                    case EVENT_HEART_SWARM_END:
                        DefaultEvents(false);
                        RemoveBugs();
                        me->SetReactState(REACT_AGGRESSIVE);
                        break;
                    case EVENT_INFESTED_MIND:
                        DoCast(SPELL_INFESTED_MIND_FILTER);
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_nythendraAI (creature);
    }
};

//111004
class npc_nythendra_gelatinized_decay : public CreatureScript
{
public:
    npc_nythendra_gelatinized_decay() : CreatureScript("npc_nythendra_gelatinized_decay") {}

    struct npc_nythendra_gelatinized_decayAI : public ScriptedAI
    {
        npc_nythendra_gelatinized_decayAI(Creature* creature) : ScriptedAI(creature) {}

        EventMap events;
        bool moveActive;
        uint16 randomMoveTimer;

        void Reset() override
        {
            events.Reset();
            moveActive = true;
            randomMoveTimer = 2000;
        }

        void EnterCombat(Unit* /*who*/) override
        {
            events.ScheduleEvent(EVENT_1, 2000);
            events.ScheduleEvent(EVENT_2, 4000);
            events.ScheduleEvent(EVENT_3, 22000);
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type != POINT_MOTION_TYPE)
                return;

            moveActive = true;
        }

        void UpdateAI(uint32 diff) override
        {
            if (moveActive && !me->isInCombat())
            {
                if (randomMoveTimer <= diff)
                {
                    moveActive = false;
                    randomMoveTimer = urand(2, 6) * IN_MILLISECONDS;
                    float angle = (float)rand_norm() * (2 * M_PI);
                    float distance = 15 * (float)rand_norm();
                    float x = me->GetHomePosition().GetPositionX() + distance * std::cos(angle);
                    float y = me->GetHomePosition().GetPositionY() + distance * std::sin(angle);
                    float z = me->GetHomePosition().GetPositionZ();
                    Trinity::NormalizeMapCoord(x);
                    Trinity::NormalizeMapCoord(y);
    
                    me->GetMotionMaster()->MovePoint(1, x, y, z);
                }
                else randomMoveTimer -= diff;
            }

            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_1:
                        DoCast(SPELL_WAVE_OF_DECAY);
                        events.ScheduleEvent(EVENT_1, 3000);
                        break;
                    case EVENT_2:
                        DoCast(SPELL_UNSTABLE_DECAY);
                        events.ScheduleEvent(EVENT_2, 10000);
                        break;
                    case EVENT_3:
                        DoCast(SPELL_PUTRID_SWARM);
                        events.ScheduleEvent(EVENT_3, 30000);
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_nythendra_gelatinized_decayAI(creature);
    }
};

//102998, 111005
class npc_nythendra_corrupted_vermin : public CreatureScript
{
public:
    npc_nythendra_corrupted_vermin() : CreatureScript("npc_nythendra_corrupted_vermin") {}

    struct npc_nythendra_corrupted_verminAI : public ScriptedAI
    {
        npc_nythendra_corrupted_verminAI(Creature* creature) : ScriptedAI(creature) 
        {
            me->SetSpeed(MOVE_WALK, 0.5f);
            me->SetSpeed(MOVE_RUN, 0.5f);
            me->SetReactState(REACT_PASSIVE);
            if (me->GetEntry() == NPC_CORRUPTED_VERMIN)
                me->SetVisible(false);
        }

        EventMap events;
        uint8 castCount;
        bool active;

        void Reset() override
        {
            events.Reset();
            events.ScheduleEvent(EVENT_1, 1000);
            me->GetMotionMaster()->MoveRandom(5.0f);
            DoCast(me, SPELL_SCALE, true);
        }

        void IsSummonedBy(Unit* summoner) override
        {
            me->SetMovementAnimKitId(11428);
        }

        void UpdateAI(uint32 diff) override
        {
            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_1:
                        if (me->HasAura(SPELL_TRANSFORM_BUGS) || me->HasAura(SPELL_TRANSFORM_INSECT) || me->GetEntry() == NPC_CORRUPTED_VERMIN_2)
                            if (Unit* pTarget = me->FindNearestPlayer(3.0f))
                                if (me->GetDistance(pTarget) < 2.0f)
                                {
                                    castCount = 0;
                                    me->StopMoving();
                                    me->GetMotionMaster()->Clear();
                                    me->AddUnitState(UNIT_STATE_ROOT);
                                    me->RemoveAurasDueToSpell(SPELL_SCALE);
                                    DoCast(me, SPELL_HEART_SWARM_SCALE, true);
                                    events.ScheduleEvent(EVENT_2, 2000);
                                    break;
                                }
                        events.ScheduleEvent(EVENT_1, 2000);
                        break;
                    case EVENT_2:
                        if (me->GetEntry() == NPC_CORRUPTED_VERMIN_2)
                            DoCast(SPELL_PUTRID_SWARM_DMG);
                        else
                        {
                            castCount++;
                            DoCast(SPELL_BURST_OF_CORRUPTION);
                            if (castCount == 3)
                            {
                                castCount = 0;
                                DoCast(me, SPELL_SCALE, true);
                                me->RemoveAurasDueToSpell(SPELL_HEART_SWARM_SCALE);
                                me->ClearUnitState(UNIT_STATE_ROOT);
                                events.ScheduleEvent(EVENT_1, 18000);
                                break;
                            }
                        }
                        events.ScheduleEvent(EVENT_2, 2000);
                        break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_nythendra_corrupted_verminAI(creature);
    }
};

//203095
class spell_nythendra_rot : public SpellScriptLoader
{
    public:
        spell_nythendra_rot() : SpellScriptLoader("spell_nythendra_rot") { }

        class spell_nythendra_rot_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_nythendra_rot_SpellScript);

            uint8 swarmCount;

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (Unit* caster = GetCaster())
                {
                    if (caster->getVictim())
                        targets.remove(caster->getVictim());

                    if (caster->IsAIEnabled)
                        swarmCount = caster->GetAI()->GetData(DATA_SWARM_COUNTER);
                }
                Trinity::Containers::RandomResizeList(targets, swarmCount);
            }

            void HandleOnHit()
            {
                if (GetCaster() && GetHitUnit())
                    GetCaster()->CastSpell(GetHitUnit(), SPELL_ROT_DOT, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_nythendra_rot_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                OnHit += SpellHitFn(spell_nythendra_rot_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_nythendra_rot_SpellScript();
        }
};

//204463, 203096
class spell_nythendra_volatile_rot : public SpellScriptLoader
{
    public:
        spell_nythendra_volatile_rot() : SpellScriptLoader("spell_nythendra_volatile_rot") { }

        class spell_nythendra_volatile_rot_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_nythendra_volatile_rot_AuraScript);

            void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_EXPIRE)
                    return;

                if (!GetCaster() || !GetTarget())
                    return;

                if (aurEff->GetId() == SPELL_VOLATILE_ROT)
                {
                    for (uint8 i = 0; i < 4; i++)
                        GetTarget()->CastSpell(GetTarget(), SPELL_INFESTED_GROUND_VIS, true);
                }
                else if (aurEff->GetId() == SPELL_ROT_DOT)
                    GetCaster()->CastSpell(GetTarget(), 203044, true); //INFESTED_GROUND_AT
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_nythendra_volatile_rot_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_nythendra_volatile_rot_AuraScript::OnRemove, EFFECT_2, SPELL_AURA_MOD_SCALE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_nythendra_volatile_rot_AuraScript();
        }
};

//204470
class spell_nythendra_volatile_rot_dmg : public SpellScriptLoader
{
    public:
        spell_nythendra_volatile_rot_dmg() : SpellScriptLoader("spell_nythendra_volatile_rot_dmg") { }

        class spell_nythendra_volatile_rot_dmg_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_nythendra_volatile_rot_dmg_SpellScript);

            void DealDamage()
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                float distance = GetCaster()->GetExactDist2d(GetHitUnit());
                float radius = GetSpellInfo()->Effects[EFFECT_0]->CalcRadius(GetCaster());
                uint32 perc;
                if (distance < 15.0f)
                    perc = 100 - (distance / 10) * 30;
                else
                    perc = 15 - (distance / radius) * 30;

                uint32 damage = CalculatePct(GetHitDamage(), perc);
                SetHitDamage(damage);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_nythendra_volatile_rot_dmg_SpellScript::DealDamage);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_nythendra_volatile_rot_dmg_SpellScript();
        }
};

//203536, 203537
class spell_nythendra_infested_ground_rot : public SpellScriptLoader
{
    public:
        spell_nythendra_infested_ground_rot() : SpellScriptLoader("spell_nythendra_infested_ground_rot") { }

        class spell_nythendra_infested_ground_rot_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_nythendra_infested_ground_rot_SpellScript);
            
            void HandleDummy(SpellEffIndex effIndex)
            {
                if (!GetCaster())
                    return;

                WorldLocation* loc = GetHitDest();
                if (InstanceScript* instance = GetCaster()->GetInstanceScript())
                    if (Creature* nythendra = instance->instance->GetCreature(instance->GetGuidData(NPC_NYTHENDRA)))
                        if (nythendra->isInCombat())
                            nythendra->CastSpell(loc->GetPositionX(), loc->GetPositionY(), loc->GetPositionZ(), SPELL_INFESTED_GROUND_AT, true);
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                if (!GetCaster() || !GetCaster()->isInCombat())
                    return;

                WorldLocation* loc = GetHitDest();
                GetCaster()->CastSpell(loc->GetPositionX(), loc->GetPositionY(), loc->GetPositionZ(), 203044, true); //INFESTED_GROUND_AT
            }
            
            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_nythendra_infested_ground_rot_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnEffectHit += SpellEffectFn(spell_nythendra_infested_ground_rot_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_TRIGGER_MISSILE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_nythendra_infested_ground_rot_SpellScript();
        }
};

//203552
class spell_nythendra_heart_of_the_swarm : public SpellScriptLoader
{
public:
    spell_nythendra_heart_of_the_swarm() : SpellScriptLoader("spell_nythendra_heart_of_the_swarm") { }

    class spell_nythendra_heart_of_the_swarm_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_nythendra_heart_of_the_swarm_AuraScript);

        void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            if (Unit* caster = GetCaster())
            {
                std::list<AreaTrigger*> list;
                caster->GetAreaObjectList(list, 203044); //INFESTED_GROUND_AT
                if(!list.empty())
                {
                    for (std::list<AreaTrigger*>::iterator itr = list.begin(); itr != list.end(); ++itr)
                    {
                        if (AreaTrigger* areaObj = (*itr))
                            if (caster->GetDistance(areaObj) <= 100.0f)
                                areaObj->CastAction();
                    }
                }
            }
        }

        void Register() override
        {
            OnEffectApply += AuraEffectApplyFn(spell_nythendra_heart_of_the_swarm_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_ENERGIZE, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_nythendra_heart_of_the_swarm_AuraScript();
    }
};

//203045,202978,203097,204470,203646,205070
class spell_nythendra_infested : public SpellScriptLoader
{
    public:
        spell_nythendra_infested() : SpellScriptLoader("spell_nythendra_infested") {}

        class spell_nythendra_infested_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_nythendra_infested_SpellScript);

            void HandleOnHit()
            {
                if (!GetCaster() || !GetHitUnit() || GetHitUnit()->HasAura(SPELL_INFESTED_MIND))
                    return;

                if (InstanceScript* instance = GetCaster()->GetInstanceScript())
                    if (Creature* nythendra = instance->instance->GetCreature(instance->GetGuidData(NPC_NYTHENDRA)))
                        if (nythendra->GetMap()->GetDifficultyID() != DIFFICULTY_NORMAL_RAID && nythendra->GetMap()->GetDifficultyID() != DIFFICULTY_LFR_RAID)
                            nythendra->CastSpell(GetHitUnit(), SPELL_INFESTED, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_nythendra_infested_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_nythendra_infested_SpellScript();
        }
};

//205043
class spell_nythendra_infested_mind : public SpellScriptLoader
{
    public:
        spell_nythendra_infested_mind() : SpellScriptLoader("spell_nythendra_infested_mind") {}
 
        class spell_nythendra_infested_mind_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_nythendra_infested_mind_AuraScript)

            void OnAbsorb(AuraEffect* aurEff, DamageInfo & dmgInfo, float & absorbAmount)
            {
                absorbAmount = 1;
                if (GetTarget() && GetTarget()->HealthBelowPct(51))
                    GetAura()->Remove();
            }

            void OnPeriodic(AuraEffect const* aurEff)
            {
                if (GetTarget())
                {
                    GetTarget()->GetMotionMaster()->Clear();
                    GetTarget()->CastSpell(GetTarget(), SPELL_SPREAD_INFESTATION);
                }
            }
 
            void Register() override
            {
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_nythendra_infested_mind_AuraScript::OnAbsorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_nythendra_infested_mind_AuraScript::OnPeriodic, EFFECT_8, SPELL_AURA_PERIODIC_DUMMY);
            }
        };
 
        AuraScript* GetAuraScript() const override
        {
            return new spell_nythendra_infested_mind_AuraScript();
        }
};

void AddSC_boss_nythendra()
{
    new boss_nythendra();
    new npc_nythendra_gelatinized_decay();
    new npc_nythendra_corrupted_vermin();
    new spell_nythendra_volatile_rot();
    new spell_nythendra_volatile_rot_dmg();
    new spell_nythendra_infested_ground_rot();
    new spell_nythendra_heart_of_the_swarm();
    new spell_nythendra_rot();
    new spell_nythendra_infested();
    new spell_nythendra_infested_mind();
}
