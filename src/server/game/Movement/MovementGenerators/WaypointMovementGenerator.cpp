/*
 * Copyright (C) 2008-2015 TrinityCore <http://www.trinitycore.org/>
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

#include "WaypointMovementGenerator.h"
#include "ObjectMgr.h"
#include "Transport.h"
#include "MapManager.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "CreatureGroups.h"
#include "Player.h"
#include "PlayerDefines.h"
#include "MoveSplineInit.h"
#include "MoveSpline.h"

void WaypointMovementGenerator<Creature>::LoadPath(Creature &creature)
{
    if (!path_id)
    {
        path_id = creature.GetWaypointPath();
        i_path = sWaypointMgr->GetPath(path_id);
        
        if (creature.GetEntry() == 120132 || creature.GetEntry() ==  120131) // hack
            forceTPToStart = true;
    }
    else
        i_path = sWaypointMgr->GetPathScript(path_id);

    if (!i_path)
    {
        // No movement found for entry
        //sLog->outError(LOG_FILTER_SQL, "WaypointMovementGenerator::LoadPath: creature %s (Entry: %u GUID: %u) doesn't have waypoint path id: %u", creature.GetName(), creature.GetEntry(), creature.GetGUIDLow(), path_id);
        return;
    }

    StartMoveNow(creature);
}

void WaypointMovementGenerator<Creature>::DoInitialize(Creature &creature)
{
    LoadPath(creature);
    creature.AddUnitState(UNIT_STATE_ROAMING|UNIT_STATE_ROAMING_MOVE);
}

void WaypointMovementGenerator<Creature>::DoFinalize(Creature &creature)
{
    creature.ClearUnitState(UNIT_STATE_ROAMING|UNIT_STATE_ROAMING_MOVE);
    creature.SetWalk(false);
}

void WaypointMovementGenerator<Creature>::DoReset(Creature &creature)
{
    creature.AddUnitState(UNIT_STATE_ROAMING|UNIT_STATE_ROAMING_MOVE);
    StartMoveNow(creature);
}

void WaypointMovementGenerator<Creature>::OnArrived(Creature& creature)
{
    if (!i_path || i_path->empty() || i_currentNode >= i_path->size())
        return;
    if (m_isArrivalDone)
        return;

    m_isArrivalDone = true;
    WaypointData const* currentPath = i_path->at(i_currentNode);

    if (currentPath->event_id && roll_chance_i(currentPath->event_chance))
    {
        sLog->outDebug(LOG_FILTER_MAPSCRIPTS, "Creature movement start script %u at point %u for %u.", currentPath->event_id, i_currentNode, creature.GetGUID().GetCounter());
        creature.ClearUnitState(UNIT_STATE_ROAMING_MOVE);
        creature.GetMap()->ScriptsStart(sWaypointScripts, currentPath->event_id, &creature, nullptr);
    }

    // Inform script
    MovementInform(creature);
    creature.UpdateWaypointID(i_currentNode);

    // For WP from sniff
    if (currentPath->delay_chance && roll_chance_i(currentPath->delay_chance))
    {
        creature.ClearUnitState(UNIT_STATE_ROAMING_MOVE);
        Stop(urand(1000, 10000));
    }
    else if (currentPath->delay)
    {
        creature.ClearUnitState(UNIT_STATE_ROAMING_MOVE);
        Stop(currentPath->delay);
    }
}

bool WaypointMovementGenerator<Creature>::StartMove(Creature &creature)
{
    if (!i_path || i_path->empty())
        return false;

    if (Stopped())
        return true;

    bool transportPath = creature.GetTransport() != nullptr;
    bool waitingInLast = false;

    if (m_isArrivalDone)
    {
        if ((i_currentNode == i_path->size() - 1) && !repeating) // If that's our last waypoint
        {
            float x = i_path->at(i_currentNode)->x + randomMoveX;
            float y = i_path->at(i_currentNode)->y + randomMoveY;
            float z = i_path->at(i_currentNode)->z;
            float o = creature.GetOrientation();

            if (!transportPath)
                creature.SetHomePosition(x, y, z, o);
            else
            {
                if (Transport* trans = creature.GetTransport())
                {
                    o -= trans->GetOrientation();
                    creature.SetTransportHomePosition(x, y, z, o);
                    trans->CalculatePassengerPosition(x, y, z, &o);
                    creature.SetHomePosition(x, y, z, o);
                }
                else
                    transportPath = false;
                // else if (vehicle) - this should never happen, vehicle offsets are const
            }

            creature.GetMotionMaster()->Initialize();
            return false;
        }

        if (i_currentNode == 0 && goBeak)
        {
            goBeak = false;
            waitingInLast = true;
        }

        if (goBeak)
            i_currentNode = (i_currentNode-1) % i_path->size();
        else
            i_currentNode = (i_currentNode+1) % i_path->size();

        // Revers go
        if ((i_currentNode == i_path->size() - 1) && !goBeak)
        {
            if (!forceTPToStart)
                goBeak = true;
            else
            {
                i_currentNode = 0;
                WaypointData const* node = i_path->at(i_currentNode);
                creature.NearTeleportTo(node->x, node->y, node->z, 0.0f);
                return true;
            }
        }
    }

    if (i_currentNode >= i_path->size())
        return false;

    WaypointData const* node = i_path->at(i_currentNode);

    m_isArrivalDone = false;

    creature.AddUnitState(UNIT_STATE_ROAMING_MOVE);

    Movement::Location formationDest(node->x, node->y, node->z, 0.0f);
    Movement::MoveSplineInit init(creature);

    //! If creature is on transport, we assume waypoints set in DB are already transport offsets
    if (transportPath)
    {
        init.DisableTransportPathTransformations();
        if (TransportBase* trans = creature.GetDirectTransport())
            trans->CalculatePassengerPosition(formationDest.x, formationDest.y, formationDest.z, &formationDest.orientation);
    }

    //! Do not use formationDest here, MoveTo requires transport offsets due to DisableTransportPathTransformations() call
    //! but formationDest contains global coordinates

    init.MoveTo(node->x + randomMoveX, node->y + randomMoveY, node->z);

    //! Accepts angles such as 0.00001 and -0.00001, 0 must be ignored, default value in waypoint table
    if (node->orientation && node->delay)
        init.SetFacing(node->orientation);

    if (node->speed > 0)
        init.SetVelocity(node->speed);

    init.SetWalk(!node->run);
    //switch (node->move_type)
    //{
    //    case WAYPOINT_MOVE_TYPE_LAND:
    //        init.SetAnimation(Movement::ToGround);
    //        break;
    //    case WAYPOINT_MOVE_TYPE_TAKEOFF:
    //        init.SetAnimation(Movement::ToFly);
    //        break;
    //    case WAYPOINT_MOVE_TYPE_RUN:
    //        init.SetWalk(false);
    //        break;
    //    case WAYPOINT_MOVE_TYPE_WALK:
    //        init.SetWalk(true);
    //        break;
    //}

    init.Launch();

    // if (waitingInLast) // Not need
    // {
        // creature.ClearUnitState(UNIT_STATE_ROAMING_MOVE);
        // Stop(10000);
    // }

    //Call for creature group update
    if (!creature.IsPlayer())
        if (creature.GetFormation() && creature.GetFormation()->getLeader() == &creature)
            creature.GetFormation()->LeaderMoveTo(node->x, node->y, node->z);

    ////Call for creature group update
    //if (creature->GetFormation() && creature->GetFormation()->getLeader() == creature)
    //{
    //    creature->SetWalk(node->move_type != WAYPOINT_MOVE_TYPE_RUN);
    //    creature->GetFormation()->LeaderMoveTo(formationDest.x, formationDest.y, formationDest.z);
    //}

    return true;
}

bool WaypointMovementGenerator<Creature>::DoUpdate(Creature &creature, const uint32 &diff)
{
    // Waypoint movement can be switched on/off
    // This is quite handy for escort quests and other stuff
    if (creature.HasUnitState(UNIT_STATE_NOT_MOVE))
    {
        creature.ClearUnitState(UNIT_STATE_ROAMING_MOVE);
        return true;
    }
    // prevent a crash at empty waypoint path.
    if (!i_path || i_path->empty())
        return false;

    if (Stopped())
    {
        if (CanMove(diff))
            return StartMove(creature);
    }
    else
    {
        // Set home position at place on waypoint movement.
        if (!creature.GetTransGUID())
            creature.SetHomePosition(creature.GetPosition());

        if (creature.IsStopped())
            Stop(STOP_TIME_FOR_PLAYER);
        else if (creature.movespline->Finalized())
        {
            OnArrived(creature);
            return StartMove(creature);
        }
    }
     return true;
 }

void WaypointMovementGenerator<Creature>::MovementInform(Creature &creature)
{
    if (creature.AI())
    {
        creature.AI()->MovementInform(WAYPOINT_MOTION_TYPE, i_currentNode);

        if (i_currentNode == i_path->size() - 1)
            creature.AI()->LastWPReached();
    }
}

bool WaypointMovementGenerator<Creature>::GetResetPos(Creature&, float& x, float& y, float& z)
{
    // prevent a crash at empty waypoint path.
    if (!i_path || i_path->empty() || i_currentNode >= i_path->size())
        return false;

    const WaypointData* node = i_path->at(i_currentNode);
    x = node->x; y = node->y; z = node->z;
    return true;
}


//----------------------------------------------------//

uint32 FlightPathMovementGenerator::GetPathAtMapEnd() const
{
    if (i_currentNode >= i_path.size())
        return i_path.size();

    uint32 curMapId = i_path[i_currentNode]->ContinentID;
    for (size_t i = i_currentNode; i < i_path.size(); ++i)
        if (i_path[i]->ContinentID != curMapId)
            return i;

    return i_path.size();
}

#define SKIP_SPLINE_POINT_DISTANCE_SQ (40.0f * 40.0f)

bool IsNodeIncludedInShortenedPath(TaxiPathNodeEntry const* p1, TaxiPathNodeEntry const* p2)
{
    return p1->ContinentID != p2->ContinentID || std::pow(p1->Loc.X - p2->Loc.X, 2) + std::pow(p1->Loc.Y - p2->Loc.Y, 2) > SKIP_SPLINE_POINT_DISTANCE_SQ;
}

void FlightPathMovementGenerator::LoadPath(Player& player, uint32 startNode /*= 0*/)
{
    i_path.clear();
    i_currentNode = startNode;
    _pointsForPathSwitch.clear();
    std::deque<uint32> const& taxi = player.m_taxi.GetPath();
    for (size_t src = 0, dst = 1; dst < taxi.size(); src = dst++)
    {
        uint32 path, cost;
        sObjectMgr->GetTaxiPath(taxi[src], taxi[dst], path, cost);
        if (path > sTaxiPathNodesByPath.size())
            return;

        TaxiPathNodeList const& nodes = sTaxiPathNodesByPath[path];
        if (!nodes.empty())
        {
            TaxiPathNodeEntry const* start = nodes[0];
            TaxiPathNodeEntry const* end = nodes[nodes.size() - 1];
            bool passedPreviousSegmentProximityCheck = false;
            for (size_t i = 0; i < nodes.size(); ++i)
            {
                if (passedPreviousSegmentProximityCheck || !src || i_path.empty() || IsNodeIncludedInShortenedPath(i_path.back(), nodes[i]))
                {
                    if ((!src || (IsNodeIncludedInShortenedPath(start, nodes[i]) && i >= 2)) &&
                        (dst == taxi.size() - 1 || (IsNodeIncludedInShortenedPath(end, nodes[i]) && i < nodes.size() - 1)))
                    {
                        passedPreviousSegmentProximityCheck = true;
                        i_path.push_back(nodes[i]);
                    }
                }
                else
                {
                    i_path.pop_back();
                    --_pointsForPathSwitch.back().PathIndex;
                }
            }
        }

        _pointsForPathSwitch.push_back({ uint32(i_path.size() - 1), int32(cost) });
    }
}

void FlightPathMovementGenerator::DoInitialize(Player &player)
{
    Reset(player);
    InitEndGridInfo();
}

void FlightPathMovementGenerator::DoFinalize(Player& player)
{
    // remove flag to prevent send object build movement packets for flight state and crash (movement generator already not at top of stack)
    player.ClearUnitState(UNIT_STATE_IN_FLIGHT);

    player.Dismount();
    player.RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_REMOVE_CLIENT_CONTROL | UNIT_FLAG_TAXI_FLIGHT);

    if (player.m_taxi.empty())
    {
        player.getHostileRefManager().setOnlineOfflineState(true);
        // update z position to ground and orientation for landing point
        // this prevent cheating with landing  point at lags
        // when client side flight end early in comparison server side
        player.StopMoving();
    }

    player.RemoveFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_TAXI_BENCHMARK);
    player.ResummonPetTemporaryUnSummonedIfAny();
    player.SummonLastSummonedBattlePet();
}

void FlightPathMovementGenerator::DoReset(Player & player)
{
    float flightSpeed = 32.0f;

    player.UnsummonPetTemporaryIfAny();
    player.UnsummonCurrentBattlePetIfAny(true);
    player.getHostileRefManager().setOnlineOfflineState(false);
    player.AddUnitState(UNIT_STATE_IN_FLIGHT);
    player.SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_REMOVE_CLIENT_CONTROL | UNIT_FLAG_TAXI_FLIGHT);

    Movement::MoveSplineInit init(player);
    uint32 end = GetPathAtMapEnd();
    for (uint32 i = GetCurrentNode(); i < end; ++i)
    {
        G3D::Vector3 vertice(i_path[i]->Loc.X, i_path[i]->Loc.Y, i_path[i]->Loc.Z);
        init.Path().push_back(vertice);
    }
    init.SetFirstPointId(GetCurrentNode());
    init.SetFly();
    init.SetSmooth();
    init.SetUncompressed();
    init.SetWalk(true);
    init.SetVelocity(flightSpeed * player.GetTotalAuraMultiplier(SPELL_AURA_MOD_FLY_PATH_SPEED));
    init.Launch();
}

bool FlightPathMovementGenerator::DoUpdate(Player &player, const uint32& /*diff*/)
{
    uint32 pointId = (uint32)player.movespline->currentPathIdx();
    if (pointId > i_currentNode)
    {
        bool departureEvent = true;
        do
        {
            if (i_path.size() == i_currentNode) // Max point
                return false;

            DoEventIfAny(player, *i_path[i_currentNode], departureEvent);
            while (!_pointsForPathSwitch.empty() && _pointsForPathSwitch.front().PathIndex <= i_currentNode)
            {
                _pointsForPathSwitch.pop_front();
                player.m_taxi.NextTaxiDestination();
                if (!_pointsForPathSwitch.empty())
                {
                    player.UpdateAchievementCriteria(CRITERIA_TYPE_GOLD_SPENT_FOR_TRAVELLING, _pointsForPathSwitch.front().Cost);
                    player.ModifyMoney(-_pointsForPathSwitch.front().Cost);
                }
            }

            if (pointId == i_currentNode)
                break;

            if (i_currentNode == _preloadTargetNode)
                PreloadEndGrid();
            i_currentNode += (uint32)departureEvent;
            departureEvent = !departureEvent;
        }
        while (true);
    }

    return i_currentNode < (i_path.size() - 1);
}

void FlightPathMovementGenerator::SetCurrentNodeAfterTeleport()
{
    if (i_path.empty() || i_currentNode >= i_path.size())
        return;

    uint32 map0 = i_path[i_currentNode]->ContinentID;
    for (size_t i = i_currentNode + 1; i < i_path.size(); ++i)
    {
        if (i_path[i]->ContinentID != map0)
        {
            i_currentNode = i;
            return;
        }
    }
}

void FlightPathMovementGenerator::DoEventIfAny(Player& player, TaxiPathNodeEntry const& node, bool departure)
{
    if (uint32 eventid = departure ? node.DepartureEventID : node.ArrivalEventID)
    {
        sLog->outDebug(LOG_FILTER_MAPSCRIPTS, "Taxi %s event %u of node %u of path %u for player %s", departure ? "departure" : "arrival", eventid, node.NodeIndex, node.PathID, player.GetName());
        player.GetMap()->ScriptsStart(sEventScripts, eventid, &player, &player);
    }
}

bool FlightPathMovementGenerator::GetResetPos(Player&, float& x, float& y, float& z)
{
    TaxiPathNodeEntry const* node = i_path[i_currentNode];
    x = node->Loc.X;
    y = node->Loc.Y;
    z = node->Loc.Z;
    return true;
}

void FlightPathMovementGenerator::InitEndGridInfo()
{
    /*! Storage to preload flightmaster grid at end of flight. For multi-stop flights, this will
       be reinitialized for each flightmaster at the end of each spline (or stop) in the flight. */
    uint32 nodeCount = i_path.size();        //! Number of nodes in path.
    _endMapId = i_path[nodeCount - 1]->ContinentID; //! MapId of last node
    _preloadTargetNode = nodeCount - 3;
    _endGridX = i_path[nodeCount - 1]->Loc.X;
    _endGridY = i_path[nodeCount - 1]->Loc.Y;
    _endGridZ = i_path[nodeCount - 1]->Loc.Z;
}

void FlightPathMovementGenerator::PreloadEndGrid()
{
    MapEntry const* mapEntry = sMapStore.LookupEntry(_endMapId);
    if (!mapEntry)
        return;

    // used to preload the final grid where the flightmaster is
    Map* endMap = nullptr;
    if (mapEntry->CanCreatedZone())
    {
        Map* endMap = sMapMgr->CreateBaseMap(_endMapId);
        uint32 instanceId = endMap->GetZoneId(_endGridX, _endGridY, _endGridZ);
        endMap = sMapMgr->FindMap(_endMapId, instanceId);
    }
    else
        endMap = sMapMgr->FindBaseNonInstanceMap(_endMapId);

    // Load the grid
    if (endMap)
    {
        sLog->outInfo(LOG_FILTER_GENERAL, "Preloading rid (%f, %f) for map %u at node index %u/%u", _endGridX, _endGridY, _endMapId, _preloadTargetNode, (uint32)(i_path.size() - 1));
        endMap->LoadGrid(_endGridX, _endGridY);
    }
    else
        sLog->outInfo(LOG_FILTER_GENERAL, "Unable to determine map to preload flightmaster grid");
}
