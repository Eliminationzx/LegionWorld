/*
    https://uwow.biz/
*/

#include "AreaTriggerAI.h"
#include "antorus.h"

enum Says
{
    SAY_AGGRO                           = 0,
    SAY_ANNIHILATION                    = 1,
    SAY_DECIMATION                      = 2,
    SAY_WARN_DECIMATION                 = 3,
    SAY_WARN_APOCALYPSE_DRIVE           = 4,
    SAY_APOCALYPSE_DRIVE                = 5,
    SAY_WARN_ERADICATION                = 6,
    SAY_ERADICATION                     = 7,
    SAY_DEATH                           = 8,
};

enum Spells
{
    //Intro
    SPELL_INTRO                         = 246256,
    SPELL_CONV                          = 249111,
    SPELL_ERADICATION_INTRO_CAST        = 249114,
    SPELL_OUTRO_CONV                    = 249117,

    //Boss Fight
    SPELL_FEL_BOMBARDMENT_FILTER        = 244150,
    SPELL_FEL_BOMBARDMENT_MISSILE       = 244533,
    SPELL_FEL_BOMBARDMENT_AURA          = 244536,
    SPELL_FEL_BOMBARDMENT_MARK          = 246220,
    SPELL_APOCALYPSE_DRIVE_AURA         = 244152,
    SPELL_APOCALYPSE_DRIVE_CAST_DMG     = 240277,
    SPELL_APOCALYPSE_DRIVE_DMG          = 253300,
    SPELL_EMPOWERED                     = 245237,
    SPELL_ERADICATION                   = 244969,
    SPELL_CARNAGE                       = 244106,
    SPELL_SEARING_BARRAGE_1             = 244395,
    SPELL_SEARING_BARRAGE_1_DUMMY       = 246369,
    SPELL_SEARING_BARRAGE_1_DMG         = 244400,
    SPELL_SEARING_BARRAGE_2             = 246368,
    SPELL_SEARING_BARRAGE_2_DUMMY       = 244398,
    SPELL_SEARING_BARRAGE_2_DMG         = 246373,
    SPELL_SEARING_BARRAGE_FILTER        = 246360,
    SPELL_DROP_ANNIHILATION             = 245527,
    SPELL_DROP_DECIMATOR                = 245515,

    SPELL_TRANSFORM_INVISIBLE_CANNON    = 245125,

    SPELL_ANNIHILATION                  = 244294,
    SPELL_ANNIHILATION_MISSILE          = 244758,
    SPELL_ANNIHILATION_DMG              = 244761,
    SPELL_ANNIHILATION_BLAST            = 244762,
    SPELL_ANNIHILATION_SUMMON           = 244790,
    SPELL_ANNIHILATION_AT               = 244795,
    SPELL_ANNIHILATION_IN_AT            = 244799,

    SPELL_DECIMATION_FILTER             = 244399,
    SPELL_DECIMATION_MARK               = 244410,
    SPELL_DECIMATION_MISSILE            = 244448,

    //Heroic+
    SPELL_SURGING_FEL_AT                = 246655,
    SPELL_SURGING_FEL_DMG               = 246663,
    SPELL_SURGING_FEL_SUMMON            = 249969,
    SPELL_SURGING_FEL_VISUAL            = 249970,

    //Mythic
    SPELL_HAYWIRE_ANNIHILATION_DUMMY    = 246965,
    SPELL_HAYWIRE_ANNIHILATION          = 246968,
    SPELL_HAYWIRE_ANNIHILATION_MISSILE  = 246969,
    SPELL_HAYWIRE_ANNIHILATION_DMG      = 246971,
    SPELL_HAYWIRE_ANNIHILATION_SHRAPNEL = 247044,

    SPELL_HAYWIRE_DECIMATION_DUMMY      = 246897,
    SPELL_HAYWIRE_DECIMATION_FILTER     = 246919,
    SPELL_HAYWIRE_DECIMATION_MARK       = 246920,
    SPELL_HAYWIRE_DECIMATION_MISSILE    = 254946,

    SPELL_LURING_DESTRUCTION            = 246848,
};

enum eEvents
{
    EVENT_FEL_BOMBARDMENT               = 1,
    EVENT_ANNIHILATOR_OR_DECIMATOR      = 2,
    EVENT_ERADICATION                   = 3,
    EVENT_CHECK_NEAREST_PLAYER          = 4,
};

Position const centerPos = { -3300.20f, 9772.24f, -63.35f };

Position const surgingPos[5] =
{
    {-3322.0f, 9810.8f, -64.84f, 4.57f},
    {-3292.0f, 9810.8f, -64.07f, 4.57f},
    {-3277.0f, 9810.8f, -63.68f, 4.57f},
    {-3307.0f, 9810.8f, -63.85f, 4.57f},
    {-3262.0f, 9810.8f, -63.93f, 4.57f}
};

//122450
struct boss_worldbreaker : BossAI
{
    boss_worldbreaker(Creature* creature) : BossAI(creature, DATA_WORLDBREAKER) 
    {
        SetCombatMovement(false);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        me->SetVisible(false);
    }

    bool introComplete = false;
    uint8 healthPct = 0;
    uint8 specialAbility = 0;
    uint32 surgingFelTimer = 0;
    ObjectGuid surgingTrigGUID[5];

    void Reset() override
    {
        _Reset();
        healthPct = IsHeroicPlusRaid() ? 66 : 61;
        specialAbility = urand(0, 1); // 0 - Annihilator, 1 - Decimator
        surgingFelTimer = 0;
        me->RemoveAllAuras();
        me->SetReactState(REACT_AGGRESSIVE);

        for (uint8 i = 0; i < 5; ++i)
        {
            if (auto summon = me->SummonCreature(NPC_SURGING_FEL_TRIGGER, surgingPos[i]))
                surgingTrigGUID[i] = summon->GetGUID();
        }
    }

    void EnterCombat(Unit* /*who*/) override
    {
        _EnterCombat();
        Talk(SAY_AGGRO);

        DefaultEvents(true);
    }

    void DefaultEvents(bool enterCombat = false)
    {
        me->SetReactState(REACT_AGGRESSIVE, 2000);

        if (IsMythicRaid() || healthPct)
            events.ScheduleEvent(EVENT_ANNIHILATOR_OR_DECIMATOR, enterCombat ? 8500 : 12000);

        events.ScheduleEvent(EVENT_FEL_BOMBARDMENT, enterCombat ? 9500 : 14000);
        events.ScheduleEvent(EVENT_CHECK_NEAREST_PLAYER, 6000);
    }

    void JustDied(Unit* /*killer*/) override
    {
        Talk(SAY_DEATH);
        _JustDied();

        me->AddDelayedEvent(1000, [this]() -> void
        {
            me->CastSpell(me, SPELL_OUTRO_CONV, true);
        });
    }

    void SummonedCreatureDies(Creature* creature, Unit* killer) override
    {
        switch (creature->GetEntry())
        {
            case NPC_ANNIHILATOR_OF_KINGAROTH:
            {
                me->SetVisible(true);
                me->CastSpell(me, SPELL_INTRO, true);
                me->CastSpell(me, SPELL_CONV, true);
                me->AddDelayedEvent(500, [this]() -> void
                {
                    me->CastSpell(me, SPELL_ERADICATION_INTRO_CAST);
                });
                me->AddDelayedEvent(8000, [this]() -> void
                {
                    EnterEvadeMode();
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                });
                break;
            }
        }
    }

    void MoveInLineOfSight(Unit*who) override
    {
        if (!who->IsPlayer() || who->ToPlayer()->isGameMaster() || introComplete)
            return;

        if (me->GetDistance(who) <= 80)
        {
            introComplete = true;
            if (auto annihilant = me->SummonCreature(NPC_ANNIHILATOR_OF_KINGAROTH, -3299.45f, 9791.80f, -55.48f, 4.55f))
            {
                annihilant->AddDelayedEvent(1000, [annihilant]() -> void
                {
                    if (annihilant)
                    {
                        annihilant->GetMotionMaster()->MovePoint(0, -3299.45f, 9791.80f, -63.48f);
                        annihilant->CastSpell(annihilant, 257955, true);
                    }
                });
                annihilant->AddDelayedEvent(3000, [annihilant]() -> void
                {
                    if (annihilant)
                    {
                        annihilant->RemoveAurasDueToSpell(257955);
                        annihilant->SetReactState(REACT_AGGRESSIVE);
                    }
                });
            }
        }
    }

    void DamageTaken(Unit* atacker, uint32 &p_Damage, DamageEffectType dmgType) override
    {
        if (me->HealthBelowPct(healthPct))
        {
            if (healthPct > 60)
                healthPct = IsHeroicPlusRaid() ? 35 : 20;
            else
                healthPct = 0;

            events.Reset();
            me->StopAttack(true);
            Talk(SAY_WARN_APOCALYPSE_DRIVE);
            Talk(SAY_APOCALYPSE_DRIVE);
            me->CastSpell(me, SPELL_APOCALYPSE_DRIVE_AURA, true);
            me->CastSpell(me, SPELL_APOCALYPSE_DRIVE_CAST_DMG);

            DoActionSummon(NPC_WORLDBREAKER_ANNIHILATOR, ACTION_2);
            DoActionSummon(NPC_WORLDBREAKER_DECIMATOR, ACTION_2);

            if (IsHeroicRaid())
                surgingFelTimer = 10000;
            else if (IsMythicRaid())
                surgingFelTimer = 5000;
        }
    }

    void DoAction(int32 const actionID) override
    {
        switch (actionID)
        {
            case ACTION_1:
                DoActionSummon(NPC_WORLDBREAKER_ANNIHILATOR, ACTION_3); //Kill
                DoActionSummon(NPC_WORLDBREAKER_DECIMATOR, ACTION_4);   //Restore
                DoCast(me, SPELL_DROP_ANNIHILATION, true);
                break;
            case ACTION_2:
                DoActionSummon(NPC_WORLDBREAKER_DECIMATOR, ACTION_3);   //Kill
                DoActionSummon(NPC_WORLDBREAKER_ANNIHILATOR, ACTION_4); //Restore
                DoCast(me, SPELL_DROP_DECIMATOR, true);
                break;
        }

        if (actionID == ACTION_1 || actionID == ACTION_2)
        {
            surgingFelTimer = 0;
            me->InterruptNonMeleeSpells(false);
            me->RemoveAurasDueToSpell(SPELL_APOCALYPSE_DRIVE_AURA);
            events.ScheduleEvent(EVENT_ERADICATION, 4000);
        }
    }

    void SpellHit(Unit* caster, SpellInfo const* spell) override
    {
        switch (spell->Id)
        {
            case SPELL_SEARING_BARRAGE_1_DUMMY:
            case SPELL_SEARING_BARRAGE_2_DUMMY:
                me->CastSpell(me, SPELL_SEARING_BARRAGE_FILTER, true);
                break;
        }
    }

    void SpellHitTarget(Unit* target, SpellInfo const* spell) override
    {
        switch (spell->Id)
        {
            case SPELL_FEL_BOMBARDMENT_FILTER:
                me->CastSpell(target, SPELL_FEL_BOMBARDMENT_MARK, true);
                break;
            case SPELL_SEARING_BARRAGE_FILTER:
                me->CastSpell(target, RAND(SPELL_SEARING_BARRAGE_1_DMG, SPELL_SEARING_BARRAGE_2_DMG), true);
                break;
        }
    }

    void SpellFinishCast(const SpellInfo* spell) override
    {
        switch (spell->Id)
        {
            case SPELL_APOCALYPSE_DRIVE_CAST_DMG:
                surgingFelTimer = 0;
                DoActionSummon(NPC_WORLDBREAKER_DECIMATOR, ACTION_4);   //Restore
                DoActionSummon(NPC_WORLDBREAKER_ANNIHILATOR, ACTION_4); //Restore
                me->CastSpell(me, SPELL_EMPOWERED, true);
                if (!IsMythicRaid())
                    me->CastSpell(me, healthPct ? SPELL_SEARING_BARRAGE_1 : SPELL_SEARING_BARRAGE_2, true);
                DefaultEvents();
                break;
            case SPELL_ERADICATION:
                DefaultEvents();
                break;
        }
    }

    void OnRemoveAuraTarget(Unit* target, uint32 spellId, AuraRemoveMode mode) override
    {
        if (!me->isInCombat() || mode != AURA_REMOVE_BY_EXPIRE)
            return;

        switch (spellId)
        {
            case SPELL_FEL_BOMBARDMENT_MARK:
                me->CastSpell(target, SPELL_FEL_BOMBARDMENT_AURA, true);
                break;
        }
    }

    void DoActionSummon(uint32 entry, int32 actionID)
    {
        EntryCheckPredicate pred(entry);
        summons.DoAction(actionID, pred);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (surgingFelTimer)
        {
            if (surgingFelTimer <= diff)
            {
                if (auto surging = Creature::GetCreature(*me, surgingTrigGUID[urand(0, 4)]))
                    surging->GetAI()->DoAction(true);

                surgingFelTimer = IsMythicRaid() ? 5000 : 10000;
            }
            else
                surgingFelTimer -= diff;
        }

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        if (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_FEL_BOMBARDMENT:
                    DoCast(SPELL_FEL_BOMBARDMENT_FILTER);
                    events.ScheduleEvent(EVENT_FEL_BOMBARDMENT, IsMythicRaid() ? 16000 : 20000);
                    break;
                case EVENT_ANNIHILATOR_OR_DECIMATOR:
                {
                    if (!specialAbility || (!IsMythicRaid() && me->HasAura(SPELL_DROP_DECIMATOR))) //Annihilator
                    {
                        specialAbility = 1;
                        DoActionSummon(NPC_WORLDBREAKER_ANNIHILATOR, ACTION_1);
                        Talk(SAY_ANNIHILATION);
                    }
                    else if (specialAbility || (!IsMythicRaid() && me->HasAura(SPELL_DROP_ANNIHILATION))) //Decimator
                    {
                        specialAbility = 0;
                        DoActionSummon(NPC_WORLDBREAKER_DECIMATOR, ACTION_1);
                        Talk(SAY_DECIMATION);
                    }
                    events.ScheduleEvent(EVENT_ANNIHILATOR_OR_DECIMATOR, 16000);
                    break;
                }
                case EVENT_ERADICATION:
                    me->CastSpell(me, SPELL_EMPOWERED, true);
                    if (!IsMythicRaid())
                        me->CastSpell(me, healthPct ? SPELL_SEARING_BARRAGE_1 : SPELL_SEARING_BARRAGE_2, true);
                    else
                        me->CastSpell(me, SPELL_LURING_DESTRUCTION, true);
                    Talk(SAY_WARN_ERADICATION);
                    Talk(SAY_ERADICATION);
                    me->CastSpell(me, SPELL_ERADICATION);
                    break;
                case EVENT_CHECK_NEAREST_PLAYER:
                {
                    events.ScheduleEvent(EVENT_CHECK_NEAREST_PLAYER, 3000);

                    if (!me->HasAura(SPELL_CARNAGE) && !me->IsWithinMeleeRange(me->getVictim()))
                    {
                        if (auto player = me->FindNearestPlayer(50.0f))
                        {
                            if (me->IsWithinMeleeRange(player))
                            {
                                DoModifyThreatPercent(me->getVictim(), -100);
                                me->AddThreat(player, 100000.0f);
                                AttackStart(player);
                                return;
                            }
                        }
                        DoCast(SPELL_CARNAGE);
                    }
                    break;
                }
            }
        }
        DoMeleeAttackIfReady();
    }
};

//122773, 122778
struct npc_worldbreaker_annihilation_decimator : ScriptedAI
{
    npc_worldbreaker_annihilation_decimator(Creature* creature) : ScriptedAI(creature)
    {
        instance = me->GetInstanceScript();
        me->SetReactState(REACT_PASSIVE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
    }

    InstanceScript* instance;
    bool died = false;
    bool haywire = false;

    void Reset() override {}

    void DoAction(int32 const actionID) override
    {
        switch (actionID)
        {
            case ACTION_1:
                if (me->GetEntry() == NPC_WORLDBREAKER_ANNIHILATOR)
                    DoCast(haywire ? SPELL_HAYWIRE_ANNIHILATION : SPELL_ANNIHILATION);
                else
                    DoCast(haywire ? SPELL_HAYWIRE_DECIMATION_FILTER : SPELL_DECIMATION_FILTER);
                break;
            case ACTION_2:
                if (!haywire)
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                }
                break;
            case ACTION_3:
                if (haywire)
                    break;

                if (!IsMythicRaid())
                {
                    me->CastSpell(me, SPELL_TRANSFORM_INVISIBLE_CANNON, true);
                    me->Kill(me);
                }
                else
                {
                    haywire = true;

                    if (me->GetEntry() == NPC_WORLDBREAKER_ANNIHILATOR)
                        DoCast(me, SPELL_HAYWIRE_ANNIHILATION_DUMMY, true);
                    else if (me->GetEntry() == NPC_WORLDBREAKER_DECIMATOR)
                        DoCast(me, SPELL_HAYWIRE_DECIMATION_DUMMY, true);
                }
                if (!haywire) //IsMythic - no break
                    break;
            case ACTION_4:
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                me->RemoveAllAuras();
                me->SetHealth(me->GetHealth());
                died = false;
                break;
        }
    }

    void DamageTaken(Unit* attacker, uint32 &damage, DamageEffectType dmgType) override
    {
        if (damage >= me->GetHealth())
        {
            damage = 0;

            if (!died)
            {
                died = true;
                me->SetHealth(1);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                me->RemoveAllAuras();

                if (auto owner = me->GetAnyOwner())
                    owner->GetAI()->DoAction(me->GetEntry() == NPC_WORLDBREAKER_ANNIHILATOR ? ACTION_1 : ACTION_2);
            }
        }
    }

    uint8 GetMaxPoint()
    {
        if (IsMythicRaid())
            return 6;

        if (me->GetMap()->GetPlayersCountExceptGMs() < 15)
            return IsHeroicRaid() ? 2 : 1;
        else if (me->GetMap()->GetPlayersCountExceptGMs() < 25)
            return IsHeroicRaid() ? 3 : 2;
        else
            return IsHeroicRaid() ? 4 : 3;
    }

    void SpellHit(Unit* caster, SpellInfo const* spell) override
    {
        switch (spell->Id)
        {
            case SPELL_ANNIHILATION:
            {
                std::list<Position> randPos;
                me->GenerateNonDuplicatePoints(randPos, centerPos, GetMaxPoint(), 10.0f, 35.0f, 5.0f);

                if (auto owner = me->GetAnyOwner())
                {
                    for (auto pos : randPos)
                    {
                        me->CastSpell(pos, SPELL_ANNIHILATION_SUMMON, true, nullptr, nullptr, owner->GetGUID());
                        me->CastSpellDelay(pos, SPELL_ANNIHILATION_MISSILE, true, 5000);
                    }
                }
                break;
            }
            case SPELL_HAYWIRE_ANNIHILATION:
            {
                std::list<Position> randPos;
                me->GenerateNonDuplicatePoints(randPos, centerPos, GetMaxPoint(), 10.0f, 35.0f, 5.0f);

                if (auto owner = me->GetAnyOwner())
                {
                    for (auto pos : randPos)
                    {
                        me->CastSpell(pos, SPELL_ANNIHILATION_SUMMON, true, nullptr, nullptr, owner->GetGUID());
                        me->CastSpellDelay(pos, SPELL_HAYWIRE_ANNIHILATION_MISSILE, true, 5000);
                    }
                }
                break;
            }
        }
    }

    void SpellHitTarget(Unit* target, SpellInfo const* spell) override
    {
        switch (spell->Id)
        {
            case SPELL_DECIMATION_FILTER:
                Talk(SAY_WARN_DECIMATION, target->GetGUID());
                me->CastSpell(target, SPELL_DECIMATION_MARK, true);
                break;
            case SPELL_HAYWIRE_DECIMATION_FILTER:
                if (me->GetGUID() != target->GetGUID())
                    me->CastSpell(target, SPELL_HAYWIRE_DECIMATION_MARK, true);
                break;
        }
    }

    void OnRemoveAuraTarget(Unit* target, uint32 spellId, AuraRemoveMode mode) override
    {
        if (mode != AURA_REMOVE_BY_EXPIRE)
            return;

        switch (spellId)
        {
            case SPELL_DECIMATION_MARK:
                me->CastSpell(target, SPELL_DECIMATION_MISSILE, true);
                break;
            case SPELL_HAYWIRE_DECIMATION_MARK:
                me->CastSpell(target, SPELL_HAYWIRE_DECIMATION_MISSILE, true);
                break;
        }
    }

    void UpdateAI(uint32 diff) override {}
};

//122818, 124330
struct npc_worldbreaker_annihilation_trigger : public ScriptedAI
{
    explicit npc_worldbreaker_annihilation_trigger(Creature* creature) : ScriptedAI(creature)
    {
        me->SetReactState(REACT_PASSIVE);
    }

    void Reset() override {}

    void IsSummonedBy(Unit* summoner) override
    {
        if (me->GetEntry() == NPC_WB_ANNIHILATION_TRIGGER)
            DoCast(me, SPELL_ANNIHILATION_AT, true);
        else
            me->CastSpell(me, SPELL_HAYWIRE_ANNIHILATION_SHRAPNEL);
    }

    void SpellHit(Unit* caster, SpellInfo const* spell) override
    {
        if (spell->Id == SPELL_ANNIHILATION_DMG)
            DespawnMe();
    }

    void SpellFinishCast(const SpellInfo* spell) override
    {
        if (spell->Id == SPELL_HAYWIRE_ANNIHILATION_SHRAPNEL)
            DespawnMe();
    }

    void DespawnMe()
    {
        me->AddDelayedCombat(200, [this]() -> void
        {
            if (me)
                me->DespawnOrUnsummon();
        });
    }

    void UpdateAI(uint32 diff) override {}
};

//124167, 128429
struct npc_worldbreaker_surging_fel_trigger : public ScriptedAI
{
    explicit npc_worldbreaker_surging_fel_trigger(Creature* creature) : ScriptedAI(creature)
    {
        me->SetReactState(REACT_PASSIVE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
        me->SetSpeed(MOVE_RUN, 10.0f, true);
    }

    void Reset() override {}

    void IsSummonedBy(Unit* summoner) override
    {
        if (me->GetEntry() == NPC_SURGING_FEL_TRIGGER_MOVER)
        {
            DoCast(me, SPELL_SURGING_FEL_VISUAL, true);

            Position pos;
            me->GetNearPosition(pos, 90.0f, 0.0f);
            me->GetMotionMaster()->MovePoint(1, pos, false);
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (type == POINT_MOTION_TYPE && id == 1)
        {
            me->DespawnOrUnsummon();
        }
    }

    void DoAction(int32 const actionID) override
    {
        if (me->GetEntry() == NPC_SURGING_FEL_TRIGGER)
        {
            DoCast(me, SPELL_SURGING_FEL_AT, true);

            me->AddDelayedCombat(4000, [this]() -> void
            {
                if (me)
                {
                    me->CastSpell(me, SPELL_SURGING_FEL_SUMMON, true);
                    me->CastSpell(me, SPELL_SURGING_FEL_DMG, true);
                }
            });
        }
    }

    void UpdateAI(uint32 diff) override {}
};

//123398
struct npc_atbt_annihilator : ScriptedAI
{
    npc_atbt_annihilator(Creature* creature) : ScriptedAI(creature)
    {
        me->SetReactState(REACT_DEFENSIVE);
    }

    EventMap events;
    std::vector<Position> positions{};

    void Reset() override
    {
        events.Reset();
    }

    void EnterCombat(Unit* who) override
    {
        events.ScheduleEvent(EVENT_1, 21000);
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
            case EVENT_1:
            {
                DoCast(245807);
                auto players = me->getThreatManager().getThreatList();

                Trinity::Containers::RandomResizeList(players, 3);
                for (const auto& targ : players)
                {
                    positions.push_back(targ->getTarget()->GetPosition());
                    me->CastSpell(targ->getTarget(), 245808); // 123459
                }
                events.ScheduleEvent(EVENT_1, 21000);
                events.ScheduleEvent(EVENT_2, 5010);
                break;
            }
            case EVENT_2:
                for (const auto& pos : positions)
                    me->CastSpell(pos, 245809, true);
                break;
            }
        }
        DoMeleeAttackIfReady();
    }
};

//244761, 247044
class spell_worldbreaker_annihilation_dmg : public SpellScript
{
    PrepareSpellScript(spell_worldbreaker_annihilation_dmg);

    uint8 targetCount = 0;

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([this](WorldObject* object) -> bool
        {
            if (object == nullptr || !object->IsPlayer())
                return true;

            return false;
        });

        targetCount = targets.size();

        if (GetCaster() && GetExplTargetDest() && targets.empty())
            GetCaster()->CastSpell(GetExplTargetDest(), SPELL_ANNIHILATION_BLAST, true);
    }

    void HandleDamage(SpellEffIndex /*effectIndex*/)
    {
        if (targetCount)
            SetHitDamage(GetHitDamage() / targetCount);
    }

    void Register()
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_worldbreaker_annihilation_dmg::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
        OnEffectHitTarget += SpellEffectFn(spell_worldbreaker_annihilation_dmg::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

//244969
class spell_worldbreaker_eradication : public SpellScript
{
    PrepareSpellScript(spell_worldbreaker_eradication);

    void HandleDamage(SpellEffIndex /*effIndex*/)
    {
        auto caster = GetCaster();
        auto target = GetHitUnit();
        if (!caster || !target)
            return;

        float distance = caster->GetExactDist2d(target) * 1.5f;
        uint8 pct = 10;
        if (distance < 100.0f)
            pct = 100 - distance;

        SetHitDamage(CalculatePct(GetHitDamage(), pct));
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_worldbreaker_eradication::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

//244536
class spell_worldbreaker_fel_bombardment : public AuraScript
{
    PrepareAuraScript(spell_worldbreaker_fel_bombardment);

    void HandlePeriodic(AuraEffect const* /*auraEffect*/)
    {
        if (GetCaster() && GetTarget())
        {
            GetCaster()->CastSpell(GetTarget(), SPELL_FEL_BOMBARDMENT_MISSILE, true);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_worldbreaker_fel_bombardment::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

//244152
class spell_worldbreaker_apocalypse_drive : public AuraScript
{
    PrepareAuraScript(spell_worldbreaker_apocalypse_drive);

    void HandlePeriodic(AuraEffect const* auraEffect)
    {
        if (GetCaster())
            GetCaster()->CastSpell(GetCaster(), SPELL_APOCALYPSE_DRIVE_DMG, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_worldbreaker_apocalypse_drive::HandlePeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

//244106
class spell_worldbreaker_carnage : public AuraScript
{
    PrepareAuraScript(spell_worldbreaker_carnage);

    void OnProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        aurEff->GetBase()->Remove();
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_worldbreaker_carnage::OnProc, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

//Custom: 15496 (Entry: 10662)
struct at_worldbreaker_annihilation : AreaTriggerAI
{
    at_worldbreaker_annihilation(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger) {}

    void ActionOnUpdate(GuidList& affectedPlayers) override
    {
        auto owner = at->GetCaster();
        if (!owner)
            return;

        if (affectedPlayers.empty() && !owner->HasAura(SPELL_ANNIHILATION_IN_AT))
        {
            owner->CastSpell(owner, SPELL_ANNIHILATION_IN_AT, true);
        }
        else if (!affectedPlayers.empty() && owner->HasAura(SPELL_ANNIHILATION_IN_AT))
        {
            owner->RemoveAurasDueToSpell(SPELL_ANNIHILATION_IN_AT);
        }
    }
};

void AddSC_boss_worldbreaker()
{
    RegisterCreatureAI(boss_worldbreaker);
    RegisterCreatureAI(npc_worldbreaker_annihilation_decimator);
    RegisterCreatureAI(npc_worldbreaker_annihilation_trigger);
    RegisterCreatureAI(npc_worldbreaker_surging_fel_trigger);
    RegisterCreatureAI(npc_atbt_annihilator);
    RegisterSpellScript(spell_worldbreaker_annihilation_dmg);
    RegisterSpellScript(spell_worldbreaker_eradication);
    RegisterAuraScript(spell_worldbreaker_fel_bombardment);
    RegisterAuraScript(spell_worldbreaker_apocalypse_drive);
    RegisterAuraScript(spell_worldbreaker_carnage);
    RegisterAreaTriggerAI(at_worldbreaker_annihilation);
}
