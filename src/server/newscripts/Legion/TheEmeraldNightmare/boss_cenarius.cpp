/*
    https://uwow.biz/
    To-DO: More texts from sniffs
*/

#include "the_emerald_nightmare.h"

enum Says
{
    SAY_AGGRO                     = 0,
    SAY_FORCES_OF_NIGHTMARE_EMOTE = 1,
    SAY_FORCES_OF_NIGHTMARE       = 2,
    SAY_NIGHTMARE_BRAMBLES        = 3,
    SAY_DEATH                     = 6,
};

enum Spells
{
    //Phase 1
    SPELL_CREEPING_NIGHTMARES           = 210280,
    SPELL_FORCES_OF_NIGHTMARE           = 212726,
    SPELL_NIGHTMARE_BRAMBLES_SUM        = 210290,
    SPELL_AURA_OF_DREAD_THORNS          = 210346,
    SPELL_SUM_ENTANGLING_NIGHTMARES     = 214454,

    //Phase 2
    SPELL_OVERWHELMING_NIGHTMARE        = 217368, //?
    SPELL_NIGHTMARE_BLAST_SAVER         = 214706,
    SPELL_SPEAR_OF_NIGHTMARES           = 214529,
    SPELL_NIGHTMARES                    = 214712, //? спелл от АТ?

    //Mythic
    SPELL_NIGHTMARE_BLAST               = 213162,
    SPELL_BEASTS_OF_NIGHTMARE           = 214876,

//Trash
    //Malfurion Stormrage - 106482
    SPELL_CLEANSING_GROUND_FILTER       = 212630,
    SPELL_CLEANSING_GROUND_AT           = 212639,
    SPELL_CLEANSING_GROUND_STAGE_2      = 214249,
    SPELL_STORMS_RAGE                   = 214713,

    //Nightmare Sapling - 106427
    SPELL_GERMINATING                   = 210861,
    SPELL_NIGHTMARE_GERMINATE           = 210974,
    SPELL_ALLIES_GERMINATE              = 212175,

    //Tormented Souls - 106895
    SPELL_TORMENTED                     = 212178,
    SPELL_WISPS_VISUAL_CORRUPTED        = 212466,
    SPELL_WISPS_VISUAL_ALLIES           = 212467,
    SPELL_SUM_NIGHTMARE_WISPS           = 212188,
    SPELL_SUM_ALLIES_WISPS              = 212192,

    //Corrupted Emerald Egg - 106898
    SPELL_INCUBATING                    = 212231,
    SPELL_SUM_NIGHTMARE_DRAKE           = 212233,
    SPELL_SUM_ALLIES_DRAKE              = 212235,

    //Corrupted Nature - 106899
    SPELL_DRYAD_DUMMY                   = 212241,
    SPELL_DRYAD_VISUAL                  = 212424,
    SPELL_SUM_NIGHTMARE_DRYAD           = 212242,
    SPELL_SUM_ALLIES_DRYAD              = 212243,

    //ALLIES_WISP - 106659
    SPELL_CLEANSING_DETONATE_VIS        = 213403,
    SPELL_CLEANSING_DETONATE_AT         = 211878,

    //CLEANSED_ANCIENT - 106667
    SPELL_NATURES_FURY                  = 215099,
    SPELL_REPLENISHING_ROOTS            = 211619,

    //EMERALD_DRAKE - 106809
    SPELL_ANCIENT_DREAM_FILTER          = 211935,
    SPELL_POISON_BLAST                  = 216510,

    //REDEEMED_SISTER - 106831
    SPELL_UNBOUND_TOUCH_FILTER          = 211976,
    SPELL_JAVELIN_TOSS                  = 215106,
    
    //CORRUPTED_WISP - 106304
    SPELL_DESTRUCTIVE_NIGHTMARES        = 210617,
    SPELL_DESTRUCTIVE_NIGHTMARES_DMG    = 210619,

    //NIGHTMARE_ANCIENT - 105468
    SPELL_DESICCATING_STOMP_DUMMY       = 226816,
    SPELL_DESICCATING_STOMP             = 211073,
    SPELL_DESICCATION                   = 211156,

    //ROTTEN_DRAKE - 105494
    SPELL_ROTTEN_BREATH_FILTER          = 211189,
    SPELL_ROTTEN_BREATH_SUM_TRIGGER     = 211188,
    SPELL_ROTTEN_BREATH_DMG             = 211192,
    SPELL_NIGHTMARE_BUFFET              = 211180,

    //NPC_TWISTED_SISTER - 105495
    SPELL_NIGHTMARE_JAVELIN             = 211498,
    SPELL_TWISTED_TOUCH_OF_LIFE_FILTER  = 211462,
    SPELL_SCORNED_TOUCH_FILTER          = 211487,

    //NIGHTMARE_BRAMBLES - 106167
    SPELL_NIGHTMARE_BRAMBLES_AT         = 210331,
    SPELL_NIGHTMARE_BRAMBLES_AT_PER     = 210312,
    SPELL_NIGHTMARE_BRAMBLES_UNKNOWN    = 210316, //?
    
    //ENTANGLING_ROOTS - 108040
    SPELL_ENTANGLING_NIGHTMARES_VIS     = 214460,
    SPELL_ENTANGLING_NIGHTMARES_AURA    = 214463,
    SPELL_ENTANGLING_NIGHTMARES_VEH     = 214462,
    SPELL_ENTANGLING_NIGHTMARES_STUN    = 214505,

    //BEAST_OF_NIGHTMARE - 108208
    SPELL_GRIPPING_FOG_AT               = 214878,

    //Other
    SPELL_NIGHTMARE_DISSOLVE_IN         = 212461,
    SPELL_NIGHTMARE_DISSOLVE_OUT        = 212563,
    SPELL_DUDE_TRACKER_AT               = 212622,
    SPELL_SUMMON_DRAKES_DUMMY           = 212604,
    SPELL_CLEANSED_GROUND               = 212170,
    SPELL_CAST_HELPFUL_SPELL            = 211639,
    SPELL_CLEANSED_DISSOLVE_OUT         = 212562,
    SPELL_DISSOLVE_CLEANSED_IN          = 210988,
    SPELL_NIGHTMARE_ENERGY              = 224200,
    SPELL_COSMETIC_CHANNEL              = 225782,
};

enum eEvents
{
    EVENT_FORCES_OF_NIGHTMARE       = 1,
    EVENT_NIGHTMARE_BRAMBLES        = 2,
    EVENT_OVERWHELMING_NIGHTMARE    = 3,
    EVENT_NIGHTMARE_BLAST_SAVER     = 4,
    EVENT_SPEAR_OF_NIGHTMARES       = 5,
    EVENT_CHECH_EVADE               = 6,
    EVENT_NIGHTMARE_BLAST_OR_BEAST  = 7, //Mythic

    EVENT_SCENE,
};

enum eVisualKit
{
    BRAMBLES_KIT_1                      = 67102, //SMSG_PLAY_SPELL_VISUAL_KIT
};

Position const forcesPos[4] =
{
    {11392.7f, -12723.7f, 487.0f},  //NPC_TORMENTED_SOULS
    {11339.5f, -12719.2f, 487.33f}, //NPC_NIGHTMARE_SAPLING
    {11340.0f, -12667.8f, 487.45f}, //NPC_CORRUPTED_NATURE
    {11391.2f, -12670.0f, 487.18f}, //NPC_CORRUPTED_EMERALD_EGG
};

Position const malfurionPos[1] =
{
    {11359.50f, -12665.90f, 487.30f, 4.79f}
};

Position const centrPos = {11366.2f, -12695.1f, 486.91f};

Position const beastsPos[9] =
{
    {11408.86f, -12738.13f, 488.3f, 2.42f}, //+
    {11405.04f, -12698.67f, 488.0f, 3.06f},
    {11409.42f, -12661.88f, 488.0f, 3.74f}, //+
    {11375.43f, -12660.12f, 487.0f, 4.64f},
    {11354.59f, -12653.14f, 488.3f, 4.86f},
    {11331.45f, -12661.74f, 488.4f, 5.38f}, //+
    {11328.11f, -12700.74f, 487.4f, 0.06f},
    {11335.28f, -12734.77f, 488.0f, 0.97f},
    {11366.40f, -12742.10f, 488.2f, 1.64f}, //+
};

//104636
class boss_cenarius : public CreatureScript
{
public:
    boss_cenarius() : CreatureScript("boss_cenarius") {}

    struct boss_cenariusAI : public BossAI
    {
        boss_cenariusAI(Creature* creature) : BossAI(creature, DATA_CENARIUS) 
        {
            DoCast(SPELL_COSMETIC_CHANNEL); // visual
        }

        bool firstConv;
        bool secondPhase;
        uint8 powerCount; //Hack
        uint8 powerTick;
        uint8 waveCount;
        uint16 powerTimer;

        void Reset() override
        {
            _Reset();
            secondPhase = false;
            powerTimer = 0;
            powerTick = 1;
            waveCount = 0;
            me->SetMaxPower(POWER_ENERGY, 100);
            me->SetPower(POWER_ENERGY, 90);
            DoCast(me, SPELL_NIGHTMARE_ENERGY, true);
            me->SummonCreature(NPC_MALFURION_STORMRAGE, malfurionPos[0]);
            me->SetReactState(REACT_AGGRESSIVE);
            firstConv = false;
        }

        void EnterCombat(Unit* /*who*/) override
        //01:13 | 49:30
        {
            _EnterCombat();
            DoCast(me, SPELL_CREEPING_NIGHTMARES, true);
            powerTimer = 1000;

            EntryCheckPredicate pred(NPC_MALFURION_STORMRAGE);
            summons.DoAction(ACTION_1, pred);

            events.ScheduleEvent(EVENT_NIGHTMARE_BRAMBLES, 25000);
            events.ScheduleEvent(EVENT_CHECH_EVADE, 2000);

            if (GetDifficultyID() == DIFFICULTY_MYTHIC_RAID)
                events.ScheduleEvent(EVENT_NIGHTMARE_BLAST_OR_BEAST, 30000);

            Talk(SAY_AGGRO);
        }

        void JustDied(Unit* /*killer*/) override
        {
            _JustDied();
            Talk(SAY_DEATH);
            if (Creature* malf = me->SummonCreature(NPC_MALFURION_STORMRAGE, me->GetPositionX() + 5.0f, me->GetPositionY() + 5.0f, me->GetPositionZ())) // outro
            {
                malf->SetFacingToObject(me);
                malf->AI()->DoAction(ACTION_4);
            }
        }

        void SpellHit(Unit* target, const SpellInfo* spell) override
        {
            if (spell->Id == SPELL_FORCES_OF_NIGHTMARE)
            {
                switch (waveCount)
                {
                    case 1:
                        me->SummonCreature(NPC_TORMENTED_SOULS, forcesPos[0], TEMPSUMMON_TIMED_DESPAWN, 13000);
                        me->SummonCreature(NPC_NIGHTMARE_SAPLING, forcesPos[1], TEMPSUMMON_TIMED_DESPAWN, 13000);
                        me->SummonCreature(NPC_CORRUPTED_EMERALD_EGG, forcesPos[3], TEMPSUMMON_TIMED_DESPAWN, 13000);
                        break;
                    case 2:
                        me->SummonCreature(NPC_TORMENTED_SOULS, forcesPos[0], TEMPSUMMON_TIMED_DESPAWN, 13000);
                        me->SummonCreature(NPC_CORRUPTED_NATURE, forcesPos[2], TEMPSUMMON_TIMED_DESPAWN, 13000);
                        me->SummonCreature(NPC_CORRUPTED_EMERALD_EGG, forcesPos[3], TEMPSUMMON_TIMED_DESPAWN, 13000);
                        break;
                    case 3:
                        me->SummonCreature(NPC_NIGHTMARE_SAPLING, forcesPos[1], TEMPSUMMON_TIMED_DESPAWN, 13000);
                        me->SummonCreature(NPC_CORRUPTED_NATURE, forcesPos[2], TEMPSUMMON_TIMED_DESPAWN, 13000);
                        me->SummonCreature(NPC_CORRUPTED_NATURE, forcesPos[3], TEMPSUMMON_TIMED_DESPAWN, 13000);
                        break;
                    case 4:
                        me->SummonCreature(NPC_TORMENTED_SOULS, forcesPos[0], TEMPSUMMON_TIMED_DESPAWN, 13000);
                        me->SummonCreature(NPC_CORRUPTED_EMERALD_EGG, forcesPos[2], TEMPSUMMON_TIMED_DESPAWN, 13000);
                        me->SummonCreature(NPC_CORRUPTED_EMERALD_EGG, forcesPos[3], TEMPSUMMON_TIMED_DESPAWN, 13000);
                        break;
                    case 5:
                        me->SummonCreature(NPC_NIGHTMARE_SAPLING, forcesPos[1], TEMPSUMMON_TIMED_DESPAWN, 13000);
                        me->SummonCreature(NPC_CORRUPTED_NATURE, forcesPos[2], TEMPSUMMON_TIMED_DESPAWN, 13000);
                        me->SummonCreature(NPC_CORRUPTED_NATURE, forcesPos[3], TEMPSUMMON_TIMED_DESPAWN, 13000);
                        break;
                    case 6:
                        me->SummonCreature(NPC_TORMENTED_SOULS, forcesPos[0], TEMPSUMMON_TIMED_DESPAWN, 13000);
                        me->SummonCreature(NPC_CORRUPTED_NATURE, forcesPos[2], TEMPSUMMON_TIMED_DESPAWN, 13000);
                        me->SummonCreature(NPC_CORRUPTED_NATURE, forcesPos[3], TEMPSUMMON_TIMED_DESPAWN, 13000);
                        break;
                }
                DoCast(me, SPELL_AURA_OF_DREAD_THORNS, true);
            }

            if (spell->Id == SPELL_BEASTS_OF_NIGHTMARE)
            {
                Position posLeft;
                Position posRight;

                std::list<uint8> randList;
                for (uint8 i = 0; i < 9; i++)
                    randList.push_back(i);

                Trinity::Containers::RandomResizeList(randList, 2);

                for (std::list<uint8>::iterator itr = randList.begin(); itr != randList.end(); ++itr)
                {
                    switch (*itr)
                    {
                        case 0:
                        case 2:
                        case 5:
                        case 8:
                        {
                            float dist = 1.5f;

                            for (uint8 i = 0; i < 3; i++)
                            {
                                beastsPos[*itr].SimplePosXYRelocationByAngle(posLeft, dist, M_PI/3); //Left
                                beastsPos[*itr].SimplePosXYRelocationByAngle(posRight, dist, -M_PI/3); //Right
                                me->SummonCreature(NPC_BEAST_OF_NIGHTMARE, posLeft.GetPositionX(), posLeft.GetPositionY(), posLeft.GetPositionZ(), beastsPos[*itr].GetOrientation());
                                me->SummonCreature(NPC_BEAST_OF_NIGHTMARE, posRight.GetPositionX(), posRight.GetPositionY(), posRight.GetPositionZ(), beastsPos[*itr].GetOrientation());
                                dist += 4;
                            }
                            break;
                        }
                        default:
                        {
                            float dist = 1.5f;

                            for (uint8 i = 0; i < 3; i++)
                            {
                                beastsPos[*itr].SimplePosXYRelocationByAngle(posLeft, dist, M_PI/2); //Left
                                beastsPos[*itr].SimplePosXYRelocationByAngle(posRight, dist, -M_PI/2); //Right
                                me->SummonCreature(NPC_BEAST_OF_NIGHTMARE, posLeft);
                                me->SummonCreature(NPC_BEAST_OF_NIGHTMARE, posRight);
                                dist += 4;
                            }
                            break;
                        }
                        break;
                    }
                }
            }
        }

        void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType dmgType) override
        {
            if (me->HealthBelowPct(31) && !secondPhase)
            {
                secondPhase = true;
                me->StopAttack();
                me->GetMotionMaster()->Clear();
                events.ScheduleEvent(EVENT_OVERWHELMING_NIGHTMARE, 100);
                events.ScheduleEvent(EVENT_SPEAR_OF_NIGHTMARES, 21000);
                EntryCheckPredicate pred(NPC_MALFURION_STORMRAGE);
                summons.DoAction(ACTION_3, pred);
                Conversation* conversation = new Conversation;
                if (!conversation->CreateConversation(sObjectMgr->GetGenerator<HighGuid::Conversation>()->Generate(), 3417, attacker, nullptr, attacker->GetPosition()))
                    delete conversation;
            }
            if (me->HealthBelowPct(65) && !firstConv)
            {
                firstConv = true;
                Conversation* conversation = new Conversation;
                if (!conversation->CreateConversation(sObjectMgr->GetGenerator<HighGuid::Conversation>()->Generate(), 3416, attacker, nullptr, attacker->GetPosition()))
                    delete conversation;
                events.ScheduleEvent(EVENT_SCENE, 20000);
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            if (powerTimer) //Хак! В снифе нет инфы о ауре.
            {
                if (powerTimer <= diff)
                {
                    powerTimer = 1000;
                    powerCount = me->GetPower(POWER_ENERGY);
                    if (powerCount < 100)
                    {
                        if (powerTick < 2)
                        {
                            powerTick++;
                            me->SetPower(POWER_ENERGY, powerCount + 1);
                        }
                        else
                        {
                            powerTick = 0;
                            me->SetPower(POWER_ENERGY, powerCount + 2);
                        }
                    }
                    else
                    {
                        if (!me->HasUnitState(UNIT_STATE_CASTING))
                            events.ScheduleEvent(EVENT_FORCES_OF_NIGHTMARE, 100);
                    }
                }
                else
                    powerTimer -= diff;
            }

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_FORCES_OF_NIGHTMARE:
                    {
                        if (!secondPhase && waveCount < 6)
                        {
                            waveCount++;
                            DoCast(SPELL_FORCES_OF_NIGHTMARE);
                            EntryCheckPredicate pred(NPC_MALFURION_STORMRAGE);
                            summons.DoAction(ACTION_2, pred);
                            Talk(SAY_FORCES_OF_NIGHTMARE_EMOTE);
                            Talk(SAY_FORCES_OF_NIGHTMARE);
                        }
                        else
                        {
                            if (Creature* malfurion = me->FindNearestCreature(NPC_MALFURION_STORMRAGE, 100.0f))
                                DoCast(malfurion, SPELL_SUM_ENTANGLING_NIGHTMARES);
                        }
                        break;
                    }
                    case EVENT_NIGHTMARE_BRAMBLES:
                        if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 1, 100.0f, true))
                            DoCast(pTarget, SPELL_NIGHTMARE_BRAMBLES_SUM);
                        Talk(SAY_NIGHTMARE_BRAMBLES);
                        events.ScheduleEvent(EVENT_NIGHTMARE_BRAMBLES, 30000);
                        break;
                    case EVENT_OVERWHELMING_NIGHTMARE:
                        DoCast(me, SPELL_OVERWHELMING_NIGHTMARE, true);
                        events.ScheduleEvent(EVENT_NIGHTMARE_BLAST_SAVER, 4000);
                        break;
                    case EVENT_NIGHTMARE_BLAST_SAVER:
                        me->RemoveAurasDueToSpell(SPELL_NIGHTMARE_ENERGY);
                        DoCast(me, SPELL_NIGHTMARE_BLAST_SAVER, true);
                        me->SetReactState(REACT_AGGRESSIVE);
                        break;
                    case EVENT_SPEAR_OF_NIGHTMARES:
                        DoCastVictim(SPELL_SPEAR_OF_NIGHTMARES);
                        events.ScheduleEvent(EVENT_SPEAR_OF_NIGHTMARES, 18000);
                        break;
                    case EVENT_CHECH_EVADE:
                        if (me->GetDistance(centrPos) > 46.0f)
                            EnterEvadeMode();
                        events.ScheduleEvent(EVENT_CHECH_EVADE, 2000);
                        break;
                    case EVENT_NIGHTMARE_BLAST_OR_BEAST:
                        if (!secondPhase && waveCount < 6)
                            DoCastVictim(SPELL_NIGHTMARE_BLAST);
                        else
                            DoCast(SPELL_BEASTS_OF_NIGHTMARE);
                        events.ScheduleEvent(EVENT_NIGHTMARE_BLAST_OR_BEAST, 30000);
                        break;
                    case EVENT_SCENE:
                        Conversation* conversation = new Conversation;
                        if (!conversation->CreateConversation(sObjectMgr->GetGenerator<HighGuid::Conversation>()->Generate(), 3406, me, nullptr, me->GetPosition()))
                            delete conversation;
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_cenariusAI (creature);
    }
};

//106427, 106895, 106898, 106899
class npc_cenarius_force_summoner : public CreatureScript
{
public:
    npc_cenarius_force_summoner() : CreatureScript("npc_cenarius_force_summoner") {}

    struct npc_cenarius_force_summonerAI : public ScriptedAI
    {
        npc_cenarius_force_summonerAI(Creature* creature) : ScriptedAI(creature) 
        {
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
        }

        EventMap events;
        Unit* owner = nullptr;
        bool alies;

        void Reset() override {}

        void IsSummonedBy(Unit* summoner) override
        {
            alies = false;
            owner = summoner;

            switch (me->GetEntry())
            {
                case NPC_NIGHTMARE_SAPLING:
                    DoCast(me, SPELL_GERMINATING, true);
                    events.ScheduleEvent(EVENT_1, 10000);
                    break;
                case NPC_TORMENTED_SOULS:
                    DoCast(me, SPELL_TORMENTED, true);
                    DoCast(me, SPELL_WISPS_VISUAL_CORRUPTED, true);
                    events.ScheduleEvent(EVENT_2, 10000);
                    break;
                case NPC_CORRUPTED_EMERALD_EGG:
                    DoCast(me, SPELL_INCUBATING, true);
                    events.ScheduleEvent(EVENT_3, 10000);
                    break;
                case NPC_CORRUPTED_NATURE:
                    DoCast(me, SPELL_DRYAD_DUMMY, true);
                    DoCast(me, SPELL_DRYAD_VISUAL, true);
                    events.ScheduleEvent(EVENT_4, 10000);
                    break;
            }
            DoCast(me, SPELL_NIGHTMARE_DISSOLVE_IN, true);
            DoCast(me, SPELL_DUDE_TRACKER_AT, true);
            DoCast(me, SPELL_SUMMON_DRAKES_DUMMY, true);
            events.ScheduleEvent(EVENT_5, 10000);
        }

        void SpellHit(Unit* target, const SpellInfo* spell) override
        {
            if (spell->Id == SPELL_CLEANSED_GROUND)
            {
                alies = true;

                if (me->GetEntry() == NPC_TORMENTED_SOULS)
                {
                    me->RemoveAurasDueToSpell(SPELL_WISPS_VISUAL_CORRUPTED);
                    DoCast(me, SPELL_WISPS_VISUAL_ALLIES, true);
                }
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
                        me->CastSpell(me, alies ? SPELL_ALLIES_GERMINATE : SPELL_NIGHTMARE_GERMINATE, true, 0, 0, owner->GetGUID());
                        break;
                    case EVENT_2:
                    {
                        Position pos;
                        centrPos.SimplePosXYRelocationByAngle(pos, 30.0f, frand(0.0f, 6.0f));
                        for (uint8 i = 0; i < 5; i++)
                            me->CastSpell(pos, alies ? SPELL_SUM_ALLIES_WISPS : SPELL_SUM_NIGHTMARE_WISPS, true, 0, 0, owner->GetGUID());
                        break;
                    }
                    case EVENT_3:
                        me->CastSpell(me, alies ? SPELL_SUM_ALLIES_DRAKE : SPELL_SUM_NIGHTMARE_DRAKE, true, 0, 0, owner->GetGUID());
                        break;
                    case EVENT_4:
                        me->CastSpell(me, alies ? SPELL_SUM_ALLIES_DRYAD : SPELL_SUM_NIGHTMARE_DRYAD, true, 0, 0, owner->GetGUID());
                        break;
                    case EVENT_5:
                        DoCast(me, SPELL_NIGHTMARE_DISSOLVE_OUT, true);
                        break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_cenarius_force_summonerAI(creature);
    }
};

//105468, 106304, 105494, 105495
class npc_cenarius_forces_of_nightmare : public CreatureScript
{
public:
    npc_cenarius_forces_of_nightmare() : CreatureScript("npc_cenarius_forces_of_nightmare") {}

    struct npc_cenarius_forces_of_nightmareAI : public ScriptedAI
    {
        npc_cenarius_forces_of_nightmareAI(Creature* creature) : ScriptedAI(creature) 
        {
            instance = creature->GetInstanceScript();
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

        InstanceScript* instance;
        EventMap events;
        bool swtichTarget; //Wisp
        bool buffet; //Drake

        void Reset() override {}

        void JustDied(Unit* /*killer*/) override
        {
            if (me->GetEntry() != NPC_CORRUPTED_WISP)
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
        }

        void IsSummonedBy(Unit* summoner) override
        {
            DoZoneInCombat();

            switch (me->GetEntry())
            {
                case NPC_CORRUPTED_WISP:
                    swtichTarget = false;
                    DoCast(me, SPELL_DESTRUCTIVE_NIGHTMARES, true);
                    me->SetSpeed(MOVE_RUN, 0.6f);
                    break;
                case NPC_NIGHTMARE_ANCIENT:
                    DefaultEvents(50);
                    break;
                case NPC_ROTTEN_DRAKE:
                    buffet = false;
                    DefaultEvents(30);
                    break;
                case NPC_TWISTED_SISTER:
                    DefaultEvents(80);
                    events.ScheduleEvent(EVENT_2, 3000);
                    events.ScheduleEvent(EVENT_3, 11000);
                    break;
            }
        }

        void DefaultEvents(uint8 powerCount)
        {
            me->SetMaxPower(POWER_ENERGY, 100);
            me->SetPower(POWER_ENERGY, powerCount);
            DoCast(me, SPELL_DISSOLVE_CLEANSED_IN, true);
            DoCast(me, SPELL_NIGHTMARE_ENERGY, true);
            DoCast(me, SPELL_CAST_HELPFUL_SPELL, true);
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
        }

        void SpellHitTarget(Unit* target, const SpellInfo* spell) override
        {
            switch (spell->Id)
            {
                case SPELL_DESTRUCTIVE_NIGHTMARES_DMG:
                {
                    if (!swtichTarget)
                        if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                        {
                            swtichTarget = true;
                            DoResetThreat();
                            me->AddThreat(pTarget, 10000.0f);
                            events.ScheduleEvent(EVENT_1, 1000);
                        }
                    break;
                }
                case SPELL_ROTTEN_BREATH_FILTER:
                    if (Creature* rotten = me->FindNearestCreature(NPC_ROTTEN_BREATH_TRIG, 100.0f))
                        DoCast(rotten, SPELL_ROTTEN_BREATH_DMG);
                    break;
            }
        }

        void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType dmgType) override
        {
            if (me->GetEntry() == NPC_ROTTEN_DRAKE)
                if (me->HealthBelowPct(46) && !buffet)
                {
                    buffet = true;
                    DoCast(me, SPELL_NIGHTMARE_BUFFET, true);
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
                    case EVENT_1:
                        swtichTarget = false;
                        break;
                    case EVENT_2:
                        if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 60.0f, true))
                            DoCast(pTarget, SPELL_NIGHTMARE_JAVELIN);
                        events.ScheduleEvent(EVENT_2, urand(5, 8) * IN_MILLISECONDS);
                        break;
                    case EVENT_3:
                        DoCast(me, SPELL_TWISTED_TOUCH_OF_LIFE_FILTER, true);
                        events.ScheduleEvent(EVENT_3, 12000);
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_cenarius_forces_of_nightmareAI(creature);
    }
};

//106667, 106659, 106809, 106831
class npc_cenarius_allies_of_nature : public CreatureScript
{
public:
    npc_cenarius_allies_of_nature() : CreatureScript("npc_cenarius_allies_of_nature") {}

    struct npc_cenarius_allies_of_natureAI : public ScriptedAI
    {
        npc_cenarius_allies_of_natureAI(Creature* creature) : ScriptedAI(creature) 
        {
            instance = creature->GetInstanceScript();
            me->SetReactState(REACT_PASSIVE);
        }

        InstanceScript* instance;
        EventMap events;
        Unit* owner = nullptr;
        uint32 spell;

        void Reset() override {}

        void IsSummonedBy(Unit* summoner) override
        {
            owner = summoner;

            switch (me->GetEntry())
            {
                case NPC_ALLIES_WISP:
                    DoCast(me, SPELL_CLEANSING_DETONATE_VIS, true);
                    events.ScheduleEvent(EVENT_1, 1000);
                    break;
                case NPC_CLEANSED_ANCIENT:
                    spell = SPELL_NATURES_FURY;
                    me->SetMaxPower(POWER_ENERGY, 100);
                    me->SetPower(POWER_ENERGY, 80);
                    DefaultEvents();
                    break;
                case NPC_EMERALD_DRAKE:
                    spell = SPELL_POISON_BLAST;
                    me->SetMaxPower(POWER_ENERGY, 100);
                    me->SetPower(POWER_ENERGY, 100);
                    SetFlyMode(true);
                    DefaultEvents();
                    break;
                case NPC_REDEEMED_SISTER:
                    spell = SPELL_JAVELIN_TOSS;
                    me->SetMaxPower(POWER_ENERGY, 100);
                    me->SetPower(POWER_ENERGY, 56);
                    DefaultEvents();
                    break;
            }
        }

        void DefaultEvents()
        {
            DoCast(me, SPELL_CAST_HELPFUL_SPELL, true);
            DoCast(me, SPELL_DISSOLVE_CLEANSED_IN, true);
            events.ScheduleEvent(EVENT_2, 2000);
            events.ScheduleEvent(EVENT_3, 58000);
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
        }

        void SpellHit(Unit* caster, const SpellInfo* spell) override
        {
            if (spell->Id == SPELL_CLEANSING_DETONATE_AT)
                me->DespawnOrUnsummon(500);
        }

        void UpdateAI(uint32 diff) override
        {
            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING) || !owner)
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_1:
                        me->CastSpell(me, SPELL_CLEANSING_DETONATE_AT, false, 0, 0, owner->GetGUID());
                        break;
                    case EVENT_2:
                    {
                        if (Unit* owner = me->GetAnyOwner())
                        {
                            if (me->GetDistance(owner) > 20.0f)
                            {
                                Position pos;
                                me->GetNearPosition(pos, me->GetDistance(owner) / 2, me->GetRelativeAngle(owner));
                                me->GetMotionMaster()->MovePoint(1, pos);
                            }
                            else
                            {
                                me->SetFacingToObject(owner);
                                DoCast(spell);
                            }
                        }
                        events.ScheduleEvent(EVENT_2, 3600);
                        break;
                    }
                    case EVENT_3:
                        events.Reset();
                        me->CastStop();
                        DoCast(me, SPELL_CLEANSED_DISSOLVE_OUT, true);
                        instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_cenarius_allies_of_natureAI(creature);
    }
};

//106482
class npc_cenarius_malfurion_stormrage : public CreatureScript
{
public:
    npc_cenarius_malfurion_stormrage() : CreatureScript("npc_cenarius_malfurion_stormrage") {}

    struct npc_cenarius_malfurion_stormrageAI : public ScriptedAI
    {
        npc_cenarius_malfurion_stormrageAI(Creature* creature) : ScriptedAI(creature) 
        {
            instance = creature->GetInstanceScript();
            introDone = false;
            IntroStart = false;
            FirstSay = false;
        }

        InstanceScript* instance;
        EventMap events;
        uint8 powerTick;
        uint16 powerTimer;
        uint32 powerCount;
        uint32 maxPower;
        bool introDone;
        bool IntroStart;
        bool FirstSay;
        uint32 IntroTimer;

        void Reset() override
        {
            me->SetMaxPower(POWER_MANA, 295104);
            maxPower = me->GetMaxPower(POWER_MANA);
            powerCount = 0;
            powerTick = 0;
            powerTimer = 0;
        }
        
        void MoveInLineOfSight(Unit* who) override
        {  
            if (me->GetPositionZ() <= 590.0f)
                return;
            
            if (who->GetTypeId() != TYPEID_PLAYER)
                return;

            if (!introDone && me->IsWithinDistInMap(who, 70.0f))
            {
                introDone = true;
                Conversation* conversation = new Conversation;
                if (!conversation->CreateConversation(sObjectMgr->GetGenerator<HighGuid::Conversation>()->Generate(), 3602, who, nullptr, who->GetPosition()))
                    delete conversation;
                DoCast(225937);
                IntroTimer = 5000;
                IntroStart = true;
                FirstSay = false;
            }
        }

        void IsSummonedBy(Unit* summoner) override
        {
            if (summoner->GetEntry() == NPC_CENARIUS)
            {
                me->setRegeneratingHealth(false);
                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
            }
        }

        void DoAction(int32 const action) override
        {
            switch (action)
            {
                case ACTION_1:
                    events.ScheduleEvent(EVENT_1, 500);
                    break;
                case ACTION_2:
                    events.ScheduleEvent(EVENT_2, 3000);
                    break;
                case ACTION_3:
                    me->SetPower(POWER_MANA, CalculatePct(maxPower, 50));
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                    powerTimer = 1000;
                    break;
                case ACTION_4:
                    events.ScheduleEvent(EVENT_5, 4000); // outro
                    break;
            }
        }

        void SpellHitTarget(Unit* target, const SpellInfo* spell) override
        {
            if (spell->Id == SPELL_CLEANSING_GROUND_FILTER)
                DoCast(target, SPELL_CLEANSING_GROUND_AT, true);
        }

        void JustDied(Unit* /*killer*/) override
        {
            if (me->GetAnyOwner())
                if (Creature* owner = me->GetAnyOwner()->ToCreature())
                    owner->AI()->EnterEvadeMode();
        }
        
        void UpdateAI(uint32 diff) override
        {
            events.Update(diff);

            if (powerTimer) //Хак! В снифе нет инфы о ауре.
            {
                if (powerTimer <= diff)
                {
                    powerTimer = 1000;
                    powerCount = me->GetPower(POWER_MANA);
                    if (powerCount < maxPower)
                    {
                        if (powerTick < 2)
                        {
                            powerTick++;
                            me->SetPower(POWER_MANA, powerCount + CalculatePct(maxPower, 2));
                        }
                        else
                        {
                            powerTick = 0;
                            me->SetPower(POWER_MANA, powerCount + CalculatePct(maxPower, 1));
                        }
                    }
                    else if (!me->HasUnitState(UNIT_STATE_STUNNED))
                    {
                        events.ScheduleEvent(EVENT_3, 100);
                        me->SetPower(POWER_MANA, 0);
                    }
                }
                else
                    powerTimer -= diff;
            }
            
            if (IntroStart) // intro on the start
            {
                if (IntroTimer <= diff)
                {
                    if (!FirstSay)
                    {
                        Talk(0);
                        FirstSay = true;
                    }
                    else
                    {
                        Talk(1);
                        IntroStart = false;
                    }
                     IntroTimer = 16000;                   
                }
                else
                    IntroTimer -= diff;
            }

            if (me->HasUnitState(UNIT_STATE_CASTING) || me->HasUnitState(UNIT_STATE_STUNNED))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_1:
                    {
                        if (Unit* owner = me->GetAnyOwner())
                        {
                            if (me->GetDistance(owner) > 20.0f)
                            {
                                Position pos;
                                me->GetNearPosition(pos, me->GetDistance(owner) / 2, me->GetRelativeAngle(owner));
                                me->GetMotionMaster()->MovePoint(1, pos);
                            }
                            else
                            {
                                me->SetFacingToObject(owner);
                                DoCast(SPELL_STORMS_RAGE);
                            }
                        }
                        events.ScheduleEvent(EVENT_1, 2000);
                        break;
                    }
                    case EVENT_2:
                        DoCast(SPELL_CLEANSING_GROUND_FILTER);
                        Talk(2);
                        break;
                    case EVENT_3:
                        DoCast(SPELL_CLEANSING_GROUND_STAGE_2);
                        events.DelayEvents(3000);
                        events.ScheduleEvent(EVENT_4, 100); //Хак. Нужно реализовать таргет
                        break;
                    case EVENT_4:
                    if (Unit* owner = me->GetAnyOwner())
                        if (Creature* summoner = owner->ToCreature())
                            if (Unit* target = summoner->AI()->SelectTarget(SELECT_TARGET_RANDOM, 0, 80.0f, true))
                                DoCast(target, SPELL_CLEANSING_GROUND_AT, true);
                        break;
                    case EVENT_5:  // outro
                        Talk(3);
                    if (Unit* owner = me->GetAnyOwner())
                        if (Creature* summoner = owner->ToCreature())
                            me->CastSpell(summoner, 225790);
                        events.ScheduleEvent(EVENT_6, 10000);
                        break;
                    case EVENT_6:
                        Talk(4);
                    if (Unit* owner = me->GetAnyOwner())
                        if (Creature* summoner = owner->ToCreature())
                            me->AddAura(225790, summoner);
                        events.ScheduleEvent(EVENT_7, 10000);
                        break; 
                    case EVENT_7:
                        Talk(5);
                        events.ScheduleEvent(EVENT_8, 10000);
                        break;
                    case EVENT_8:
                        Talk(6);
                        events.ScheduleEvent(EVENT_9, 10000);
                        break;
                    case EVENT_9:
                        Talk(7);
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_cenarius_malfurion_stormrageAI(creature);
    }
};

//106167
class npc_cenarius_nightmare_brambles : public CreatureScript
{
public:
    npc_cenarius_nightmare_brambles() : CreatureScript("npc_cenarius_nightmare_brambles") {}

    struct npc_cenarius_nightmare_bramblesAI : public ScriptedAI
    {
        npc_cenarius_nightmare_bramblesAI(Creature* creature) : ScriptedAI(creature) 
        {
            me->SetSpeed(MOVE_RUN, 0.5f);
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
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

        EventMap events;
        Unit* plrTarget = nullptr;

        void Reset() override {}

        void IsSummonedBy(Unit* summoner) override
        {
            events.ScheduleEvent(EVENT_1, 1000);
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
                        DoCast(me, SPELL_NIGHTMARE_BRAMBLES_AT, true);
                        DoCast(me, SPELL_NIGHTMARE_BRAMBLES_AT_PER, true);
                        DoCast(me, SPELL_NIGHTMARE_BRAMBLES_UNKNOWN, true);
                        events.ScheduleEvent(EVENT_2, 1000);
                        events.ScheduleEvent(EVENT_3, 2000);
                        break;
                    case EVENT_2:
                    {
                        events.ScheduleEvent(EVENT_2, 2000);
                        if (plrTarget && plrTarget->IsInWorld() && plrTarget->isAlive() && (plrTarget->GetDistance(me) < 100.0f))
                            break;
                        if (Unit* owner = me->GetAnyOwner())
                            if (Creature* summoner = owner->ToCreature())
                            if (Unit* target = summoner->AI()->SelectTarget(SELECT_TARGET_RANDOM, 1, 80.0f, true))
                            {
                                plrTarget = target;
                            }
                        break;
                    }
                    case EVENT_3:
                        me->SendPlaySpellVisualKit(0, BRAMBLES_KIT_1);
                        if (plrTarget && plrTarget->IsInWorld())
                            me->GetMotionMaster()->MovePoint(1, plrTarget->GetPositionX(), plrTarget->GetPositionY(), plrTarget->GetPositionZ());
                        events.ScheduleEvent(EVENT_3, 1000);
                        break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_cenarius_nightmare_bramblesAI(creature);
    }
};

//108040
class npc_cenarius_entangling_roots : public CreatureScript
{
public:
    npc_cenarius_entangling_roots() : CreatureScript("npc_cenarius_entangling_roots") {}

    struct npc_cenarius_entangling_rootsAI : public ScriptedAI
    {
        npc_cenarius_entangling_rootsAI(Creature* creature) : ScriptedAI(creature)
        {
            me->SetReactState(REACT_PASSIVE);
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

        void Reset() override {}

        void IsSummonedBy(Unit* summoner) override
        {
            DoCast(me, SPELL_ENTANGLING_NIGHTMARES_VIS, true);
            DoCast(me, SPELL_NIGHTMARE_DISSOLVE_IN, true);
            DoCast(me, SPELL_ENTANGLING_NIGHTMARES_AURA, true);
        }

        void SpellHitTarget(Unit* target, const SpellInfo* spell) override
        {
            if (spell->Id == SPELL_ENTANGLING_NIGHTMARES_VEH)
            {
                target->CastSpell(me, 46598, true); //Ride vehicle
                DoCast(me, SPELL_ENTANGLING_NIGHTMARES_STUN, true);
            }
        }

        void JustDied(Unit* /*killer*/) override
        {
            if (Creature* malfurion = me->FindNearestCreature(NPC_MALFURION_STORMRAGE, 30.0f))
                malfurion->RemoveAurasDueToSpell(SPELL_ENTANGLING_NIGHTMARES_STUN);
        }

        void UpdateAI(uint32 diff) override {}
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_cenarius_entangling_rootsAI(creature);
    }
};

//108208
class npc_cenarius_beast_of_nightmare : public CreatureScript
{
    public:
        npc_cenarius_beast_of_nightmare() : CreatureScript("npc_cenarius_beast_of_nightmare") {}

        struct npc_cenarius_beast_of_nightmareAI : public ScriptedAI
        {
            npc_cenarius_beast_of_nightmareAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
            }

            uint16 moveTimer;

            void Reset() override {}

            void IsSummonedBy(Unit* summoner) override
            {
                moveTimer = 2000;
            }

            void SpellHit(Unit* caster, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_GRIPPING_FOG_AT)
                {
                    Position pos;
                    me->GetNearPosition(pos, 100.0f, 0.0f);
                    me->GetMotionMaster()->MovePoint(1, pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), false);
                }
            }

            void MovementInform(uint32 type, uint32 id) override
            {
                if (id == 1)
                    me->DespawnOrUnsummon();
            }

            void UpdateAI(uint32 diff) override
            {
                if (moveTimer)
                {
                    if (moveTimer <= diff)
                    {
                        moveTimer = 0;
                        DoCast(me, SPELL_GRIPPING_FOG_AT, true);
                    }
                    else moveTimer -= diff;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_cenarius_beast_of_nightmareAI(creature);
        }
};

//212630
class spell_cenarius_cleansing_ground : public SpellScriptLoader
{
public:
    spell_cenarius_cleansing_ground() : SpellScriptLoader("spell_cenarius_cleansing_ground") { }

    class spell_cenarius_cleansing_ground_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_cenarius_cleansing_ground_SpellScript);

        void FilterTargets(std::list<WorldObject*>& targetsList)
        {
            std::vector<WorldObject*> tempCreature;
            for (auto creature : targetsList)
                if (creature && creature->IsInWorld())
                    if (creature->GetEntry() == NPC_NIGHTMARE_SAPLING || creature->GetEntry() == NPC_TORMENTED_SOULS ||
                        creature->GetEntry() == NPC_CORRUPTED_EMERALD_EGG || creature->GetEntry() == NPC_CORRUPTED_NATURE)
                            tempCreature.push_back(creature);

            uint8 counter[4] = {0, 0, 0, 0};

            for (auto plr : targetsList)
            {
                if (plr->GetTypeId() != TYPEID_PLAYER)
                    continue;
 
                for (uint8 i = 0; i < tempCreature.size(); i++)
                {
                    if (tempCreature[i]->GetDistance(plr) < 33.0f)
                        if (i < 4)
                            counter[i]++;
                }
            }

            uint8 saveCount = counter[0];
            uint8 selectId = 0;

            for (uint8 i = 1; i < 4; ++i)
                if (saveCount < counter[i])
                {
                    saveCount = counter[i];
                    selectId = i;
                }

            targetsList.clear();
            if (!tempCreature.empty())
                targetsList.push_back(tempCreature[selectId]);
        }

        void Register() override
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_cenarius_cleansing_ground_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_cenarius_cleansing_ground_SpellScript();
    }
};

//212188, 212192
class spell_cenarius_sum_wisp : public SpellScriptLoader
{
    public:
        spell_cenarius_sum_wisp() : SpellScriptLoader("spell_cenarius_sum_wisp") { }

        class spell_cenarius_sum_wisp_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_cenarius_sum_wisp_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                PreventHitDefaultEffect(EFFECT_0);

                Position pos;
                centrPos.SimplePosXYRelocationByAngle(pos, 30.0f, frand(0.0f, 6.0f));
                WorldLocation* dest = const_cast<WorldLocation*>(GetExplTargetDest());
                dest->Relocate(pos);
            }

            void Register() override
            {
                OnEffectLaunch += SpellEffectFn(spell_cenarius_sum_wisp_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_TRIGGER_MISSILE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_cenarius_sum_wisp_SpellScript();
        }
};

//211639
class spell_cenarius_allies_periodic_energize : public SpellScriptLoader
{
public:
    spell_cenarius_allies_periodic_energize() : SpellScriptLoader("spell_cenarius_allies_periodic_energize") { }

    class spell_cenarius_allies_periodic_energize_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_cenarius_allies_periodic_energize_AuraScript);

        uint8 PowerCount;
        uint8 powerTick = 0;

        void OnTick(AuraEffect const* aurEff)
        {
            Unit* caster = GetCaster();
            if (!caster)
                return;

            PowerCount = caster->GetPower(POWER_ENERGY);

            if (PowerCount < 100)
            {
                switch (caster->GetEntry())
                {
                    case NPC_CLEANSED_ANCIENT:
                    {
                        if (powerTick < 7)
                        {
                            powerTick++;
                            caster->SetPower(POWER_ENERGY, PowerCount + 3);
                        }
                        else
                        {
                            powerTick = 0;
                            caster->SetPower(POWER_ENERGY, PowerCount + 2);
                        }
                        break;
                    }
                    case NPC_EMERALD_DRAKE:
                    {
                        if (powerTick < 2)
                        {
                            powerTick++;
                            caster->SetPower(POWER_ENERGY, PowerCount + 3);
                        }
                        else
                        {
                            powerTick = 0;
                            caster->SetPower(POWER_ENERGY, PowerCount + 2);
                        }
                        break;
                    }
                    case NPC_REDEEMED_SISTER:
                    case NPC_NIGHTMARE_ANCIENT:
                    {
                        if (powerTick < 2)
                        {
                            powerTick++;
                            caster->SetPower(POWER_ENERGY, PowerCount + 3);
                        }
                        else
                        {
                            powerTick = 0;
                            caster->SetPower(POWER_ENERGY, PowerCount + 4);
                        }
                        break;
                    }
                    case NPC_ROTTEN_DRAKE:
                        caster->SetPower(POWER_ENERGY, PowerCount + 4);
                        break;
                    case NPC_TWISTED_SISTER:
                        caster->SetPower(POWER_ENERGY, PowerCount + 5);
                        break;
                }
            }
            else if (!caster->HasUnitState(UNIT_STATE_CASTING))
            {
                switch (caster->GetEntry())
                {
                    case NPC_CLEANSED_ANCIENT:
                        caster->CastSpell(caster, SPELL_REPLENISHING_ROOTS);
                        break;
                    case NPC_EMERALD_DRAKE:
                        caster->CastSpell(caster, SPELL_ANCIENT_DREAM_FILTER);
                        break;
                    case NPC_REDEEMED_SISTER:
                        caster->CastSpell(caster, SPELL_UNBOUND_TOUCH_FILTER, true);
                        break;
                    case NPC_NIGHTMARE_ANCIENT:
                        caster->CastSpell(caster, SPELL_DESICCATING_STOMP);
                        break;
                    case NPC_ROTTEN_DRAKE:
                        caster->CastSpell(caster, SPELL_ROTTEN_BREATH_FILTER);
                        break;
                    case NPC_TWISTED_SISTER:
                        caster->CastSpell(caster, SPELL_SCORNED_TOUCH_FILTER);
                        break;
                }
            }
        }

        void Register() override
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_cenarius_allies_periodic_energize_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_cenarius_allies_periodic_energize_AuraScript();
    }
};

//211935
class spell_cenarius_ancient_dream_filter : public SpellScriptLoader
{
    public:
        spell_cenarius_ancient_dream_filter() : SpellScriptLoader("spell_cenarius_ancient_dream_filter") { }

        class spell_cenarius_ancient_dream_filter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_cenarius_ancient_dream_filter_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.clear();

                if (GetCaster())
                    if (Unit* summoner = GetCaster()->GetAnyOwner())
                        if (summoner && summoner->getVictim())
                            targets.push_back(summoner->getVictim());
            }

            void HandleOnHit()
            {
                if (GetCaster() && GetHitUnit())
                    GetHitUnit()->CastSpell(GetHitUnit(), 211939, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_cenarius_ancient_dream_filter_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                OnHit += SpellHitFn(spell_cenarius_ancient_dream_filter_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_cenarius_ancient_dream_filter_SpellScript();
        }
};

//211939
class spell_cenarius_ancient_dream : public SpellScriptLoader
{
    public:
        spell_cenarius_ancient_dream() : SpellScriptLoader("spell_cenarius_ancient_dream") { }

        class spell_cenarius_ancient_dream_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_cenarius_ancient_dream_AuraScript);

            void CalculateAmount(AuraEffect const* /*AuraEffect**/, float& amount, bool& /*canBeRecalculated*/)
            {
                amount = -1;
            }

            void Absorb(AuraEffect* aurEff, DamageInfo& dmgInfo, float& absorbAmount)
            {
                if (Unit* target = GetTarget())
                {
                    if (dmgInfo.GetDamage() < target->GetHealth())
                        return;

                    target->SetHealth(target->CountPctFromMaxHealth(GetSpellInfo()->Effects[EFFECT_1]->BasePoints));
                    aurEff->GetBase()->Remove();
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_cenarius_ancient_dream_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_cenarius_ancient_dream_AuraScript::Absorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_cenarius_ancient_dream_AuraScript();
        }
};

//211073
class spell_cenarius_desiccating_stomp_filter : public SpellScriptLoader
{
    public:
        spell_cenarius_desiccating_stomp_filter() : SpellScriptLoader("spell_cenarius_desiccating_stomp_filter") { }

        class spell_cenarius_desiccating_stomp_filter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_cenarius_desiccating_stomp_filter_SpellScript);

            bool targetsEmpty = false;

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (targets.size() == 0)
                    targetsEmpty = true;
            }

            void OnAfterCast()
            {
                if (GetCaster() && targetsEmpty)
                    GetCaster()->CastSpell(GetCaster(), SPELL_DESICCATION, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_cenarius_desiccating_stomp_filter_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
                AfterCast += SpellCastFn(spell_cenarius_desiccating_stomp_filter_SpellScript::OnAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_cenarius_desiccating_stomp_filter_SpellScript();
        }
};

//211462
class spell_cenarius_twisted_touch_of_life_filter : public SpellScriptLoader
{
    public:
        spell_cenarius_twisted_touch_of_life_filter() : SpellScriptLoader("spell_cenarius_twisted_touch_of_life_filter") { }

        class spell_cenarius_twisted_touch_of_life_filter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_cenarius_twisted_touch_of_life_filter_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targetsList)
            {
                std::list<WorldObject*> tempCreature;
                for (auto creature : targetsList)
                    if (creature->GetEntry() == NPC_NIGHTMARE_ANCIENT || creature->GetEntry() == NPC_ROTTEN_DRAKE || creature->GetEntry() == NPC_TWISTED_SISTER)
                        tempCreature.push_back(creature);

                tempCreature.sort(Trinity::UnitHealthState(true));

                if (tempCreature.size() > 1)
                    tempCreature.resize(1);

                if (!tempCreature.empty())
                    targetsList = tempCreature;
            }

            void HandleOnHit()
            {
                if (GetCaster() && GetHitUnit())
                    GetCaster()->CastSpell(GetHitUnit(), GetSpellInfo()->Effects[EFFECT_0]->BasePoints);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_cenarius_twisted_touch_of_life_filter_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
                OnHit += SpellHitFn(spell_cenarius_twisted_touch_of_life_filter_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_cenarius_twisted_touch_of_life_filter_SpellScript();
        }
};

//210340
class spell_cenarius_dread_thorns_reflects : public SpellScriptLoader
{
    public:
        spell_cenarius_dread_thorns_reflects() : SpellScriptLoader("spell_cenarius_dread_thorns_reflects") { }

        class spell_cenarius_dread_thorns_reflects_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_cenarius_dread_thorns_reflects_AuraScript);

            void OnProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
            {
                if (!GetCaster())
                    return;

                Unit* target = eventInfo.GetProcTarget();
                if (!target)
                    return;

                float damage = CalculatePct(eventInfo.GetDamageInfo()->GetDamage(), GetSpellInfo()->Effects[EFFECT_0]->BasePoints);
                if (!damage)
                    return;

                GetCaster()->CastCustomSpell(target, 210342, &damage, nullptr, nullptr, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_cenarius_dread_thorns_reflects_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_cenarius_dread_thorns_reflects_AuraScript();
        }
};

void AddSC_boss_cenarius()
{
    new boss_cenarius();
    new npc_cenarius_force_summoner();
    new npc_cenarius_forces_of_nightmare();
    new npc_cenarius_allies_of_nature();
    new npc_cenarius_malfurion_stormrage();
    new npc_cenarius_nightmare_brambles();
    new npc_cenarius_entangling_roots();
    new npc_cenarius_beast_of_nightmare();
    new spell_cenarius_cleansing_ground();
    new spell_cenarius_sum_wisp();
    new spell_cenarius_allies_periodic_energize();
    new spell_cenarius_ancient_dream_filter();
    new spell_cenarius_ancient_dream();
    new spell_cenarius_desiccating_stomp_filter();
    new spell_cenarius_twisted_touch_of_life_filter();
    new spell_cenarius_dread_thorns_reflects();
}
