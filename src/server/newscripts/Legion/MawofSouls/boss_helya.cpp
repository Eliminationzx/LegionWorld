/*
    http://uwow.biz
    Dungeon : Maw of Souls 100-110
    Encounter: Helya
    Normal: 100%, Heroic: 100%, Mythic: 100%
*/

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "maw_of_souls.h"

enum Says
{
    SAY_INTRO               = 0,
    SAY_AGGRO               = 1,
    SAY_TAINT_SEA           = 2,
    SAY_97                  = 3,
    SAY_94                  = 4,
    SAY_90                  = 5,
    SAY_87                  = 6,
    SAY_PHASE2              = 7,
    SAY_WARN_WING_BUFFET    = 8,
    SAY_DEATH               = 9,
};

enum Spells
{
    //Intro
    SPELL_SOULLESS_SCREAM           = 194603,

    SPELL_INTERFERE_TARGETTING      = 197710,
    SPELL_TAINT_OF_THE_SEA          = 197517,
    SPELL_TORRENT                   = 197805,
    SPELL_TAINTED_ESSENCE           = 202470,
    //Phase 2
    SPELL_SUBMERGED                 = 196947, //32:34
    SPELL_TRANSFORM                 = 197734, //32:44 - Self hit
                        //SMSG_MOVE_UPDATE_TELEPORT 32:44 - 2933.5 Y: 961.792 Z: 510.38 O: 4.712389
    SPELL_SMASH                     = 197689, //Force cast all players
    SPELL_WING_BUFFET               = 198520,
    SPELL_TORRENT_2                 = 198495,
    SPELL_CORRUPTED_BELLOW          = 227233,

    //Tentacle
    SPELL_GRASPING_STUN_LEFT        = 198180,
    SPELL_GRASPING_STUN_RIGHT       = 196450,
    SPELL_PIERCING_TENTACLE         = 197112,
    SPELL_SWIRLING_POOL_AT          = 195167,
    SPELL_SWIRLING_POOL_DMG         = 195309,
    SPELL_SWIRLING_POOL_JUMP        = 194839,

    //Swirling Pool
    SPELL_TURBULENT_WATERS          = 197753,

    //Heroic
    SPELL_BRACKWATER_BARRAGE        = 202088,
    SPELL_BRACKWATER_BARRAGE_DMG    = 202098,

    //trash
    SPELL_GIVE_NO                   = 196885,
    SPELL_DEBILITATING              = 195293,
    SPELL_BIND                      = 195279,
};

enum eEvents
{
    EVENT_INTRO                     = 1,
    EVENT_TAINT_SEA                 = 2,
    EVENT_TORRENT                   = 3,
    EVENT_SUM_PIERCING_TENTACLE     = 4,
    EVENT_SUBMERGED_START           = 5,
    EVENT_SUBMERGED_END             = 6,
    EVENT_WING_BUFFET               = 7,
    EVENT_ACTION_SWIRLING_POOL      = 8,
    EVENT_BRACKWATER_BARRAGE        = 9,
    EVENT_TORRENT_2                 = 10, //Second phase
    EVENT_CORRUPTED_BELLOW          = 11,

    //trash
    EVENT_SUMMON_1                  = 1,
    EVENT_SUMMON_2                  = 2,
    EVENT_GIVE_NO                   = 3,
    EVENT_DEBILITATING              = 4,
    EVENT_BIND                      = 5,
};

enum eVisualKits
{
    VISUAL_KIT_1                    = 60920, //30:14 - visual spawn
    VISUAL_KIT_2                    = 62877, //30:22 - start intro
    VISUAL_KIT_3                    = 60222, //30:28
    VISUAL_KIT_4                    = 63777, //Brackwater barrage?

    //Brackwater missle
    VISUAL_BW_MIS_RIGHT             = 52865,
    VISUAL_BW_MIS_LEFT              = 52868,
    //Heroic
    VISUAL_H_BRACKWATER_LEFT        = 52869,
    VISUAL_H_BRACKWATER_RIGHT       = 52870,
    //Mythic
    VISUAL_M_BRACKWATER_LEFT        = 65462,
    VISUAL_M_BRACKWATER_RIGHT       = 65463,
};

Position const tentaclePos[15] =
{
//1 (100% -> 90%)
    {2933.28f, 960.37f, 510.60f, 4.71f}, //NPC_DESTRUCTOR_TENTACLE
    {2941.37f, 966.34f, 513.79f, 4.08f}, //NPC_GRASPING_TENTACLE_1
    {2925.54f, 965.84f, 512.26f, 5.65f}, //NPC_GRASPING_TENTACLE_2
//2 (90% -> 87%)
    {2921.71f, 959.25f, 513.21f, 5.85f}, //NPC_GRASPING_TENTACLE_3
//3 (87% -> 83%)
    {2933.28f, 960.37f, 512.42f, 4.71f}, //NPC_DESTRUCTOR_TENTACLE
    {2933.40f, 941.15f, 512.42f, 4.71f},
    {2948.52f, 921.45f, 512.42f, 4.71f},
//4 (83% -> 80%)
    {2945.93f, 958.59f, 513.23f, 3.58f}, //NPC_GRASPING_TENTACLE_4
//Swirling Pool VEH - 97099
    {2939.53f, 928.46f, 510.39f, 1.57f},
    {2922.20f, 937.07f, 510.39f, 1.57f},
    {2927.44f, 928.84f, 510.39f, 1.57f},
    {2948.52f, 921.45f, 510.39f, 1.57f},
    {2933.40f, 941.15f, 510.39f, 1.57f},
    {2943.52f, 935.80f, 510.39f, 1.57f},
    {2918.25f, 921.30f, 510.39f, 1.57f},
};

// Skyal
Position const addsPos[18] =
{
    {2942.57f, 886.99f, 537.76f, 4.48f},
    {2924.55f, 887.20f, 537.65f, 5.26f},

    {2906.68f, 752.27f, 538.55f, 4.72f}, //97097
    {2959.93f, 751.26f, 538.52f, 4.63f}, //97097
    {2916.09f, 807.88f, 535.81f, 4.66f}, //97097
    {2950.01f, 804.85f, 535.65f, 4.66f}, //97097
    {2923.85f, 771.78f, 538.59f, 4.53f}, //98919
    {2957.19f, 775.12f, 538.66f, 5.03f}, //98919
    {2923.93f, 838.52f, 536.17f, 4.66f}, //98919
    {2942.26f, 838.82f, 536.16f, 4.64f}, //98919
    {2917.46f, 772.97f, 538.55f, 4.40f}, //99033
    {2950.14f,  773.0f, 538.52f, 4.93f}, //99033
    {2921.54f, 807.65f, 535.82f, 4.67f}, //99033
    {2945.05f, 805.94f, 535.84f, 4.53f}, //99033
    {2911.42f,  775.0f, 538.55f, 4.43f}, //97365
    {2944.38f, 771.65f, 538.54f, 4.94f}, //97365
    {2919.12f, 839.14f, 536.17f, 4.74f}, //97200
    {2946.58f, 838.83f, 536.17f, 4.74f}  //97200
};

uint32 swirlingGO[8]
{
    GO_SWIRLING_POOL_1,
    GO_SWIRLING_POOL_2,
    GO_SWIRLING_POOL_3,
    GO_SWIRLING_POOL_4,
    GO_SWIRLING_POOL_5,
    GO_SWIRLING_POOL_6,
    GO_SWIRLING_POOL_7,
    GO_SWIRLING_POOL_8, 
};

std::vector<Position> brackwaterLeftPos =
{
    {2939.40f, 962.60f, 512.62f},
    {2942.39f, 959.01f, 512.62f},
    {2936.74f, 958.69f, 512.44f},
    {2940.28f, 954.87f, 512.62f},
    {2945.83f, 955.33f, 512.62f},
    {2935.46f, 953.10f, 512.62f},
    {2938.70f, 950.05f, 512.44f},
    {2944.88f, 950.54f, 512.62f},
    {2935.38f, 946.62f, 512.62f},
    {2949.86f, 946.64f, 512.62f},
    {2941.57f, 945.62f, 512.62f},
    {2939.18f, 942.02f, 512.62f},
    {2947.10f, 941.91f, 512.62f},
    {2936.93f, 938.13f, 512.62f},
    {2943.34f, 937.14f, 512.62f},
    {2950.44f, 936.89f, 512.62f},
    {2936.81f, 931.11f, 512.62f},
    {2947.02f, 931.25f, 512.62f},
    {2941.39f, 929.56f, 512.44f},
    {2936.53f, 924.82f, 512.62f},
    {2943.62f, 924.13f, 512.62f},
    {2950.87f, 923.85f, 512.62f},
    {2939.42f, 917.48f, 512.62f},
    {2946.80f, 916.83f, 512.62f}
};

std::vector<Position> brackwaterRightPos =
{
    {2925.56f, 963.48f, 512.62f},
    {2929.07f, 960.50f, 512.62f},
    {2923.64f, 958.06f, 512.62f},
    {2919.76f, 956.44f, 512.96f},
    {2928.60f, 955.48f, 512.62f},
    {2922.82f, 953.38f, 512.62f},
    {2930.63f, 950.74f, 512.62f},
    {2918.42f, 950.52f, 512.62f},
    {2925.68f, 949.56f, 512.44f},
    {2920.87f, 946.22f, 512.62f},
    {2929.98f, 945.46f, 512.62f},
    {2925.92f, 944.20f, 512.62f},
    {2917.76f, 944.13f, 512.62f},
    {2929.15f, 940.10f, 512.62f},
    {2921.97f, 939.96f, 512.62f},
    {2925.20f, 937.02f, 512.44f},
    {2916.68f, 937.24f, 512.62f},
    {2921.16f, 934.19f, 512.62f},
    {2927.87f, 932.90f, 512.62f},
    {2916.64f, 931.38f, 512.62f},
    {2923.48f, 929.93f, 512.44f},
    {2927.61f, 925.69f, 512.62f},
    {2918.18f, 925.71f, 512.62f},
    {2922.55f, 922.01f, 512.62f},
    {2913.16f, 922.54f, 512.62f},
    {2931.84f, 921.01f, 512.34f},
    {2917.70f, 919.10f, 512.54f},
    {2927.18f, 918.08f, 512.62f}
};

Position const smashPos[5] =
{
    {2947.05f, 914.22f, 513.0f},
    {2950.39f, 914.91f, 512.26f},
    {2933.61f, 914.46f, 512.38f},
    {2923.04f, 914.18f, 512.26f},
    {2915.98f, 914.39f, 512.26f}
};

//96759
class boss_helya : public CreatureScript
{
public:
    boss_helya() : CreatureScript("boss_helya") {}

    struct boss_helyaAI : public BossAI
    {
        boss_helyaAI(Creature* creature) : BossAI(creature, DATA_HELYA)
        {
            me->SetStandState(UNIT_STAND_STATE_SUBMERGED);
            SetCombatMovement(false);
            introSpawn = false;
            introEvent = false;
            eventDelay = false;
        }

        bool introSpawn, introEvent, eventDelay, encounterComplete;
        uint8 tentacleDiedCound{};
        uint8 idxSumm{};
        uint8 healthSwitch;
        uint8 healthSay{};
        uint8 BWrand{};
        uint32 piercingTentacleTimer = 0;

        void Reset() override
        {
            _Reset();
            tentacleDiedCound = 0;
            idxSumm = 0;
            healthSwitch = 0;
            healthSay = healthSwitch + 3;
            encounterComplete = false;
            piercingTentacleTimer = 8000;
            UpdateShipState(STATE_REBUILDING);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
            me->SetReactState(REACT_AGGRESSIVE);

            std::list<Creature*> poolList;
            GetCreatureListWithEntryInGrid(poolList, me, NPC_SWIRLING_POOL_TRIG, 200.0f);
            for (auto const& pool : poolList)
                pool->DespawnOrUnsummon();

            ChangePoolState(GO_STATE_READY);

            if (introEvent)
                SummonTentacle(idxSumm);

            for (uint8 i = 8; i < 15; ++i)
                me->SummonCreature(NPC_SWIRLING_POOL_VEH, tentaclePos[i]);
        }

        void EnterCombat(Unit* /*who*/) override
        {
            Talk(SAY_AGGRO);
            _EnterCombat();
            events.ScheduleEvent(EVENT_TAINT_SEA, 0);
            events.ScheduleEvent(EVENT_SUM_PIERCING_TENTACLE, piercingTentacleTimer);

            if (GetDifficultyID() != DIFFICULTY_NORMAL)
                events.ScheduleEvent(EVENT_BRACKWATER_BARRAGE, 15000);
            else
                events.ScheduleEvent(EVENT_TORRENT, 3000);

            std::list<Creature*> tentacle;
            GetCreatureListWithEntryInGrid(tentacle, me, 99801, 200.0f); //DESTRUCTOR_TENTACLE
            GetCreatureListWithEntryInGrid(tentacle, me, 98363, 200.0f); //NPC_GRASPING_TENTACLE_1
            GetCreatureListWithEntryInGrid(tentacle, me, 100362, 200.0f); //NPC_GRASPING_TENTACLE_2
            for (std::list<Creature*>::iterator itr = tentacle.begin(); itr != tentacle.end(); ++itr)
                if (!(*itr)->isInCombat())
                    (*itr)->AI()->DoZoneInCombat((*itr), 150.0f);
        }

        void EnterEvadeMode() override
        {
            BossAI::EnterEvadeMode();
            me->NearTeleportTo(me->GetHomePosition());
        }

        void MoveInLineOfSight(Unit* who) override
        {
            if (!who->IsPlayer() || who->ToPlayer()->isGameMaster())
                return;

            if (!sWorld->getBoolConfig(CONFIG_IS_TEST_SERVER))
            {
                if (instance->GetBossState(DATA_YMIRON) != DONE || instance->GetBossState(DATA_HARBARON) != DONE || instance->GetBossState(DATA_SKJAL) != DONE)
                    return;
            }

            if (me->GetDistance(who) < 140.0f && !introSpawn)
            {
                introSpawn = true;
                eventDelay = true;
                me->RemoveStandStateFlags(UNIT_STAND_STATE_SUBMERGED);
                me->SendPlaySpellVisualKit(0, VISUAL_KIT_1);
                AddDelayedEvent(5000, [=]() -> void { eventDelay = false; });
                return;
            }

            if (me->GetDistance(who) < 130.0f && !introEvent && !eventDelay)
            {
                introEvent = true;
                Talk(SAY_INTRO); //You ALL will regret trespassing in my realm.
                me->SendPlaySpellVisualKit(0, VISUAL_KIT_2);
                events.ScheduleEvent(EVENT_INTRO, 5000);
            }
        }

        void SummonTentacle(uint8 idPos)
        {
            switch (idPos)
            {
                case 0:
                    me->SummonCreature(NPC_DESTRUCTOR_TENTACLE, tentaclePos[0]);
                    me->SummonCreature(NPC_GRASPING_TENTACLE_1, tentaclePos[1]);
                    me->SummonCreature(NPC_GRASPING_TENTACLE_2, tentaclePos[2]);
                    break;
                case 1:
                    me->SummonCreature(NPC_GRASPING_TENTACLE_3, tentaclePos[3]);
                    break;
                case 2:
                    me->SummonCreature(NPC_DESTRUCTOR_TENTACLE, tentaclePos[urand(4,6)]);
                    me->SummonCreature(NPC_GRASPING_TENTACLE_4, tentaclePos[7]);
                    break;
                default:
                    return;
            }
            idxSumm++;
        }

        void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
        {
            switch (summon->GetEntry())
            {
                case NPC_DESTRUCTOR_TENTACLE:
                case NPC_GRASPING_TENTACLE_1:
                case NPC_GRASPING_TENTACLE_2:
                case NPC_GRASPING_TENTACLE_3:
                case NPC_GRASPING_TENTACLE_4:
                    tentacleDiedCound++;
                    break;
                default:
                    return;
            }

            switch (tentacleDiedCound)
            {
                case 3:              
                case 4:              
                case 5:
                    SummonTentacle(idxSumm);
                    break;
                case 6:
                    events.ScheduleEvent(EVENT_SUBMERGED_START, 1000);
                    Talk(SAY_87);
                    break;
                default:
                    break;
            }
            
            if (healthSwitch < 2)
            {
                me->SetHealth(me->GetHealth() - me->CountPctFromMaxHealth(3));
                healthSwitch++;
                healthSay = healthSwitch + 3;
                Talk(healthSay);
            }
            else
            {
                me->SetHealth(me->GetHealth() - me->CountPctFromMaxHealth(4));
                healthSwitch = 0;
                healthSay = healthSwitch + 3;
                Talk(healthSay);
            }
        }

        void UpdateShipState(uint8 state)
        {
            if (GameObject* pGo = instance->instance->GetGameObject(instance->GetGuidData(DATA_SHIP)))
            {
                if (state == STATE_DESTROY)
                {
                    pGo->SetDisplayId(31853);
                    pGo->SetDestructibleState(GO_DESTRUCTIBLE_DESTROYED, nullptr, true);
                }
                else if (state == STATE_REBUILDING)
                {
                    pGo->SetDisplayId(31852);
                    pGo->SetDestructibleState(GO_DESTRUCTIBLE_REBUILDING, nullptr, true);
                }

                Map::PlayerList const& players = me->GetMap()->GetPlayers();
                for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                    if (Player* plr = itr->getSource())
                        if (plr->CanContact())
                            pGo->SendUpdateToPlayer(plr);
            }
        }

        void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType dmgType) override
        {
            if (me->HealthBelowPct(71) && !encounterComplete)
            {
                encounterComplete = true;
                me->StopAttack();
                Talk(SAY_DEATH);
                _JustDied();
                instance->DoUpdateAchievementCriteria(CRITERIA_TYPE_COMPLETE_DUNGEON_ENCOUNTER, 1824);

                // for quest need "kill"
                Map::PlayerList const& players = me->GetMap()->GetPlayers();
                if (!players.isEmpty())
                {
                    for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                        if (Player* pPlayer = itr->getSource())
                            pPlayer->KilledMonsterCredit(me->GetEntry());
                }
                me->DespawnOrUnsummon(100);
            }
        }

        void SpellHit(Unit* caster, const SpellInfo* spell) override
        {
            if (spell->Id == SPELL_BRACKWATER_BARRAGE)
            {
                std::vector<Position> brackwaterPos = !BWrand ? brackwaterLeftPos : brackwaterRightPos;
                uint8 count = 0;
                
                for (auto const& pos : brackwaterPos)
                {
                    ++count;
                
                    AddDelayedEvent(50 * count, [this, pos]() -> void
                    {
                        if (me)
                            me->SendSpellCreateVisual(sSpellMgr->GetSpellInfo(SPELL_BRACKWATER_BARRAGE_DMG), &pos, nullptr, 1, !BWrand ? VISUAL_BW_MIS_LEFT : VISUAL_BW_MIS_RIGHT);
                    });
                
                    AddDelayedEvent(50 * count + 1200, [this, pos]() -> void
                    {
                        if (me)
                            me->CastSpell(pos, SPELL_BRACKWATER_BARRAGE_DMG, true);
                    });
                }
            }
        }

        void SpellHitTarget(Unit* target, const SpellInfo* spell) override
        {
            if (spell->Id == SPELL_SMASH)
                target->GetMotionMaster()->MoveJump(smashPos[urand(0,4)], 30.0f, 10.0f);
        }

        void ChangePoolState(GOState state)
        {
            for (uint8 i = 0; i < 8; ++i)
                if (GameObject* go = me->FindNearestGameObject(swirlingGO[i], 200.0f))
                    go->SetGoState(state);
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim() && me->isInCombat())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_INTRO:
                    {
                        me->SendPlaySpellVisualKit(0, VISUAL_KIT_3);
                        DoCast(me, SPELL_SOULLESS_SCREAM, true);
                        DoCast(me, SPELL_INTERFERE_TARGETTING, true);
                        SummonTentacle(idxSumm);
                        break;
                    }
                    case EVENT_TAINT_SEA:
                        DoCast(SPELL_TAINT_OF_THE_SEA);
                        events.ScheduleEvent(EVENT_TAINT_SEA, 12000);
                        Talk(SAY_TAINT_SEA);
                        break;
                    case EVENT_TORRENT:
                        DoCast(SPELL_TORRENT);
                        events.ScheduleEvent(EVENT_TORRENT, 5000);
                        break;
                    case EVENT_TORRENT_2: //Second Phase
                        DoCast(SPELL_TORRENT_2);
                        events.ScheduleEvent(EVENT_TORRENT_2, 8000);
                        break;
                    case EVENT_SUM_PIERCING_TENTACLE:
                    {
                        std::list<Creature*> tentacle;
                        GetCreatureListWithEntryInGrid(tentacle, me, NPC_SWIRLING_POOL_VEH, 200.0f);
                        Trinity::Containers::RandomResizeList(tentacle, 1);
                        for (std::list<Creature*>::iterator itr = tentacle.begin(); itr != tentacle.end(); ++itr)
                            if (!tentacle.empty())
                                (*itr)->AI()->DoAction(true);
                        events.ScheduleEvent(EVENT_SUM_PIERCING_TENTACLE, piercingTentacleTimer);
                        break;
                    }
                    case EVENT_SUBMERGED_START:
                        me->InterruptNonMeleeSpells(false);
                        DoCast(me, SPELL_SUBMERGED, true);
                        Talk(SAY_PHASE2);
                        piercingTentacleTimer = 2000;
                        events.CancelEvent(EVENT_BRACKWATER_BARRAGE);
                        events.ScheduleEvent(EVENT_SUBMERGED_END, 10000);
                        break;
                    case EVENT_SUBMERGED_END:
                        ChangePoolState(GO_STATE_ACTIVE);
                        me->RemoveAurasDueToSpell(SPELL_SUBMERGED);
                        DoTeleportTo(2933.5f, 961.79f, 512.38f, 4.71f);
                        me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_PREVENT_SELECT_NPC);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                        DoCast(me, SPELL_TRANSFORM, true);
                        DoCast(me, SPELL_SMASH, true);
                        UpdateShipState(STATE_DESTROY);
                        events.CancelEvent(EVENT_SUM_PIERCING_TENTACLE);
                        events.ScheduleEvent(EVENT_TORRENT_2, 16000);
                        events.ScheduleEvent(EVENT_ACTION_SWIRLING_POOL, 1000);
                        events.ScheduleEvent(EVENT_WING_BUFFET, 10000);
                        events.ScheduleEvent(EVENT_CORRUPTED_BELLOW, 15000);
                        break;
                    case EVENT_WING_BUFFET:
                        if (Unit* target = me->FindNearestPlayer(50))
                        {
                            if (!me->IsWithinMeleeRange(target))
                            {
                                Talk(SAY_WARN_WING_BUFFET);
                                me->CastSpell(me, SPELL_WING_BUFFET);
                            }
                        }
                        events.ScheduleEvent(EVENT_WING_BUFFET, 2000);
                        break;
                    case EVENT_ACTION_SWIRLING_POOL:
                    {
                        std::list<Creature*> poolList;
                        GetCreatureListWithEntryInGrid(poolList, me, NPC_SWIRLING_POOL_TRIG, 150.0f);
                        for (auto const& pool : poolList)
                            pool->CastSpell(pool, SPELL_TURBULENT_WATERS, true);
                        break;
                    }
                    case EVENT_BRACKWATER_BARRAGE:
                    {
                        Position pos = {2934.49f, 1055.94f, 510.51f, 0.0f};
                        BWrand = urand(0, 1);
                        uint32 visualBrackwater = 0;
                        if (GetDifficultyID() == DIFFICULTY_HEROIC)
                            visualBrackwater = VISUAL_H_BRACKWATER_LEFT + BWrand;
                        else if (GetDifficultyID() == DIFFICULTY_MYTHIC_DUNGEON || GetDifficultyID() == DIFFICULTY_MYTHIC_KEYSTONE)
                            visualBrackwater = VISUAL_M_BRACKWATER_LEFT + BWrand;
                        me->SendSpellCreateVisual(sSpellMgr->GetSpellInfo(SPELL_BRACKWATER_BARRAGE), &pos, nullptr, 1, visualBrackwater);
                        DoCast(SPELL_BRACKWATER_BARRAGE);
                        events.ScheduleEvent(EVENT_BRACKWATER_BARRAGE, 22000);
                        break;
                    }
                    case EVENT_CORRUPTED_BELLOW:
                    {
                        me->StopAttack();
                        float facing[3] = {4.41f, 4.71f, 5.01f};
                        me->SetFacingTo(facing[urand(0, 2)]);
                        me->CastSpell(me, SPELL_CORRUPTED_BELLOW);
                        events.ScheduleEvent(EVENT_CORRUPTED_BELLOW, 21000);
                        me->AddDelayedCombat(8000, [this]() -> void
                        {
                            me->SetFacingTo(4.71f);
                            me->SetReactState(REACT_AGGRESSIVE);
                        });
                        break;
                    }
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_helyaAI (creature);
    }
};

//98363, 99801, 100354, 100360, 100362, 100188
class npc_helya_tentacle : public CreatureScript
{
public:
    npc_helya_tentacle() : CreatureScript("npc_helya_tentacle") { }

    struct npc_helya_tentacleAI : public ScriptedAI
    {
        npc_helya_tentacleAI(Creature* creature) : ScriptedAI(creature) 
        {
            instance = me->GetInstanceScript();
            me->SetDisableGravity(true);
            SetCombatMovement(false);
            me->SetCollision(true);
            me->SetReactState(REACT_DEFENSIVE);
        }

        InstanceScript* instance;
        EventMap events;
        bool inCombat = false;

        void Reset() override {}

        void EnterCombat(Unit* /*who*/) override
        {
            if (instance && instance->GetBossState(DATA_HELYA) != IN_PROGRESS)
                if (Creature* helya = instance->instance->GetCreature(instance->GetGuidData(DATA_HELYA)))
                    helya->SetInCombatWithZone();

            switch (me->GetEntry())
            {
                case 99801: //DESTRUCTOR_TENTACLE
                    inCombat = true;
                    Talk(0);
                    events.ScheduleEvent(EVENT_5, 2000);
                    events.ScheduleEvent(EVENT_6, 2000);
                    break;
                default:
                    break;
            }
        }

        void JustDied(Unit* /*killer*/) override
        {
            if (auto summoner = me->GetAnyOwner())
                summoner->Kill(summoner);
        }
        
        void IsSummonedBy(Unit* summoner) override
        {
            switch (me->GetEntry())
            {
                case 100360: // NPC_GRASPING_TENTACLE_3
                case 100362: // NPC_GRASPING_TENTACLE_2
                    events.ScheduleEvent(EVENT_1, 1000);
                    break;
                case 98363:  // NPC_GRASPING_TENTACLE_1
                case 100354: // NPC_GRASPING_TENTACLE_4
                    events.ScheduleEvent(EVENT_2, 1000);
                    break;
                case 100188: // NPC_PIERCING_TENTACLE
                    events.ScheduleEvent(EVENT_3, 1000);
                    return;
                default:
                    break;
            }
            if (instance->GetBossState(DATA_HELYA) == IN_PROGRESS)
            {
                me->SetReactState(REACT_AGGRESSIVE);
                DoZoneInCombat(me, 100.0f);
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim() && inCombat)
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_1:
                        DoCast(me, SPELL_GRASPING_STUN_LEFT, true);
                        break;
                    case EVENT_2:
                        DoCast(me, SPELL_GRASPING_STUN_RIGHT, true);
                        break;
                    case EVENT_3:
                        me->CastSpell(me, SPELL_PIERCING_TENTACLE, TriggerCastFlags(TRIGGERED_IGNORE_CASTER_MOUNTED_OR_ON_VEHICLE));
                        events.ScheduleEvent(EVENT_4, 4000);
                        break;
                    case EVENT_4:
                        me->SetDisplayId(11686);
                        break;
                    case EVENT_5:
                        if (Unit* target = me->FindNearestPlayer(5))
                            me->CastSpell(target, 185539, TriggerCastFlags(TRIGGERED_IGNORE_CASTER_MOUNTED_OR_ON_VEHICLE));
                        events.ScheduleEvent(EVENT_5, 6000);
                        break;
                    case EVENT_6:
                        if (Unit* target = me->FindNearestPlayer(50.0f))
                        {
                            if (!me->IsWithinMeleeRange(target))
                                me->CastSpell(me, 196534, TriggerCastFlags(TRIGGERED_IGNORE_CASTER_MOUNTED_OR_ON_VEHICLE | TRIGGERED_IGNORE_CASTER_AURASTATE));
                        }
                        events.ScheduleEvent(EVENT_6, 2000);
                        break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_helya_tentacleAI(creature);
    }
};

//97099
class npc_helya_tentacle_veh : public CreatureScript
{
public:
    npc_helya_tentacle_veh() : CreatureScript("npc_helya_tentacle_veh") {}

    struct npc_helya_tentacle_vehAI : public ScriptedAI
    {
        npc_helya_tentacle_vehAI(Creature* creature) : ScriptedAI(creature)
        {
            me->SetReactState(REACT_PASSIVE);
            createAT = false;
        }

        bool createAT;

        void Reset() override {}

        void DoAction(int32 const action) override
        {
            if (!createAT)
            {
                createAT = true;
                DoCast(me, SPELL_SWIRLING_POOL_AT, true);
                for (uint8 i = 0; i < 8; i++)
                    if (GameObject* go = me->FindNearestGameObject(swirlingGO[i], 1.0f))
                        go->SetGoState(GO_STATE_ACTIVE);
            }
            float x = me->GetPositionX();
            float y = me->GetPositionY();
            float z = me->GetPositionZ();
            if (Unit* owner = me->GetAnyOwner())
            {
                owner->SummonCreature(NPC_PIERCING_TENTACLE, x, y, z, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 6 * IN_MILLISECONDS);
                owner->SummonCreature(NPC_SWIRLING_POOL_TRIG, x, y, z);
            }
        }

        void UpdateAI(uint32 diff) override {}
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_helya_tentacle_vehAI(creature);
    }
};

//99803
class npc_helya_destructor_tentacle_veh : public CreatureScript
{
public:
    npc_helya_destructor_tentacle_veh() : CreatureScript("npc_helya_destructor_tentacle_veh") {}

    struct npc_helya_destructor_tentacle_vehAI : public ScriptedAI
    {
        npc_helya_destructor_tentacle_vehAI(Creature* creature) : ScriptedAI(creature)
        {
            me->SetReactState(REACT_PASSIVE);
        }

        void Reset() override {}

        void IsSummonedBy(Unit* summoner) override
        {
            DoCast(me, SPELL_SWIRLING_POOL_AT, true);

            if (GameObject* go = me->FindNearestGameObject(GO_SWIRLING_POOL_2, 1.0f))
                go->SetGoState(GO_STATE_ACTIVE);
        }

        void UpdateAI(uint32 diff) override {}
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_helya_destructor_tentacle_vehAI(creature);
    }
};

//99307
class npc_skyal : public CreatureScript
{
public:
    npc_skyal() : CreatureScript("npc_skyal") {}

    struct npc_skyalAI : public ScriptedAI
    {
        npc_skyalAI(Creature* creature) : ScriptedAI(creature) 
        {
            instance = me->GetInstanceScript();
        }

        void Reset() override
        {
            instance->SetBossState(DATA_SKJAL, NOT_STARTED);

            if (instance->GetBossState(DATA_HARBARON) != DONE)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                me->SetReactState(REACT_PASSIVE);
                me->SetVisible(false);
            }
        }

        void DoAction(int32 const action) override
        {  
            if (action == ACTION_1)
            {
                me->SetVisible(true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                me->SetReactState(REACT_AGGRESSIVE);
                ZoneTalk(0);

                for (uint8 i = 2; i < 18; ++i)
                {
                    if (i < 6)
                    {
                        if (Creature* summon = instance->instance->SummonCreature(NPC_HELARJAR_CHAMPION, me->GetPosition()))
                        {
                            summon->SetHomePosition(addsPos[i]);
                            summon->GetMotionMaster()->MoveTargetedHome();
                        }
                    }
                    else if (i < 10)
                    {
                        if (Creature* summon = instance->instance->SummonCreature(NPC_SEACURSED_SWIFTBLADE, me->GetPosition()))
                        {
                            summon->SetHomePosition(addsPos[i]);
                            summon->GetMotionMaster()->MoveTargetedHome();
                        }
                    }
                    else if (i < 14)
                    {
                        if (Creature* summon = instance->instance->SummonCreature(NPC_HELARJAR_MISTCALLER, me->GetPosition()))
                        {
                            summon->SetHomePosition(addsPos[i]);
                            summon->GetMotionMaster()->MoveTargetedHome();
                        }
                    }
                    else if (i < 16)
                    {
                        if (Creature* summon = instance->instance->SummonCreature(NPC_SEACURSED_MISTMENDER, me->GetPosition()))
                        {
                            summon->SetHomePosition(addsPos[i]);
                            summon->GetMotionMaster()->MoveTargetedHome();
                        }
                    }
                    else if (i < 18)
                    {
                        if (Creature* summon = instance->instance->SummonCreature(NPC_SEACURSED_SOULKEEPER, me->GetPosition()))
                        {
                            summon->SetHomePosition(addsPos[i]);
                            summon->GetMotionMaster()->MoveTargetedHome();
                        }
                    }
                }
            }
        }

        void EnterCombat(Unit* /*who*/) override
        {
            events.ScheduleEvent(EVENT_SUMMON_1, 1000);
            events.ScheduleEvent(EVENT_SUMMON_2, 10000);
            events.ScheduleEvent(EVENT_GIVE_NO, 7000);
            events.ScheduleEvent(EVENT_DEBILITATING, 13000);
            events.ScheduleEvent(EVENT_BIND, 17000);
        }

        void JustDied(Unit* /*killer*/) override
        {
            Talk(2);
            instance->SetBossState(DATA_SKJAL, DONE);
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
                    case EVENT_SUMMON_1:
                        for (uint8 i = 0; i < 2; i++)
                        {
                            if (Creature* sum = me->SummonCreature(98973, addsPos[i], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 4000))
                                sum->AI()->DoZoneInCombat(sum, 90.0f);
                        }
                        Talk(1);
                        events.ScheduleEvent(EVENT_SUMMON_1, 18000);
                        break;
                    case EVENT_SUMMON_2:
                        for (uint8 i = 0; i < 2; i++)
                        {
                            if (Creature* sum = me->SummonCreature(99447, addsPos[i], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 4000))
                                sum->AI()->DoZoneInCombat(sum, 90.0f);
                        }
                        Talk(1);
                        events.ScheduleEvent(EVENT_SUMMON_2, 18000);
                        break;
                    case EVENT_GIVE_NO:
                        if (Unit *target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                            me->CastSpell(target, SPELL_GIVE_NO);
                        events.ScheduleEvent(EVENT_GIVE_NO, 13000);
                        break;
                    case EVENT_DEBILITATING:
                        DoCast(SPELL_DEBILITATING);
                        events.ScheduleEvent(EVENT_DEBILITATING, 13000);
                        break;
                    case EVENT_BIND:
                        DoCast(SPELL_BIND);
                        events.ScheduleEvent(EVENT_BIND, 13000);
                        break;
                }
                
            }
            DoMeleeAttackIfReady();
        }
        private:
            EventMap events;
            InstanceScript* instance;
    };
    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_skyalAI (creature);
    }
};

//195309
class spell_helya_swirling_water : public SpellScriptLoader
{
    public:
        spell_helya_swirling_water() : SpellScriptLoader("spell_helya_swirling_water") {}

        class spell_helya_swirling_water_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_helya_swirling_water_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (auto caster = GetCaster()->ToPlayer())
                {
                    caster->AddAura(SPELL_SWIRLING_POOL_JUMP, caster);

                    caster->AddDelayedEvent(100, [caster]() -> void
                    {
                        if (caster && caster->isAlive() && caster->GetPositionZ() < 512.0f)
                        {
                            float x, y, z;
                            caster->GetClosePoint(x, y, z, caster->GetObjectSize(), 5.0f, frand(0.0f, 6.28f));
                            caster->GetMotionMaster()->MoveJump(x, y, z + 10.0f, 15.0f, 15.0f);
                        }
                    });
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_helya_swirling_water_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_helya_swirling_water_SpellScript();
        }
};

// trash 199589
class spell_whirpool_of_souls : public SpellScriptLoader
{
    public:                                                      
        spell_whirpool_of_souls() : SpellScriptLoader("spell_whirpool_of_souls") {}

        class spell_whirpool_of_souls_AuraScript : public AuraScript 
        {
            PrepareAuraScript(spell_whirpool_of_souls_AuraScript) 

            void OnPereodic(AuraEffect const* aurEff) 
            {
                PreventDefaultAction();
                Unit* caster = GetCaster();
                caster->CastSpell(caster, 199519);
                caster->CastSpell(caster->SelectNearbyTarget(), 199519);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_whirpool_of_souls_AuraScript::OnPereodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_whirpool_of_souls_AuraScript();
        }
};

//200208
class spell_brackwater : public SpellScriptLoader
{
    public:
        spell_brackwater() : SpellScriptLoader("spell_brackwater") {}

        class spell_brackwater_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_brackwater_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();
                if (Creature* pTarget = target->FindNearestCreature(98919, 40.0f, true))
                    {
                        pTarget->CastSpell(target, 201397, true);
                    }
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_brackwater_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_brackwater_AuraScript();
        }
};

//197752
class spell_helya_turbulent_waters : public SpellScriptLoader
{
    public:
        spell_helya_turbulent_waters() : SpellScriptLoader("spell_helya_turbulent_waters") {}

        class spell_helya_turbulent_waters_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_helya_turbulent_waters_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                PreventHitDefaultEffect(EFFECT_0);

                if (!GetCaster())
                    return;

                Position pos;
                GetCaster()->GetRandomNearPosition(pos, 30.0f);
                WorldLocation* dest = const_cast<WorldLocation*>(GetExplTargetDest());
                dest->Relocate(pos);
            }

            void Register() override
            {
                OnEffectLaunch += SpellEffectFn(spell_helya_turbulent_waters_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_TRIGGER_MISSILE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_helya_turbulent_waters_SpellScript();
        }
};

//197262
class spell_helya_taint_of_the_sea : public SpellScriptLoader
{
    public:
        spell_helya_taint_of_the_sea() : SpellScriptLoader("spell_helya_taint_of_the_sea") {}

        class spell_helya_taint_of_the_sea_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_helya_taint_of_the_sea_AuraScript);

            void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (!GetCaster() || !GetTarget())
                    return;

                if (GetCaster()->GetMap()->GetDifficultyID() != DIFFICULTY_NORMAL && GetCaster()->isInCombat())
                    GetCaster()->CastSpell(GetTarget(), SPELL_TAINTED_ESSENCE, true);
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_helya_taint_of_the_sea_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_helya_taint_of_the_sea_AuraScript();
        }
};

//227233
class spell_helya_corrupted_bellow_visual : public AuraScript
{
    PrepareAuraScript(spell_helya_corrupted_bellow_visual);

    Position centrPos;
    Position pos;
    float dist = 0;
    float distAngle = 0.f;

    void HandlePeriodicTick(AuraEffect const* aurEff)
    {
        dist = -8.0f;

        for (uint8 i = 0; i < 12; ++i)
        {
            if (!(i % 3))
            {
                GetTarget()->GetNearPoint2D(centrPos, dist, 0.f);
                dist += 8.f;
                distAngle = 1.5f;
            }

            centrPos.SimplePosXYRelocationByAngle(pos, distAngle, GetTarget()->GetAngle(centrPos.GetPositionX(), centrPos.GetPositionY()) - 1.57f);
            GetTarget()->PlaySpellVisual(pos, 61273, 95.0f, ObjectGuid::Empty, false);

            centrPos.SimplePosXYRelocationByAngle(pos, distAngle, GetTarget()->GetAngle(centrPos.GetPositionX(), centrPos.GetPositionY()) + 1.57f);
            GetTarget()->PlaySpellVisual(pos, 61273, 95.0f, ObjectGuid::Empty, false);

            distAngle += 1.5f + float(i / 2.5f);
        }
    }

    void Register()
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_helya_corrupted_bellow_visual::HandlePeriodicTick, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

void AddSC_boss_helya()
{
    new boss_helya();
    new npc_helya_tentacle();
    new npc_helya_tentacle_veh();
    new npc_helya_destructor_tentacle_veh();
    new npc_skyal();
    new spell_helya_swirling_water();
    new spell_whirpool_of_souls();
    new spell_brackwater();
    new spell_helya_turbulent_waters();
    new spell_helya_taint_of_the_sea();
    RegisterAuraScript(spell_helya_corrupted_bellow_visual);
}