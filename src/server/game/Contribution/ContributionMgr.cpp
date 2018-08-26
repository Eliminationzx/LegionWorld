/*
    Created by d7561985@gmail.com
*/
#include "ContributionMgr.h"
#include "MiscPackets.h"
#include "QuestData.h"
#include "WorldStates/WorldStateMgr.h"

ContributionMgr& ContributionMgr::Instance()
{
    static ContributionMgr instance;
    return instance;
}

ContributionMgr::ContributionMgr()
{
    _contributionObjects.clear();
}

void ContributionMgr::Update(uint32 diff)
{
    for (auto& v : _contributionObjects)
    {
        switch (v.second.State)
        {
            case ContributionData::ContributionState::CONTRIBUTION_STATE_ACTIVE:
                v.second.CurrentLifeTimer += diff;
                if (v.second.CurrentLifeTimer >= v.second.UpTimeSecs)
                    OnChangeContributionState(v.first, ContributionData::ContributionState::CONTRIBUTION_STATE_UNDERATTACK);
                break;
            //case ContributionData::ContributionState::CONTRIBUTION_STATE_ACTIVE:
            //    v.second.CurrentUnderAtackTimer += diff;
            //    if (v.second.CurrentUnderAtackTimer >= v.second.DownTimeSecs)
            //        OnChangeContributionState(v.first, ContributionData::ContributionState::CONTRIBUTION_STATE_DESTROYED);
            //    break;
            default:
                break;
        }
    }
}

void ContributionMgr::Initialize()
{
    for (auto v : sManagedWorldStateStore)
    {
        WorldStateTemplate ws1(v->CurrentStageWorldStateID, WorldStatesData::Types::World, 0, 1 << WorldStatesData::Flags::InitialState, ContributionData::ContributionState::CONTRIBUTION_STATE_BUILDING, 0);
        sWorldStateMgr.AddTemplate(v->CurrentStageWorldStateID, ws1);

        WorldStateTemplate ws2(v->ProgressWorldStateID, WorldStatesData::Types::World, 0, 1 << WorldStatesData::Flags::InitialState, 0, 0);
        sWorldStateMgr.AddTemplate(v->ProgressWorldStateID, ws2);

        WorldStateTemplate ws3(0/*v->WorldStateTimerUI*/, WorldStatesData::Types::World, 0, 1 << WorldStatesData::Flags::InitialState, 1, 0);
        //sWorldStateMgr.AddTemplate(0/*v->WorldStateTimerUI*/, ws3);

        auto& obj = _contributionObjects[v->ID];
        obj.CurrentLifeTimer = 0;
        obj.CurrentUnderAtackTimer = 0;
        obj.UpTimeSecs = v->UpTimeSecs;
        obj.DownTimeSecs = v->DownTimeSecs;
        obj.State = ContributionData::ContributionState::CONTRIBUTION_STATE_ACTIVE;
        obj.WorldStateVareables[0] = v->CurrentStageWorldStateID;
        obj.WorldStateVareables[1] = v->ProgressWorldStateID;
        obj.WorldStateVareables[2] = 0/*v->WorldStateTimerUI*/;
    }
}

void ContributionMgr::OnChangeContributionState(uint32 contribuiontID, ContributionData::ContributionState newState)
{
    auto& obj = _contributionObjects[contribuiontID];
    obj.State = newState;

    switch (newState)
    {
        case ContributionData::ContributionState::CONTRIBUTION_STATE_BUILDING:
        {
            obj.CurrentUnderAtackTimer = 0;
            obj.DownTimeSecs = 0;
            break;
        }
        case ContributionData::ContributionState::CONTRIBUTION_STATE_ACTIVE:
        {
            obj.CurrentUnderAtackTimer = 0;
            obj.DownTimeSecs = 0;
            break;
        }
        case ContributionData::ContributionState::CONTRIBUTION_STATE_UNDERATTACK:
        {
            obj.CurrentLifeTimer = 0;
            obj.DownTimeSecs = 0;
            break;
        }
        case ContributionData::ContributionState::CONTRIBUTION_STATE_DESTROYED:
        {
            obj.CurrentLifeTimer = 0;
            obj.CurrentUnderAtackTimer = 0;
            obj.UpTimeSecs = 0;
            obj.DownTimeSecs = 0;
            //player->ModifyCurrency(GetPersonalTracker(contributuinID), player->GetCurrency(GetPersonalTracker(contributuinID)) + 1);
            break;
        }
        default:
            break;
    }

    sWorldStateMgr.SetWorldStateValueFor(7543, obj.WorldStateVareables[0], newState);
}

inline uint32 GetPersonalTracker(uint32 contributionID)
{
    switch (contributionID)
    {
        case ContributionData::CONTRIBUTION_MAGE_TOWER:
            return CURRENCY_TYPE_LEGIONFALL_PERSONAL_TRACKER_MAGE_TOWER;
        case ContributionData::CONTRIBUTION_COMMAND_CENTER:
            return CURRENCY_TYPE_LEGIONFALL_PERSONAL_TRACKER_COMMAND_TOWER;
        case ContributionData::CONTRIBUTION_NETHER_DISRUPTOR:
            return CURRENCY_TYPE_LEGIONFALL_PERSONAL_TRACKER_NETHER_TOWER;
        default:
            return 0;
    }
}

void ContributionMgr::Contribute(Player* player, uint8 contributuinID)
{
    //@TODO correct packet to usage ContributionResult

    if (!player->HasCurrency(CURRENCY_TYPE_LEGIONFALL_WAR_SUPPLIES, 100))
    {
        WorldPackets::Misc::DisplayGameError display;
        display.Error = UIErrors::ERR_NOT_ENOUGH_CURRENCY;
        player->SendDirectMessage(display.Write());
        return;
    }

    uint32 rewardQuest = 0;
    //for (auto v : sManagedWorldStateInputStore)
    //    if (v->ContributionID == contributuinID)
    //        rewardQuest = v->RewardQuest;

    if (!rewardQuest)
    {
        WorldPackets::Misc::DisplayGameError display;
        display.Error = UIErrors::ERR_CANT_DO_THAT_RIGHT_NOW;
        player->SendDirectMessage(display.Write());
        return;
    }

    auto quest = sQuestDataStore->GetQuestTemplate(rewardQuest);
    if (!quest || player->CanAddQuest(quest, true))
    {
        WorldPackets::Misc::DisplayGameError display;
        display.Error = UIErrors::ERR_CANT_DO_THAT_RIGHT_NOW;
        player->SendDirectMessage(display.Write());
        return;
    }

    player->ModifyCurrency(CURRENCY_TYPE_LEGIONFALL_WAR_SUPPLIES, -100, false, true);
    player->ModifyCurrency(GetPersonalTracker(contributuinID), player->GetCurrency(GetPersonalTracker(contributuinID)) + 1, false, true);

    player->AddQuest(quest, nullptr);

    if (player->CanCompleteQuest(rewardQuest))
        player->CompleteQuest(rewardQuest);

    auto var1 = _contributionObjects[contributuinID].WorldStateVareables[1];
    sWorldStateMgr.SetWorldStateValueFor(player, var1, sWorldStateMgr.GetWorldStateValue(var1) + 1);

    //! ToDo: registre contributuin at worldstate mgr.
}

void ContributionMgr::ContributionGetState(Player* player, uint32 contributionID, uint32 contributionGuid)
{
    WorldPackets::Misc::ContributionResponse data;
    data.Data = 0;  //ToDo:
    data.ContributionID = contributionID;
    data.ContributionGUID = contributionGuid;
    player->SendDirectMessage(data.Write());
}
