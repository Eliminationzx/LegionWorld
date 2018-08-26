/*
    https://uwow.biz/
    To-Do: нужны еще снифы для фраз/варнингов. Не хватает парочки
*/

#include "the_emerald_nightmare.h"
#include "CreatureGroups.h"

enum Says
{
    SAY_AGGRO           = 0,
    SAY_CALL            = 1,
    SAY_DEATH           = 3,
};

enum Spells
{
    SPELL_EMPTY_ENERGY              = 205283, //224200, 203909, 204719?
    SPELL_ENERGIZE_UP               = 205284,
    SPELL_REMOVE_MARK_TAERAR        = 224627,
    SPELL_REMOVE_MARK_LETHON        = 224626,
    SPELL_REMOVE_MARK_EMERISS       = 224625,
    SPELL_NIGHTMARE_BOND            = 203966, //Share damage. 203969
    SPELL_CORRUPTED_BREATH          = 203028,
    SPELL_TAIL_LASH                 = 204119,
    SPELL_ENERGIZE_DRAGONS          = 205281,
    SPELL_SLUMBERING_NIGHTMARE      = 203110,

    //Ysondre - 102679
    SPELL_MARK_OF_YSONDRE           = 203083,
    SPELL_ENERGIZE_YSONDRE          = 205282,
    SPELL_NIGHTMARE_BLAST           = 203147,
    SPELL_CALL_DEFILED_SPIRIT       = 207573,

    //Taerar - 102681
    SPELL_MARK_OF_TAERAR            = 203085,
    SPELL_SHADES_OF_TAERAR          = 204100,
    SPELL_SEEPING_FOG               = 205331,
    SPELL_BELLOWING_ROAR_AURA       = 205172, //Heroic+
    SPELL_BELLOWING_ROAR            = 204078,

    //Lethon - 102682
    SPELL_MARK_OF_LETHON            = 203086,
    SPELL_SIPHON_SPIRIT             = 203888,
    SPELL_SIPHON_SPIRIT_SUM         = 203837,
    SPELL_GLOOM_AURA                = 206758,
    SPELL_SHADOW_BURST              = 204030, //Heroic+

    //Emeriss - 102683
    SPELL_MARK_OF_EMERISS           = 203084,
    SPELL_NIGHTMARE_ENERGY          = 224200,
    SPELL_GROUNDED                  = 204719,
    SPELL_VOLATILE_INFECTION        = 203787,
    SPELL_ESSENCE_OF_CORRUPTION     = 205298,

    //Nightmare Bloom - 102804
    SPELL_NIGHTMARE_BLOOM_VISUAL    = 203672,
    SPELL_NIGHTMARE_BLOOM_DUMMY     = 205945,
    SPELL_NIGHTMARE_BLOOM_DUMMY_2   = 207681,
    SPELL_NIGHTMARE_BLOOM_DUMMY_3   = 220938,
    SPELL_NIGHTMARE_BLOOM_DUMMY_4   = 211497,
    SPELL_NIGHTMARE_BLOOM_AT        = 203687,
    SPELL_NIGHTMARE_BLOOM_PERIODIC  = 205278,

    //Defiled Druid Spirit - 103080
    SPELL_DEFILED_SPIRIT_ROOT       = 203768,
    SPELL_DEFILED_ERUPTION          = 203771,

    //Essence of Corruption - 103691
    SPELL_CORRUPTION                = 205300,

    //Shade of Taerar - 103145
    SPELL_NIGHTMARE_VISAGE          = 204084,
    SPELL_CORRUPTED_BREATH_2        = 204767,
    SPELL_TAIL_LASH_2               = 204768,

    //Dread Horror - 103044
    SPELL_WASTING_DREAD_AT          = 204729, //Heroic
    SPELL_UNRELENTING               = 221419, //Mythic. Нужна инфа с офа, как часто использует эту способность. Пока что раз в 10 секунд.

    //Corrupted Mushroom
    SPELL_CORRUPTED_BURST_4         = 203817,
    SPELL_CORRUPTED_BURST_7         = 203827,
    SPELL_CORRUPTED_BURST_10        = 203828,
};

enum eEvents
{
    EVENT_CORRUPTED_BREATH          = 1,

    //Ysondre
    EVENT_SWITCH_DRAGONS            = 2,
    EVENT_NIGHTMARE_BLAST           = 3,
    EVENT_CALL_DEFILED_SPIRIT       = 4,

    //Taerar
    EVENT_SHADES_OF_TAERAR          = 5,
    EVENT_SEEPING_FOG               = 6,

    //Emeriss
    EVENT_VOLATILE_INFECTION        = 7,
    EVENT_ESSENCE_OF_CORRUPTION     = 8,

    //Lethon
    EVENT_SIPHON_SPIRIT             = 9,
    EVENT_SHADOW_BURST              = 10, //Heroic+

    //Mythic
    EVENT_TAIL_LASH                 = 11,
};

Position const dragonPos[5] =
{
    {617.51f, -12807.0f,  4.84f, 3.39f},
    {674.78f, -12875.5f, 42.21f, 2.91f},
    {623.48f, -12845.71f, 4.13f, 2.47f},
    //Mythic
    {681.84f, -12869.25f, 43.62f, 2.74f},
    {662.45f, -12893.78f, 43.62f, 2.31f}
};

//102679
class boss_dragon_ysondre : public CreatureScript
{
public:
    boss_dragon_ysondre() : CreatureScript("boss_dragon_ysondre") {}

    struct boss_dragon_ysondreAI : public BossAI
    {
        boss_dragon_ysondreAI(Creature* creature) : BossAI(creature, DATA_DRAGON_NIGHTMARE) 
        {
            group_member = sFormationMgr->CreateCustomFormation(me);
            helpersList = {NPC_TAERAR, NPC_LETHON, NPC_EMERISS};
            Trinity::Containers::RandomResizeList(helpersList, GetDifficultyID() == DIFFICULTY_MYTHIC_RAID ? 3 : 2);
            introDone = false;
        }

        std::vector<uint32> helpersList;
        FormationInfo* group_member;
        uint8 eventPhaseHP;
        bool introDone;

        void Reset() override
        {
            _Reset();
            DoCast(me, SPELL_EMPTY_ENERGY, true);
            eventPhaseHP = 71;

            me->GetMotionMaster()->MoveIdle();
            me->NearTeleportTo(me->GetHomePosition()); //HomePos

            if (Creature* dragon = me->SummonCreature(helpersList[0], dragonPos[0]))
                if (CreatureGroup* f = me->GetFormation())
                    f->AddMember(dragon, group_member);

            if (GetDifficultyID() == DIFFICULTY_MYTHIC_RAID)
            {
                for (uint8 i = 0; i < 2; i++)
                    if (Creature* dragon = me->SummonCreature(helpersList[1 + i], dragonPos[3 + i]))
                    {
                        dragon->SetReactState(REACT_PASSIVE);
                        dragon->AI()->SetFlyMode(true);
                        dragon->SetVisible(false);
                    }
            }
            else
            {
                if (Creature* dragon = me->SummonCreature(helpersList[1], dragonPos[1]))
                {
                    dragon->SetReactState(REACT_PASSIVE);
                    dragon->AI()->SetFlyMode(true);
                    dragon->SetVisible(false);
                }
            }

            DespawnTrash();
        }

        void EnterCombat(Unit* who) override
        //51:15
        {
            _EnterCombat();
            DoCast(me, SPELL_ENERGIZE_YSONDRE, true);
            DoCast(me, SPELL_MARK_OF_YSONDRE, true);

            events.ScheduleEvent(EVENT_CORRUPTED_BREATH, 16000);
            events.ScheduleEvent(EVENT_SWITCH_DRAGONS, 1000);
            events.ScheduleEvent(EVENT_NIGHTMARE_BLAST, 40000);

            if (GetDifficultyID() == DIFFICULTY_MYTHIC_RAID)
                events.ScheduleEvent(EVENT_TAIL_LASH, 5000);

            EntryCheckPredicate pred(helpersList[0]);
            summons.DoAction(ACTION_3, pred);

            Talk(SAY_AGGRO);
        }

        void MoveInLineOfSight(Unit* who) override
        {  
            if (who->GetTypeId() != TYPEID_PLAYER)
                return;

            if (!introDone && me->IsWithinDistInMap(who, 150.0f))
            {
                introDone = true;
                Conversation* conversation = new Conversation;
                if (!conversation->CreateConversation(sObjectMgr->GetGenerator<HighGuid::Conversation>()->Generate(), 3608, who, nullptr, who->GetPosition()))
                    delete conversation;
            }
        }

        void JustDied(Unit* /*killer*/) override
        {
            _JustDied();
            DespawnTrash();
            Talk(SAY_DEATH);
        }

        void DespawnTrash()
        {
            std::list<Creature*> creList;
            GetCreatureListWithEntryInGrid(creList, me, NPC_NIGHTMARE_BLOOM, 120.0f);
            GetCreatureListWithEntryInGrid(creList, me, NPC_ESSENCE_OF_CORRUPTION, 120.0f);
            GetCreatureListWithEntryInGrid(creList, me, NPC_SHADE_OF_TAERAR, 120.0f);
            GetCreatureListWithEntryInGrid(creList, me, NPC_SEEPING_FOG, 120.0f);
            GetCreatureListWithEntryInGrid(creList, me, NPC_SPIRIT_SHADE, 120.0f);
            GetCreatureListWithEntryInGrid(creList, me, NPC_DREAD_HORROR, 120.0f);
            if (!creList.empty())
                for (auto const& trash : creList)
                {
                    if (trash && trash->IsInWorld())
                        trash->DespawnOrUnsummon();
                }
        }

        void DoAction(int32 const action, Creature* caller) override
        {
            switch (action)
            {
                case ACTION_1:
                    if (CreatureGroup* f = me->GetFormation())
                        f->AddMember(caller, group_member);
                    break;
                case ACTION_2:
                    if (CreatureGroup* f = me->GetFormation())
                        f->RemoveMember(caller);
                    break;
                case ACTION_4: //Special Attack
                    events.ScheduleEvent(EVENT_CALL_DEFILED_SPIRIT, 100);
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

            if (CheckHomeDistToEvade(diff, 100.0f, 576.33f, -12813.32f, 6.17f))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_CORRUPTED_BREATH:
                        DoCastVictim(SPELL_CORRUPTED_BREATH);
                        events.ScheduleEvent(EVENT_CORRUPTED_BREATH, 32000);
                        break;
                    case EVENT_SWITCH_DRAGONS:
                        if (me->HealthBelowPct(eventPhaseHP))
                        {
                            if (eventPhaseHP > 41)
                            {
                                eventPhaseHP = 41;
                                EntryCheckPredicate pred0(helpersList[0]);
                                summons.DoAction(ACTION_1, pred0);
                                EntryCheckPredicate pred1(helpersList[1]);
                                summons.DoAction(ACTION_2, pred1);
                            }
                            else
                            {
                                eventPhaseHP = 0;
                                if (GetDifficultyID() == DIFFICULTY_MYTHIC_RAID)
                                {
                                    EntryCheckPredicate pred0(helpersList[1]);
                                    summons.DoAction(ACTION_1, pred0);
                                    EntryCheckPredicate pred1(helpersList[2]);
                                    summons.DoAction(ACTION_2, pred1);
                                }
                                else
                                {
                                    EntryCheckPredicate pred0(helpersList[0]);
                                    summons.DoAction(ACTION_2, pred0);
                                    EntryCheckPredicate pred1(helpersList[1]);
                                    summons.DoAction(ACTION_1, pred1);
                                }
                                break;
                            }
                        }
                        events.ScheduleEvent(EVENT_SWITCH_DRAGONS, 1000);
                        break;
                    case EVENT_NIGHTMARE_BLAST:
                        DoCast(SPELL_NIGHTMARE_BLAST);
                        events.ScheduleEvent(EVENT_NIGHTMARE_BLAST, 15000);
                        break;
                    case EVENT_CALL_DEFILED_SPIRIT:
                        Talk(SAY_CALL);
                        DoCast(SPELL_CALL_DEFILED_SPIRIT);
                        break;
                    case EVENT_TAIL_LASH:
                        DoCast(SPELL_TAIL_LASH);
                        events.ScheduleEvent(EVENT_TAIL_LASH, 5000);
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_dragon_ysondreAI (creature);
    }
};

//102681
class boss_dragon_taerar : public CreatureScript
{
public:
    boss_dragon_taerar() : CreatureScript("boss_dragon_taerar") { }

    struct boss_dragon_taerarAI : public ScriptedAI
    {
        boss_dragon_taerarAI(Creature* creature) : ScriptedAI(creature) 
        {
            instance = me->GetInstanceScript();
            me->SetSpeed(MOVE_FLIGHT, 2.0f);
        }

        InstanceScript* instance;
        EventMap events;

        void Reset() override
        {
            me->SetMaxPower(POWER_ENERGY, 100);
            DoCast(me, SPELL_EMPTY_ENERGY, true);
        }

        void EnterCombat(Unit* who) override
        {
            DoCast(me, SPELL_ENERGIZE_UP, true);
            Talk(SAY_AGGRO);
            DoZoneInCombat(me, 150.0f);
        }

        void DefaultEvents()
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
            me->RemoveAurasDueToSpell(SPELL_BELLOWING_ROAR_AURA);
            DoCast(me, SPELL_MARK_OF_TAERAR, true);
            DoCast(me, SPELL_ENERGIZE_DRAGONS, true);
            events.ScheduleEvent(EVENT_CORRUPTED_BREATH, 16000);
            events.ScheduleEvent(EVENT_SEEPING_FOG, 26000);

            if (GetDifficultyID() == DIFFICULTY_MYTHIC_RAID)
                events.ScheduleEvent(EVENT_TAIL_LASH, 5000);
        }

        void JustDied(Unit* /*killer*/) override {}

        void DoAction(int32 const action) override
        {
            switch (action)
            {
                case ACTION_1:
                if (Unit* owner = me->GetAnyOwner())
                    if (Creature* summoner = owner->ToCreature())
                        summoner->AI()->DoAction(ACTION_2, me); //Leave group
                    events.Reset();
                    me->StopAttack();
                    SetFlyMode(true);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                    me->RemoveAurasDueToSpell(SPELL_MARK_OF_TAERAR);
                    me->RemoveAurasDueToSpell(SPELL_ENERGIZE_DRAGONS);
                    me->GetMotionMaster()->MovePoint(1, dragonPos[1].GetPositionX(), dragonPos[1].GetPositionY(), dragonPos[1].GetPositionZ(), false);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    break;
                case ACTION_2:
                    me->SetVisible(true);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                    me->GetMotionMaster()->MovePoint(2, dragonPos[2].GetPositionX(), dragonPos[2].GetPositionY(), dragonPos[2].GetPositionZ(), false);
                    //No break!
                case ACTION_3:
                    DefaultEvents();
                    break;
                case ACTION_4: //Special Attack
                    events.ScheduleEvent(EVENT_SHADES_OF_TAERAR, 100);
                    break;
            }
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (id == 1)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                me->SetFacingTo(2.47f);

                if (GetDifficultyID() == DIFFICULTY_HEROIC_RAID || GetDifficultyID() == DIFFICULTY_MYTHIC_RAID)
                    DoCast(me, SPELL_BELLOWING_ROAR_AURA, true);
            }
            else if (id == 2)
            {
                SetFlyMode(false);
                me->SetReactState(REACT_AGGRESSIVE);
                DoZoneInCombat(me, 150.0f);
            if (Unit* owner = me->GetAnyOwner())
                if (Creature* summoner = owner->ToCreature())
                {
                    me->SetHealth(summoner->GetHealth());
                    summoner->AI()->DoAction(ACTION_1, me); //Invite group
                }
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);
            DoAggroPulse(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (me->GetReactState() == REACT_AGGRESSIVE && CheckHomeDistToEvade(diff, 100.0f, 576.33f, -12813.32f, 6.17f))
            {
            if (Unit* owner = me->GetAnyOwner())
                if (Creature* summoner = owner->ToCreature())
                    if (summoner->isInCombat())
                        summoner->AI()->EnterEvadeMode();
                return;
            }

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_CORRUPTED_BREATH:
                        DoCastVictim(SPELL_CORRUPTED_BREATH);
                        events.ScheduleEvent(EVENT_CORRUPTED_BREATH, 32000);
                        break;
                    case EVENT_SHADES_OF_TAERAR:
                        DoCast(SPELL_SHADES_OF_TAERAR);
                        Talk(1);
                        break;
                    case EVENT_SEEPING_FOG:
                        DoCast(me, SPELL_SEEPING_FOG, true);
                        events.ScheduleEvent(EVENT_SEEPING_FOG, 16000);
                        break;
                    case EVENT_TAIL_LASH:
                        DoCast(SPELL_TAIL_LASH);
                        events.ScheduleEvent(EVENT_TAIL_LASH, 5000);
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_dragon_taerarAI (creature);
    }
};

//102682
class boss_dragon_lethon : public CreatureScript
{
public:
    boss_dragon_lethon() : CreatureScript("boss_dragon_lethon") {}

    struct boss_dragon_lethonAI : public ScriptedAI
    {
        boss_dragon_lethonAI(Creature* creature) : ScriptedAI(creature) 
        {
            instance = me->GetInstanceScript();
            me->SetSpeed(MOVE_FLIGHT, 2.0f);
        }

        InstanceScript* instance;
        EventMap events;

        void Reset() override
        {
            me->SetMaxPower(POWER_ENERGY, 100);
            DoCast(me, SPELL_EMPTY_ENERGY, true);
        }

        void EnterCombat(Unit* who) override
        {
            DoCast(me, SPELL_ENERGIZE_UP, true);
            Talk(SAY_AGGRO);
            DoZoneInCombat(me, 150.0f);
        }

        void DefaultEvents()
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
            DoCast(me, SPELL_MARK_OF_LETHON, true);
            DoCast(me, SPELL_ENERGIZE_DRAGONS, true);
            DoCast(me, SPELL_GLOOM_AURA, true);
            events.ScheduleEvent(EVENT_CORRUPTED_BREATH, 16000);
            events.CancelEvent(EVENT_SHADOW_BURST);

            if (GetDifficultyID() == DIFFICULTY_MYTHIC_RAID)
                events.ScheduleEvent(EVENT_TAIL_LASH, 5000);
        }

        void JustDied(Unit* /*killer*/) override {}

        void DoAction(int32 const action) override
        {
            switch (action)
            {
                case ACTION_1:
                if (Unit* owner = me->GetAnyOwner())
                    if (Creature* summoner = owner->ToCreature())
                        summoner->AI()->DoAction(ACTION_2, me); //Leave group
                    events.Reset();
                    me->StopAttack();
                    SetFlyMode(true);
                    me->RemoveAurasDueToSpell(SPELL_MARK_OF_LETHON);
                    me->RemoveAurasDueToSpell(SPELL_ENERGIZE_DRAGONS);
                    me->RemoveAurasDueToSpell(SPELL_GLOOM_AURA);
                    me->GetMotionMaster()->MovePoint(1, dragonPos[1].GetPositionX(), dragonPos[1].GetPositionY(), dragonPos[1].GetPositionZ(), false);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    break;
                case ACTION_2:
                    me->SetVisible(true);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                    me->GetMotionMaster()->MovePoint(2, dragonPos[2].GetPositionX(), dragonPos[2].GetPositionY(), dragonPos[2].GetPositionZ(), false);
                    //No break!
                case ACTION_3:
                    DefaultEvents();
                    break;
                case ACTION_4: //Special Attack
                    events.ScheduleEvent(EVENT_SIPHON_SPIRIT, 100);
                    break;
            }
        }

        void SpellHitTarget(Unit* target, const SpellInfo* spell) override
        {
            if (spell->Id == SPELL_SIPHON_SPIRIT)
                target->CastSpell(target, SPELL_SIPHON_SPIRIT_SUM, true);
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (id == 1)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                me->SetFacingTo(2.47f);

                if (GetDifficultyID() == DIFFICULTY_HEROIC_RAID || GetDifficultyID() == DIFFICULTY_MYTHIC_RAID)
                    events.ScheduleEvent(EVENT_SHADOW_BURST, 15000);
            }
            else if (id == 2)
            {
                SetFlyMode(false);
                me->SetReactState(REACT_AGGRESSIVE);
                DoZoneInCombat(me, 150.0f);
            if (Unit* owner = me->GetAnyOwner())
                if (Creature* summoner = owner->ToCreature())
                {
                    me->SetHealth(summoner->GetHealth());
                    summoner->AI()->DoAction(ACTION_1, me); //Invite group
                }
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);
            DoAggroPulse(diff);

            if (me->GetReactState() == REACT_AGGRESSIVE && CheckHomeDistToEvade(diff, 100.0f, 576.33f, -12813.32f, 6.17f))
            {
                if (Creature* owner = me->GetAnyOwner()->ToCreature())
                    if (owner && owner->isInCombat())
                        owner->AI()->EnterEvadeMode();
                return;
            }

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_CORRUPTED_BREATH:
                        DoCastVictim(SPELL_CORRUPTED_BREATH);
                        events.ScheduleEvent(EVENT_CORRUPTED_BREATH, 32000);
                        break;
                    case EVENT_SIPHON_SPIRIT:
                        DoCast(SPELL_SIPHON_SPIRIT);
                        Talk(1);
                        Talk(2);
                        break;
                    case EVENT_SHADOW_BURST:
                        DoCast(me, SPELL_SHADOW_BURST, true);
                        events.ScheduleEvent(EVENT_SHADOW_BURST, 15000);
                        break;
                    case EVENT_TAIL_LASH:
                        DoCast(SPELL_TAIL_LASH);
                        events.ScheduleEvent(EVENT_TAIL_LASH, 5000);
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_dragon_lethonAI (creature);
    }
};

//102683
class boss_dragon_emeriss : public CreatureScript
{
public:
    boss_dragon_emeriss() : CreatureScript("boss_dragon_emeriss") {}

    struct boss_dragon_emerissAI : public ScriptedAI
    {
        boss_dragon_emerissAI(Creature* creature) : ScriptedAI(creature) 
        {
            instance = me->GetInstanceScript();
            me->SetSpeed(MOVE_FLIGHT, 2.0f);
        }

        InstanceScript* instance;
        EventMap events;

        void Reset() override
        {
            me->SetMaxPower(POWER_ENERGY, 100);
            DoCast(me, SPELL_EMPTY_ENERGY, true);
            DoCast(me, SPELL_NIGHTMARE_ENERGY, true);
        }

        void EnterCombat(Unit* who) override
        //52:25
        {
            DoCast(me, SPELL_ENERGIZE_UP, true);
            Talk(SAY_AGGRO);
            DoZoneInCombat(me, 150.0f);
        }

        void DefaultEvents()
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
            DoCast(me, SPELL_MARK_OF_EMERISS, true);
            DoCast(me, SPELL_ENERGIZE_DRAGONS, true);
            events.ScheduleEvent(EVENT_CORRUPTED_BREATH, 16000);
            events.ScheduleEvent(EVENT_ESSENCE_OF_CORRUPTION, 30000); //52.55

            if (GetDifficultyID() == DIFFICULTY_MYTHIC_RAID)
                events.ScheduleEvent(EVENT_TAIL_LASH, 5000);
        }

        void JustDied(Unit* /*killer*/) override {}

        void DoAction(int32 const action) override
        {
            switch (action)
            {
                case ACTION_1:
                if (Unit* owner = me->GetAnyOwner())
                    if (Creature* summoner = owner->ToCreature())
                        summoner->AI()->DoAction(ACTION_2, me); //Leave group
                    events.Reset();
                    me->StopAttack();
                    SetFlyMode(true);
                    me->RemoveAurasDueToSpell(SPELL_MARK_OF_EMERISS);
                    me->RemoveAurasDueToSpell(SPELL_ENERGIZE_DRAGONS);
                    me->GetMotionMaster()->MovePoint(1, dragonPos[1].GetPositionX(), dragonPos[1].GetPositionY(), dragonPos[1].GetPositionZ(), false);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    break;
                case ACTION_2:
                    me->SetVisible(true);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                    me->GetMotionMaster()->MovePoint(2, dragonPos[2].GetPositionX(), dragonPos[2].GetPositionY(), dragonPos[2].GetPositionZ(), false);
                    //No break!
                case ACTION_3:
                    DefaultEvents();
                    break;
                case ACTION_4: //Special Attack
                    events.ScheduleEvent(EVENT_VOLATILE_INFECTION, 100);
                    break;
            }
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (id == 1)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                me->SetFacingTo(2.47f);
            }
            else if (id == 2)
            {
                SetFlyMode(false);
                me->SetReactState(REACT_AGGRESSIVE);
                DoZoneInCombat(me, 150.0f);
            if (Unit* owner = me->GetAnyOwner())
                if (Creature* summoner = owner->ToCreature())
                {
                    me->SetHealth(summoner->GetHealth());
                    summoner->AI()->DoAction(ACTION_1, me); //Invite group
                }
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);
            DoAggroPulse(diff);

            if (me->GetReactState() == REACT_AGGRESSIVE && CheckHomeDistToEvade(diff, 100.0f, 576.33f, -12813.32f, 6.17f))
            {
            if (Unit* owner = me->GetAnyOwner())
                if (Creature* summoner = owner->ToCreature())
                    if (summoner->isInCombat())
                        summoner->AI()->EnterEvadeMode();
                return;
            }

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_CORRUPTED_BREATH:
                        DoCastVictim(SPELL_CORRUPTED_BREATH);
                        events.ScheduleEvent(EVENT_CORRUPTED_BREATH, 32000);
                        break;
                    case EVENT_VOLATILE_INFECTION:
                        DoCast(me, SPELL_VOLATILE_INFECTION, true);
                        break;
                    case EVENT_ESSENCE_OF_CORRUPTION:
                        DoCast(me, SPELL_ESSENCE_OF_CORRUPTION, true);
                        events.ScheduleEvent(EVENT_ESSENCE_OF_CORRUPTION, 30000);
                        break;
                    case EVENT_TAIL_LASH:
                        DoCast(SPELL_TAIL_LASH);
                        events.ScheduleEvent(EVENT_TAIL_LASH, 5000);
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_dragon_emerissAI (creature);
    }
};

//102804
class npc_ysondre_nightmare_bloom : public CreatureScript
{
public:
    npc_ysondre_nightmare_bloom() : CreatureScript("npc_ysondre_nightmare_bloom") {}

    struct npc_ysondre_nightmare_bloomAI : public ScriptedAI
    {
        npc_ysondre_nightmare_bloomAI(Creature* creature) : ScriptedAI(creature) 
        {
            me->SetReactState(REACT_PASSIVE);
        }

        uint32 despawnTimer = 0;

        void Reset() override {}

        void IsSummonedBy(Unit* summoner) override
        {
            DoCast(me, SPELL_NIGHTMARE_BLOOM_VISUAL, true);
            DoCast(me, SPELL_NIGHTMARE_BLOOM_DUMMY, true);
            DoCast(me, SPELL_NIGHTMARE_BLOOM_AT, true);
            DoCast(me, SPELL_NIGHTMARE_BLOOM_PERIODIC, true);

            uint8 playerCount = me->GetMap()->GetPlayersCountExceptGMs();

            if (GetDifficultyID() == DIFFICULTY_MYTHIC_RAID)
                despawnTimer = 30000;
            else if (playerCount < 20)
                despawnTimer = 20000;
            else if (playerCount >= 20 && playerCount < 30)
                despawnTimer = 40000;
        }

        void UpdateAI(uint32 diff) override
        {
            if (despawnTimer)
            {
                if (despawnTimer <= diff)
                {
                    despawnTimer = 0;
                    me->DespawnOrUnsummon(100);
                }
                else
                    despawnTimer -= diff;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_ysondre_nightmare_bloomAI(creature);
    }
};

//103080
class npc_ysondre_defiled_druid_spirit : public CreatureScript
{
public:
    npc_ysondre_defiled_druid_spirit() : CreatureScript("npc_ysondre_defiled_druid_spirit") {}

    struct npc_ysondre_defiled_druid_spiritAI : public ScriptedAI
    {
        npc_ysondre_defiled_druid_spiritAI(Creature* creature) : ScriptedAI(creature) 
        {
            me->SetReactState(REACT_PASSIVE);
        }

        uint16 explodeTimer;

        void Reset() override {}

        void IsSummonedBy(Unit* summoner) override
        {
            explodeTimer = 500;
            DoCast(me, SPELL_DEFILED_SPIRIT_ROOT, true);
        }

        void UpdateAI(uint32 diff) override
        {
            if (explodeTimer)
            {
                if (explodeTimer <= diff)
                {
                    explodeTimer = 0;
                    me->DespawnOrUnsummon(8000);
                    DoCast(SPELL_DEFILED_ERUPTION);
                }
                else
                    explodeTimer -= diff;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_ysondre_defiled_druid_spiritAI(creature);
    }
};

//103145
class npc_ysondre_shade_of_taerar : public CreatureScript
{
public:
    npc_ysondre_shade_of_taerar() : CreatureScript("npc_ysondre_shade_of_taerar") {}

    struct npc_ysondre_shade_of_taerarAI : public ScriptedAI
    {
        npc_ysondre_shade_of_taerarAI(Creature* creature) : ScriptedAI(creature) {}

        EventMap events;

        void Reset() override {}

        void IsSummonedBy(Unit* summoner) override
        {
            DoCast(me, SPELL_NIGHTMARE_VISAGE, true); //Scale
            DoZoneInCombat(me, 150.0f);
            events.ScheduleEvent(EVENT_1, 5000);
            if (GetDifficultyID() == DIFFICULTY_MYTHIC_RAID)
                events.ScheduleEvent(EVENT_TAIL_LASH, 5000);
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
                        DoCastVictim(SPELL_CORRUPTED_BREATH_2);
                        events.ScheduleEvent(EVENT_1, 30000);
                        break;
                    case EVENT_TAIL_LASH:
                        DoCast(SPELL_TAIL_LASH_2);
                        events.ScheduleEvent(EVENT_TAIL_LASH, 5000);
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_ysondre_shade_of_taerarAI(creature);
    }
};

//103100
class npc_ysondre_spirit_shade : public CreatureScript
{
public:
    npc_ysondre_spirit_shade() : CreatureScript("npc_ysondre_spirit_shade") {}

    struct npc_ysondre_spirit_shadeAI : public ScriptedAI
    {
        npc_ysondre_spirit_shadeAI(Creature* creature) : ScriptedAI(creature) 
        {
            instance = me->GetInstanceScript();
            me->SetSpeed(MOVE_RUN, 0.5f);
            me->SetReactState(REACT_PASSIVE);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
            me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
        }

        InstanceScript* instance;
        bool despawn;
        uint16 moveTimer;
        uint16 checkBossStateTimer;

        void Reset() override {}

        void IsSummonedBy(Unit* summoner) override
        {
            despawn = false;
            checkBossStateTimer = 1000;
            moveTimer = 500;

            summoner->CastSpell(me, 203840, true); //Clone Player
            DoCast(me, 203950, true); //Mod Scale
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (id == 1)
            {
                if (Creature* ysondre = instance->instance->GetCreature(instance->GetGuidData(NPC_YSONDRE)))
                {
                    if (ysondre && me->GetDistance(ysondre) < 5.0f)
                    {
                        if (!despawn)
                        {
                            despawn = true;
                            moveTimer = 0;
                            DoCast(me, 203897, true); //Heal
                            me->GetMotionMaster()->Clear();
                            me->DespawnOrUnsummon(1000);
                        }
                    }
                }
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (checkBossStateTimer)
            {
                if (checkBossStateTimer <= diff)
                {
                    checkBossStateTimer = 0;
                    despawn = true;
                    me->DespawnOrUnsummon(100);
                    return;
                }
                else checkBossStateTimer -= diff;
            }

            if (moveTimer)
            {
                if (moveTimer <= diff)
                {
                    moveTimer = 2000;
                    if (!me->HasUnitState(UNIT_STATE_STUNNED))
                    {
                        if (Creature* ysondre = instance->instance->GetCreature(instance->GetGuidData(NPC_YSONDRE)))
                        {
                            me->GetMotionMaster()->MovePoint(1, ysondre->GetPosition());
                        }
                    }
                }
                else
                    moveTimer -= diff;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_ysondre_spirit_shadeAI(creature);
    }
};

//103044
class npc_ysondre_dread_horror : public CreatureScript
{
public:
    npc_ysondre_dread_horror() : CreatureScript("npc_ysondre_dread_horror") {}

    struct npc_ysondre_dread_horrorAI : public ScriptedAI
    {
        npc_ysondre_dread_horrorAI(Creature* creature) : ScriptedAI(creature) {}

        uint16 unrelentingTimer;

        void Reset() override {}

        void IsSummonedBy(Unit* summoner) override
        {
            unrelentingTimer = 0;

            if (GetDifficultyID() == DIFFICULTY_HEROIC_RAID || GetDifficultyID() == DIFFICULTY_MYTHIC_RAID)
            {
                DoCast(me, SPELL_WASTING_DREAD_AT, true);

                if (GetDifficultyID() == DIFFICULTY_MYTHIC_RAID)
                    unrelentingTimer = 10000;
            }

            DoZoneInCombat(me, 100.0f);
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            if (unrelentingTimer)
            {
                if (unrelentingTimer <= diff)
                {
                    unrelentingTimer = 10000;
                    DoCast(me, SPELL_UNRELENTING, true);
                }
                else
                    unrelentingTimer -= diff;
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_ysondre_dread_horrorAI(creature);
    }
};

//103095, 103096, 103097
class npc_ysondre_corrupted_mushroom : public CreatureScript
{
public:
    npc_ysondre_corrupted_mushroom() : CreatureScript("npc_ysondre_corrupted_mushroom") {}

    struct npc_ysondre_corrupted_mushroomAI : public ScriptedAI
    {
        npc_ysondre_corrupted_mushroomAI(Creature* creature) : ScriptedAI(creature)
        {
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC);
        }

        EventMap events;

        void Reset() override {}

        void IsSummonedBy(Unit* summoner) override
        {
            switch (me->GetEntry())
            {
                case NPC_CORRUPTED_MUSHROOM_SMALL:
                    events.ScheduleEvent(EVENT_1, 500);
                    break;
                case NPC_CORRUPTED_MUSHROOM_MEDIUM:
                    events.ScheduleEvent(EVENT_2, 500);
                    break;
                case NPC_CORRUPTED_MUSHROOM_BIG:
                    events.ScheduleEvent(EVENT_3, 500);
                    break;
            }
            events.ScheduleEvent(EVENT_4, 5000);
        }

        void UpdateAI(uint32 diff) override
        {
            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_1:
                        DoCast(me, SPELL_CORRUPTED_BURST_4, false);
                        break;
                    case EVENT_2:
                        DoCast(me, SPELL_CORRUPTED_BURST_7, false);
                        break;
                    case EVENT_3:
                        DoCast(me, SPELL_CORRUPTED_BURST_10, false);
                        break;
                    case EVENT_4:
                        me->DespawnOrUnsummon(100);
                        break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_ysondre_corrupted_mushroomAI(creature);
    }
};

//205281, 205282
class spell_ysondre_periodic_energize : public SpellScriptLoader
{
public:
    spell_ysondre_periodic_energize() : SpellScriptLoader("spell_ysondre_periodic_energize") { }

    class spell_ysondre_periodic_energize_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_ysondre_periodic_energize_AuraScript);

        bool fullPower = false;
        uint8 PowerCount;

        void OnTick(AuraEffect const* aurEff)
        {
            Creature* caster = GetCaster()->ToCreature();
            if (!caster || !caster->isInCombat())
                return;

            PowerCount = caster->GetPower(POWER_ENERGY);

            if (PowerCount < 100)
            {
                caster->SetPower(POWER_ENERGY, PowerCount + 2);
                if (fullPower)
                    fullPower = false;
            }
            else
            {
                if (!fullPower)
                {
                    fullPower = true;
                    if (GetCaster()->GetEntry() == NPC_YSONDRE)
                        caster->AI()->DoAction(ACTION_4, nullptr);
                    else
                        caster->AI()->DoAction(ACTION_4);
                }
            }
        }

        void Register() override
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_ysondre_periodic_energize_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_ysondre_periodic_energize_AuraScript();
    }
};

//203102,203121,203124,203125
class spell_ysondre_marks : public SpellScriptLoader
{
public:
    spell_ysondre_marks() : SpellScriptLoader("spell_ysondre_marks") { }

    class spell_ysondre_marks_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_ysondre_marks_AuraScript);

        void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            if (!GetTarget())
                return;

            if (Aura* aura = aurEff->GetBase())
                if (aura->GetStackAmount() == 10)
                    GetTarget()->CastSpell(GetTarget(), SPELL_SLUMBERING_NIGHTMARE, true);
        }

        void Register() override
        {
            OnEffectApply += AuraEffectApplyFn(spell_ysondre_marks_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAPPLY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_ysondre_marks_AuraScript();
    }
};

//203110
class spell_ysondre_slumbering_nightmare : public SpellScriptLoader
{
    public:
        spell_ysondre_slumbering_nightmare() : SpellScriptLoader("spell_ysondre_slumbering_nightmare") {}

        class spell_ysondre_slumbering_nightmare_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ysondre_slumbering_nightmare_AuraScript);

            void CalculateMaxDuration(int32& duration)
            {
                if (!GetCaster())
                    return;

                if (GetCaster()->GetMap()->GetDifficultyID() == DIFFICULTY_LFR_RAID)
                    duration = 10000;
                else
                    duration = 30000;
            }

            void Register() override
            {
                DoCalcMaxDuration += AuraCalcMaxDurationFn(spell_ysondre_slumbering_nightmare_AuraScript::CalculateMaxDuration);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_ysondre_slumbering_nightmare_AuraScript();
        }
};

//203686
class spell_ysondre_nightmare_bloom : public SpellScriptLoader
{
    public:
        spell_ysondre_nightmare_bloom() : SpellScriptLoader("spell_ysondre_nightmare_bloom") { }

        class spell_ysondre_nightmare_bloom_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ysondre_nightmare_bloom_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (targets.empty())
                    if (GetCaster() && GetCaster()->GetAnyOwner())
                        if (Unit* summoner = GetCaster()->GetAnyOwner())
                            GetCaster()->CastSpell(GetCaster(), 203667, true, 0, 0, summoner->GetGUID());
            }

            void HandleOnHit()
            {
                if (GetCaster() && GetHitUnit())
                    GetCaster()->CastSpell(GetHitUnit(), 203690, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ysondre_nightmare_bloom_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnHit += SpellHitFn(spell_ysondre_nightmare_bloom_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_ysondre_nightmare_bloom_SpellScript();
        }
};

//204040, 204044
class spell_ysondre_shadow_burst : public SpellScriptLoader
{
    public:
        spell_ysondre_shadow_burst() : SpellScriptLoader("spell_ysondre_shadow_burst") { }

        class spell_ysondre_shadow_burst_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ysondre_shadow_burst_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (GetCaster() && GetSpellInfo()->Id == 204044)
                    targets.sort(Trinity::UnitSortDistance(true, GetCaster()));
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ysondre_shadow_burst_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ysondre_shadow_burst_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ALLY);
            }
        };

        class spell_ysondre_shadow_burst_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ysondre_shadow_burst_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (!GetCaster() || !GetTarget() || GetId() != 204040)
                    return;

                if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
                    GetTarget()->CastSpell(GetTarget(), 204044, true, 0, 0, GetCaster()->GetGUID());
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_ysondre_shadow_burst_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_ysondre_shadow_burst_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_ysondre_shadow_burst_AuraScript();
        }
};

//205172
class spell_ysondre_bellowing_roar : public SpellScriptLoader
{
    public:
        spell_ysondre_bellowing_roar() : SpellScriptLoader("spell_ysondre_bellowing_roar") { }
 
        class spell_ysondre_bellowing_roar_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ysondre_bellowing_roar_AuraScript)

            void OnPeriodic(AuraEffect const* aurEff)
            {
                if (!GetCaster())
                    return;

                //GetCaster()->AI()->Talk();
                GetCaster()->CastSpell(GetCaster(), SPELL_BELLOWING_ROAR);
            }
 
            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_ysondre_bellowing_roar_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };
 
        AuraScript* GetAuraScript() const override
        {
            return new spell_ysondre_bellowing_roar_AuraScript();
        }
};

void AddSC_boss_dragons_of_nightmare()
{
    new boss_dragon_ysondre();
    new boss_dragon_taerar();
    new boss_dragon_lethon();
    new boss_dragon_emeriss();
    new npc_ysondre_nightmare_bloom();
    new npc_ysondre_defiled_druid_spirit();
    new npc_ysondre_shade_of_taerar();
    new npc_ysondre_spirit_shade();
    new npc_ysondre_dread_horror();
    new npc_ysondre_corrupted_mushroom();
    new spell_ysondre_shadow_burst();
    new spell_ysondre_periodic_energize();
    new spell_ysondre_marks();
    new spell_ysondre_slumbering_nightmare();
    new spell_ysondre_nightmare_bloom();
    new spell_ysondre_bellowing_roar();
}
