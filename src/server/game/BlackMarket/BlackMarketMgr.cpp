/*
* Copyright (C) 2008-2015 TrinityCore <http://www.trinitycore.org/>
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "BlackMarketMgr.h"
#include "Player.h"
#include "DatabaseEnv.h"
#include "Mail.h"
#include "ObjectMgr.h"
#include "BlackMarketPackets.h"

BlackMarketMgr::BlackMarketMgr(): _LastUpdate(0)
{
}

BlackMarketMgr::~BlackMarketMgr()
{
    for (auto& itr : mAuctions)
        delete itr.second;

    for (auto& itr : mTemplates)
        delete itr.second;
}

BlackMarketMgr* BlackMarketMgr::instance()
{
    static BlackMarketMgr instance;
    return &instance;
}

void BlackMarketMgr::LoadTemplates()
{
    uint32 oldMSTime = getMSTime();

    if (!mTemplates.empty())
    {
        for (auto& itr : mTemplates)
            delete itr.second;

        mTemplates.clear();
    }

    PreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_SEL_BLACKMARKET_TEMPLATE);
    PreparedQueryResult result = WorldDatabase.Query(stmt);

    if (!result)
        return;

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();
        auto templ = new BlackMarketTemplate;

        if (!templ->LoadFromDB(fields))
        {
            delete templ;
            continue;
        }

        AddTemplate(templ);
        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u black market templates in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));
}

void BlackMarketMgr::LoadAuctions()
{
    uint32 oldMSTime = getMSTime();

    if (!mAuctions.empty())
    {
        for (auto& itr : mAuctions)
            delete itr.second;

        mAuctions.clear();
    }

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_BLACKMARKET_AUCTIONS);
    PreparedQueryResult result = CharacterDatabase.Query(stmt);
    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 black market auctions. DB table `blackmarket_auctions` is empty.");
        return;
    }

    uint32 count = 0;
    _LastUpdate = time(nullptr);

    SQLTransaction trans = CharacterDatabase.BeginTransaction();
    do
    {
        Field* fields = result->Fetch();
        auto auction = new BlackMarketEntry;

        if (!auction->LoadFromDB(fields))
        {
            auction->DeleteFromDB(trans);
            delete auction;
            continue;
        }

        if (auction->IsCompleted())
        {
            auction->DeleteFromDB(trans);
            delete auction;
            continue;
        }

        AddAuction(auction);
        ++count;
    }
    while (result->NextRow());

    CharacterDatabase.CommitTransaction(trans);

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u black market auctions in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));
}

void BlackMarketMgr::Update(bool updateTime)
{
    SQLTransaction trans = CharacterDatabase.BeginTransaction();
    time_t now = time(nullptr);
    for (auto& itr : mAuctions)
    {
        BlackMarketEntry* entry = itr.second;

        if (entry->IsCompleted() && entry->GetBidder())
            SendAuctionWonMail(entry, trans);

        if (updateTime)
            entry->Update(now);
    }

    if (updateTime)
        _LastUpdate = now;

    CharacterDatabase.CommitTransaction(trans);
}

void BlackMarketMgr::RefreshAuctions()
{
    SQLTransaction trans = CharacterDatabase.BeginTransaction();
    for (auto itr = mAuctions.begin(); itr != mAuctions.end();)
    {
        BlackMarketEntry* entry = itr->second;
        if (!entry->IsCompleted())
        {
            ++itr;
            continue;
        }

        entry->DeleteFromDB(trans);
        itr = mAuctions.erase(itr);
        delete entry;
    }

    CharacterDatabase.CommitTransaction(trans);
    trans = CharacterDatabase.BeginTransaction();

    while (mAuctions.size() < sWorld->getIntConfig(CONFIG_BLACKMARKET_MAXAUCTIONS))
    {
        if (mAuctions.size() >= mTemplates.size())
            break;

        auto itr = mTemplates.begin();
        std::advance(itr, rand32() % mTemplates.size());

        if (!roll_chance_f(itr->second->Chance))
            continue;

        int32 marketId = itr->second->MarketID;
        if (!GetAuctionByID(marketId))
        {
            auto entry = new BlackMarketEntry;
            entry->Initialize(marketId);
            entry->SaveToDB(trans);
            AddAuction(entry);
        }
    }

    CharacterDatabase.CommitTransaction(trans);

    Update(true);
}

time_t BlackMarketMgr::GetLastUpdate() const
{
    return _LastUpdate;
}

bool BlackMarketMgr::IsEnabled() const
{
    return sWorld->getBoolConfig(CONFIG_BLACKMARKET_ENABLED);
}

void BlackMarketMgr::BuildItemsResponse(WorldPackets::BlackMarket::BlackMarketRequestItemsResult& packet, Player* player)
{
    packet.LastUpdateID = _LastUpdate;
    for (auto& itr : mAuctions)
    {
        WorldPackets::BlackMarket::BlackMarketItem item;
        item.Initialize(itr.second, player);
        packet.Items.push_back(item);
    }
}

void BlackMarketMgr::AddAuction(BlackMarketEntry* auction)
{
    mAuctions.insert(std::make_pair(auction->GetMarketId(), auction));
}

void BlackMarketMgr::AddTemplate(BlackMarketTemplate* templ)
{
    mTemplates.insert(std::make_pair(templ->MarketID, templ));
}

void BlackMarketMgr::SendAuctionWonMail(BlackMarketEntry* entry, SQLTransaction& trans)
{
    // Mail already sent
    if (entry->GetMailSent())
        return;

    ObjectGuid bidderGuid = ObjectGuid::Create<HighGuid::Player>(entry->GetBidder());
    Player* bidder = ObjectAccessor::FindPlayer(bidderGuid);

    BlackMarketTemplate* templ = entry->GetTemplate();
    Item* item = Item::CreateItem(templ->Item.ItemID, templ->Quantity);
    if (!item)
        return;

    if (templ->Item.ItemBonus)
        for (uint32 bonusList : templ->Item.ItemBonus->BonusListIDs)
            item->AddBonuses(bonusList);

    item->SetOwnerGUID(bidderGuid);

    item->SaveToDB(trans);

    if (bidder)
        bidder->GetSession()->SendBlackMarketWonNotification(entry, item);

    MailDraft(entry->BuildAuctionMailSubject(BMAH_AUCTION_WON), entry->BuildAuctionMailBody())
        .AddItem(item)
        .SendMailTo(trans, MailReceiver(bidder, entry->GetBidder()), entry, MAIL_CHECK_MASK_COPIED);

    entry->MailSent();
}

void BlackMarketMgr::SendAuctionOutbidMail(BlackMarketEntry* entry, SQLTransaction& trans)
{
    ObjectGuid oldBidderGuid = ObjectGuid::Create<HighGuid::Player>(entry->GetBidder());
    Player* oldBidder = ObjectAccessor::FindPlayer(oldBidderGuid);

    uint32 oldBidderAccID = 0;
    if (!oldBidder)
        oldBidderAccID = ObjectMgr::GetPlayerAccountIdByGUID(oldBidderGuid);

    if (!oldBidder && !oldBidderAccID)
        return;

    if (oldBidder)
        oldBidder->GetSession()->SendBlackMarketOutbidNotification(entry->GetTemplate());

    MailDraft(entry->BuildAuctionMailSubject(BMAH_AUCTION_OUTBID), entry->BuildAuctionMailBody())
        .AddMoney(entry->GetCurrentBid())
        .SendMailTo(trans, MailReceiver(oldBidder, entry->GetBidder()), entry, MAIL_CHECK_MASK_COPIED);
}

BlackMarketEntry* BlackMarketMgr::GetAuctionByID(int32 marketId) const
{
    return Trinity::Containers::MapGetValuePtr(mAuctions, marketId);
}

BlackMarketTemplate* BlackMarketMgr::GetTemplateByID(int32 marketId) const
{
    return Trinity::Containers::MapGetValuePtr(mTemplates, marketId);
}

bool BlackMarketTemplate::LoadFromDB(Field* fields)
{
    MarketID = fields[0].GetInt32();
    SellerNPC = fields[1].GetInt32();
    Item.ItemID = fields[2].GetUInt32();
    Quantity = fields[3].GetInt32();
    MinBid = fields[4].GetUInt64();
    Duration = static_cast<time_t>(fields[5].GetUInt32());
    Chance = fields[6].GetFloat();

    Tokenizer bonusListIDsTok(fields[7].GetString(), ' ');
    std::vector<uint32> bonusListIDs;
    for (char const* token : bonusListIDsTok)
        bonusListIDs.push_back(atol(token));

    if (!bonusListIDs.empty())
    {
        Item.ItemBonus = boost::in_place();
        Item.ItemBonus->BonusListIDs = bonusListIDs;
    }

    if (!sObjectMgr->GetCreatureTemplate(SellerNPC))
        return false;

    if (!sObjectMgr->GetItemTemplate(Item.ItemID))
        return false;

    return true;
}

void BlackMarketEntry::Update(time_t newTimeOfUpdate)
{
    _secondsRemaining = _secondsRemaining - (newTimeOfUpdate - sBlackMarketMgr->GetLastUpdate());
}

void BlackMarketEntry::Initialize(int32 marketId)
{
    _marketId = marketId;
    _secondsRemaining = GetTemplate()->Duration;
}

BlackMarketTemplate* BlackMarketEntry::GetTemplate() const
{
    return sBlackMarketMgr->GetTemplateByID(_marketId);
}

int32 BlackMarketEntry::GetMarketId() const
{
    return _marketId;
}

uint32 BlackMarketEntry::GetCurrentBid() const
{
    return _currentBid;
}

void BlackMarketEntry::SetCurrentBid(uint32 bid)
{
    _currentBid = bid;
}

int32 BlackMarketEntry::GetNumBids() const
{
    return _numBids;
}

void BlackMarketEntry::SetNumBids(int32 numBids)
{
    _numBids = numBids;
}

ObjectGuid::LowType BlackMarketEntry::GetBidder() const
{
    return _bidder;
}

void BlackMarketEntry::SetBidder(ObjectGuid::LowType bidder)
{
    _bidder = bidder;
}

time_t BlackMarketEntry::GetSecondsRemaining() const
{
    return _secondsRemaining - (time(nullptr) - sBlackMarketMgr->GetLastUpdate());
}

time_t BlackMarketEntry::GetExpirationTime() const
{
    return time(nullptr) + GetSecondsRemaining();
}

bool BlackMarketEntry::IsCompleted() const
{
    return GetSecondsRemaining() <= 0;
}

bool BlackMarketEntry::LoadFromDB(Field* fields)
{
    _marketId = fields[0].GetInt32();

    BlackMarketTemplate* templ = sBlackMarketMgr->GetTemplateByID(_marketId);
    if (!templ)
        return false;

    _currentBid = fields[1].GetUInt64();
    _secondsRemaining = static_cast<time_t>(fields[2].GetInt32()) - sBlackMarketMgr->GetLastUpdate();
    _numBids = fields[3].GetInt32();
    _bidder = fields[4].GetUInt64();

    if (_bidder && !sObjectMgr->GetPlayerAccountIdByGUID(ObjectGuid::Create<HighGuid::Player>(_bidder))) // Probably a better way to check if player exists
        return false;

    return true;
}

uint64 BlackMarketEntry::GetMinIncrement() const
{
    return (_currentBid / 20) - ((_currentBid / 20) % GOLD);
}

void BlackMarketEntry::SaveToDB(SQLTransaction& trans) const
{
    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_BLACKMARKET_AUCTIONS);

    stmt->setInt32(0, _marketId);
    stmt->setUInt64(1, _currentBid);
    stmt->setInt32(2, GetExpirationTime());
    stmt->setInt32(3, _numBids);
    stmt->setUInt64(4, _bidder);

    trans->Append(stmt);
}

void BlackMarketEntry::DeleteFromDB(SQLTransaction& trans) const
{
    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_BLACKMARKET_AUCTIONS);
    stmt->setInt32(0, _marketId);
    trans->Append(stmt);
}

bool BlackMarketEntry::ValidateBid(uint64 bid) const
{
    if (bid <= _currentBid)
        return false;

    if (bid < _currentBid + GetMinIncrement())
        return false;

    if (bid >= BMAH_MAX_BID)
        return false;

    return true;
}

void BlackMarketEntry::PlaceBid(uint64 bid, Player* player, SQLTransaction& trans)
{
    if (bid < _currentBid)
        return;

    _currentBid = bid;
    ++_numBids;

    if (GetSecondsRemaining() < 30 * MINUTE)
        _secondsRemaining += 30 * MINUTE;

    _bidder = player->GetGUID().GetCounter();

    player->ModifyMoney(-static_cast<int64>(bid));


    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_BLACKMARKET_AUCTIONS);

    stmt->setUInt64(0, _currentBid);
    stmt->setInt32(1, GetExpirationTime());
    stmt->setInt32(2, _numBids);
    stmt->setUInt64(3, _bidder);
    stmt->setInt32(4, _marketId);

    trans->Append(stmt);

    sBlackMarketMgr->Update(true);
}

std::string BlackMarketEntry::BuildAuctionMailSubject(BMAHMailAuctionAnswers response) const
{
    std::ostringstream strm;
    strm << GetTemplate()->Item.ItemID << ":0:" << response << ':' << GetMarketId() << ':' << GetTemplate()->Quantity;
    return strm.str();
}

std::string BlackMarketEntry::BuildAuctionMailBody()
{
    std::ostringstream strm;
    strm << GetTemplate()->SellerNPC << ':' << _currentBid;

    return strm.str();
}

void BlackMarketEntry::MailSent()
{
    _mailSent = true;
}

bool BlackMarketEntry::GetMailSent() const
{
    return _mailSent;
}
