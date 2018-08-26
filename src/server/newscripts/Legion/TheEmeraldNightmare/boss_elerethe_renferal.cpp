/*
    https://uwow.biz/
*/

#include "the_emerald_nightmare.h"
#include "Packets/UpdatePackets.h"

enum Says
{
    SAY_FEEDING_TIME        = 0,
    SAY_GATHERING_CLOUDS    = 1,
    SAY_RAZOR_WING          = 2,
    SAY_AGGRO               = 3,
    SAY_FEEDING_TIME_1      = 4,
    SAY_GATHERING_CLOUDS_1  = 5,
    SAY_RAZOR_WING_1        = 6,
};

enum Spells
{
    SPELL_SPIDER_START                  = 210284, //Не используется на билде выше 21737?
    SPELL_ENERGY_TRACKER_TRANSFORM      = 225364,
    SPELL_ENERGY_TRANSFORM_COST         = 210366,
    SPELL_TRANSFORM_DISSOLVE            = 210309,
    SPELL_WEB_WRAPPED                   = 215188, //?
    SPELL_BERSERK                       = 26662,

    //Phase One - Spider form
    SPELL_SPIDER_TRANSFORM_DUMMY        = 210326, //226020?
    SPELL_SPIDER_TRANSFORM              = 210292,
    SPELL_WEB_OF_PAIN_FILTER            = 215288,
    SPELL_FEEDING_TIME                  = 212364,
    SPELL_FEEDING_TIME_2                = 214305, //?
    SPELL_FEEDING_TIME_FILTER           = 214311,
    SPELL_FEEDING_TIME_SUM_STALKER      = 214275, //NPC_VILE_AMBUSH_STALKER
    SPELL_VILE_AMBUSH_FILTER            = 214328,
    SPELL_VILE_AMBUSH_JUMP              = 214339,
    SPELL_VILE_AMBUSH_VISUAL_JUMP       = 215253,
    SPELL_NECROTIC_VENOM                = 215443,

    //Heroic
    SPELL_VENOMOUS_POOL_AOE             = 213781,

    //Mythic
    SPELL_PAIN_LASH                     = 215306,

    //Phase Second - Darkwing form
    SPELL_DARKWING_FORM_DUMMY           = 210308,
    SPELL_DARKWING_FORM                 = 210293,
    SPELL_TWISTING_SHADOWS_FILTER       = 210864,
    SPELL_TWISTING_SHADOWS_AT           = 210792, //casters: 106350, 111439
    SPELL_GATHERING_CLOUDS              = 212707,
    SPELL_SHIMMERING_FEATHER_TRIG       = 213064,
    SPELL_RAZOR_WING_FILTER             = 210553,
    SPELL_RAZOR_WING                    = 210547,
    SPELL_RAZOR_WING_VISUAL             = 212842,
    SPELL_RAZOR_WING_DMG                = 212853,
    SPELL_DARK_STORM_FILTER             = 210948,
    SPELL_DARK_STORM_AURA               = 211122,
    SPELL_DARK_STORM_AT                 = 211124,
    SPELL_RAKING_TALONS                 = 215582,
    SPELL_RAKING_TALONS_2               = 222037,
    SPELL_FOUL_WINDS                    = 211137,
    //Mythic
    SPELL_VIOLENT_WINDS                 = 218124,

    //Other
    SPELL_DRUID_TRANSFORM               = 226017, //?
    SPELL_SPIDER_TRANSFORM_2            = 228523, //На текущем билде (21737) его нет.
    SPELL_SHIMMERING_FEATHER            = 212993,
    SPELL_SHIMMERING_FEATHER_OVERRIDE   = 221913,
    SPELL_SWARM                         = 214450, //Mythic buff boss
};

enum eEvents
{
    //Phase One - Spider form
    EVENT_WEB_OF_PAIN               = 1,
    EVENT_FEEDING_TIME              = 2,
    EVENT_NECROTIC_VENOM            = 3,
    EVENT_SPIDER_TRANSFORM          = 4,

    //Phase Second - Darkwing form
    EVENT_DARKWING_FORM             = 5,
    EVENT_TWISTING_SHADOWS          = 6,
    EVENT_GATHERING_CLOUDS          = 7,
    EVENT_SUM_SHIMMERING_FEATHER    = 8,
    EVENT_RAZOR_WING                = 9,
    EVENT_DARK_STORM                = 10,
    EVENT_RAKING_TALONS             = 11,
    EVENT_RAKING_TALONS_2           = 12,

    EVENT_REACT_AGGRESSIVE          = 13,
    EVENT_NEXT_PLATFORM             = 14,
    EVENT_CHECH_PLAYER              = 15,
};

enum eTransform
{
    SPIDER_FORM                 = 1,
    DARKWING_FORM               = 2
};

Position const platformPos[3] =
{
    {11405.59f, 11400.70f, -85.32f},
    {11376.42f, 11224.97f, -102.0f},
    {11538.02f, 11298.43f, -87.06f}
};

Position const spiderPos[12] =
{
    {11411.42f, 11402.46f, -51.12f}, //Platform 1
    {11413.96f, 11369.06f, -50.60f},
    {11434.47f, 11374.71f, -51.50f},
    {11373.11f, 11388.3f,  -50.58f},
    {11343.57f, 11240.51f, -44.54f}, //Platform 2
    {11382.04f, 11262.68f, -53.15f},
    {11403.85f, 11224.34f, -51.75f},
    {11377.61f, 11199.40f, -45.50f},
    {11537.53f, 11264.99f, -47.43f}, //Platform 3
    {11503.03f, 11288.73f, -45.12f},
    {11522.21f, 11323.06f, -43.80f},
    {11552.95f, 11302.63f, -38.93f}
};

Position const twistBridgePos[5] =
{
    {11362.16f, 11372.33f, -87.34f}, //NPC_TWISTING_SHADOWS_BRIDGE
    {11449.68f, 11389.96f, -85.37f},
    {11365.16f, 11308.75f, -98.53f},
    {11491.63f, 11268.83f, -89.58f},
    {11456.38f, 11238.37f, -93.77f}
};

//106087
class boss_elerethe_renferal : public CreatureScript
{
public:
    boss_elerethe_renferal() : CreatureScript("boss_elerethe_renferal") {}

    struct boss_elerethe_renferalAI : public BossAI
    {
        boss_elerethe_renferalAI(Creature* creature) : BossAI(creature, DATA_RENFERAL) 
        {
            me->SetSpeed(MOVE_FLIGHT, 4.0f);
            introDone = false;
        }

        uint8 platformId;
        uint16 checkEvadeTimer;
        uint32 berserkTimer;
        bool introDone;

        void Reset() override
        {
            _Reset();
            me->SetPower(POWER_ENERGY, 0);
            RemoveEventTrash();
            me->RemoveAllAuras();
            me->SetReactState(REACT_DEFENSIVE);
            me->GetMotionMaster()->MoveIdle();
            me->NearTeleportTo(11405.6f, 11400.7f, -85.32f, 4.62f); //HomePos
            SetAnim(0);
            platformId = 0;
            checkEvadeTimer = 2000;
        }
        
        void MoveInLineOfSight(Unit* who) override
        {              
            if (who->GetTypeId() != TYPEID_PLAYER)
                return;

            if (!introDone && me->IsWithinDistInMap(who, 250.0f))
            {
                introDone = true;
                Conversation* conversation = new Conversation;
                if (!conversation->CreateConversation(sObjectMgr->GetGenerator<HighGuid::Conversation>()->Generate(), 3626, who, nullptr, who->GetPosition()))
                    delete conversation;
            }
        }

        void EnterCombat(Unit* /*who*/) override
        {
            _EnterCombat();
            me->SetReactState(REACT_AGGRESSIVE);
            DoCast(me, SPELL_ENERGY_TRACKER_TRANSFORM, true);
            DoCast(me, SPELL_WEB_WRAPPED, true);
            berserkTimer = 9 * MINUTE * IN_MILLISECONDS;

            EventsForm(SPIDER_FORM);
            Talk(SAY_AGGRO);
        }

        void JustDied(Unit* /*killer*/) override
        {
            SetAnim(0);
            RemoveEventTrash();
            _JustDied();
        }

        void EnterEvadeMode() override
        {
            BossAI::EnterEvadeMode();
        }

        void RemoveEventTrash()
        {
            instance->DoRemoveAurasDueToSpellOnPlayers(215464);
            instance->DoRemoveAurasDueToSpellOnPlayers(215460);
            instance->DoRemoveAurasDueToSpellOnPlayers(215449);

            std::list<AreaTrigger*> list;
            me->GetAreaTriggerListWithEntryInGrid(list, 11547, 222.0f); //NECROTIC_VENOM - 213123
            if(!list.empty())
                for (std::list<AreaTrigger*>::iterator itr = list.begin(); itr != list.end(); ++itr)
                    if (AreaTrigger* areaObj = (*itr))
                        areaObj->Despawn();

            std::list<Creature*> creatureList;
            me->GetCreatureListWithEntryInGrid(creatureList, NPC_TWISTING_SHADOWS, 222.0f);
            for (auto creture : creatureList)
                creture->DespawnOrUnsummon();
        }

        void SetAnim(uint8 Id)
        {
            WorldPackets::Update::SetAnimTimer anim;
            anim.Tier = Id;
            anim.Unit = me->GetGUID();
            me->SendMessageToSet(anim.Write(), true);
        }

        void EventsForm(uint8 form)
        {
            switch (form)
            {
                case SPIDER_FORM:
                    events.ScheduleEvent(EVENT_WEB_OF_PAIN, 6000);
                    events.ScheduleEvent(EVENT_NECROTIC_VENOM, 12000);
                    events.ScheduleEvent(EVENT_FEEDING_TIME, 16000);
                    break;
                case DARKWING_FORM:
                    events.ScheduleEvent(EVENT_TWISTING_SHADOWS, 7000);
                    events.ScheduleEvent(EVENT_GATHERING_CLOUDS, 16000);
                    events.ScheduleEvent(EVENT_DARK_STORM, 26000);
                    events.ScheduleEvent(EVENT_RAZOR_WING, 59000);
                    break;
            }
        }

        void SpellHit(Unit* caster, const SpellInfo* spell) override
        {
            switch (spell->Id)
            {
                case SPELL_FEEDING_TIME:
                    checkEvadeTimer = 15000;
                    me->StopAttack();
                    me->SetDisableGravity(true);
                    me->GetMotionMaster()->MoveTakeoff(1, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 32.0f);
                    break;
                case SPELL_ENERGY_TRANSFORM_COST:
                    events.Reset();
                    me->StopAttack();
                    me->RemoveAurasDueToSpell(SPELL_ENERGY_TRACKER_TRANSFORM);
                    if (me->HasAura(SPELL_DARKWING_FORM))
                        events.ScheduleEvent(EVENT_SPIDER_TRANSFORM, 0);
                    else
                        events.ScheduleEvent(EVENT_DARKWING_FORM, 0);
                    break;
                case SPELL_SPIDER_TRANSFORM_DUMMY:
                case SPELL_DARKWING_FORM_DUMMY:
                    events.ScheduleEvent(EVENT_REACT_AGGRESSIVE, 5000);
                    break;
            }
        }

        void SpellHitTarget(Unit* target, const SpellInfo* spell) override
        {
            switch (spell->Id)
            {
                case SPELL_FEEDING_TIME_FILTER:
                    DoCast(target, SPELL_FEEDING_TIME_SUM_STALKER, true);
                    me->GetMotionMaster()->MoveTakeoff(2, target->GetPositionX(), target->GetPositionY(), me->GetPositionZ());
                    break;
                case SPELL_VILE_AMBUSH_FILTER:
                    DoCast(me, SPELL_VILE_AMBUSH_VISUAL_JUMP, true);
                    DoCast(target, SPELL_VILE_AMBUSH_JUMP, true);
                    break;
                case SPELL_RAZOR_WING_FILTER:
                    me->StopAttack();
                    me->SetFacingToObject(target);
                    DoCast(target, SPELL_RAZOR_WING);
                    events.ScheduleEvent(EVENT_REACT_AGGRESSIVE, 5000);
                    break;
                case SPELL_RAZOR_WING:
                    DoCast(target, SPELL_RAZOR_WING_VISUAL, true);
                    DoCast(target, SPELL_RAZOR_WING_DMG, true);
                    break;
            }
        }

        void OnAreaTriggerDespawn(uint32 spellId, Position pos, bool duration) override
        {
            if (duration)
                return;

            //Venomous Pool AOE
            if (spellId == 213123 || spellId == 222925 || spellId == 225519)
                if (GetDifficultyID() == DIFFICULTY_HEROIC_RAID || GetDifficultyID() == DIFFICULTY_MYTHIC_RAID)
                    DoCast(me, SPELL_VENOMOUS_POOL_AOE, true);
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type == EFFECT_MOTION_TYPE)
            {
                switch (id)
                {
                    case 1:
                    {
                        uint8 spiderPosId = 0;
                        if (platformId == 1)
                            spiderPosId = 4;
                        else if (platformId == 2)
                            spiderPosId = 8;
                        for (uint8 i = spiderPosId; i < 4 + spiderPosId; i++)
                            me->SummonCreature(NPC_VENOMOUS_SPIDERLING, spiderPos[i]);
                        DoCast(me, SPELL_FEEDING_TIME_FILTER, true);
                        break;
                    }
                    case 2:
                        DoCast(me, SPELL_VILE_AMBUSH_FILTER, true);
                        break;
                    case 214346:
                        SetAnim(0);
                        me->SetDisableGravity(false);
                        me->SetReactState(REACT_AGGRESSIVE);
                        break;
                }
            }
            if (type == WAYPOINT_MOTION_TYPE)
            {
                if (id == 4)
                {
                    SetFlyMode(false);
                    DoCast(me, SPELL_DARK_STORM_AT, true);
                    checkEvadeTimer = 2000;
                    events.ScheduleEvent(EVENT_CHECH_PLAYER, 5000);
                }
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (berserkTimer)
            {
                if (berserkTimer <= diff)
                {
                    berserkTimer = 0;
                    DoCast(me, SPELL_BERSERK, true);
                }
                else
                    berserkTimer -= diff;
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (checkEvadeTimer)
            {
                if (checkEvadeTimer <= diff)
                {
                    checkEvadeTimer = 2000;
                    if (me->GetDistance(platformPos[platformId]) > 45.0f)
                    {
                        Talk(11); //Platform Evade
                        EnterEvadeMode();
                        return;
                    }
                }
                else
                    checkEvadeTimer -= diff;
            }

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_REACT_AGGRESSIVE:
                        me->SetReactState(REACT_AGGRESSIVE);
                        if (!me->HasAura(SPELL_ENERGY_TRACKER_TRANSFORM))
                            DoCast(me, SPELL_ENERGY_TRACKER_TRANSFORM, true);
                        break;
                    case EVENT_SPIDER_TRANSFORM:
                        me->RemoveAurasDueToSpell(SPELL_DARKWING_FORM);
                        me->RemoveAurasDueToSpell(SPELL_DARK_STORM_AURA);
                        me->RemoveAreaObject(SPELL_DARK_STORM_AT);
                        DoCast(me, SPELL_SPIDER_TRANSFORM, true);
                        DoCast(SPELL_SPIDER_TRANSFORM_DUMMY);
                        EventsForm(SPIDER_FORM);
                        break;
                    case EVENT_DARKWING_FORM:
                        me->RemoveAurasDueToSpell(SPELL_SPIDER_TRANSFORM);
                        DoCast(me, SPELL_DARKWING_FORM, true);
                        DoCast(SPELL_DARKWING_FORM_DUMMY);
                        EventsForm(DARKWING_FORM);
                        break;
                    case EVENT_WEB_OF_PAIN:
                        DoCast(SPELL_WEB_OF_PAIN_FILTER);
                        break;
                    case EVENT_FEEDING_TIME:
                        Talk(SAY_FEEDING_TIME);
                        Talk(SAY_FEEDING_TIME_1);
                        DoCast(SPELL_FEEDING_TIME);
                        events.ScheduleEvent(EVENT_FEEDING_TIME, 50000);
                        break;
                    case EVENT_NECROTIC_VENOM:
                        DoCast(SPELL_NECROTIC_VENOM);
                        events.ScheduleEvent(EVENT_NECROTIC_VENOM, 22000);
                        break;
                    case EVENT_TWISTING_SHADOWS:
                        DoCast(SPELL_TWISTING_SHADOWS_FILTER);
                        events.ScheduleEvent(EVENT_TWISTING_SHADOWS, 40000);
                        break;
                    case EVENT_GATHERING_CLOUDS:
                        Talk(SAY_GATHERING_CLOUDS);
                        Talk(SAY_GATHERING_CLOUDS_1);
                        DoCast(SPELL_GATHERING_CLOUDS);
                        events.ScheduleEvent(EVENT_SUM_SHIMMERING_FEATHER, 8000);
                        break;
                    case EVENT_SUM_SHIMMERING_FEATHER:
                    {
                        Position pos;
                        float angle = -1.0f;
                        for (uint8 i = 0; i < 8; i++)
                        {
                            me->GetNearPosition(pos, 15.0f, angle);
                            me->CastSpell(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), SPELL_SHIMMERING_FEATHER_TRIG, true);
                            angle += 0.25f;
                        }
                        break;
                    }
                    case EVENT_RAZOR_WING:
                        Talk(SAY_RAZOR_WING);
                        Talk(SAY_RAZOR_WING_1);
                        DoCast(me, SPELL_RAZOR_WING_FILTER, true);
                        events.ScheduleEvent(EVENT_RAZOR_WING, 32000);
                        break;
                    case EVENT_DARK_STORM:
                        for (uint8 i = 0; i < 5; i++)
                            me->SummonCreature(NPC_TWISTING_SHADOWS_BRIDGE, twistBridgePos[i]);
                        DoCast(SPELL_DARK_STORM_FILTER);
                        events.ScheduleEvent(EVENT_NEXT_PLATFORM, 5000);
                        break;
                    case EVENT_RAKING_TALONS:
                        DoCastVictim(SPELL_RAKING_TALONS);
                        events.ScheduleEvent(EVENT_RAKING_TALONS, 30000);
                        events.ScheduleEvent(EVENT_RAKING_TALONS_2, 4000);
                        break;
                    case EVENT_RAKING_TALONS_2:
                        DoCastVictim(SPELL_RAKING_TALONS_2);
                        break;
                    case EVENT_NEXT_PLATFORM:
                        me->StopAttack();
                        checkEvadeTimer = 0;
                        DoCast(me, SPELL_DARK_STORM_AURA, true);
                        SetFlyMode(true);
                        me->GetMotionMaster()->MovePath(me->GetEntry() * 100 + platformId, false);
                        platformId++;
                        if (platformId == 3)
                            platformId = 0;
                        break;
                    case EVENT_CHECH_PLAYER:
                        if (Unit* target = SelectTarget(SELECT_TARGET_NEAREST, 0, 30.0f, true))
                        {
                            me->SetReactState(REACT_AGGRESSIVE);
                            events.ScheduleEvent(EVENT_RAKING_TALONS, 7000);
                        }
                        else
                            events.ScheduleEvent(EVENT_CHECH_PLAYER, 1000);
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_elerethe_renferalAI (creature);
    }
};

//107459
class npc_elerethe_venomous_spiderling : public CreatureScript
{
public:
    npc_elerethe_venomous_spiderling() : CreatureScript("npc_elerethe_venomous_spiderling") {}

    struct npc_elerethe_venomous_spiderlingAI : public ScriptedAI
    {
        npc_elerethe_venomous_spiderlingAI(Creature* creature) : ScriptedAI(creature) 
        {
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
        }

        EventMap events;

        void Reset() override
        {
            events.Reset();
        }

        void IsSummonedBy(Unit* summoner) override
        {
            me->SetDisableGravity(true);
            events.ScheduleEvent(EVENT_1, 500);
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type != POINT_MOTION_TYPE)
                return;

            me->SetDisableGravity(false);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
            me->SetReactState(REACT_AGGRESSIVE);
            DoZoneInCombat(me, 120.0f);

            if (GetDifficultyID() == DIFFICULTY_MYTHIC_RAID)
                DoCast(me, SPELL_SWARM, true); //Swarm
        }

        void JustDied(Unit* /*killer*/) override
        {
            if (me->ToTempSummon())
                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                    me->CastSpell(me, 213123, true, NULL, NULL, summoner->GetGUID());
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim() && me->isInCombat())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_1:
                        me->GetMotionMaster()->MovePoint(1, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() - 32.0f);
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_elerethe_venomous_spiderlingAI(creature);
    }
};

//106350, 107767, 111439
class npc_elerethe_twisting_shadows : public CreatureScript
{
    public:
        npc_elerethe_twisting_shadows() : CreatureScript("npc_elerethe_twisting_shadows") {}

        struct npc_elerethe_twisting_shadowsAI : public ScriptedAI
        {
            npc_elerethe_twisting_shadowsAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            EventMap events;
            ObjectGuid playerGUID;

            void Reset() override {}

            void IsSummonedBy(Unit* summoner) override
            {
                if (me->GetEntry() == 107767) //Twisting Passenger
                    return;

                if (me->GetEntry() == NPC_TWISTING_SHADOWS_BRIDGE)
                {
                    for (uint8 i = 0; i < 5; i++)
                        if (me->GetDistance(twistBridgePos[i]) < 5.0f)
                            me->GetMotionMaster()->MovePath((me->GetEntry() * 100 + i), true);
                }
                DoCast(me, SPELL_TWISTING_SHADOWS_AT, true);
            }

            void PassengerBoarded(Unit* who, int8 /*seatId*/, bool apply) override
            {
                if (me->GetEntry() != 107767 || apply)
                    return;

                playerGUID = who->GetGUID();
                events.ScheduleEvent(EVENT_1, 300);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);
                
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_1:
                            if (Player* player = ObjectAccessor::GetPlayer(*me, playerGUID))
                                player->KnockbackFrom(me->GetPositionX(), me->GetPositionY(), 30, 10);
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_elerethe_twisting_shadowsAI(creature);
        }
};

enum EGmisc
{
    //SMSG_SET_MOVEMENT_ANIM_KIT
    ANIM_1 = 753,
    ANIM_2 = 2737,

    //SMSG_PLAY_ONE_SHOT_ANIM_KIT
    ANIM_3 = 9779,

    SPELL_EGG_DUMMY_1                   = 223438,
    SPELL_EGG_DUMMY_2                   = 215508,
    SPELL_SUM_SKITTERING_SPIDERLING     = 215505, //4x
};

//108540
class npc_elerethe_surging_egg_sac : public CreatureScript
{
public:
    npc_elerethe_surging_egg_sac() : CreatureScript("npc_elerethe_surging_egg_sac") {}

    struct npc_elerethe_surging_egg_sacAI : public ScriptedAI
    {
        npc_elerethe_surging_egg_sacAI(Creature* creature) : ScriptedAI(creature) 
        {
            instance = me->GetInstanceScript();
            me->SetReactState(REACT_PASSIVE);
            animSwitch = false;
        }

        InstanceScript* instance;
        EventMap events;
        bool animSwitch;

        void Reset() override
        {
            events.ScheduleEvent(EVENT_1, frand(1000, 10000));
            events.ScheduleEvent(EVENT_2, 500);
        }

        void SpellHit(Unit* caster, const SpellInfo* spell) override
        {
            if (spell->Id == SPELL_EGG_DUMMY_1)
            {
                DoCast(me, SPELL_EGG_DUMMY_2, true);
                me->PlayOneShotAnimKit(ANIM_3);
            }
            if (spell->Id == SPELL_EGG_DUMMY_2)
            {
                for (uint8 i = 0; i < 4; i++)
                {
                    Position pos;
                    me->GetRandomNearPosition(pos, 3.0f);
                    me->CastSpell(pos, SPELL_SUM_SKITTERING_SPIDERLING, true);
                }
                me->DespawnOrUnsummon(3000);
            }
        }

        void UpdateAI(uint32 diff) override
        {
            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_1:
                        me->SetMovementAnimKitId(animSwitch ? ANIM_1 : ANIM_2);
                        animSwitch = animSwitch ? false : true;
                        events.ScheduleEvent(EVENT_1, 2500);
                        break;
                    case EVENT_2:
                    {
                        if (instance->GetBossState(DATA_RENFERAL) == IN_PROGRESS)
                        {
                            if (Unit* plr = me->FindNearestPlayer(5.0f))
                                if (me->GetDistance(plr) < 2.0f)
                                {
                                    events.Reset();
                                    DoCast(me, SPELL_EGG_DUMMY_1, true);
                                    break;
                                }
                        }
                        events.ScheduleEvent(EVENT_2, 500);
                        break;
                    }
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_elerethe_surging_egg_sacAI(creature);
    }
};

//225364
class spell_elerethe_energy_tracker_transform : public SpellScriptLoader
{
public:
    spell_elerethe_energy_tracker_transform() : SpellScriptLoader("spell_elerethe_energy_tracker_transform") { }

    class spell_elerethe_energy_tracker_transform_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_elerethe_energy_tracker_transform_AuraScript);

        uint8 PowerCount;
        uint8 PowerTick = 0;

        void OnTick(AuraEffect const* aurEff)
        {
            Creature* caster = GetCaster()->ToCreature();
            if (!caster || !caster->isInCombat())
                return;

            PowerCount = caster->GetPower(POWER_ENERGY);

            if (PowerCount < 100)
            {
                if (PowerTick < 9)
                {
                    PowerTick++;
                    caster->SetPower(POWER_ENERGY, PowerCount + 1);
                }
                else
                {
                    PowerTick = 0;
                    caster->SetPower(POWER_ENERGY, PowerCount + 2);
                }
            }
            else if (!caster->HasUnitState(UNIT_STATE_CASTING))
                caster->CastSpell(caster, SPELL_ENERGY_TRANSFORM_COST);
        }

        void Register() override
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_elerethe_energy_tracker_transform_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_elerethe_energy_tracker_transform_AuraScript();
    }
};

//215288
class spell_elerethe_web_of_pain_filter : public SpellScriptLoader
{
public:
    spell_elerethe_web_of_pain_filter() : SpellScriptLoader("spell_elerethe_web_of_pain_filter") { }

    class spell_elerethe_web_of_pain_filter_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_elerethe_web_of_pain_filter_SpellScript);

        Unit* secondTarget;

        void FilterTargets(std::list<WorldObject*>& targetsList)
        {
            std::list<WorldObject*> tempList;

            if (targetsList.size() > 1)
            {
                secondTarget = nullptr;
                for (auto plrTarget : targetsList)
                {
                    if (Player* plr = plrTarget->ToPlayer())
                    {
                        if (plr->isInTankSpec())
                            tempList.push_front(plrTarget);
                        else
                        {
                            if (!secondTarget && plrTarget->ToUnit())
                                secondTarget = plrTarget->ToUnit();
                            else if (secondTarget && plrTarget->ToUnit())
                            {
                                secondTarget->CastSpell(plrTarget->ToUnit(), 215300, true);
                                plrTarget->ToUnit()->CastSpell(secondTarget, 215307, true);
                                break;
                            }
                        }
                    }
                }
            }

            if (tempList.size() > 1)
            {
                secondTarget = nullptr;
                for (auto tankTarget : tempList)
                {
                    if (!secondTarget && tankTarget->ToUnit())
                        secondTarget = tankTarget->ToUnit();
                    else if (secondTarget && tankTarget->ToUnit())
                    {
                        secondTarget->CastSpell(tankTarget->ToUnit(), 215300, true);
                        tankTarget->ToUnit()->CastSpell(secondTarget, 215307, true);
                        return;
                    }
                }
            }
        }

        void Register() override
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_elerethe_web_of_pain_filter_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_elerethe_web_of_pain_filter_SpellScript();
    }
};

//215300, 215307
class spell_elerethe_web_of_pain : public SpellScriptLoader
{
    public:
        spell_elerethe_web_of_pain() : SpellScriptLoader("spell_elerethe_web_of_pain") { }

        class spell_elerethe_web_of_pain_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_elerethe_web_of_pain_AuraScript);

            uint16 m_checkTimer = 500;

            void OnUpdate(uint32 diff, AuraEffect* aurEff)
            {
                if (m_checkTimer <= diff)
                {
                    m_checkTimer = 500;
                    if (!GetCaster() || !GetCaster()->isAlive())
                        aurEff->GetBase()->Remove();
                }
                else
                    m_checkTimer -= diff;
            }

            void OnTick(AuraEffect const* aurEff)
            {
                if (!GetCaster() || !GetTarget() || GetCaster()->GetMap()->GetDifficultyID() != DIFFICULTY_MYTHIC_RAID)
                    return;

                Map::PlayerList const& players = GetCaster()->GetMap()->GetPlayers();
                for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                {
                    if (Player* player = itr->getSource())
                    {
                        if (player->GetGUID() == GetCaster()->GetGUID() || player->GetGUID() == GetTarget()->GetGUID())
                            continue;

                        if (player->IsInBetween(GetCaster(), GetTarget(), 1.0f))
                            player->CastSpell(player, SPELL_PAIN_LASH, true);
                    }
                }
            }

            void Register() override
            {
                OnEffectUpdate += AuraEffectUpdateFn(spell_elerethe_web_of_pain_AuraScript::OnUpdate, EFFECT_0, SPELL_AURA_SHARE_DAMAGE_PCT);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_elerethe_web_of_pain_AuraScript::OnTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_elerethe_web_of_pain_AuraScript();
        }
};

//214348
class spell_elerethe_vile_ambush : public SpellScriptLoader
{
    public:
        spell_elerethe_vile_ambush() : SpellScriptLoader("spell_elerethe_vile_ambush") { }

        class spell_elerethe_vile_ambush_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_elerethe_vile_ambush_SpellScript);

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
                    perc = 20 - (distance / radius) * 30;

                uint32 damage = CalculatePct(GetHitDamage(), perc);
                SetHitDamage(damage);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_elerethe_vile_ambush_SpellScript::DealDamage);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_elerethe_vile_ambush_SpellScript();
        }
};

//214311
class spell_elerethe_feeding_time_filter : public SpellScriptLoader
{
    public:
        spell_elerethe_feeding_time_filter() : SpellScriptLoader("spell_elerethe_feeding_time_filter") { }

        class spell_elerethe_feeding_time_filter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_elerethe_feeding_time_filter_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targetsList)
            {
                std::list<WorldObject*> tempTargets;
                for (auto target : targetsList)
                    if (GetCaster()->IsWithinLOSInMap(target))
                        tempTargets.push_back(target);

                targetsList.clear();
                if (!tempTargets.empty())
                    targetsList = tempTargets;
                else
                {
                    if (Creature* caster = GetCaster()->ToCreature())
                    {
                        caster->AI()->Talk(10);
                        caster->AI()->EnterEvadeMode();
                    }
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_elerethe_feeding_time_filter_SpellScript::FilterTargets, EFFECT_0,TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_elerethe_feeding_time_filter_SpellScript();
        }
};

//215449
class spell_elerethe_necrotic_venom : public SpellScriptLoader
{
    public:
        spell_elerethe_necrotic_venom() : SpellScriptLoader("spell_elerethe_necrotic_venom") { }

        class spell_elerethe_necrotic_venom_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_elerethe_necrotic_venom_AuraScript);

            void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (!GetCaster() || !GetTarget())
                    return;

                if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
                {
                    GetTarget()->CastSpell(GetTarget(), GetSpellInfo()->Effects[EFFECT_0]->BasePoints, true, nullptr, nullptr, GetCaster()->GetGUID());
                }
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_elerethe_necrotic_venom_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_elerethe_necrotic_venom_AuraScript();
        }
};

//212993
class spell_elerethe_shimmering_feather_proc : public SpellScriptLoader
{
public:
    spell_elerethe_shimmering_feather_proc() : SpellScriptLoader("spell_elerethe_shimmering_feather_proc") { }

    class spell_elerethe_shimmering_feather_proc_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_elerethe_shimmering_feather_proc_AuraScript);

        void OnProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
        {
            if (GetCaster())
                GetCaster()->CastSpell(GetCaster(), SPELL_SHIMMERING_FEATHER_OVERRIDE, true);
        }

        void Register() override
        {
            OnEffectProc += AuraEffectProcFn(spell_elerethe_shimmering_feather_proc_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_elerethe_shimmering_feather_proc_AuraScript();
    }
};

void AddSC_boss_elerethe_renferal()
{
    new boss_elerethe_renferal();
    new npc_elerethe_venomous_spiderling();
    new npc_elerethe_twisting_shadows();
    new npc_elerethe_surging_egg_sac();
    new spell_elerethe_energy_tracker_transform();
    new spell_elerethe_web_of_pain_filter();
    new spell_elerethe_web_of_pain();
    new spell_elerethe_vile_ambush();
    new spell_elerethe_feeding_time_filter();
    new spell_elerethe_necrotic_venom();
    new spell_elerethe_shimmering_feather_proc();
}
