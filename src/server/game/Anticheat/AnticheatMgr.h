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

#ifndef __ANTICHEAT_MGR_H
#define __ANTICHEAT_MGR_H

#include "Common.h"
#include "SharedDefines.h"
#include "Unit.h"

class Player;
namespace Movement { class MoveSpline; }

class AnticheatMgr
{
public:
    explicit AnticheatMgr(Player* owner);
    ~AnticheatMgr() {}

    void Update(uint32 diff);

    bool MovementChecks(uint16 opcode, MovementInfo const& movementInfo);
    bool CanAllowUpdatePosition(uint16 opcode, MovementInfo const& movementInfo);

    bool ValidateMovementFlags(MovementInfo const& movementInfo);

    float GetBaseSpeed(Unit* mover);

    void SetLastClientMoveTime(uint32 time) { lastClientMoveTime = time; }
    uint32 GetLastClientMoveTime() { return lastClientMoveTime; }

    void SetLastServerMoveTime(uint32 time) { lastServerMoveTime = time; }
    uint32 GetLastServerMoveTime() { return lastServerMoveTime; }

    void SetLastHeartbeatTime(uint32 time) { lastHeartbeatTime = time; }
    uint32 GetLastHeartbeatTime() { return lastHeartbeatTime; }

    void SetFallData(const Position& startPos, float vcos, float vsin, float speedXY, float speedZ)
    {
        fall.StartPos.Pos = startPos;
        fall.Direction.Pos = Position(vcos, vsin);
        fall.HorizontalSpeed = speedXY;
        fall.JumpVelocity = speedZ;
    }
    void ResetFallData() { fall.Reset(); }

    // temp
    uint32 nextScannedTime;
    void HandleSpecialLogs(Unit* victim, uint32 damage, DamageEffectType damagetype, SpellInfo const* spellProto);
    void SaveCheatCommonData();
    void LoadCheatCommonData();
    void HandleAddTransmuteCooldown(uint32 spellID, double cooldownEnd);
    void HandleLoot(ObjectGuid sourceGuid, uint32 itemEntry);
    uint32 transmCooldownEnd;
    uint32 transmCheatUseCount;
    uint32 cheatLootCount;

private:
    // time
    uint32 lastHeartbeatTime;
    uint32 lastClientMoveTime;
    uint32 lastServerMoveTime;
    // player
    Player* m_player;
    // test data
    float clientSpeeds[MAX_MOVE_TYPE];
    uint8 jumpCount;
    bool isSwimming;

    struct lastFallData
    {
        void Reset()
        {
            StartPos.Pos.Relocate(0.0f, 0.0f, 0.0f, 0.0f);
            Direction.Pos.Relocate(0.0f, 0.0f);
            HorizontalSpeed = 0.0f;
            JumpVelocity = 0.0f;
            fallTime = 0;
        }

        TaggedPosition<Position::XYZO> StartPos;
        TaggedPosition<Position::XY> Direction;
        float HorizontalSpeed;
        float JumpVelocity;
        uint32 fallTime;
    } fall;
};

#endif