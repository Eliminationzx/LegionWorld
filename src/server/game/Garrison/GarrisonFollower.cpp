
#include "GarrisonFollower.h"
#include "Garrison.h"

using namespace GarrisonConst;

Follower const* Garrison::GetFollower(uint64 dbId) const
{
    for (uint8 i = GARRISON_TYPE_GARRISON; i < GARRISON_TYPE_MAX; ++i)
    {
        auto itr = _followers[i].find(dbId);
        if (itr != _followers[i].end())
            return &itr->second;
    }

    return nullptr;
}

Follower* Garrison::GetFollower(uint64 dbId)
{
    for (uint8 i = GARRISON_TYPE_GARRISON; i < GARRISON_TYPE_MAX; ++i)
    {
        auto itr = _followers[i].find(dbId);
        if (itr != _followers[i].end())
            return &itr->second;
    }

    return nullptr;
}

Follower* Garrison::GetFollowerByID(uint32 entry)
{
    for (uint8 i = GARRISON_TYPE_GARRISON; i < GARRISON_TYPE_MAX; ++i)
    {
        for (auto& v : _followers[i])
            if (v.second.PacketInfo.GarrFollowerID == entry)
                return &v.second;
    }

    return nullptr;
}

uint32 Follower::GetItemLevel() const
{
    return (PacketInfo.ItemLevelWeapon + PacketInfo.ItemLevelArmor) / 2;
}

void Follower::ModAssistant(SpellInfo const* spellInfo, Player* caster)
{
    enum Types : uint32
    {
        SetWeaponLevel = 0,
        SetArmorLevel = 1,
        IncreaseWeaponLevel = 2,
        IncreaseArmorLevel = 3
    };

    auto itemLevelUpgradeDataEntry = sGarrItemLevelUpgradeDataStore[spellInfo->Effects[0]->MiscValue];
    if (!itemLevelUpgradeDataEntry)
        return;

    auto followerTypeData = sGarrFollowerTypeStore[TypeID];
    if (!followerTypeData)
        return;

    auto maxAllowedItemLevel = itemLevelUpgradeDataEntry->MaxItemLevel;
    if (maxAllowedItemLevel > followerTypeData->MaxItemLevel)
        return;

    if (!maxAllowedItemLevel && TypeID == FollowerType::Garrison)
        maxAllowedItemLevel = followerTypeData->MaxItemLevel;

    auto updateInfo = false;
    uint32 value = spellInfo->Effects[0]->BasePoints;

    switch (itemLevelUpgradeDataEntry->Operation)
    {
    case SetWeaponLevel:
        updateInfo = true;
        PacketInfo.ItemLevelWeapon = value;
        break;
    case SetArmorLevel:
        updateInfo = true;
        PacketInfo.ItemLevelArmor = value;
        break;
    case IncreaseWeaponLevel:
        updateInfo = true;
        PacketInfo.ItemLevelWeapon += value;
        break;
    case IncreaseArmorLevel:
        updateInfo = true;
        PacketInfo.ItemLevelArmor += value;
        break;
    default:
        break;
    }

    if (!updateInfo)
        return;

    PacketInfo.ItemLevelWeapon = std::min(PacketInfo.ItemLevelWeapon, static_cast<uint32>(followerTypeData->MaxItemLevel));
    PacketInfo.ItemLevelArmor = std::min(PacketInfo.ItemLevelArmor, static_cast<uint32>(followerTypeData->MaxItemLevel));

    DbState = DB_STATE_CHANGED;

    WorldPackets::Garrison::GarrisonFollowerChangedItemLevel update;
    update.Follower = PacketInfo;
    caster->SendDirectMessage(update.Write());
}

void Follower::IncreaseFollowerExperience(SpellInfo const* spellInfo, Player* caster)
{
    WorldPackets::Garrison::GarrisonFollowerChangedXP update;
    update.Follower = PacketInfo;

    if (TypeID == spellInfo->Effects[0]->MiscValueB)
    {
        GiveXP(spellInfo->Effects[0]->BasePoints);
        update.TotalXp = spellInfo->Effects[0]->BasePoints;
    }
    else
        update.Result = GARRISON_ERROR_FOLLOWER_CANNOT_GAIN_XP;

    update.Follower2 = PacketInfo;
    caster->SendDirectMessage(update.Write());
}

uint8 Follower::RollQuality(uint32 baseQuality)
{
    uint8 quality = FOLLOWER_QUALITY_UNCOMMON;
    // 35% - rare, 7% - epic
    uint32 r = urand(0, 100);
    if (r >= 65 && r < 90)
        quality = FOLLOWER_QUALITY_RARE;
    else if (r >= 93 && r <= 100)
        quality = FOLLOWER_QUALITY_EPIC;

    return quality > baseQuality ? quality : baseQuality;
}

void Follower::GiveXP(uint32 xp)
{
    auto curXp = PacketInfo.Xp;
    auto nextLvlXP = GetXPForNextUpgrade();
    auto newXP = curXp + xp;

    auto modXP([this, &nextLvlXP, &newXP](uint32 maxAllowedQuality, uint32 maxAllowedLevel) -> void
    {
        if (PacketInfo.FollowerLevel == maxAllowedLevel && PacketInfo.Quality == maxAllowedQuality)
            newXP = 0;

        while (newXP >= nextLvlXP && PacketInfo.FollowerLevel < maxAllowedLevel)
        {
            newXP -= nextLvlXP;

            if (PacketInfo.FollowerLevel < maxAllowedLevel)
                GiveLevel(PacketInfo.FollowerLevel + 1);

            nextLvlXP = GetXPForNextUpgrade();
            //++PacketInfo.FollowerLevel;
        }

        while (newXP >= nextLvlXP && PacketInfo.FollowerLevel == maxAllowedLevel && PacketInfo.Quality < maxAllowedQuality)
        {
            newXP -= nextLvlXP;

            if (PacketInfo.Quality < maxAllowedQuality)
            {
                if (auto nextQuality = sDB2Manager.GetNextFollowerQuality(PacketInfo.Quality, TypeID))
                    PacketInfo.Quality = nextQuality;
            }
            else
            {
                //result = GARRISON_ERROR_INVALID_FOLLOWER_QUALITY
            }

            nextLvlXP = GetXPForNextUpgrade();
        }
    });

    switch (TypeID)
    {
    case FollowerType::Garrison:
        modXP(FOLLOWER_QUALITY_EPIC, Globals::MaxFollowerLevel);
        break;
    case FollowerType::Follower:
        modXP(FOLLOWER_QUALITY_TITLE, Globals::MaxFollowerLevelHall);
        break;
    case FollowerType::Shipyard: //@TODO find rules
        break;
    default:
        break;
    }

    DbState = DB_STATE_CHANGED;
    PacketInfo.Xp = newXP;
}

void Follower::GiveLevel(uint32 level)
{
    PacketInfo.FollowerLevel = level;
}

void Follower::SetQuality(uint32 quality)
{
    PacketInfo.Quality = quality;
}

uint32 Follower::GetXPForNextUpgrade()
{
    if (PacketInfo.FollowerLevel < FollowerType::MaxLevel[TypeID])
        return sDB2Manager.GetXPForNextFollowerLevel(PacketInfo.FollowerLevel, TypeID);

    if (PacketInfo.FollowerLevel == FollowerType::MaxLevel[TypeID])
        return sDB2Manager.GetXPForNextFollowerQuality(PacketInfo.Quality, TypeID);

    return 0;
}
