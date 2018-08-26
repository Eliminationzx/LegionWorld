/*
* Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
* Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
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

#include "AccountMgr.h"
#include "AnticheatMgr.h"
#include "Challenge.h"
#include "Player.h"
#include "ObjectMgr.h"
#include "MoveSpline.h"
#include "PlayerDefines.h"
#include "Vehicle.h"
#include "DatabaseEnv.h"

AnticheatMgr::AnticheatMgr(Player* owner) : m_player(owner)
{
    lastHeartbeatTime = 0;
    lastClientMoveTime = 0;
    lastServerMoveTime = 0;

    isSwimming = false;
    jumpCount = 0;
    fall.Reset();

    // temp
    nextScannedTime = 0;

    transmCooldownEnd = 0;
    transmCheatUseCount = 0;
    cheatLootCount = 0;
}

void AnticheatMgr::Update(uint32 diff)
{
}

bool AnticheatMgr::ValidateMovementFlags(MovementInfo const& movementInfo)
{
    Unit* mover = m_player->m_mover;

    if (sWorld->getIntConfig(CONFIG_NEW_ANTICHEAT_MODE) == 2)
        return true;

    if (mover->IsPlayer())
    {
        // transport temp
        if (movementInfo.hasTransportData || m_player->GetTransport())
            return true;

        // taxi / spline
        if (m_player->isInFlight() || m_player->IsSplineEnabled())
            return true;
    }

    // check swimming state
    if (movementInfo.HasMovementFlag(MOVEMENTFLAG_SWIMMING) && !isSwimming)
    {
        sLog->outWarden("MovementAnticheat: Detect old flyhack, player %s", m_player->GetName());
        return false;
    }

    // check flying states
    if (movementInfo.HasMovementFlag(MOVEMENTFLAG_CAN_FLY | MOVEMENTFLAG_FLYING) && !mover->HasUnitMovementFlag(MOVEMENTFLAG_CAN_FLY))
    {
        sLog->outWarden("MovementAnticheat: Detect flyhack type 1, player %s", m_player->GetName());
        return false;
    }

    // check gravity
    if (movementInfo.HasMovementFlag(MOVEMENTFLAG_DISABLE_GRAVITY) && !mover->HasUnitMovementFlag(MOVEMENTFLAG_DISABLE_GRAVITY))
    {
        sLog->outWarden("MovementAnticheat: Detect disable gravity, player %s", m_player->GetName());
        return false;
    }

    // check waterwalking
    if (movementInfo.HasMovementFlag(MOVEMENTFLAG_WATERWALKING) && !mover->HasUnitMovementFlag(MOVEMENTFLAG_WATERWALKING))
    {
        sLog->outWarden("MovementAnticheat: Detect waterwalk hack, player %s", m_player->GetName());
        return false;
    }

    // check hover
    if (movementInfo.HasMovementFlag(MOVEMENTFLAG_HOVER) && !mover->HasUnitMovementFlag(MOVEMENTFLAG_HOVER))
    {
        sLog->outWarden("MovementAnticheat: Detect hover hack, player %s", m_player->GetName());
        return false;
    }

    // detect EWT new flyhack
    if (movementInfo.HasMovementFlag(MOVEMENTFLAG_ASCENDING | MOVEMENTFLAG_DESCENDING) && !mover->HasUnitMovementFlag(MOVEMENTFLAG_FLYING))
    {
        if (!isSwimming)
        {
            sLog->outWarden("MovementAnticheat: Detect new flyhack, player %s", m_player->GetName());
            return false;
        }
    }

    return true;
}

bool AnticheatMgr::MovementChecks(uint16 opcode, MovementInfo const& movementInfo)
{
    // set swimming state
    if (movementInfo.HasMovementFlag(MOVEMENTFLAG_SWIMMING))
    {
        if (opcode == CMSG_MOVE_START_SWIM)
            isSwimming = true;
    }
    else
    {
        if (opcode == CMSG_MOVE_STOP_SWIM)
            isSwimming = false;
    }

    if (!sWorld->getIntConfig(CONFIG_NEW_ANTICHEAT_MODE))
        return true;

    // check movement flags from client
    if (!ValidateMovementFlags(movementInfo))
        return false;

    if (!CanAllowUpdatePosition(opcode, movementInfo))
    {
        Player::SavePositionInDB(m_player->GetMapId(), m_player->m_movementInfo.Pos.GetPositionX(), m_player->m_movementInfo.Pos.GetPositionY(), m_player->m_movementInfo.Pos.GetPositionZ(), m_player->m_movementInfo.Pos.GetOrientation(), m_player->GetCurrentZoneID(), m_player->GetGUID());
        m_player->GetSession()->KickPlayer();
        return false;
    }

    return true;
}

bool AnticheatMgr::CanAllowUpdatePosition(uint16 opcode, MovementInfo const& movementInfo)
{
    // check gamemasters
    if (m_player->GetSession()->GetSecurity() > SEC_MODERATOR && m_player->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_GM))
        return true;

    // check valid source coordinates
    if (m_player->GetPositionX() == 0.0f || m_player->GetPositionY() == 0.0f || m_player->GetPositionZ() == 0.0f)
        return true;

    // check valid destination coordinates
    if (movementInfo.Pos.GetPositionX() == 0.0f || movementInfo.Pos.GetPositionY() == 0.0f || movementInfo.Pos.GetPositionZ() == 0.0f)
        return true;

    // check zones/areas
    uint32 destZoneId = 0;
    uint32 destAreaId = 0;
    m_player->GetMap()->GetZoneAndAreaId(destZoneId, destAreaId, movementInfo.Pos.GetPositionX(), movementInfo.Pos.GetPositionY(), movementInfo.Pos.GetPositionZ());

    // exclude unknown source/destination zones
    if (!m_player->GetCurrentZoneID() || !m_player->GetCurrentAreaID() || !destZoneId || !destAreaId)
        return true;

    // temp exclude Broken Shore Scenario
    if (m_player->GetMapId() == 1460)
        return true;

    bool failSpeedCheck = false;
    float moveDist = 0.0f;
    float allowedDist = 0.0f;
    float moveDistZ = 0.0f;
    float allowedDistZ = 0.0f;
    float speed = GetBaseSpeed(m_player->m_mover);

    // check speed - test two ways of it
    if (sWorld->getIntConfig(CONFIG_NEW_ANTICHEAT_MODE) == 2)
    {
        float serverX, clientX = 0.0f;
        float serverY, clientY = 0.0f;
        float serverZ, clientZ = 0.0f;
        bool serverTransport = false;
        bool clientTransport = false;

        // temp return primitive check for remove cheat long teleports
        if (m_player->GetTransport() || m_player->m_movementInfo.hasTransportData)
        {
            serverX = m_player->m_movementInfo.transport.Pos.m_positionX;
            serverY = m_player->m_movementInfo.transport.Pos.m_positionY;
            serverZ = m_player->m_movementInfo.transport.Pos.m_positionZ;
            serverTransport = true;
        }

        if (movementInfo.hasTransportData)
        {
            clientX = movementInfo.transport.Pos.m_positionX;
            clientY = movementInfo.transport.Pos.m_positionY;
            clientZ = movementInfo.transport.Pos.m_positionZ;
            clientTransport = true;
        }

        if (!serverTransport)
        {
            serverX = m_player->GetPositionX();
            serverY = m_player->GetPositionY();
            serverZ = m_player->GetPositionZ();
        }

        if (!clientTransport)
        {
            clientX = movementInfo.Pos.m_positionX;
            clientY = movementInfo.Pos.m_positionY;
            clientZ = movementInfo.Pos.m_positionZ;
        }

        // transform coordinates are not supported in anticheat
        if (serverTransport && !clientTransport || !serverTransport && clientTransport)
            return true;

        float tempDeltaX = clientX - serverX;
        float tempDeltaY = clientY - serverY;
        float tempDeltaZ = clientZ - serverZ;

        float totalTempDelta = sqrt(pow(tempDeltaX, 2) + pow(tempDeltaY, 2) + pow(tempDeltaZ, 2));

        if (totalTempDelta > 70.0f && speed < 70.0f)
            failSpeedCheck = true;

        moveDist = totalTempDelta;
        allowedDist = 70.0f;
    }
    else
    {
        // temporary - need check delta on server and send correct timestamps to other clients
        int32 clientTimeDelta = movementInfo.clientTickCount - lastClientMoveTime;

        // delta between two movement packets can't be bigger than 500 ms
        if (clientTimeDelta > 500)
            clientTimeDelta = 500;

        float dxy = 0.001f;
        float dz = 0.001f;

        if (movementInfo.HasMovementFlag(MOVEMENTFLAG_FALLING) || movementInfo.hasFallData)
        {
            // this calculates on client and send JumpVelocity as result. Yes, we need checked it.
            dxy = fall.HorizontalSpeed / 1000 * clientTimeDelta;
            dz = -Movement::computeFallElevation(clientTimeDelta, m_player->m_mover->HasUnitMovementFlag(MOVEMENTFLAG_FEATHER_FALL), -fall.JumpVelocity);
        }
        else
        {
            // TODO: Maximum dyx/dz (max climb angle) if not swimming.
            dxy = speed / 1000.0f * clientTimeDelta;
            dz = speed / 1000.0f * clientTimeDelta;

            // Allow some margin
            // real XY margin - 0.5f, real Z margin - 0.5f
            dxy += 0.5f;
            dz += 2.5f;

            // player->m_movementInfo.GetPosition() != player->GetPosition() what the fuck???
            //float realDistance2D_sq = pow(movementInfo.Pos.m_positionX - m_player->m_movementInfo.Pos.m_positionX, 2) + pow(movementInfo.Pos.m_positionY - m_player->m_movementInfo.Pos.m_positionY, 2);
            //float realDistanceZ = fabs(movementInfo.Pos.m_positionZ - m_player->m_movementInfo.Pos.m_positionZ);
            float realDistance2D_sq = pow(movementInfo.Pos.m_positionX - m_player->GetPositionX(), 2) + pow(movementInfo.Pos.m_positionY - m_player->GetPositionY(), 2);
            float realDistanceZ = fabs(movementInfo.Pos.m_positionZ - m_player->GetPositionZ());

            // XY distance, real margin coeff - 1.1f
            allowedDist = dxy * dxy * 1.5f;

            if (realDistance2D_sq > allowedDist)
                failSpeedCheck = true;

            if (realDistanceZ > dz)
                failSpeedCheck = true;

            moveDist = realDistance2D_sq;
            moveDistZ = realDistanceZ;
            allowedDistZ = dz;
        }
    }

    if (failSpeedCheck)
    {
        // get zone and area info
        MapEntry const* mapEntry = sMapStore.LookupEntry(m_player->GetMapId());
        AreaTableEntry const* srcZoneEntry = sAreaTableStore.LookupEntry(m_player->GetCurrentZoneID());
        AreaTableEntry const* srcAreaEntry = sAreaTableStore.LookupEntry(m_player->GetCurrentAreaID());
        AreaTableEntry const* destZoneEntry = sAreaTableStore.LookupEntry(destZoneId);
        AreaTableEntry const* destAreaEntry = sAreaTableStore.LookupEntry(destAreaId);

        const char *mapName = mapEntry ? mapEntry->MapName->Str[sObjectMgr->GetDBCLocaleIndex()] : "<unknown>";
        const char *srcZoneName = srcZoneEntry ? srcZoneEntry->AreaName->Str[sObjectMgr->GetDBCLocaleIndex()] : "<unknown>";
        const char *srcAreaName = srcAreaEntry ? srcAreaEntry->AreaName->Str[sObjectMgr->GetDBCLocaleIndex()] : "<unknown>";
        const char *destZoneName = destZoneEntry ? destZoneEntry->AreaName->Str[sObjectMgr->GetDBCLocaleIndex()] : "<unknown>";
        const char *destAreaName = destAreaEntry ? destAreaEntry->AreaName->Str[sObjectMgr->GetDBCLocaleIndex()] : "<unknown>";

        bool hasHoverAuras = m_player->HasAuraType(SPELL_AURA_HOVER);
        bool hasWWAuras = m_player->HasAuraType(SPELL_AURA_WATER_WALK);
        bool hasFlyingAuras = m_player->HasAuraType(SPELL_AURA_FLY) || m_player->HasAuraType(SPELL_AURA_MOD_INCREASE_VEHICLE_FLIGHT_SPEED)
            || m_player->HasAuraType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED) || m_player->HasAuraType(SPELL_AURA_MOD_INCREASE_FLIGHT_SPEED)
            || m_player->HasAuraType(SPELL_AURA_MOD_MOUNTED_FLIGHT_SPEED_ALWAYS);

        uint32 vehicleID = 0;
        uint32 vehicleKitID = 0;
        if (Vehicle* vehicle = m_player->GetVehicle())
            if (vehicle->GetVehicleInfo())
                vehicleID = vehicle->GetVehicleInfo()->ID;

        if (Vehicle* vehicleKit = m_player->GetVehicleKit())
            if (vehicleKit->GetVehicleInfo())
                vehicleKitID = vehicleKit->GetVehicleInfo()->ID;

        sLog->outWarden("MovementAnticheat: Change position with movement violation: player %s, %s\n"
            "    XY: %.2f yd, but allowed %.2f, Z: %.2f yd, but allowed %.2f\n"
            "    speed: %f, currentMovementFlags: 0x%X, packetMovementFlags: 0x%X\n"
            "    compatibleAuras: hover(%u), waterwalk(%u, ghost %u), fly/can_fly(%u), mount: %u (%u)\n"
            "    transportInfo: transportServer(%u), transportClient(%u, next: %u), vehicleID(%u), vehicleKitID(%u)\n"
            "    map %u \"%s\"\n"
            "    source: zone %u \"%s\" area %u \"%s\" %.2f, %.2f, %.2f\n"
            "    dest:   zone %u \"%s\" area %u \"%s\" %.2f, %.2f, %.2f",
            m_player->GetName(), GetOpcodeNameForLogging(static_cast<OpcodeClient>(opcode)).c_str(),
            moveDist, allowedDist, moveDistZ, allowedDistZ, speed,
            m_player->m_mover->GetUnitMovementFlags(), movementInfo.GetMovementFlags(),
            hasHoverAuras, hasWWAuras, !m_player->isAlive(), hasFlyingAuras, m_player->IsMounted(), m_player->IsMounted() ? m_player->GetMountID() : 0,
            m_player->GetTransport() ? 1 : 0, m_player->m_movementInfo.hasTransportData, movementInfo.hasTransportData, vehicleID, vehicleKitID,
            m_player->GetMapId(), mapName,
            m_player->GetCurrentZoneID(), srcZoneName, m_player->GetCurrentAreaID(), srcAreaName,
            m_player->GetPositionX(), m_player->GetPositionY(), m_player->GetPositionZ(),
            destZoneId, destZoneName, destAreaId, destAreaName,
            movementInfo.Pos.GetPositionX(), movementInfo.Pos.GetPositionY(), movementInfo.Pos.GetPositionZ());
        return false;
    }

    return true;
}

// copy from client CMovement_C::GetBaseSpeed(CMovement_C* this, int a2)
float AnticheatMgr::GetBaseSpeed(Unit* mover)
{
    //if (!(mover->GetUnitMovementFlags() & 0x60000F))
        //return 0.0f;

    if (mover->HasUnitMovementFlag(MOVEMENTFLAG_FLYING))
    {
        if (mover->HasUnitMovementFlag(MOVEMENTFLAG_BACKWARD) && mover->GetSpeed(MOVE_FLIGHT) >= mover->GetSpeed(MOVE_FLIGHT_BACK))
            return mover->GetSpeed(MOVE_FLIGHT_BACK);
        return mover->GetSpeed(MOVE_FLIGHT);
    }
    if (mover->HasUnitMovementFlag(MOVEMENTFLAG_SWIMMING))
    {
        if (mover->HasUnitMovementFlag(MOVEMENTFLAG_BACKWARD) && mover->GetSpeed(MOVE_SWIM) >= mover->GetSpeed(MOVE_SWIM_BACK))
            return mover->GetSpeed(MOVE_SWIM_BACK);
        return mover->GetSpeed(MOVE_SWIM);
    }
    if (mover->HasUnitMovementFlag(MOVEMENTFLAG_WALKING))
    {
        if (mover->GetSpeed(MOVE_RUN) > mover->GetSpeed(MOVE_WALK))
            return mover->GetSpeed(MOVE_WALK);
    }
    else if (mover->HasUnitMovementFlag(MOVEMENTFLAG_BACKWARD) && mover->GetSpeed(MOVE_RUN) >= mover->GetSpeed(MOVE_RUN_BACK))
        return mover->GetSpeed(MOVE_RUN_BACK);

    return mover->GetSpeed(MOVE_RUN);
}

void AnticheatMgr::HandleSpecialLogs(Unit* victim, uint32 damage, DamageEffectType damagetype, SpellInfo const* spellProto)
{
    // extended log
    if (nextScannedTime < getMSTime())
    {
        nextScannedTime = getMSTime() + 10 * MINUTE * IN_MILLISECONDS;

        sLog->outWarden("AnticheatSpecial: Player %s (GUID: %u) =======BEGIN======= scanned for detect cheat equipment", m_player->GetName(), m_player->GetGUIDLow());

        uint32 artifactLevel, artifactItemLevel = 0;
        if (Item* artifact = m_player->GetArtifactWeapon())
        {
            artifactLevel = artifact->GetTotalPurchasedArtifactPowers();
            artifactItemLevel = artifact->GetItemLevel(artifact->GetOwnerLevel());
        }

        // check cheat items in bank
        uint32 playerItemLevelTotal = m_player->GetAverageItemLevelTotal(true, true);

        sLog->outWarden("AnticheatSpecial: Player %s (GUID: %u) =======FINISH====== scanned for detect cheat equipment\n"
            "                    equipInfo: artifactLevel %u, artifactWeaponLvl %u, totalItemLevel %u",
            m_player->GetName(), m_player->GetGUIDLow(), artifactLevel, artifactItemLevel, playerItemLevelTotal);
    }

    float crit = m_player->GetFloatValue(PLAYER_FIELD_CRIT_PERCENTAGE);
    float mastery = m_player->GetFloatValue(PLAYER_FIELD_MASTERY) + m_player->GetRatingBonusValue(CR_MASTERY);
    float hasteM = m_player->GetRatingBonusValue(CR_HASTE_MELEE);
    float hasteR = m_player->GetRatingBonusValue(CR_HASTE_RANGED);
    float hasteS = m_player->GetRatingBonusValue(CR_HASTE_SPELL);
    float vers = m_player->GetFloatValue(PLAYER_FIELD_VERSATILITY);

    // impossible, but check it...
    if (!victim)
        return;

    std::string damageDesc = damage >= 6000000 ? "cheat" : "suspicious";

    sLog->outWarden("AnticheatSpecial: Player %s (GUID: %u) dealed %s damage %u at victim with entry %u (healthPct %f) on map %u\n"
        "                    damageInfo: spell %u, damageType %u, totalDoneMod(player) %.2f, totalTakenMod(victim) %.2f\n"
        "                    stats: class %u, str %.2f, agi %.2f, int %.2f, critRating %.2f, masteryRating %.2f, hasteRating %.2f (%.2f %.2f), versRating %.2f",
        m_player->GetName(), m_player->GetGUIDLow(), damageDesc.c_str(), damage, victim->GetEntry(), victim->GetHealthPct(), m_player->GetMapId(),
        spellProto ? spellProto->Id : 0, uint8(damagetype), m_player->GetTotalAuraMultiplier(SPELL_AURA_MOD_DAMAGE_PERCENT_DONE), victim->GetTotalAuraMultiplier(SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN),
        m_player->getClass(), m_player->GetTotalStatValue(STAT_STRENGTH), m_player->GetTotalStatValue(STAT_AGILITY), m_player->GetTotalStatValue(STAT_INTELLECT), crit, mastery, hasteM, hasteR, hasteS, vers);
}

void AnticheatMgr::SaveCheatCommonData()
{
    if (!transmCheatUseCount && !cheatLootCount && !transmCooldownEnd)
        return;

    CharacterDatabase.PQuery("REPLACE INTO character_cheat_common VALUES ('%u', '%u', '%u', '%u')", m_player->GetGUIDLow(), transmCheatUseCount, transmCooldownEnd, cheatLootCount);
}

void AnticheatMgr::LoadCheatCommonData()
{
    QueryResult result = CharacterDatabase.PQuery("SELECT transmCheatUseCount, transmCooldownEnd, cheatLootCount FROM character_cheat_common WHERE guid = '%u'", m_player->GetGUIDLow());

    if (result)
    {
        Field * fetch = result->Fetch();
        uint32 transmCheatUseCount_db = fetch[0].GetUInt32();
        uint32 transmCooldownEnd_db = fetch[1].GetUInt32();
        uint32 cheatLootCount_db = fetch[2].GetUInt32();

        transmCheatUseCount = transmCheatUseCount_db;
        cheatLootCount = cheatLootCount_db;
        time_t now = time(nullptr);
        if (transmCooldownEnd_db < now)
            transmCooldownEnd = 0;
        else
            transmCooldownEnd = transmCooldownEnd_db;
    }
}

void AnticheatMgr::HandleAddTransmuteCooldown(uint32 spellID, double cooldownEnd)
{
    if (spellID == 213255 || spellID == 213248 || spellID == 213257 || spellID == 213251 || spellID == 213254 || spellID == 213252 || spellID == 213249 || spellID == 213250 || spellID == 213253 || spellID == 213256)
    {
        time_t now = time(nullptr);
        if (!transmCooldownEnd || transmCooldownEnd < now)
        {
            transmCooldownEnd = now + cooldownEnd;
            sLog->outWarden("AnticheatSpecial: Player %s (GUID: %u) used transmute spell %u legal and set cooldown time %u", m_player->GetName(), m_player->GetGUIDLow(), spellID, transmCooldownEnd);
        }
        else
        {
            ++transmCheatUseCount;

            if (transmCheatUseCount < 5)
                sLog->outWarden("AnticheatSpecial: Player %s (GUID: %u) used transmute spell %u illegal, useCount %u", m_player->GetName(), m_player->GetGUIDLow(), spellID, transmCheatUseCount);
            else
            {
                sLog->outWarden("AnticheatSpecial: Player %s (GUID: %u) used transmute spell %u illegal, useCount over 5 times (%u), banned for exploiting", m_player->GetName(), m_player->GetGUIDLow(), spellID, transmCheatUseCount);

                /*bool removeMoney = false;
                uint32 itemCount = transmCheatUseCount * 10;
                uint32 banTime = 3 * DAY;
                if (transmCheatUseCount >= 200)
                {
                    banTime = 60 * DAY;
                    removeMoney = true;
                    itemCount = 250000;
                }
                else if (transmCheatUseCount >= 90)
                {
                    banTime = 30 * DAY;
                    removeMoney = true;
                    itemCount = 250000;
                }
                else if (transmCheatUseCount >= 50)
                    banTime = 14 * DAY;
                else if (transmCheatUseCount >= 25)
                    banTime = 7 * DAY;

                m_player->DestroyItemCount(124106, itemCount, true); // felwort
                m_player->DestroyItemCount(123919, itemCount, true); // felslate
                m_player->DestroyItemCount(124115, itemCount, true); // stormscale
                m_player->DestroyItemCount(124113, itemCount, true); // stonehide leather
                m_player->DestroyItemCount(123918, itemCount, true); // leystone ore
                m_player->DestroyItemCount(124437, itemCount, true); // shaldorei silk
                m_player->DestroyItemCount(124101, itemCount, true); // aethril
                m_player->DestroyItemCount(124102, itemCount, true); // dreamleaf
                m_player->DestroyItemCount(124103, itemCount, true); // foxflower
                m_player->DestroyItemCount(124104, itemCount, true); // fjarnskaggl
                m_player->DestroyItemCount(124105, itemCount, true); // starlight rose
                m_player->DestroyItemCount(128304, itemCount, true); // yseralline seed

                if (removeMoney)
                    m_player->SetMoney(0);

                transmCheatUseCount = 0;

                std::stringstream duration;
                duration << banTime << "s";

                std::string accountName;
                AccountMgr::GetName(m_player->GetSession()->GetAccountId(), accountName);
                std::stringstream banReason;
                banReason << "Exploitation of game mechanics (transmutation bug) (Realm: " << sWorld->GetRealmName() << ")";

                sWorld->BanAccount(BAN_ACCOUNT, accountName, duration.str(), banReason.str(), "uWoW Anticheat");*/
            }
        }
    }
}

void AnticheatMgr::HandleLoot(ObjectGuid sourceGuid, uint32 itemEntry)
{
    bool cheating = true;

    if (itemEntry != 124106 && itemEntry != 124444 && itemEntry != 124116)
        return;

    uint32 felwortWQ[15] = { 41524, 41515, 41523, 41522, 41225, 41511, 41512, 41513, 41514, 41521, 41516, 41517, 41518, 41519, 41520 };
    if (itemEntry == 124106)
    {
        if (!sourceGuid.IsGameObject())
            cheating = false;
        else
        {
            for (uint8 i = 0; i < 15; ++i)
            {
                if (m_player->FindQuestSlot(felwortWQ[i]) != MAX_QUEST_LOG_SIZE)
                {
                    cheating = false;
                    break;
                }
            }
        }
    }

    uint32 brimstoneWQ[15] = { 41210, 41487, 41208, 41481, 41482, 41483, 41484, 41486, 41209, 41488, 41489, 41490, 41491, 41492, 41493 };
    if (itemEntry == 124444)
    {
        if (!sourceGuid.IsGameObject() && !sourceGuid.IsCreature())
            cheating = false;
        else
        {
            for (uint8 i = 0; i < 15; ++i)
            {
                if (m_player->FindQuestSlot(brimstoneWQ[i]) != MAX_QUEST_LOG_SIZE)
                {
                    cheating = false;
                    break;
                }
            }
        }
    }

    uint32 felhideWQ[15] = { 41563, 41567, 41239, 41561, 41562, 41564, 41565, 41566, 41560, 41568, 41569, 41570, 41571, 41572, 41573 };
    if (itemEntry == 124116)
    {
        if (!sourceGuid.IsCreature())
            cheating = false;
        else
        {
            for (uint8 i = 0; i < 15; ++i)
            {
                if (m_player->FindQuestSlot(felhideWQ[i]) != MAX_QUEST_LOG_SIZE)
                {
                    cheating = false;
                    break;
                }
            }
        }
    }

    if (cheating)
    {
        ++cheatLootCount;
        sLog->outWarden("AnticheatSpecial: Player %s (GUID: %u) looting item %u illegal, lootCountTotal %u", m_player->GetName(), m_player->GetGUIDLow(), itemEntry, cheatLootCount);
    }
}