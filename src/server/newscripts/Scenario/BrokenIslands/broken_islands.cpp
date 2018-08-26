/*######
## uwow.biz
## The Broken Islands Scenario
######*/

#include "LFGMgr.h"
#include "LFGQueue.h"
#include "LFGPackets.h"
#include "broken_islands.h"
#include "DynamicObject.h"
#include "ScriptedEscortAI.h"
#include "CreatureTextMgr.h"
#include "MiscPackets.h"
#include "GameObjectAI.h"
#include "QuestData.h"

#define GOSSIP_ACCEPT_DUEL      "Let''s duel"
#define EVENT_SPECIAL 20

enum eDuelEnums
{
    SAY_DUEL_A = -1609080,
    SAY_DUEL_B = -1609081,
    SAY_DUEL_C = -1609082,
    SAY_DUEL_D = -1609083,
    SAY_DUEL_E = -1609084,
    SAY_DUEL_F = -1609085,
    SAY_DUEL_G = -1609086,
    SAY_DUEL_H = -1609087,
    SAY_DUEL_I = -1609088,

    SPELL_DUEL = 52996,
    //SPELL_DUEL_TRIGGERED        = 52990,
    SPELL_DUEL_VICTORY = 52994,
    SPELL_DUEL_FLAG = 52991,

    QUEST_42782 = 42782,
    QUEST_44281 = 44281,
    FACTION_HOSTILE = 2068
};

int32 _m_auiRandomSay[] =
{
    SAY_DUEL_A, SAY_DUEL_B, SAY_DUEL_C, SAY_DUEL_D, SAY_DUEL_E, SAY_DUEL_F, SAY_DUEL_G, SAY_DUEL_H, SAY_DUEL_I
};


class npc_q42782 : public CreatureScript
{
public:
    npc_q42782(const std::string str) : CreatureScript(str.c_str()) { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF)
        {
            player->CLOSE_GOSSIP_MENU();
            if (player->isInCombat() || creature->isInCombat())
                return true;

            if (npc_q42782AI* pInitiateAI = CAST_AI(npc_q42782::npc_q42782AI, creature->AI()))
            {
                if (pInitiateAI->m_bIsDuelInProgress)
                    return true;
            }
            creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
            creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_15);

            int32 uiSayId = rand() % (sizeof(_m_auiRandomSay) / sizeof(int32));
            DoScriptText(_m_auiRandomSay[uiSayId], creature, player);

            player->CastSpell(creature, SPELL_DUEL, false);
            player->CastSpell(player, SPELL_DUEL_FLAG, true);
        }
        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if ((player->GetQuestStatus(QUEST_42782) == QUEST_STATUS_INCOMPLETE ||
            player->GetQuestStatus(QUEST_44281) == QUEST_STATUS_INCOMPLETE)
            && creature->IsFullHealth())
        {
            if (player->HealthBelowPct(10))
                return true;

            if (player->isInCombat() || creature->isInCombat())
                return true;

            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ACCEPT_DUEL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        }
        return true;
    }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_q42782AI(creature);
    }

    struct npc_q42782AI : public CombatAI
    {
        npc_q42782AI(Creature* creature) : CombatAI(creature)
        {
            m_bIsDuelInProgress = false;
        }

        bool lose;
        ObjectGuid m_uiDuelerGUID;
        uint32 m_uiDuelTimer;
        bool m_bIsDuelInProgress;
        uint32 spelltimer;

        void Reset() override
        {
            lose = false;
            me->RestoreFaction();
            CombatAI::Reset();

            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_15);

            m_uiDuelerGUID.Clear();
            m_uiDuelTimer = 5000;
            spelltimer = 2000;
            m_bIsDuelInProgress = false;
        }

        void SpellHit(Unit* pCaster, const SpellInfo* pSpell) override
        {
            if (!m_bIsDuelInProgress && pSpell->Id == SPELL_DUEL)
            {
                m_uiDuelerGUID = pCaster->GetGUID();
                m_bIsDuelInProgress = true;
            }
        }

        void DamageTaken(Unit* pDoneBy, uint32 &uiDamage, DamageEffectType dmgType) override
        {
            if (m_bIsDuelInProgress && pDoneBy->IsControlledByPlayer())
            {
                if (pDoneBy->GetGUID() != m_uiDuelerGUID && pDoneBy->GetOwnerGUID() != m_uiDuelerGUID) // other players cannot help
                    uiDamage = 0;
                else if (uiDamage >= me->GetHealth())
                {
                    uiDamage = 0;

                    if (!lose)
                    {
                        if (Player* plr = pDoneBy->ToPlayer())
                            plr->KilledMonsterCredit(108722);

                        pDoneBy->RemoveGameObject(SPELL_DUEL_FLAG, true);
                        pDoneBy->AttackStop();
                        me->CastSpell(pDoneBy, SPELL_DUEL_VICTORY, true);
                        lose = true;
                        me->CastSpell(me, 7267, true);
                        me->RestoreFaction();
                    }
                }
            }
        }

        void UpdateAI(uint32 uiDiff) override
        {
            if (!UpdateVictim())
            {
                if (m_bIsDuelInProgress)
                {
                    if (m_uiDuelTimer <= uiDiff)
                    {
                        me->setFaction(FACTION_HOSTILE);

                        if (Unit* unit = Unit::GetUnit(*me, m_uiDuelerGUID))
                            AttackStart(unit);
                    }
                    else
                        m_uiDuelTimer -= uiDiff;
                }
                return;
            }

            if (m_bIsDuelInProgress)
            {
                if (lose)
                {
                    if (!me->HasAura(7267))
                        EnterEvadeMode();
                    return;
                }
                else if (me->getVictim() && me->getVictim()->GetTypeId() == TYPEID_PLAYER && me->getVictim()->HealthBelowPct(10))
                {
                    me->getVictim()->CastSpell(me->getVictim(), 7267, true); // beg
                    me->getVictim()->RemoveGameObject(SPELL_DUEL_FLAG, true);
                    EnterEvadeMode();
                    return;
                }

                if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (spelltimer <= uiDiff)
                {
                    uint32 spell = 0;
                    switch (me->GetEntry())
                    {
                        case 108752:
                            spell = 172673;
                            spelltimer = 2000;
                            break;
                        case 108765:
                            spell = 198623;
                            spelltimer = 2000;
                            break;
                        case 108767:
                        case 108750:
                            spell = 172757;
                            spelltimer = 11000;
                            break;
                        case 108723:
                            spell = 171777;
                            spelltimer = 5500;
                            break;
                        default:
                            break;
                    }
                    if (spell > 0)
                        DoCast(spell);
                }
                else
                    spelltimer -= uiDiff;
            }

            // TODO: spells

            CombatAI::UpdateAI(uiDiff);
        }
    };
};

class npc_q44281 : public npc_q42782
{
public:
    npc_q44281() : npc_q42782("npc_q44281") { }
};

/*
ClientToServer: CMSG_CLOSE_INTERACTION (0x348A) Length: 12 ConnIdx: 2 Time: 06/05/2016 08:13:31.801 Number: 23743
Guid: Full: 0x20209000006A5E00001EB7000053DE11; HighType: Creature; Low: 5496337; Map: 0; Entry: 108920;

ServerToClient: SMSG_LFG_UPDATE_STATUS (0x2A24) Length: 39 ConnIdx: 0 Time: 06/05/2016 08:13:31.802 Number: 23744
(Ticket) RequesterGuid: Full: 0x080F28000000000000000000001C37E5; HighType: Player; Low: 1849317 Name: пњљпњљпњљпњљ; Map: 0
(Ticket) Id: 76194
(Ticket) Type: TICKET_TYPE_LFD_SYSTEM (2)
(Ticket) Time: 06/05/2016 08:13:46
SubType: LFG_SUBTYPE_SCENARIO (3)
Reason: LFG_UPDATETYPE_REMOVED_FROM_QUEUE (25)
SlotsCount: 1
RequestedRoles: 8
SuspendedPlayersCount: 0
[0] Slots: 16778124
IsParty: True
NotifyUI: False
Joined: True
LfgJoined: True
Queued: False

ServerToClient: SMSG_LFG_UPDATE_STATUS (0x2A24) Length: 39 ConnIdx: 0 Time: 06/05/2016 08:13:31.805 Number: 23745
(Ticket) RequesterGuid: Full: 0x080F28000000000000000000001C37E5; HighType: Player; Low: 1849317 Name: пњљпњљпњљпњљ; Map: 0
(Ticket) Id: 76194
(Ticket) Type: TICKET_TYPE_LFD_SYSTEM (2)
(Ticket) Time: 06/05/2016 08:13:46
SubType: LFG_SUBTYPE_SCENARIO (3)
Reason: LFG_UPDATETYPE_ADDED_TO_QUEUE (13)
SlotsCount: 1
RequestedRoles: 8
SuspendedPlayersCount: 0
[0] Slots: 16778124
IsParty: True
NotifyUI: False
Joined: True
LfgJoined: True
Queued: True

ServerToClient: SMSG_LFG_JOIN_RESULT (0x2A1C) Length: 26 ConnIdx: 0 Time: 06/05/2016 08:13:31.807 Number: 23746
(Ticket) RequesterGuid: Full: 0x080F28000000000000000000001C37E5; HighType: Player; Low: 1849317 Name: пњљпњљпњљпњљ; Map: 0
(Ticket) Id: 76194
(Ticket) Type: TICKET_TYPE_LFD_SYSTEM (2)
(Ticket) Time: 06/05/2016 08:13:46
Result: Ok (0)
ResultDetail: 0
BlackListCount: 0

ServerToClient: SMSG_LFG_UPDATE_STATUS (0x2A24) Length: 39 ConnIdx: 0 Time: 06/05/2016 08:13:31.808 Number: 23747
(Ticket) RequesterGuid: Full: 0x080F28000000000000000000001C37E5; HighType: Player; Low: 1849317 Name: пњљпњљпњљпњљ; Map: 0
(Ticket) Id: 76194
(Ticket) Type: TICKET_TYPE_LFD_SYSTEM (2)
(Ticket) Time: 06/05/2016 08:13:46
SubType: LFG_SUBTYPE_SCENARIO (3)
Reason: LFG_UPDATETYPE_ADDED_TO_QUEUE (13)
SlotsCount: 1
RequestedRoles: 8
SuspendedPlayersCount: 0
[0] Slots: 16778124
IsParty: True
NotifyUI: False
Joined: True
LfgJoined: True
Queued: True

---------------------------------->

ServerToClient: SMSG_LFG_UPDATE_STATUS (0x2A24) Length: 39 ConnIdx: 0 Time: 06/05/2016 08:13:32.894 Number: 23809
(Ticket) RequesterGuid: Full: 0x080F28000000000000000000001C37E5; HighType: Player; Low: 1849317 Name: пњљпњљпњљпњљ; Map: 0
(Ticket) Id: 76194
(Ticket) Type: TICKET_TYPE_LFD_SYSTEM (2)
(Ticket) Time: 06/05/2016 08:13:46
SubType: LFG_SUBTYPE_SCENARIO (3)
Reason: LFG_UPDATETYPE_PROPOSAL_BEGIN (14)
SlotsCount: 1
RequestedRoles: 8
SuspendedPlayersCount: 0
[0] Slots: 16778124
IsParty: True
NotifyUI: False
Joined: True
LfgJoined: True
Queued: False

ServerToClient: SMSG_LFG_PROPOSAL_UPDATE (0x2A2D) Length: 61 ConnIdx: 0 Time: 06/05/2016 08:13:32.896 Number: 23810
(Ticket) RequesterGuid: Full: 0x080F28000000000000000000001C37E5; HighType: Player; Low: 1849317 Name: пњљпњљпњљпњљ; Map: 0
(Ticket) Id: 76194
(Ticket) Type: TICKET_TYPE_LFD_SYSTEM (2)
(Ticket) Time: 06/05/2016 08:13:46
InstanceID: 2252081288682140542
ProposalID: 8864
Slot: 16778124
State: Initiating (0)
CompletedMask: 0
PlayersCount: 3
[0] Roles: 448
[0] Me: False
[0] SameParty: False
[0] MyParty: False
[0] Responded: False
[0] Accepted: False
[1] Roles: 24
[1] Me: False
[1] SameParty: False
[1] MyParty: False
[1] Responded: False
[1] Accepted: False
[2] Roles: 280
[2] Me: False
[2] SameParty: False
[2] MyParty: False
[2] Responded: False
[2] Accepted: False
ValidCompletedMask: True
ProposalSilent: False

ServerToClient: SMSG_LFG_PROPOSAL_UPDATE (0x2A2D) Length: 61 ConnIdx: 0 Time: 06/05/2016 08:13:32.898 Number: 23811
(Ticket) RequesterGuid: Full: 0x080F28000000000000000000001C37E5; HighType: Player; Low: 1849317 Name: пњљпњљпњљпњљ; Map: 0
(Ticket) Id: 76194
(Ticket) Type: TICKET_TYPE_LFD_SYSTEM (2)
(Ticket) Time: 06/05/2016 08:13:46
InstanceID: 2252081288682140542
ProposalID: 8864
Slot: 16778124
State: Success (2)
CompletedMask: 0
PlayersCount: 3
[0] Roles: 448
[0] Me: False
[0] SameParty: False
[0] MyParty: False
[0] Responded: False
[0] Accepted: False
[1] Roles: 24
[1] Me: False
[1] SameParty: False
[1] MyParty: False
[1] Responded: False
[1] Accepted: False
[2] Roles: 280
[2] Me: False
[2] SameParty: False
[2] MyParty: False
[2] Responded: False
[2] Accepted: False
ValidCompletedMask: True
ProposalSilent: False

ServerToClient: SMSG_LFG_UPDATE_STATUS (0x2A24) Length: 39 ConnIdx: 0 Time: 06/05/2016 08:13:32.900 Number: 23812
(Ticket) RequesterGuid: Full: 0x080F28000000000000000000001C37E5; HighType: Player; Low: 1849317 Name: пњљпњљпњљпњљ; Map: 0
(Ticket) Id: 76194
(Ticket) Type: TICKET_TYPE_LFD_SYSTEM (2)
(Ticket) Time: 06/05/2016 08:13:46
SubType: LFG_SUBTYPE_SCENARIO (3)
Reason: LFG_UPDATETYPE_GROUP_FOUND (11)
SlotsCount: 1
RequestedRoles: 8
SuspendedPlayersCount: 0
[0] Slots: 16778124
IsParty: True
NotifyUI: False
Joined: False
LfgJoined: False
Queued: False

ServerToClient: SMSG_LFG_UPDATE_STATUS (0x2A24) Length: 39 ConnIdx: 0 Time: 06/05/2016 08:13:32.901 Number: 23813
(Ticket) RequesterGuid: Full: 0x080F28000000000000000000001C37E5; HighType: Player; Low: 1849317 Name: пњљпњљпњљпњљ; Map: 0
(Ticket) Id: 76194
(Ticket) Type: TICKET_TYPE_LFD_SYSTEM (2)
(Ticket) Time: 06/05/2016 08:13:46
SubType: LFG_SUBTYPE_SCENARIO (3)
Reason: LFG_UPDATETYPE_ADDED_TO_QUEUE (13)
SlotsCount: 1
RequestedRoles: 8
SuspendedPlayersCount: 0
[0] Slots: 16778124
IsParty: True
NotifyUI: True
Joined: True
LfgJoined: True
Queued: True

ServerToClient: SMSG_LFG_JOIN_RESULT (0x2A1C) Length: 26 ConnIdx: 0 Time: 06/05/2016 08:13:32.904 Number: 23814
(Ticket) RequesterGuid: Full: 0x080F28000000000000000000001C37E5; HighType: Player; Low: 1849317 Name: пњљпњљпњљпњљ; Map: 0
(Ticket) Id: 76194
(Ticket) Type: TICKET_TYPE_LFD_SYSTEM (2)
(Ticket) Time: 06/05/2016 08:13:46
Result: Ok (0)
ResultDetail: 0
BlackListCount: 0

ServerToClient: SMSG_LFG_UPDATE_STATUS (0x2A24) Length: 39 ConnIdx: 0 Time: 06/05/2016 08:13:32.905 Number: 23815
(Ticket) RequesterGuid: Full: 0x080F28000000000000000000001C37E5; HighType: Player; Low: 1849317 Name: пњљпњљпњљпњљ; Map: 0
(Ticket) Id: 76194
(Ticket) Type: TICKET_TYPE_LFD_SYSTEM (2)
(Ticket) Time: 06/05/2016 08:13:46
SubType: LFG_SUBTYPE_SCENARIO (3)
Reason: LFG_UPDATETYPE_ROLECHECK_ABORTED (6)
SlotsCount: 1
RequestedRoles: 8
SuspendedPlayersCount: 0
[0] Slots: 16778124
IsParty: True
NotifyUI: True
Joined: True
LfgJoined: True
Queued: True

ServerToClient: SMSG_PLAY_SCENE (0x264F) Length: 34 ConnIdx: 0 Time: 06/05/2016 08:13:32.907 Number: 23816
SceneID: 1335
PlaybackFlags: 26
SceneInstanceID: 1
SceneScriptPackageID: 1661
TransportGUID: Full: 0x0
Pos: X: -8336.28 Y: 1376.329 Z: 9.09365
Facing: 3.868121

ServerToClient: SMSG_AURA_UPDATE (0x2C21) Length: 44 ConnIdx: 0 Time: 06/05/2016 08:13:32.921 Number: 23817
UpdateAll: False
AurasCount: 1
[0] Slot: 35
[0] HasAura: True
[0] CastGuid: Full: 0xBC20900000D34903001EB7000053DF3C; HighType: Spell; Low: 5496636; Map: 0; Entry: 216356;
[0] SpellID: 216356 (216356)
[0] SpellXSpellVisualID: 0
[0] Flags: NoCaster, Positive (3)
[0] ActiveFlags: 3
[0] CastLevel: 100
[0] Applications: 0
[0] HasCastUnit: False
[0] HasDuration: False
[0] HasRemaining: False
[0] hasUnkFloat: False
[0] PointsSize: 0
[0] EstimatedPointsSize: 0
[0] HasPlayerStatsInfo: False
UnitGUID: Full: 0x080F28000000000000000000001C37E5; HighType: Player; Low: 1849317 Name: пњљпњљпњљпњљ; Map: 0

*/




class npc_q42740 : public CreatureScript
{
public:
    npc_q42740() : CreatureScript("npc_q42740") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();

        //214608
        player->KilledMonsterCredit(creature->GetEntry());
        player->CastSpell(player, 216356, false); //scene

        return true;
    };
};

class npc_q40518 : public CreatureScript
{
public:
    npc_q40518() : CreatureScript("npc_q40518") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();

        //214608
        player->KilledMonsterCredit(creature->GetEntry());
        player->CastSpell(player, 225147, false); //scene

        return true;
    };
};

class sceneTrigger_enterBrockenShores : public SceneTriggerScript
{
public:
    sceneTrigger_enterBrockenShores() : SceneTriggerScript("enterBrockenShores")
    {}

    bool OnTrigger(Player* player, SpellScene const* trigger, std::string type) override
    {
        std::set<uint32> Slot;
        Slot.insert(908); //The Battle for Broken Shore
        sLFGMgr->JoinLfg(player, player->GetSpecializationRoleMaskForGroup(), Slot);
        return true;
    }
};


//! 227058 WARN! Spell not exits.
class spell_q42740 : public SpellScriptLoader
{
public:
    spell_q42740() : SpellScriptLoader("spell_q42740") { }

    class spell_q42740_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_q42740_SpellScript);

        enum data
        {
            QUEST = 42740,
            NPC_CONV = 581
        };

        void HandleScriptEffect(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(EFFECT_0);
            if (Unit* caster = GetCaster())
            {
                Player *player = caster->ToPlayer();
                if (!player)
                    return;

                std::set<uint32> Slot;
                Slot.insert(908);
                sLFGMgr->JoinLfg(player, player->GetSpecializationRoleMaskForGroup(), Slot);
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_q42740_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_q42740_SpellScript();
    }
};


//! 217781 phase update.
class spell_bi_enter_stage1 : public SpellScriptLoader
{
public:
    spell_bi_enter_stage1() : SpellScriptLoader("spell_bi_enter_stage1") { }

    class spell_bi_enter_stage1_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_bi_enter_stage1_AuraScript);


        void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (Unit* caster = GetCaster())
            {
                Player *player = caster->ToPlayer();
                if (!player)
                    return;


                Map* m = player->FindMap();
                if (!m)
                    return;
                InstanceScript *script = player->GetInstanceScript();
                if (!script)
                    return;

                ObjectGuid guid = script->GetGuidData(player->GetTeam() == HORDE ? GO_HORDE_SHIP : GO_ALLIANCE_SHIP);
                if (!guid)
                    return;

                if (GameObject *go = m->GetGameObject(guid))
                    go->SetVisible(true);

                if (script->getScenarionStep() != 0)
                    return;

                //scenation ID 1189 step 0
                player->UpdateAchievementCriteria(CRITERIA_TYPE_SCRIPT_EVENT_2, 54140);

                //scenation ID 786 step 0
                player->UpdateAchievementCriteria(CRITERIA_TYPE_SCRIPT_EVENT_2, 44060);


                Map::PlayerList const &PlList = player->GetMap()->GetPlayers();

                if (PlList.isEmpty())
                    return;

                for (Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
                    if (Player* plr = i->getSource())
                    {
                        if (Transport* transport = plr->GetTransport())
                        {
                            transport->RemovePassenger(plr);
                            plr->setTransport(NULL);
                            plr->m_movementInfo.transport.Reset();
                        }

                        plr->SendMovieStart(486);
                        plr->CastSpell(plr, plr->GetTeam() == ALLIANCE ? 199358 : 225152, false);
                    }
            }
        }

        void Register() override
        {
            AfterEffectApply += AuraEffectApplyFn(spell_bi_enter_stage1_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_bi_enter_stage1_AuraScript();
    }
};

class sceneTrigger_part1 : public SceneTriggerScript
{
public:
    sceneTrigger_part1() : SceneTriggerScript("sceneTrigger_part1")
    {}

    bool OnTrigger(Player* player, SpellScene const* trigger, std::string type) override
    {
        if (type == "farsight")
        {
            if (auto data = player->GetInstanceScript())
            {
                Map* m = player->FindMap();
                if (!m)
                    return true;

                WorldLocation destTarget;
                destTarget.m_mapId = 1460;
                if (player->GetTeam() == HORDE)
                {
                    destTarget.m_positionX = 591.77f;
                    destTarget.m_positionY = 1898.48f;
                    destTarget.m_positionZ = 1.41f;
                    m->LoadGrid(521.7239f, 1862.63f);
                }
                else
                {
                    destTarget.m_positionX = 472.92f;
                    destTarget.m_positionY = 2037.86f;
                    destTarget.m_positionZ = 0.09f;
                    m->LoadGrid(461.8785f, 2032.679f);
                }
                destTarget.m_orientation = 0.5030039f;

                m->LoadGrid(destTarget.m_positionX, destTarget.m_positionY);

                DynamicObject* dynObj = new DynamicObject(true);
                if (!dynObj->CreateDynamicObject(sObjectMgr->GetGenerator<HighGuid::DynamicObject>()->Generate(), player, player->GetTeam() == HORDE ? 225153 : 215222, destTarget, 1.0f, DYNAMIC_OBJECT_AREA_SPELL))
                {
                    delete dynObj;
                    return true;
                }

                //dynObj->SetUInt32Value(DYNAMICOBJECT_FIELD_BYTES, 536903323);
                dynObj->SetDuration(60000);

                player->SetViewpoint(dynObj, true);
                player->SetUInt32Value(UNIT_FIELD_CHANNEL_SPELL, player->GetTeam() == HORDE ? 225153 : 215222);
                player->SetUInt32Value(UNIT_FIELD_CHANNEL_SPELL_XSPELL_VISUAL, player->GetTeam() == HORDE ? 107211 : 101301);
            }
        }
        if (type == "phaseupdate")
        {
            Map* m = player->FindMap();
            if (!m)
                return true;
            InstanceScript *script = player->GetInstanceScript();
            if (!script)
                return true;

            ObjectGuid guid = script->GetGuidData(player->GetTeam() == HORDE ? GO_HORDE_SHIP : GO_ALLIANCE_SHIP);
            if (!guid)
                return true;

            if (GameObject *go = m->GetGameObject(guid))
                go->SetVisible(true);
        }
        if (type == "port")
        {
            InstanceScript *script = player->GetInstanceScript();

            if (!script)
                return true;

            if (script->getScenarionStep() != 0)
                return true;

            //scenation ID 1189 step 0
            player->UpdateAchievementCriteria(CRITERIA_TYPE_SCRIPT_EVENT_2, 54140);

            //scenation ID 786 step 0
            player->UpdateAchievementCriteria(CRITERIA_TYPE_SCRIPT_EVENT_2, 44060);

            if (WorldObject* obj = player->GetViewpoint())
                player->SetViewpoint(obj, false);

            player->SetUInt32Value(UNIT_FIELD_CHANNEL_SPELL, 0);
            player->SetUInt32Value(UNIT_FIELD_CHANNEL_SPELL_XSPELL_VISUAL, 0);

            if (Transport* transport = player->GetTransport())
            {
                transport->RemovePassenger(player);
                player->setTransport(NULL);
                player->m_movementInfo.transport.Reset();
            }

            Map::PlayerList const &PlList = player->GetMap()->GetPlayers();

            if (PlList.isEmpty())
                return true;

            for (Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
                if (Player* plr = i->getSource())
                    plr->CastSpell(plr, plr->GetTeam() == ALLIANCE ? 199358 : 225152, false);
        }
        if (type == "complete")
        {
            //scenation ID 1189 step 0
            player->UpdateAchievementCriteria(CRITERIA_TYPE_SCRIPT_EVENT_2, 54140);

            //scenation ID 786 step 0
            player->UpdateAchievementCriteria(CRITERIA_TYPE_SCRIPT_EVENT_2, 44060);

            /*if (player->GetDistance(461.8785f, 2032.679f, 0.1627506f) > 300.0f)
            {
                if (WorldObject* obj = player->GetViewpoint())
                    player->SetViewpoint(obj, false);

                if (Transport* transport = player->GetTransport())
                {
                    transport->RemovePassenger(player);
                    player->setTransport(NULL);
                    player->m_movementInfo.transport.Reset();
                }

                player->CastSpell(player, player->GetTeam() == ALLIANCE ? 199358 : 225152, false);
            }*/
        }
        return true;
    }
};

//! 90705 Dread Commander Arganoth slain
class npc_bi_dread_commander : public CreatureScript
{
public:
    npc_bi_dread_commander() : CreatureScript("npc_bi_dread_commander") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_bi_dread_commanderAI(creature);
    }

    struct npc_bi_dread_commanderAI : public npc_escortAI
    {
        npc_bi_dread_commanderAI(Creature* creature) : npc_escortAI(creature), summons(me)
        {
            onFinish = false;
            firstcheck = false;
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
            /*  if (InstanceScript *script = me->GetInstanceScript())
              {
                  if (script->getScenarionStep() < 3)
                  {
                      Start(false, true);
                  }
              } */
        }

        bool PlayerOn;
        bool onFinish;
        bool firstcast;
        bool firstcheck;
        bool below_1;
        EventMap events;
        SummonList summons;

        void Reset() override
        {
            firstcast = true;
            PlayerOn = false;
            below_1 = false;
            SetDespawnAtEnd(false);
            events.Reset();
            summons.DespawnAll();

            if (!me->isAlive() || onFinish)
                return;

            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);


            /*   if (InstanceScript *script = me->GetInstanceScript())
               {
                   if (script->getScenarionStep() < 3)
                   {
                       Start(false, true);
                   }
               } */
        }

        void JustSummoned(Creature* summon) override
        {
            summons.Summon(summon);
        }

        void EnterCombat(Unit* who) override
        {
            firstcast = true;
            below_1 = false;
            events.ScheduleEvent(EVENT_1, 8000); // 200465
            events.ScheduleEvent(EVENT_2, 13000); // 183956
            events.ScheduleEvent(EVENT_3, 9000); // conv
            who->CastSpell(who, 199677);
            /* std::list<Creature*> guards;
             GetCreatureListWithEntryInGrid(guards, me, 90717, 30.0f);
             GetCreatureListWithEntryInGrid(guards, me, 90714, 30.0f);
             GetCreatureListWithEntryInGrid(guards, me, 93219, 30.0f);
             GetCreatureListWithEntryInGrid(guards, me, 97496, 30.0f);
             GetCreatureListWithEntryInGrid(guards, me, 97486, 30.0f);
             GetCreatureListWithEntryInGrid(guards, me, 93219, 30.0f);
             if (!guards.empty())
                 for (std::list<Creature*>::iterator itr = guards.begin(); itr != guards.end(); ++itr)
                 {
                     (*itr)->AI()->AttackStart(me);
                 } */
        }

        void JustDied(Unit* who) override
        {
            if (InstanceScript *script = me->GetInstanceScript())
                if (Map *m = script->instance)
                    if (Creature* sayer = m->GetCreature(script->GetGuidData(90717)))
                        sayer->AI()->DoAction(1);
            summons.DespawnAll();
        }

        void MoveInLineOfSight(Unit* who) override  // пњљпњљпњљпњљпњљ пњљпњљ пњљпњљпњљпњљпњљпњљпњљпњљ пњљпњљпњљпњљпњљпњљ, пњљпњљпњљ пњљпњљпњљпњљпњљпњљпњљ.........
        {
            if (who->GetTypeId() != TYPEID_PLAYER)
                return;

            if (onFinish || firstcheck)
                return;

            if (!me->isAlive())
                return;

            firstcheck = true;
            if (InstanceScript *script = me->GetInstanceScript())
            {
                if (script->getScenarionStep() < 3)
                {
                    Start(false, true);
                }
                else 
                {
                    if (onFinish)
                        return;

                    if (script->getScenarionStep() == 2)
                    {
                        DoCast(218619);
                        onFinish = true;
                        SetNextWaypoint(9, false, false);
                    }
                 
                 }
            }
        }

        void DamageTaken(Unit* attacker, uint32& /*damage*/, DamageEffectType dmgType) override
        {
            if (!below_1 && me->HealthBelowPct(3))
            {
                me->CastSpell(me, 182647, true);
                below_1 = true;
                // me->Kill(me);
            }
        }

        void WaypointReached(uint32 i) override
        {
            switch (i)
            {
                case 8:
                    SetNextWaypoint(1, false, false);
                    break;
                case 9:
                    DoCast(199676);
                    break;
                case 11:
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
                    SetEscortPaused(true);
                    me->SetInCombatWithZone();
                    break;
                default:
                {
                    if (onFinish)
                        break;

                    if (InstanceScript *script = me->GetInstanceScript())
                    {
                        if (script->getScenarionStep() == 2)
                        {
                            DoCast(218619);
                            onFinish = true;
                            SetNextWaypoint(9, false, false);
                        }
                    }
                    break;
                }
            }
        }

        void UpdateAI(uint32 diff) override
        {
            npc_escortAI::UpdateAI(diff);

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
                        DoCast(200465);
                        events.ScheduleEvent(EVENT_1, 8000);
                        break;
                    case EVENT_2:
                        if (firstcast)
                        {
                            DoCast(182441);
                            firstcast = false;
                        }
                        DoCast(183956);
                        events.ScheduleEvent(EVENT_2, 13000);
                        break;
                    case EVENT_3:
                        DoCast(185271);
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };
};

//! 93719 fel-commander-azgalor
class npc_bi_felcommander_azgalor : public CreatureScript
{
public:
    npc_bi_felcommander_azgalor() : CreatureScript("npc_bi_felcommander_azgalor") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_bi_felcommander_azgalorAI(creature);
    }

    struct npc_bi_felcommander_azgalorAI : ScriptedAI
    {
        npc_bi_felcommander_azgalorAI(Creature* creature) : ScriptedAI(creature)
        {
            below_1 = false;
            checkconv = false;
        }

        EventMap events;
        bool below_1;
        bool checkconv;

        void Reset() override
        {
            checkconv = false;
            if (!me->isAlive())
                return;

            events.Reset();

            if (InstanceScript *script = me->GetInstanceScript())
            {
                if (script->getScenarionStep() == 2)
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
                    return;
                }
            }
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
        }

        void DoAction(int32 const action) override
        {
            me->GetMotionMaster()->MovePoint(1, 667.1615f, 1929.12f, 5.4915f);
            DoCast(224906);
        }

        void EnterCombat(Unit* who) override
        {
            DoCast(224910);
            events.ScheduleEvent(EVENT_1, 7000); // 224907
            events.ScheduleEvent(EVENT_2, 23000); // 224908 + пњљпњљпњљпњљ
        }

        void MovementInform(uint32 moveType, uint32 pointId) override
        {
            if (moveType != POINT_MOTION_TYPE)
                return;

            me->SetHomePosition(667.1615f, 1929.12f, 5.4915f, me->GetOrientation());
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
            me->SetInCombatWithZone();
        }

        void DamageTaken(Unit* attacker, uint32& /*damage*/, DamageEffectType dmgType) override
        {
            if (!below_1 && me->HealthBelowPct(3))
            {
                me->CastSpell(me, 224911, true);
                below_1 = true;
                // me->Kill(me);
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
                        DoCast(224907);
                        events.ScheduleEvent(EVENT_1, 7000); // 224907
                        break;

                    case EVENT_2:
                        DoCast(224908);
                        events.ScheduleEvent(EVENT_2, 23000); // 224908 + пњљпњљпњљпњљ
                        if (!checkconv)
                        {
                            DoCast(224909);
                            checkconv = true;
                        }
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };
};

//! 182420 scene 6
class spell_scenarion_bi_step_6 : public SpellScriptLoader
{
public:
    spell_scenarion_bi_step_6() : SpellScriptLoader("spell_scenarion_bi_step_6") { }

    class spell_scenarion_bi_step_6_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_scenarion_bi_step_6_SpellScript);

        enum data
        {
            CRITERIA_VALUE = 53062,
        };


        void HandleScriptEffect(SpellEffIndex effIndex)
        {
            Unit* p = GetOriginalCaster();
            if (!p)
                return;

            Player *player = p->ToPlayer();
            if (!player)
                return;

            //criteria 31029. rew. 2
            player->UpdateAchievementCriteria(CRITERIA_TYPE_SCRIPT_EVENT_2, CRITERIA_VALUE);
        }

        void Register() override
        {
            OnEffectLaunchTarget += SpellEffectFn(spell_scenarion_bi_step_6_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_scenarion_bi_step_6_SpellScript();
    }
};

class sceneTrigger_part7 : public SceneTriggerScript
{
public:
    sceneTrigger_part7() : SceneTriggerScript("sceneTrigger_part7")
    {}

    enum data
    {
        __spell = 181546,
    };
    bool OnTrigger(Player* player, SpellScene const* trigger, std::string type) override
    {
        if (type == "port")
        {
            player->CastSpell(player, __spell, false);
        }
        return true;
    }
};


class scenarion_bi_heroes : public CreatureScript
{
public:
    scenarion_bi_heroes() : CreatureScript("scenarion_bi_heroes") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new scenarion_bi_heroeslAI(creature);
    }

    struct scenarion_bi_heroeslAI : ScriptedAI
    {
        scenarion_bi_heroeslAI(Creature* creature) : ScriptedAI(creature)
        {
            firstcheck = false;
            check = false;
            check1 = false;
            check2 = false;
            check3 = false;
            tempcheck = false;
            //  if (me->GetEntry() == 90717 || me->GetEntry() == 90714)
              //    me->SetReactState(REACT_AGGRESSIVE);
        }

        bool firstcheck;
        bool check;
        bool check1;
        bool check2;
        bool check3;
        bool tempcheck;
        uint32 timer;
        EventMap events;

        void Reset() override
        {
            events.Reset();
        }

        void MovementInform(uint32 moveType, uint32 pointId) override
        {
            if (moveType == POINT_MOTION_TYPE || moveType == EFFECT_MOTION_TYPE || moveType == WAYPOINT_MOTION_TYPE)
            {
                //intro 9
                if (me->GetEntry() == NPC_VARIAN)
                {
                    InstanceScript *script = me->GetInstanceScript();
                    if (!script)
                        return;

                    if (script->getScenarionStep() != SCENARION_STEP_9)
                        return;

                    Map *m = script->instance;
                    if (!m)
                        return;

                    Creature* gualdan = m->GetCreature(script->GetGuidData(NPC_GULDAN));
                    if (!gualdan)
                        return;

                    gualdan->NearTeleportTo(1660.127f, 1655.793f, 79.36142f, 2.33f);
                    //sCreatureTextMgr->SendChat(me, TEXT_GENERIC_5);
                    gualdan->AI()->SetData(SCENARION_STEP_9, 1);
                    gualdan->SetFacingTo(me);
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                    
                    std::list<Creature*> guards;
                    GetCreatureListWithEntryInGrid(guards, me, 90714, 100.0f);
                    GetCreatureListWithEntryInGrid(guards, me, 93219, 100.0f);
                    GetCreatureListWithEntryInGrid(guards, me, 97496, 100.0f);
                    GetCreatureListWithEntryInGrid(guards, me, 97486, 100.0f);
                    GetCreatureListWithEntryInGrid(guards, me, 93219, 100.0f);
                    GetCreatureListWithEntryInGrid(guards, me, 90717, 100.0f);
                    
                    if (!guards.empty())
                        for (std::list<Creature*>::iterator itr = guards.begin(); itr != guards.end(); ++itr)
                        {
                            (*itr)->SetReactState(REACT_AGGRESSIVE);
                            (*itr)->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                        }
                }
            }
        }
        void MoveInLineOfSight(Unit* who) override
        {
            if (me->GetEntry() == 90717) // genn
            {
                if (who->GetTypeId() != TYPEID_PLAYER || !me->IsWithinDistInMap(who, 20.0f))
                    return;
                if (firstcheck)
                    return;
                firstcheck = true;
                Talk(0);
                timer = 8000;
                check = true;
            }

            if (me->GetEntry() == NPC_VARIAN)
            {
                Player* p = who->ToPlayer();
                if (!p)
                    return;

                if (InstanceScript *script = me->GetInstanceScript())
                {
                    if (script->getScenarionStep() == 3)
                    {
                        if (me->FindNearestCreature(90714, 20.0f, true))
                        {
                            p->UpdateAchievementCriteria(CRITERIA_TYPE_SCRIPT_EVENT_2, 45228);
                            if (!firstcheck)
                            {
                                DoCast(199399);
                                firstcheck = true;
                            }
                        }
                    }
                }
            }
        };

        void DoAction(int32 const action) override
        {
            if (me->GetEntry() == 90717)
            {
                std::list<Creature*> guards;
                GetCreatureListWithEntryInGrid(guards, me, 90714, 300.0f);
                GetCreatureListWithEntryInGrid(guards, me, 93219, 300.0f);
                GetCreatureListWithEntryInGrid(guards, me, 97496, 300.0f);
                GetCreatureListWithEntryInGrid(guards, me, 97486, 300.0f);
                GetCreatureListWithEntryInGrid(guards, me, 93219, 300.0f);
                if (!guards.empty())
                    for (std::list<Creature*>::iterator itr = guards.begin(); itr != guards.end(); ++itr)
                    {
                        (*itr)->GetMotionMaster()->Clear();
                        (*itr)->GetMotionMaster()->MovePath(439142, false, irand(-5, 5), irand(-5, 5));
                    }
                me->GetMotionMaster()->MovePath(439136, false); // 6 пњљпњљпњљпњљпњљ
                me->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                timer = 50000;
                check2 = true;
            }
        }


        void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType dmgType) override
        {
            damage = 0;
        }

        void EnterCombat(Unit* who) override
        {
            switch (me->GetEntry())
            {
                case 90717:
                    events.ScheduleEvent(EVENT_1, 11000); //200397
                    events.ScheduleEvent(EVENT_3, 5000); //222584
                    DoCast(199256);
                    break;
                case 90714:
                    events.ScheduleEvent(EVENT_2, 3000); // 199509
                    events.ScheduleEvent(EVENT_3, 5000); //222584
                    break;
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (me->GetEntry() == 90717)
            {
                if (check)
                    if (timer <= diff)
                    {
                        Talk(1);
                        check = false;
                        std::list<Creature*> guards;
                        GetCreatureListWithEntryInGrid(guards, me, 92586, 300.0f);
                        GetCreatureListWithEntryInGrid(guards, me, 92074, 300.0f);
                        GetCreatureListWithEntryInGrid(guards, me, 91353, 300.0f);
                        GetCreatureListWithEntryInGrid(guards, me, 90714, 300.0f);
                        GetCreatureListWithEntryInGrid(guards, me, 93219, 300.0f);
                        GetCreatureListWithEntryInGrid(guards, me, 97496, 300.0f);
                        GetCreatureListWithEntryInGrid(guards, me, 97486, 300.0f);
                        if (!guards.empty())
                            for (std::list<Creature*>::iterator itr = guards.begin(); itr != guards.end(); ++itr)
                            {
                                uint8 rand = urand(1, 3);
                                switch (rand)
                                {
                                    case 1:
                                        (*itr)->GetMotionMaster()->MovePoint(1, 538.84f, 2107.58f, 3.0f);
                                        break;
                                    case 2:
                                        (*itr)->GetMotionMaster()->MovePoint(1, 498.36f, 2138.39f, 4.21f);
                                        break;
                                    case 3:
                                        (*itr)->GetMotionMaster()->MovePoint(1, 554.48f, 2057.90f, 5.94f);
                                        break;
                                }
                            }
                        std::list<Player*> playerList;
                        me->GetPlayerListInGrid(playerList, 60.0f);
                        for (auto plr : playerList)
                        {
                            me->CastSpell(plr, 185265, true);
                        }
                        DoCast(199256);
                        me->GetMotionMaster()->MovePoint(1, 538.84f, 2107.58f, 3.0f);
                        timer = 5000;
                        check1 = true;
                    }
                    else timer -= diff;
                    if (check1)
                    {
                        if (timer <= diff)
                        {
                            check1 = false;
                            DoCast(183988);
                        }
                        else timer -= diff;
                    }

                    if (check2)
                    {
                        if (timer <= diff)
                        {
                            Map::PlayerList const &PlList = me->GetMap()->GetPlayers();
                            if (!PlList.isEmpty())
                            {
                                for (Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
                                    if (Player* player = i->getSource())
                                    {
                                        if (player->GetTeam() == ALLIANCE)
                                        {
                                            player->CastSpell(player, 218626);
                                            player->TeleportTo(1460, 812.54f, 2166.87f, 85.84f, 0.4f);
                                            check3 = true;
                                            timer = 24000;
                                        }
                                    }
                            }

                            check2 = false;
                        }
                        else timer -= diff;
                    }
                    if (check3)
                    {
                        if (timer <= diff)
                        {
                            check3 = false;
                            DoCast(199718);
                            std::list<Creature*> guards;
                            GetCreatureListWithEntryInGrid(guards, me, 92586, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 92074, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 91353, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 90714, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 93219, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 97496, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 97486, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 93219, 300.0f);
                            if (!guards.empty())
                                for (std::list<Creature*>::iterator itr = guards.begin(); itr != guards.end(); ++itr)
                                {
                                    (*itr)->GetMotionMaster()->Clear();
                                    (*itr)->GetMotionMaster()->MovePath(439143, false, irand(-5, 5), irand(-5, 5));
                                }
                            me->GetMotionMaster()->MovePath(439137, false);
                        }
                        else timer -= diff;
                    }
            }

            if (me->GetEntry() == NPC_VARIAN)
            {
                if (InstanceScript *script = me->GetInstanceScript())
                {
                    if (script->getScenarionStep() == 5)
                    {
                        if (!check1)
                        {
                            check1 = true;
                            std::list<Creature*> guards;
                            GetCreatureListWithEntryInGrid(guards, me, 92586, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 92074, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 91353, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 90716, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 91949, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 92122, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 90717, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 90714, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 93219, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 97496, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 97486, 300.0f);

                            if (!guards.empty())
                                for (std::list<Creature*>::iterator itr = guards.begin(); itr != guards.end(); ++itr)
                                {
                                    (*itr)->GetMotionMaster()->Clear();
                                    (*itr)->GetMotionMaster()->MovePath(439144, false, irand(-5, 5), irand(-5, 5)); //4 9 14 21
                                }
                            me->GetMotionMaster()->MovePath(439144, false); //4 9 14 21
                            me->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                        }
                    }

                    if (script->getScenarionStep() == 6)
                    {
                        if (!check2)
                        {
                            check2 = true;
                            std::list<Creature*> guards;
                            GetCreatureListWithEntryInGrid(guards, me, 90716, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 91949, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 92122, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 90717, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 90714, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 93219, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 97496, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 97486, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 93219, 300.0f);
                            if (!guards.empty())
                                for (std::list<Creature*>::iterator itr = guards.begin(); itr != guards.end(); ++itr)
                                {
                                    (*itr)->SetReactState(REACT_PASSIVE);
                                    (*itr)->GetMotionMaster()->Clear();
                                    (*itr)->GetMotionMaster()->MovePath(439145, false, irand(-5, 5), irand(-5, 5));
                                    (*itr)->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                                }
                            me->SetReactState(REACT_PASSIVE);
                            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                            me->GetMotionMaster()->MovePath(439145, false);
                            me->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                        }
                    }
                }
            }

            if (me->GetEntry() == 90714)
            {
                if (InstanceScript *script = me->GetInstanceScript())
                {
                    if ((script->getScenarionStep() == 6 || script->getScenarionStep() == 7) && script->GetData(DATA_SCENARIO_TEAM) == ALLIANCE)
                    {
                        if (check1)
                            return;

                        if (!tempcheck)
                        {
                            tempcheck = true;
                            me->SetReactState(REACT_PASSIVE);
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePath(439145, false, irand(-5, 5), irand(-5, 5));
                            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                            return;
                        }

                        if (!me->isMoving())
                        {
                            if (!(me->FindNearestCreature(90713, 300.0f, true)))
                                return;

                            std::list<Creature*> guards;
                            GetCreatureListWithEntryInGrid(guards, me, 90716, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 91949, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 92122, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 90717, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 90713, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 93219, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 97496, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 97486, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 93219, 300.0f);
                            if (!guards.empty())
                                for (std::list<Creature*>::iterator itr = guards.begin(); itr != guards.end(); ++itr)
                                {
                                    (*itr)->SetReactState(REACT_AGGRESSIVE);
                                    (*itr)->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                                    (*itr)->GetMotionMaster()->Clear();
                                    (*itr)->GetMotionMaster()->MovePath(439146, false, irand(-2, 2), irand(-2, 2));
                                }
                            if (Creature* targ = me->FindNearestCreature(90716, 40.0f, true))
                            {
                                targ->AI()->Talk(0);
                                Talk(3);
                            }
                            me->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                            me->SetReactState(REACT_AGGRESSIVE);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                            DoCast(220571);

                            me->AddDelayedEvent(6000, [this]() -> void
                            {
                                me->RemoveAura(220571);
                                me->GetMotionMaster()->MovePath(439146, false);
                                me->SummonGameObject(242549, 1420.85f, 2107.47f, 21.659f, 1.8675f, 0.0f, 0.0f, 0.0f, 0.0f, DAY);
                            });
                            check1 = true;
                        }
                    }
                }
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
                        DoCast(200397);
                        events.ScheduleEvent(EVENT_1, 11000);
                        break;
                    case EVENT_2:
                        DoCast(199509);
                        events.ScheduleEvent(EVENT_2, 3000);
                        break;
                    case EVENT_3:
                        DoCast(222584);
                        events.ScheduleEvent(EVENT_3, 5000);
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };
};

//1494.33f, 1752.15f, 9.25f
float kross_pos[2][3] =
{
    {1494.573f, 1780.335f, 36.86848f},
    {1455.05f, 1748.94f, 36.86848f},
};

//kross. 90544
class sceneTrigger_part8 : public CreatureScript
{
public:
    sceneTrigger_part8() : CreatureScript("sceneTrigger_part8") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new sceneTrigger_part8AI(creature);
    }

    struct sceneTrigger_part8AI : public Scripted_NoMovementAI
    {
        sceneTrigger_part8AI(Creature* creature) : Scripted_NoMovementAI(creature), intro(false)
        {
            _iventIntro = false;
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_UNK_15);
            me->SetReactState(REACT_PASSIVE);
        }
        bool intro, below_75, below_50, below_25, _iventIntro;
        EventMap events;
        bool movePointer;

        void Reset() override
        {
            if (!intro)
                me->SetAnimKitId(1455);
            me->setFaction(2780);
            below_75 = false;
            below_50 = false;
            below_25 = false;
            events.Reset();
        }

        void EnterCombat(Unit* victim) override
        {
            sCreatureTextMgr->SendChat(me, TEXT_GENERIC_0);
            DoCast(225099);
            events.ScheduleEvent(EVENT_4, 14000);
            events.ScheduleEvent(EVENT_5, 27000);
            events.ScheduleEvent(EVENT_6, urand(20000, 29000));
            InCombatAlliance(true);
            //     me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_UNK_15);
            me->SetReactState(REACT_AGGRESSIVE);
        }

        void EnterEvadeMode() override
        {
            initPos();
            moveByWPWithSetHome(movePointer);
        }

        void DamageTaken(Unit* attacker, uint32& /*damage*/, DamageEffectType dmgType) override
        {
            if (!below_75 && me->HealthBelowPct(75))
            {
                below_75 = true;
                attacker->CastSpell(attacker, 225069, true); //conv 3581
                moveByWPWithSetHome(movePointer = !movePointer);
                DoCast(225098);
                InCombatAlliance(false);
                events.ScheduleEvent(EVENT_7, 4000);
                events.ScheduleEvent(EVENT_8, 6000);
            }

            if (!below_50 && me->HealthBelowPct(50))
            {
                below_50 = true;
                attacker->CastSpell(attacker, 225070, true); //conv 3582
                moveByWPWithSetHome(movePointer = !movePointer);
                DoCast(225099);
                events.ScheduleEvent(EVENT_7, 4000);
                events.ScheduleEvent(EVENT_9, 6000);
            }

            if (!below_25 && me->HealthBelowPct(25))
            {
                below_25 = true;
                attacker->CastSpell(attacker, 225071, true); //conv 3583
                moveByWPWithSetHome(movePointer = !movePointer);
                DoCast(225098);
                InCombatAlliance(false);
                events.ScheduleEvent(EVENT_7, 4000);
                events.ScheduleEvent(EVENT_8, 6000);
            }
        }
        void MovementInform(uint32 moveType, uint32 pointId) override
        {
            if (moveType == POINT_MOTION_TYPE || moveType == EFFECT_MOTION_TYPE)
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_UNK_15);
                me->SetReactState(REACT_AGGRESSIVE);
                /*me->SetAnimKitId(0);
                me->PlayOneShotAnimKit(0);*/
                //me->CastSpell(me, 225099, true);

                //me->SetControlled(true, UNIT_STATE_STUNNED);
                //me->SetHover(true);
                /*WorldPackets::Misc::SetPlayHoverAnim packet;
                packet.UnitGUID = me->GetGUID();
                me->SendMessageToSet(packet.Write(), true);*/
            }
        }
        void SetData(uint32, uint32) override
        {
            intro = true;

            me->CastSpell(me, 208495, true);

            events.ScheduleEvent(EVENT_2, 2000);
            events.ScheduleEvent(EVENT_3, 6000);
            initPos();
        }
        void initPos()
        {
            movePointer = 0;
            // if (InstanceScript *script = me->GetInstanceScript())
            //     movePointer = script->GetData(DATA_SCENARIO_TEAM) != ALLIANCE;
        }

        void moveByWPWithSetHome(uint8 point)
        {
            me->GetMotionMaster()->MoveTakeoff(0, kross_pos[point][0], kross_pos[point][1], kross_pos[point][2]);
            me->SetHomePosition(kross_pos[point][0], kross_pos[point][1], kross_pos[point][2], me->GetOrientation());
        }

        void JustDied(Unit* attacker) override
        {
            sCreatureTextMgr->SendChat(me, TEXT_GENERIC_2);

            attacker->CastSpell(attacker, 225106, true); //conv 3583

            Map::PlayerList const &PlList = me->GetMap()->GetPlayers();

            if (PlList.isEmpty())
                return;

            for (Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
                if (Player* plr = i->getSource())
                {
                    //! just one plr need for update scenario.
                    plr->UpdateAchievementCriteria(CRITERIA_TYPE_SCRIPT_EVENT_2, 44669);
                    break;
                }
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim() && _iventIntro)
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING) || me->HasUnitState(UNIT_STATE_MOVING))
                return;
            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_1:
                        me->SetAnimKitId(0);
                        me->PlayOneShotAnimKit(0);
                        //me->SetMovementAnimKitId(11668);
                        moveByWPWithSetHome(movePointer);
                        _iventIntro = true;
                        break;
                    case EVENT_2:
                        me->SetAnimKitId(0);
                        me->PlayOneShotAnimKit(0);
                        //me->SetMovementAnimKitId(11668);
                        if (Creature* guld = me->FindNearestCreature(NPC_GULDAN, 80.0f, true))
                            guld->AI()->Talk(0);
                        if (Creature* tirion = me->FindNearestCreature(91951, 80.0f, true))
                        {
                            me->CastSpell(tirion, 184464);
                            tirion->CastSpell(tirion, 208504);
                        }
                        break;
                    case EVENT_3:
                        me->RemoveAura(184464);
                        if (Creature* tirion = me->FindNearestCreature(91951, 80.0f, true))
                        {
                            tirion->RemoveAura(208504);
                            tirion->AI()->Talk(0);
                            tirion->CastSpell(tirion, 208505);
                            tirion->GetMotionMaster()->MovePoint(0, 1494.33f, 1752.15f, 9.25f);
                        }
                        events.ScheduleEvent(EVENT_1, 10000);
                        break;
                    case EVENT_4:
                        DoCast(183393);
                        events.ScheduleEvent(EVENT_4, 14000);
                        break;
                    case EVENT_5:
                        DoCast(221080);
                        events.ScheduleEvent(EVENT_5, 14000);
                        break;
                    case EVENT_6:
                        DoCast(me->getVictim(), 183498);
                        events.ScheduleEvent(EVENT_6, urand(20000, 29000));
                        break;
                    case EVENT_7:
                        if (Player* pl = me->FindNearestPlayer(40.0f, true))
                            AttackStart(pl);
                        break;
                    case EVENT_8:
                        InCombatAlliance(false);
                        break;
                    case EVENT_9:
                        InCombatAlliance(true);
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }

        void InCombatAlliance(bool attack)
        {
            std::list<Creature*> guards;
            GetCreatureListWithEntryInGrid(guards, me, 90716, 100.0f);
            GetCreatureListWithEntryInGrid(guards, me, 91949, 100.0f);
            GetCreatureListWithEntryInGrid(guards, me, 92122, 100.0f);
            GetCreatureListWithEntryInGrid(guards, me, 90717, 100.0f);
            GetCreatureListWithEntryInGrid(guards, me, 90714, 100.0f);
            GetCreatureListWithEntryInGrid(guards, me, 90713, 100.0f);
            GetCreatureListWithEntryInGrid(guards, me, 93219, 100.0f);
            GetCreatureListWithEntryInGrid(guards, me, 97496, 100.0f);
            GetCreatureListWithEntryInGrid(guards, me, 97486, 100.0f);
            GetCreatureListWithEntryInGrid(guards, me, 93219, 100.0f);
            if (!guards.empty())
                for (std::list<Creature*>::iterator itr = guards.begin(); itr != guards.end(); ++itr)
                {
                    if (attack)
                    {
                        (*itr)->AI()->AttackStart(me);
                        (*itr)->SetReactState(REACT_AGGRESSIVE);
                    }
                    else
                    {
                        (*itr)->SetReactState(REACT_PASSIVE);
                    }
                }

        }
    };
};

//0x202090B6805C1100001EFF000053DF33
class scenario_bi_gualdan : public CreatureScript
{
public:
    scenario_bi_gualdan() : CreatureScript("scenario_bi_gualdan") {}

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new scenario_bi_gualdanAI(creature);
    }
    enum data
    {

    };

    struct scenario_bi_gualdanAI : public Scripted_NoMovementAI
    {
        scenario_bi_gualdanAI(Creature* creature) : Scripted_NoMovementAI(creature),
            step7_intro(false), step_8_waighting(false), step_8_intro(false)
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
        }

        bool step7_intro;
        bool step_8_waighting;
        bool step_8_intro;

        EventMap events;
        enum data
        {
            EVENT_1 = 1,
            EVENT_2,
            EVENT_3,
            EVENT_4,
            EVENT_5,
            EVENT_6,
        };
        void Reset() override
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
            events.Reset();
            if (!step7_intro)
            {
                InstanceScript *script = me->GetInstanceScript();
                if (!script)
                    return;

                //if (script->getScenarionStep() == 7)
                {
                    Map *m = script->instance;
                    if (!m)
                        return;
                    Creature* tirion = m->GetCreature(script->GetGuidData(NPC_TIRION));
                    if (!tirion)
                        return;

                    me->CastSpell(tirion, 186589, true);
                }
            }
        }

        void MoveInLineOfSight(Unit* who) override
        {

            if (!me->IsWithinDist(who, 170.0))
                return;

            Player* p = who->ToPlayer();
            if (!p)
                return;

            if (InstanceScript *script = me->GetInstanceScript())
            {
                //  if (script->getScenarionStep() == 6)
                {
                    if (me->IsWithinDist(who, 160.0f))
                    {
                        p->UpdateAchievementCriteria(CRITERIA_TYPE_SCRIPT_EVENT_2, 50027);
                    }
                }
            }

            if (!step7_intro)
            {
                if (!me->IsWithinDist(who, 150.0f))
                    return;

                InstanceScript *script = me->GetInstanceScript();
                if (!script)
                    return;

                Map *m = script->instance;
                if (!m)
                    return;

                m->LoadGrid(1376.963f, 1744.453f);

                //! champions movement not ready. so just hack for preapare. 
                //! ToDo: create path movement for all champions
                if (script->GetData(DATA_SCENARIO_TEAM) == ALLIANCE)
                {

                    if (Creature* jaina = m->GetCreature(script->GetGuidData(NPC_JAINA)))
                    {
                        if (me->GetDistance2d(jaina) >= 100.0f)
                            return;
                    }
                    else
                        me->SummonCreature(NPC_JAINA, 1502.416f, 1816.275f, 37.43131f, 0.0f);

                    if (Creature* varian = m->GetCreature(script->GetGuidData(NPC_VARIAN)))
                    {
                        if (me->GetDistance2d(varian) >= 100.0f)
                        {
                            varian->GetMotionMaster()->Clear();
                            varian->NearTeleportTo(1482.425f, 1813.54f, 38.26539f, 0.0f);
                        }
                    }
                    else
                        me->SummonCreature(NPC_VARIAN, 1482.425f, 1813.54f, 38.26539f, 0.0f);

                    // if (Creature* sylvana = m->GetCreature(script->GetGuidData(NPC_SYLVANA)))
                    // {
                        // if (me->GetDistance2d(sylvana) >= 150.0f)
                        // {
                            // sylvana->GetMotionMaster()->Clear();
                            // sylvana->NearTeleportTo(1427.27f, 1776.18f, 34.23f, sylvana->GetOrientation());
                        // }
                    // }
                    // else
                        me->SummonCreature(NPC_SYLVANA, 1427.27f, 1776.18f, 34.23f, 0.0f);
                }
                else
                {
                    // if (Creature* jaina = m->GetCreature(script->GetGuidData(NPC_JAINA)))
                    // {
                        // if (me->GetDistance2d(jaina) >= 100.0f)
                        // {
                            // me->SummonCreature(NPC_JAINA, 1502.416f, 1816.275f, 37.43131f, 0.0f);
                        // }
                    // }
                    // else
                        me->SummonCreature(NPC_JAINA, 1502.416f, 1816.275f, 37.43131f, 0.0f);

                    // if (Creature* varian = m->GetCreature(script->GetGuidData(NPC_VARIAN)))
                    // {
                        // if (me->GetDistance2d(varian) >= 100.0f)
                        // {
                            // me->SummonCreature(NPC_VARIAN, 1482.425f, 1813.54f, 38.26539f, 0.0f);
                        // }
                    // }
                    // else
                        me->SummonCreature(NPC_VARIAN, 1482.425f, 1813.54f, 38.26539f, 0.0f);

                    if (Creature* sylvana = m->GetCreature(script->GetGuidData(NPC_SYLVANA)))
                    {
                        if (me->GetDistance2d(sylvana) >= 140.0f)
                            return;
                    }
                    else
                        me->SummonCreature(NPC_SYLVANA, 1427.27f, 1776.18f, 34.23f, 0.0f);
                }

                step7_intro = true;

                if (Creature* tirion = m->GetCreature(script->GetGuidData(NPC_TIRION)))
                {
                    me->CastSpell(tirion, 186589, true);
                }

                events.ScheduleEvent(EVENT_1, 1000);
                events.ScheduleEvent(EVENT_SPECIAL, 36000);
                events.ScheduleEvent(EVENT_2, 55000);
                events.ScheduleEvent(EVENT_3, 64000);
            }

            if (step_8_waighting && !step_8_intro)
            {
                step_8_intro = true;
                uint32 t = 15000;
                events.ScheduleEvent(EVENT_4, t += 6000);  //Time: 06/05/2016 08:38:08.451
                events.ScheduleEvent(EVENT_5, t += 6000);  //Time: 06/05/2016 08:38:08.451
                events.ScheduleEvent(EVENT_6, t += 12000); //Time: 06/05/2016 08:38:20.280
            }
        };

        //ServerToClient: SMSG_CHAT (0x2BAD) Length: 256 ConnIdx: 0 Time: 06/05/2016 08:38:02.064 Number: 260361
        void SetData(uint32 data, uint32 step) override
        {
            if (data != SCENARION_STEP_9)
                return;
            switch (step)
            {
                case 1:
                    step_8_waighting = true;
                    break;
                case 2:
                {
                    InstanceScript *script = me->GetInstanceScript();
                    if (!script)
                        return;
                    Map *m = script->instance;
                    if (!m)
                        return;

                    Creature* var = m->GetCreature(script->GetGuidData(NPC_VARIAN));
                    if (!var)
                        return;
                    var->GetMotionMaster()->MoveTargetedHome();

                    uint32 t = 0;
                    events.ScheduleEvent(EVENT_7, t += 3000);  //Time: 06/05/2016 08:40:28.590 Number: 303433
                    events.ScheduleEvent(EVENT_8, t += 3000);
                    events.ScheduleEvent(EVENT_9, t += 3000);
                    events.ScheduleEvent(EVENT_10, t += 2000);  //Time: 06/05/2016 08:40:37.861 Number: 303971
                    events.ScheduleEvent(EVENT_11, t += 6000);  //Time: 06/05/2016 08:40:43.938
                    events.ScheduleEvent(EVENT_12, t += 6000);  //Time: 06/05/2016 08:40:49.993
                    events.ScheduleEvent(EVENT_13, t += 4000);  //Time: 06/05/2016 08:40:53.632
                    events.ScheduleEvent(EVENT_14, t += 6000);  //Time: 06/05/2016 08:40:59.717
                    break;
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
                        me->CastSpell(me, 208490, true); //06/05/2016 08:29:17.520 Number: 161804
                        break;
                    case EVENT_SPECIAL:
                    {
                        InstanceScript *script = me->GetInstanceScript();
                        if (!script)
                            return;

                        Map *m = script->instance;
                        if (!m)
                            return;

                        if (Creature* kross = m->GetCreature(script->GetGuidData(NPC_KROSS)))
                            kross->AI()->SetData(0, 0);
                    }
                    break;
                    case EVENT_2:
                        me->CastSpell(me, 208512, true); //06/05/2016 08:30:02.613 Number: 162604
                        break;
                    case EVENT_3:
                    {
                        sCreatureTextMgr->SendChat(me, TEXT_GENERIC_1); // 06/05/2016 08:30:11.340 

                        me->GetMotionMaster()->MovePoint(0, 1660.127f, 1655.793f, 79.36142f);
                        me->SetHomePosition(1660.127f, 1655.793f, 79.36142f, me->GetOrientation());
                        break;
                    }
                    case EVENT_4:
                    {
                        InstanceScript *script = me->GetInstanceScript();
                        if (!script)
                            return;

                        //script->SetData(SCENARION_STEP_9, 0);

                        Map *m = script->instance;
                        if (!m)
                            return;

                        if (Creature* var = m->GetCreature(script->GetGuidData(NPC_VARIAN)))
                            sCreatureTextMgr->SendChat(var, TEXT_GENERIC_5);
                        break;
                    }
                    case EVENT_5:
                        sCreatureTextMgr->SendChat(me, TEXT_GENERIC_2); // Time: 06/05/2016 08:38:08.451 Number: 260429
                        break;
                    case EVENT_6:
                    {
                        InstanceScript *script = me->GetInstanceScript();
                        if (!script)
                            return;

                        script->SetData(SCENARION_STEP_9, 0);

                        Map *m = script->instance;
                        if (!m)
                            return;

                        if (script->GetData(DATA_SCENARIO_TEAM) != ALLIANCE)
                            if (Creature* sylvana = m->GetCreature(script->GetGuidData(NPC_SYLVANA)))
                                sylvana->AI()->DoAction(1);

                        if (Creature* var = m->GetCreature(script->GetGuidData(NPC_VARIAN)))
                            sCreatureTextMgr->SendChat(var, TEXT_GENERIC_6);
                        break;
                    }
                    case EVENT_7:
                    {
                        InstanceScript *script = me->GetInstanceScript();
                        if (!script)
                            return;

                        script->SetData(SCENARION_STEP_9, 2);
                        script->SetData(SCENARION_STEP_9, 3);

                        Map *m = script->instance;
                        if (!m)
                            return;

                        if (Creature* var = m->GetCreature(script->GetGuidData(NPC_VARIAN)))
                            sCreatureTextMgr->SendChat(var, TEXT_GENERIC_7);

                        script->SetData(SCENARION_STEP_9, 4);
                        break;
                    }
                    case EVENT_8:
                    {
                        InstanceScript *script = me->GetInstanceScript();
                        if (!script)
                            return;
                        script->SetData(SCENARION_STEP_9, 5);
                        break;
                    }
                    case EVENT_9:
                    {
                        InstanceScript *script = me->GetInstanceScript();
                        if (!script)
                            return;
                        script->SetData(SCENARION_STEP_9, 6);
                        break;
                    }
                    case EVENT_10:
                    {
                        InstanceScript *script = me->GetInstanceScript();
                        if (!script)
                            return;

                        Map *m = script->instance;
                        if (!m)
                            return;

                        if (Creature* var = m->GetCreature(script->GetGuidData(NPC_LORD_JAR)))
                            sCreatureTextMgr->SendChat(var, TEXT_GENERIC_0);
                        break;
                    }
                    case EVENT_11:
                    {
                        InstanceScript *script = me->GetInstanceScript();
                        if (!script)
                            return;

                        Map *m = script->instance;
                        if (!m)
                            return;

                        if (Creature* var = m->GetCreature(script->GetGuidData(NPC_BRUTAL)))
                            sCreatureTextMgr->SendChat(var, TEXT_GENERIC_0);
                        break;
                    }
                    case EVENT_12:
                    {
                        InstanceScript *script = me->GetInstanceScript();
                        if (!script)
                            return;

                        Map *m = script->instance;
                        if (!m)
                            return;

                        if (Creature* var = m->GetCreature(script->GetGuidData(NPC_TIHONDR)))
                            sCreatureTextMgr->SendChat(var, TEXT_GENERIC_0);
                        break;
                    }
                    case EVENT_13:
                    {
                        sCreatureTextMgr->SendChat(me, TEXT_GENERIC_3); // 06/05/2016 08:30:11.340 
                        break;
                    }
                    case EVENT_14:
                    {
                        sCreatureTextMgr->SendChat(me, TEXT_GENERIC_4); // 06/05/2016 08:30:11.340 
                        InstanceScript *script = me->GetInstanceScript();
                        if (!script)
                            return;
                        script->SetData(SCENARION_STEP_END, 0);
                        break;
                    }
                    default:
                        break;
                }
            }
        }
    };
};


//! Q: 40517
class npc_q40517_p1 : public CreatureScript
{
public:
    npc_q40517_p1() : CreatureScript("npc_q40517_p1") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_q40517_p1AI(creature);
    }

    enum
    {
        _SPELL_INTRO = 200569,
        VEHICLE_SPELL_RIDE_HARDCODED = 125684,
    };

    struct npc_q40517_p1AI : public npc_escortAI
    {
        npc_q40517_p1AI(Creature* creature) : npc_escortAI(creature) {}

        bool PlayerOn;
        void Reset() override
        {
            PlayerOn = false;
        }

        void OnCharmed(bool /*apply*/) override
        {
        }

        void IsSummonedBy(Unit* summoner) override
        {
            summoner->CastSpell(me, VEHICLE_SPELL_RIDE_HARDCODED, true);
            me->setFaction(summoner->getFaction());
        }

        void PassengerBoarded(Unit* who, int8 /*seatId*/, bool apply) override
        {
            if (!apply || who->GetTypeId() != TYPEID_PLAYER)
                return;

            who->ToPlayer()->KilledMonsterCredit(100696);

            PlayerOn = true;
            Start(false, true, who->GetGUID());
            who->ToPlayer()->PlayDistanceSound(16422, who->ToPlayer());
        }

        void WaypointReached(uint32 i) override
        {
            switch (i)
            {
                case 21:
                    if (Player* player = GetPlayerForEscort())
                    {
                        SetEscortPaused(true);
                        player->ExitVehicle();
                        sCreatureTextMgr->SendChat(me, TEXT_GENERIC_0, player->GetGUID());
                    }
                    break;
            }
        }

        void UpdateAI(uint32 diff) override
        {
            npc_escortAI::UpdateAI(diff);

            if (PlayerOn)
            {
                if (Player* player = GetPlayerForEscort())
                    player->SetClientControl(me, 0);
                PlayerOn = false;
            }
        }
    };
};

class npc_q42782_1 : public CreatureScript
{
public:
    npc_q42782_1() : CreatureScript("npc_q42782_1") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        if (player->IsQuestRewarded(120001)) //check for account-wide quest complete before player can skip legion scenario
            return false;

        player->PlayerTalkClass->SendCloseGossip();
        
        Quest const* quest = sQuestDataStore->GetQuestTemplate(44663);
        if (!quest)
            return false;

        player->AddQuest(quest, NULL);
        player->CompleteQuest(44663);
        player->AddDelayedEvent(1500, [player] {
            player->CastSpell(player, 230156);
        });

        return true;
    }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_q42782_1AI(creature);
    }
    struct npc_q42782_1AI : ScriptedAI
    {
        npc_q42782_1AI(Creature* creature) : ScriptedAI(creature)
        {
            timer = 60000;
        }

        GuidSet m_player_for_event;
        uint32 timer;

        void MoveInLineOfSight(Unit* who) override
        {
            if (who->GetTypeId() != TYPEID_PLAYER || !me->IsWithinDistInMap(who, 15.0f))
                return;
            if (who->ToPlayer()->GetQuestStatus(40519) == QUEST_STATUS_COMPLETE || who->ToPlayer()->GetQuestStatus(42782) == QUEST_STATUS_INCOMPLETE || who->ToPlayer()->GetQuestStatus(42782) == QUEST_STATUS_COMPLETE)
            {
                GuidSet::iterator itr = m_player_for_event.find(who->GetGUID());
                if (itr != m_player_for_event.end())
                    return;

                m_player_for_event.insert(who->GetGUID());
                sCreatureTextMgr->SendChat(me, TEXT_GENERIC_0, who->GetGUID());
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (timer <= diff)  // пњљпњљпњљ пњљпњљпњљпњљ пњљпњљпњљпњљпњљ пњљпњљпњљпњљпњљпњљпњљпњљ пњљ 2, пњљ 3 пњљпњљпњљпњљ, пњљпњљпњљпњљпњљпњљпњљпњљ пњљпњљ пњљпњљпњљпњљпњљпњљ пњљ пњљпњљпњљпњљ пњљпњљ пњљпњљпњљпњљпњљпњљ
            {
                m_player_for_event.clear();
                timer = 60000;
            }
            else
                timer -= diff;

        }
    };
};


class scene_bi_alliance_q40593 : public SceneTriggerScript
{
public:
    scene_bi_alliance_q40593() : SceneTriggerScript("scene_bi_alliance_q40593")
    {}

    bool OnTrigger(Player* player, SpellScene const* trigger, std::string type) override
    {
        //100616 / quest - invisibility - detection - 27
        player->CastSpell(player, 100616, true);
        player->CastSpell(player, 199046, true);
        //93216
        return true;
    }
};

class npc_109494 : public CreatureScript
{
public:
    npc_109494() : CreatureScript("npc_109494") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_109494AI(creature);
    }
    struct npc_109494AI : ScriptedAI
    {
        npc_109494AI(Creature* creature) : ScriptedAI(creature)
        {
            timer = 100;
        }

        uint32 timer;

        void UpdateAI(uint32 diff) override
        {
            if (timer <= diff)
            {
                me->GetMotionMaster()->MoveRotate(20000, ROTATE_DIRECTION_LEFT);
                timer = 30000;
            }
            else
                timer -= diff;
        }
    };
};


class go_240535 : public GameObjectScript
{
public:
    go_240535() : GameObjectScript("go_240535") { }

    struct go_240535AI : public GameObjectAI
    {
        go_240535AI(GameObject* go) : GameObjectAI(go)
        {

        }

        void OnSpellClick(Unit* player)
        {
            if (player->GetTypeId() == TYPEID_PLAYER)
            {
                player->ToPlayer()->UpdateAchievementCriteria(CRITERIA_TYPE_SCRIPT_EVENT_2, 53064);
                go->SetGoState(GO_STATE_ACTIVE);
                go->SetPhaseMask(2, true);
                go->SetVisible(false);
            }
        }
    };


    GameObjectAI* GetAI(GameObject* go) const override
    {
        return new go_240535AI(go);
    }
};

class npc_100395 : public CreatureScript
{
public:
    npc_100395() : CreatureScript("npc_100395") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest) override
    {
        if (quest->GetQuestId() == 40517)
            player->CastSpell(player, 197678);
        return true;
    }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_100395AI(creature);
    }
    struct npc_100395AI : ScriptedAI
    {
        npc_100395AI(Creature* creature) : ScriptedAI(creature) { }

        GuidSet m_player_for_event;

        void MoveInLineOfSight(Unit* who) override
        {
            if (who->GetTypeId() != TYPEID_PLAYER || !me->IsWithinDistInMap(who, 15.0f))
                return;
            if (who->ToPlayer()->GetQuestStatus(42740) == QUEST_STATUS_COMPLETE || who->ToPlayer()->GetQuestStatus(43806) == QUEST_STATUS_COMPLETE)
            {
                GuidSet::iterator itr = m_player_for_event.find(who->GetGUID());
                if (itr != m_player_for_event.end())
                    return;

                m_player_for_event.insert(who->GetGUID());
                who->ToPlayer()->CastSpell(who->ToPlayer(), 197872);
            }
        }
    };
};

class scene_bi_horde_q40607 : public SceneTriggerScript
{
public:
    scene_bi_horde_q40607() : SceneTriggerScript("scene_bi_horde_q40607")
    {}

    bool OnTrigger(Player* player, SpellScene const* trigger, std::string type) override
    {
        //229399
        player->CastSpell(player, 225648, true);
        player->CastSpell(player, 182254, true);

        return true;
    }
};

class npc_q44281_1 : public CreatureScript
{
public:
    npc_q44281_1() : CreatureScript("npc_q44281_1") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_q44281_1AI(creature);
    }
    struct npc_q44281_1AI : ScriptedAI
    {
        npc_q44281_1AI(Creature* creature) : ScriptedAI(creature)
        {
            timer = 60000;
        }

        GuidSet m_player_for_event;
        uint32 timer;

        void MoveInLineOfSight(Unit* who) override
        {
            if (who->GetTypeId() != TYPEID_PLAYER || !me->IsWithinDistInMap(who, 20.0f))
                return;


            if (me->GetEntry() == 100442)
            {
                if (who->ToPlayer()->GetQuestStatus(40522) != QUEST_STATUS_NONE && who->ToPlayer()->GetQuestStatus(40522) == QUEST_STATUS_INCOMPLETE) // пњљпњљ пњљпњљпњљпњљпњљ пњљпњљпњљпњљпњљпњљпњљ
                {
                    GuidSet::iterator itr = m_player_for_event.find(who->GetGUID());
                    if (itr != m_player_for_event.end())
                        return;
                    m_player_for_event.insert(who->GetGUID());
                    who->CastSpell(who, 197944, true); // conv
                    who->CastSpell(who, 200252, true); //delayed criteria
                    who->CastSpell(who, 200282, true); // criteria
                    who->CastSpell(who, 198897, true); // movie
                }
            }
            else
            {
                if (who->ToPlayer()->GetQuestStatus(44281) == QUEST_STATUS_INCOMPLETE || who->ToPlayer()->GetQuestStatus(44281) == QUEST_STATUS_COMPLETE)
                {
                    GuidSet::iterator itr = m_player_for_event.find(who->GetGUID());
                    if (itr != m_player_for_event.end())
                        return;
                    uint32 conversationid = 0;
                    switch (me->GetEntry())
                    {
                        case 113539:
                            conversationid = 2478;
                            break;
                        case 113540:
                            conversationid = 2506;
                            break;
                        case 113551:
                            conversationid = 2401;
                            break;
                        case 114135:
                            conversationid = 2463;
                            break;
                        case 113541:
                            conversationid = 2505;
                            break;
                        case 114106:
                            conversationid = 2539;
                            break;
                        case 114132:
                            conversationid = 2411;
                            break;
                        case 113547:
                            conversationid = 2553;
                            break;
                        default:
                            break;
                    }

                    m_player_for_event.insert(who->GetGUID());
                    if (conversationid == 0)
                        return;

                    Conversation* conversation = new Conversation;
                    if (!conversation->CreateConversation(sObjectMgr->GetGenerator<HighGuid::Conversation>()->Generate(), conversationid, who->ToPlayer(), NULL, *who->ToPlayer()))
                        delete conversation;
                }
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (me->GetEntry() != 100442)
            {
                if (timer <= diff)  // пњљпњљпњљ пњљпњљпњљпњљ пњљпњљпњљпњљпњљ пњљпњљпњљпњљпњљпњљпњљпњљ пњљ 2, пњљ 3 пњљпњљпњљпњљ, пњљпњљпњљпњљпњљпњљпњљпњљ пњљпњљ пњљпњљпњљпњљпњљпњљ пњљ пњљпњљпњљпњљ пњљпњљ пњљпњљпњљпњљпњљпњљ
                {
                    m_player_for_event.clear();
                    timer = 60000;
                }
                else
                    timer -= diff;
            }

        }
    };
};

class scenarion_bi_heroes_horde : public CreatureScript
{
public:
    scenarion_bi_heroes_horde() : CreatureScript("scenarion_bi_heroes_horde") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new scenarion_bi_heroes_hordeAI(creature);
    }
    struct scenarion_bi_heroes_hordeAI : ScriptedAI
    {
        scenarion_bi_heroes_hordeAI(Creature* creature) : ScriptedAI(creature)
        {
            check1 = false;
            check2 = false;
            check3 = false;
            check4 = false;
            check5 = false;
            check6 = false;
            firstcheck = false;
            hackcheck = false;
            tempcheck = false;
            timerforevent = 3000;
        }

        EventMap events;
        bool check1, check2, check3, check4, check5, check6, firstcheck, hackcheck, tempcheck;
        uint32 timer;
        uint32 timerforevent;

        void Reset() override
        {
            events.Reset();
        }

        void EnterCombat(Unit* who) override
        {
            switch (me->GetEntry())
            {
                case 90709:
                    events.ScheduleEvent(EVENT_1, 2000); // 224782
                    break;
                case 90711:
                    events.ScheduleEvent(EVENT_2, 3000); // 224721
                    events.ScheduleEvent(EVENT_3, 24000); // 199207
                    events.ScheduleEvent(EVENT_4, 14000); // 167014
                    break;
                case 90708:
                    events.ScheduleEvent(EVENT_5, 13000); // 178532
                    break;
            }
        }

        void MoveInLineOfSight(Unit* who) override
        {
            if (me->GetEntry() == 90708) // voldzhin
            {
                if (check1)
                    return;

                if (who->GetTypeId() != TYPEID_PLAYER || !me->IsWithinDistInMap(who, 20.0f))
                    return;
                check1 = true;
                Talk(0);
                timer = 9000;
                check2 = true;
            }

            if (me->GetEntry() == NPC_SYLVANA)
            {
                Player* p = who->ToPlayer();
                if (!p)
                    return;

                if (me->FindNearestCreature(90708, 20.0f, true))
                {
                    if (!firstcheck)
                    {
                        DoCast(224948);
                        firstcheck = true;
                    }

                    if (InstanceScript *script = me->GetInstanceScript())
                    {
                        if (script->getScenarionStep() == 3)
                            p->UpdateAchievementCriteria(CRITERIA_TYPE_SCRIPT_EVENT_2, 54123);
                    }
                }
                if (!hackcheck)
                    if (me->GetPositionZ() >= 140.23f) // пњљпњљпњљпњљпњљпњљпњљпњљ пњљпњљпњљпњљпњљпњљ пњљпњљпњљпњљ пњљпњљ пњљпњљпњљпњљпњљпњљпњљ. пњљпњљ пњљпњљпњљ пњљпњљпњљпњљпњљпњљпњљ пњљпњљпњљ, пњљпњљпњљпњљпњљпњљ пњљпњљпњљпњљ пњљпњљпњљпњљпњљ.
                    {
                        hackcheck = true;
                        InstanceScript *script = me->GetInstanceScript();
                        if (!script)
                            return;

                        script->SetData(SCENARION_STEP_9, 0);
                        me->AI()->DoAction(1);
                    }
            }
        }

        void DoAction(int32 const action) override
        {
            if (action == 1)
            {
                if (me->GetEntry() == 90709)
                {
                    DoCast(225234);
                    for (int i = 0; i < 4; ++i)
                    {
                        if (Creature* targ = me->SummonCreature(90677, 1542.98f + irand(-5, 5), 1400.00f + irand(-5, 5), 105.53f, 1.8f))
                            targ->GetMotionMaster()->MovePoint(0, 1537.02f + irand(-3, 3), 1473.59f + irand(-3, 3), 125.87f);
                    }
                    for (int i = 0; i < 4; ++i)
                    {
                        if (Creature* targ = me->SummonCreature(105199, 1542.98f + irand(-5, 5), 1400.00f + irand(-5, 5), 105.53f, 1.8f))
                            targ->GetMotionMaster()->MovePoint(0, 1537.02f + irand(-3, 3), 1473.59f + irand(-3, 3), 125.87f);
                    }
                    check4 = true;
                    timer = 20000;
                }
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (me->GetEntry() == 90708) // пњљпњљпњљпњљпњљпњљпњљ
            {
                if (check2) // пњљпњљпњљпњљпњљпњљпњљпњљпњљ пњљпњљпњљпњљпњљпњљпњљ пњљ пњљпњљпњљпњљпњљпњљ
                {
                    if (timer <= diff)
                    {
                        check2 = false;
                        std::list<Creature*> guards;
                        GetCreatureListWithEntryInGrid(guards, me, 112920, 300.0f);
                        GetCreatureListWithEntryInGrid(guards, me, 93704, 300.0f);
                        GetCreatureListWithEntryInGrid(guards, me, 90712, 300.0f);
                        GetCreatureListWithEntryInGrid(guards, me, 90711, 300.0f);
                        if (!guards.empty())
                            for (std::list<Creature*>::iterator itr = guards.begin(); itr != guards.end(); ++itr)
                            {
                                uint8 rand = urand(1, 3);
                                switch (rand)
                                {
                                    case 1:
                                        (*itr)->GetMotionMaster()->MovePoint(0, 642.20f, 1899.23f, 2.26f);
                                        break;
                                    case 2:
                                        (*itr)->GetMotionMaster()->MovePoint(0, 638.84f, 1942.78f, 2.45f);
                                        break;
                                    case 3:
                                        (*itr)->GetMotionMaster()->MovePoint(0, 626.20f, 1848.42f, 1.61f);
                                        break;
                                }
                            }
                        me->GetMotionMaster()->MovePoint(0, 642.20f, 1899.23f, 2.26f);
                        DoCast(224822);
                    }
                    else timer -= diff;
                }
                if (!check3) // пњљпњљпњљ пњљпњљпњљпњљпњљпњљ - пњљпњљпњљпњљпњљпњљпњљ пњљпњљпњљпњљпњљпњљ пњљ пњљпњљпњљпњљпњљпњљ
                {
                    if (InstanceScript *script = me->GetInstanceScript())
                    {
                        if (script->getScenarionStep() == 3)
                        {
                            check3 = true;
                            std::list<Creature*> guards;
                            GetCreatureListWithEntryInGrid(guards, me, 93704, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 90712, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 90711, 300.0f);
                            if (!guards.empty())
                                for (std::list<Creature*>::iterator itr = guards.begin(); itr != guards.end(); ++itr)
                                {
                                    (*itr)->GetMotionMaster()->Clear();
                                    (*itr)->GetMotionMaster()->MovePath(439148, false, irand(-5, 5), irand(-5, 5));
                                }
                            me->GetMotionMaster()->MovePath(439148, false); // 1 2
                            timer = 10000;
                            check4 = true;
                        }
                    }
                }
                if (check4) // пњљпњљпњљ пњљпњљпњљпњљпњљпњљпњљ?
                {
                    if (timer <= diff)
                    {
                        DoCast(224945);
                        check4 = false;
                        timer = 30000;
                        check5 = true;
                    }
                    else
                        timer -= diff;
                }
                if (check5) // пњљпњљпњљпњљпњљпњљ пњљпњљпњљпњљпњљпњљ пњљпњљпњљпњљпњљпњљпњљпњљ
                {
                    if (timer <= diff)
                    {
                        Map::PlayerList const &PlList = me->GetMap()->GetPlayers();
                        if (!PlList.isEmpty())
                        {
                            for (Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
                                if (Player* player = i->getSource())
                                {
                                    if (player->GetTeam() == HORDE)
                                    {
                                        player->CastSpell(player, 224947);
                                        player->TeleportTo(1460, 867.85f, 1847.99f, 54.08f, 6.25f);
                                        check6 = true;
                                        timer = 27000;
                                    }
                                }
                        }
                        check5 = false;
                    }
                    else
                        timer -= diff;
                }

                if (check6) // пњљпњљпњљ-пњљпњљ пњљпњљпњљпњљпњљпњљ пњљ пњљпњљпњљпњљпњљ пњљпњљпњљпњљпњљпњљ пњљ пњљпњљпњљпњљпњљпњљ
                {
                    if (timer <= diff)
                    {
                        DoCast(224946);
                        std::list<Creature*> guards;
                        GetCreatureListWithEntryInGrid(guards, me, 93704, 300.0f);
                        GetCreatureListWithEntryInGrid(guards, me, 90712, 300.0f);
                        GetCreatureListWithEntryInGrid(guards, me, 90711, 300.0f);
                        if (!guards.empty())
                            for (std::list<Creature*>::iterator itr = guards.begin(); itr != guards.end(); ++itr)
                            {
                                (*itr)->GetMotionMaster()->Clear();
                                (*itr)->GetMotionMaster()->MovePath(439149, false, irand(-4, 4), irand(-4, 4));
                            }
                        me->GetMotionMaster()->MovePath(439149, false);
                        check6 = false;
                    }
                    else
                        timer -= diff;
                }
            }
            if (me->GetEntry() == 90709) // пњљпњљпњљпњљпњљпњљпњљпњљ
            {
                if (!check1) // пњљпњљпњљпњљпњљпњљпњљ пњљпњљпњљпњљпњљпњљ, пњљпњљпњљпњљпњљ пњљпњљпњљпњљпњљпњљпњљпњљ пњљпњљпњљпњљ 5
                {
                    if (InstanceScript *script = me->GetInstanceScript())
                    {
                        if (script->getScenarionStep() == 5)
                        {
                            check1 = true;
                            std::list<Creature*> guards;
                            GetCreatureListWithEntryInGrid(guards, me, 97525, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 112920, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 93704, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 90712, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 90711, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 90708, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 90710, 300.0f);
                            if (!guards.empty())
                                for (std::list<Creature*>::iterator itr = guards.begin(); itr != guards.end(); ++itr)
                                {
                                    (*itr)->GetMotionMaster()->Clear();
                                    (*itr)->GetMotionMaster()->MovePath(439150, false, irand(-8, 8), irand(-8, 8));
                                }
                            me->GetMotionMaster()->MovePath(439150, false); // 1 4
                            check2 = true;
                            timer = 14000;
                        }
                    }
                }
                if (check2) // пњљпњљпњљпњљпњљпњљпњљ пњљ пњљпњљпњљпњљпњљ пњљ пњљпњљпњљпњљпњљ
                {
                    if (timer <= diff)
                    {
                        DoCast(224967);
                        check2 = false;
                    }
                    else
                        timer -= diff;
                }
                if (!check3) // пњљпњљпњљпњљпњљ 100%, пњљпњљпњљпњљпњљ пњљ пњљпњљпњљпњљпњљ
                {
                    if (InstanceScript *script = me->GetInstanceScript())
                    {
                        if (script->getScenarionStep() == 6)
                        {
                            check3 = true;
                            std::list<Creature*> guards;
                            GetCreatureListWithEntryInGrid(guards, me, 97525, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 112920, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 93704, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 90712, 300.0f);
                            // GetCreatureListWithEntryInGrid(guards, me, 90711, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 90708, 300.0f);
                            GetCreatureListWithEntryInGrid(guards, me, 90710, 300.0f);
                            if (!guards.empty())
                                for (std::list<Creature*>::iterator itr = guards.begin(); itr != guards.end(); ++itr)
                                {
                                    (*itr)->SetReactState(REACT_PASSIVE);
                                    (*itr)->GetMotionMaster()->Clear();
                                    (*itr)->GetMotionMaster()->MovePath(439151, false, irand(-8, 8), irand(-8, 8));
                                    (*itr)->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                                }
                            me->SetReactState(REACT_PASSIVE);
                            me->GetMotionMaster()->Clear();
                            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                            me->GetMotionMaster()->MovePath(439151, false); // 1 4
                        }
                    }
                }
                if (check4)
                {
                    if (timer <= diff)
                    {
                        DoCast(225236);
                        check5 = true;
                        timer = 6000;
                        check4 = false;
                    }
                    else
                        timer -= diff;
                }
                if (check5)
                {
                    if (timer <= diff)
                    {
                        DoCast(225242);
                        check5 = false;
                        check6 = true;
                        timer = 10000;
                    }
                    else
                        timer -= diff;
                }
                if (check6)
                {
                    if (timer <= diff)
                    {
                        if (InstanceScript *script = me->GetInstanceScript())
                            script->SetData(SCENARION_STEP_END, 0);
                        check6 = false;
                    }
                    else
                        timer -= diff;
                }
                if (me->GetPositionZ() >= 140.23f) // пњљпњљпњљпњљпњљ пњљ пњљпњљпњљпњљпњљпњљпњљпњљпњљ пњљпњљпњљпњљпњљпњљпњљпњљпњљ
                {
                    if (timerforevent <= diff)
                    {
                        uint8 const counter = urand(1, 5);
                        for (uint8 i = 0; i < counter; ++i)
                        {
                            if (Creature* targ = me->SummonCreature(92801, 1703.13f, 1449.93f, 104.0f, 2.62f))
                                targ->GetMotionMaster()->MovePath(439155, false, irand(-5, 5), irand(-4, 4)); // 8
                        }
                        DoCast(225218);
                        timerforevent = 3000;
                    }
                    else
                        timerforevent -= diff;
                }
            }
            if (me->GetEntry() == 90711) // пњљпњљпњљпњљпњљ
            {
                if (!firstcheck)
                {
                    if (InstanceScript *script = me->GetInstanceScript())
                        if (script->getScenarionStep() != 6)
                            return;
                    if (!tempcheck)
                    {
                        tempcheck = true;
                        // me->AI()->EnterEvadeMode();
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MovePath(439151, false);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                    }
                    if (me->isMoving())
                        return;

                    check3 = true;
                    timer = 20000;
                    firstcheck = true;
                }

                if (check1) // пњљпњљпњљпњљпњљ пњљпњљпњљпњљпњљ пњљпњљпњљпњљпњљпњљ
                {
                    if (timer <= diff)
                    {
                        Talk(2);
                        DoCast(224826);
                        check2 = true;
                        timer = 3000;
                        check1 = false;
                    }
                    else
                        timer -= diff;
                }
                if (check2) // пњљпњљпњљпњљпњљпњљпњљпњљ пњљпњљ пњљпњљпњљпњљпњљ
                {
                    if (timer <= diff)
                    {

                        me->RemoveAura(224826);
                        if (Creature* sylv = me->FindNearestCreature(90709, 40.0f, true))
                            sylv->AI()->Talk(5);
                        me->SummonGameObject(254234, 1324.90f, 1735.47f, 18.68f, 3.38f, 0.0f, 0.0f, 0.0f, 0.0f, DAY);
                        std::list<Creature*> guards;
                        GetCreatureListWithEntryInGrid(guards, me, 97525, 300.0f);
                        GetCreatureListWithEntryInGrid(guards, me, 112920, 300.0f);
                        GetCreatureListWithEntryInGrid(guards, me, 93704, 300.0f);
                        GetCreatureListWithEntryInGrid(guards, me, 90712, 300.0f);
                        GetCreatureListWithEntryInGrid(guards, me, 90709, 300.0f);
                        GetCreatureListWithEntryInGrid(guards, me, 90708, 300.0f);
                        GetCreatureListWithEntryInGrid(guards, me, 90710, 300.0f);
                        if (!guards.empty())
                            for (std::list<Creature*>::iterator itr = guards.begin(); itr != guards.end(); ++itr)
                            {
                                (*itr)->SetReactState(REACT_AGGRESSIVE);
                                (*itr)->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                                (*itr)->GetMotionMaster()->Clear();
                                (*itr)->GetMotionMaster()->MovePath(439152, false, irand(-3, 3), irand(-3, 3));
                            }
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MovePath(439152, false); // 1
                        check2 = false;

                    }
                    else
                        timer -= diff;
                }
                if (check3)
                {

                    if (timer <= diff)
                    {
                        if (Creature* sylv = me->FindNearestCreature(90709, 20.0f, true))
                            sylv->AI()->Talk(4);
                        check1 = true;
                        timer = 3000;
                        check3 = false;
                    }
                    else
                        timer -= diff;
                }
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
                        DoCast(224782);
                        events.ScheduleEvent(EVENT_1, 2000); // 224782
                        break;
                    case EVENT_2:
                        DoCast(224721);
                        events.ScheduleEvent(EVENT_2, 3000); // 224721
                        break;
                    case EVENT_3:
                        DoCast(199207);
                        events.ScheduleEvent(EVENT_3, 24000); // 199207
                        break;
                    case EVENT_4:
                        DoCast(167014);
                        events.ScheduleEvent(EVENT_4, 14000); // 167014
                        break;
                    case EVENT_5:
                        DoCast(178532);
                        events.ScheduleEvent(EVENT_5, 13000); // 178532
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };
};

void AddSC_brokenIslands()
{
    new npc_q42782("npc_q42782");
    new npc_q44281();
    new npc_q42740();
    new npc_q40518();
    new spell_bi_enter_stage1();
    new sceneTrigger_enterBrockenShores();
    new spell_q42740();
    new sceneTrigger_part1();
    new npc_bi_dread_commander();
    new npc_bi_felcommander_azgalor();

    new spell_scenarion_bi_step_6();
    new sceneTrigger_part7();
    new scenarion_bi_heroes();
    new sceneTrigger_part8();
    new scenario_bi_gualdan();

    new npc_q40517_p1();
    new scene_bi_alliance_q40593();

    new npc_q42782_1();
    new npc_109494();
    new go_240535();

    new npc_100395();
    new scenarion_bi_heroes_horde();

    new scene_bi_horde_q40607();
    new npc_q44281_1();
}