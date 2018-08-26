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

#include "CreatureAI.h"
#include "CreatureAIImpl.h"
#include "Creature.h"
#include "World.h"
#include "SpellMgr.h"
#include "Vehicle.h"
#include "Log.h"
#include "Player.h"
#include "CreatureTextMgr.h"
#include "Group.h"

AISpellInfoType* UnitAI::AISpellInfo;

EventMap::EventMap(): _time(0), _phase(0)
{
}

uint32 EventMap::GetTimer() const
{
    return _time;
}

void EventMap::Reset()
{
    clear();
    _time = 0;
    _phase = 0;
}

void EventMap::Update(uint32 time)
{
    _time += time;
}

uint32 EventMap::GetPhaseMask() const
{
    return _phase;
}

bool EventMap::IsInPhase(uint8 phase)
{
    return phase <= 8 && (!phase || _phase & (1 << (phase - 1)));
}

bool EventMap::Empty() const
{
    return empty();
}

void EventMap::SetPhase(uint32 phase)
{
    if (!phase)
        _phase = 0;
    else if (phase <= 8)
        _phase = (1 << (phase - 1));
}

void EventMap::AddPhase(uint8 phase)
{
    if (phase && phase <= 8)
        _phase |= (1 << (phase - 1));
}

void EventMap::RemovePhase(uint8 phase)
{
    if (phase && phase <= 8)
        _phase &= ~(1 << (phase - 1));
}

void EventMap::ScheduleEvent(uint32 eventId, Seconds time, uint32 group, uint32 phase)
{
    ScheduleEvent(eventId, time.count(), group, phase);
}

void EventMap::ScheduleEvent(uint32 eventId, Minutes time, uint32 group, uint32 phase)
{
    ScheduleEvent(eventId, time.count(), group, phase);
}

void EventMap::ScheduleEvent(uint32 eventId, uint32 time, uint32 group, uint32 phase)
{
    uint32 data = 0;
    if (group && group <= 8)
        data |= (1 << (group + 15));
    if (phase && phase <= 8)
        data |= (1 << (phase + 23));
    time += _time;
    const_iterator itr = find(time);
    while (itr != end())
    {
        ++time;
        itr = find(time);
    }

    insert(std::make_pair(time, MAKE_PAIR64(eventId, data)));
}

void EventMap::RescheduleEvent(uint32 eventId, uint32 time, uint32 groupId, uint32 phase)
{
    CancelEvent(eventId);
    ScheduleEvent(eventId, time, groupId, phase);
}

void EventMap::RepeatEvent(uint32 time)
{
    if (empty())
        return;
    uint64 eventData = begin()->second;
    erase(begin());
    time += _time;
    const_iterator itr = find(time);
    while (itr != end())
    {
        ++time;
        itr = find(time);
    }
    insert(std::make_pair(time, eventData));
}

void EventMap::PopEvent()
{
    erase(begin());
}

uint32 EventMap::ExecuteEvent()
{
    while (!empty())
    {
        if (begin()->first > _time)
            return 0;

        uint32 data = PAIR64_HIPART(begin()->second);
        if (_phase && (data & 0xFF000000) && !((data >> 24) & _phase))
            erase(begin());
        else
        {
            uint32 eventId = PAIR64_LOPART(begin()->second);
            erase(begin());
            return eventId;
        }
    }
    return 0;
}

uint32 EventMap::GetEvent()
{
    while (!empty())
    {
        if (begin()->first > _time)
            return 0;

        uint32 data = PAIR64_HIPART(begin()->second);
        if (_phase && (data & 0xFF000000) && !((data >> 24) & _phase))
            erase(begin());
        else
            return PAIR64_LOPART(begin()->second);
    }
    return 0;
}

void EventMap::DelayEvent(uint32 eventID, uint32 delay)
{
    uint32 nextTime = _time + delay;
    for (iterator itr = begin(); itr != end() && itr->first < nextTime;)
    {
        if (PAIR64_LOPART(itr->second) == eventID)
        {
            uint32 data = PAIR64_HIPART(begin()->second);
            ScheduleEvent(eventID, itr->first - _time + delay, data >> 24, data >> 16);
            erase(itr);
            itr = begin();
        }
        else
            ++itr;
    }
}

void EventMap::DelayEvents(uint32 delay)
{
    if (delay < _time)
        _time -= delay;
    else
        _time = 0;
}

void EventMap::DelayEvents(uint32 delay, uint32 groupId)
{
    uint32 nextTime = _time + delay;
    uint32 groupMask = (1 << (groupId + 16));
    for (iterator itr = begin(); itr != end() && itr->first < nextTime;)
    {
        uint32 data = PAIR64_HIPART(itr->second);
        if (data & groupMask)
        {
            ScheduleEvent(PAIR64_LOPART(itr->second), itr->first - _time + delay, data >> 24, data >> 16);
            erase(itr);
            itr = begin();
        }
        else
            ++itr;
    }
}

void EventMap::CancelEvent(uint32 eventId)
{
    for (iterator itr = begin(); itr != end();)
    {
        if (eventId == PAIR64_LOPART(itr->second))
        {
            erase(itr);
            itr = begin();
        }
        else
            ++itr;
    }
}

void EventMap::CancelEventGroup(uint32 groupId)
{
    uint32 groupMask = (1 << (groupId + 16));
    for (iterator itr = begin(); itr != end();)
    {
        uint32 data = PAIR64_HIPART(itr->second);
        if (data & groupMask)
        {
            erase(itr);
            itr = begin();
        }
        else
            ++itr;
    }
}

uint32 EventMap::GetNextEventTime(uint32 eventId) const
{
    for (const_iterator itr = begin(); itr != end(); ++itr)
        if (eventId == PAIR64_LOPART(itr->second))
            return itr->first;
    return 0;
}


//Disable CreatureAI when charmed
void CreatureAI::OnCharmed(bool /*apply*/)
{
    //me->IsAIEnabled = !apply;*/
    me->NeedChangeAI = true;
    me->IsAIEnabled = false;
}

CreatureAI::CreatureAI(Creature* creature) : UnitAI(creature), me(creature), m_MoveInLineOfSight_locked(false), m_canSeeEvenInPassiveMode(false), inFightAggroCheck_Timer(TIME_INTERVAL_LOOK)
{
}

void CreatureAI::Talk(std::initializer_list<uint8> ids, ObjectGuid WhisperGuid /*= ObjectGuid::Empty*/)
{
    sCreatureTextMgr->SendChat(me, Trinity::Containers::SelectRandomContainerElement(ids), WhisperGuid);
}

void CreatureAI::Talk(uint8 id, ObjectGuid WhisperGuid)
{
    if (!this)
        return;

    sCreatureTextMgr->SendChat(me, id, WhisperGuid);
}

void CreatureAI::TalkAuto(uint8 id, ObjectGuid WhisperGuid)
{
    if (!this)
        return;

    sCreatureTextMgr->SendChat(me, id, WhisperGuid, CHAT_MSG_ADDON, LANG_ADDON, TEXT_RANGE_NORMAL, 0, TEAM_OTHER, false, nullptr, true);
}

void CreatureAI::DelayTalk(uint32 delayTimer, uint8 id, ObjectGuid WhisperGuid)
{
    if (!this)
        return;

    delayTimer *= IN_MILLISECONDS;

    me->AddDelayedEvent(delayTimer, [this, id, WhisperGuid]() -> void
    {
        if (me && me->isAlive())
            Talk(id, WhisperGuid);
    });
}

void CreatureAI::ZoneTalk(uint8 id, ObjectGuid WhisperGuid)
{
    if (!this)
        return;

    sCreatureTextMgr->SendChat(me, id, WhisperGuid, CHAT_MSG_ADDON, LANG_ADDON, TEXT_RANGE_ZONE);
}

void CreatureAI::DoZoneInCombat(Creature* creature /*= NULL*/, float maxRangeToNearestTarget /* = 50.0f*/)
{
    if (!creature)
        creature = me;

    if (!creature->CanHaveThreatList())
        return;

    Map* map = creature->GetMap();
    if (!map->IsDungeon())                                  //use IsDungeon instead of Instanceable, in case battlegrounds will be instantiated
    {
        sLog->outDebug(LOG_FILTER_GENERAL, "DoZoneInCombat call for map that isn't an instance (creature entry = %d)", creature->IsCreature() ? creature->ToCreature()->GetEntry() : 0);
        return;
    }

    if (!creature->HasReactState(REACT_PASSIVE) && !creature->getVictim())
    {
        if (Unit* nearTarget = creature->SelectNearestTarget(maxRangeToNearestTarget))
            creature->AI()->AttackStart(nearTarget);
        else if (creature->isSummon())
        {
            if (Unit* summoner = creature->ToTempSummon()->GetSummoner())
            {
                Unit* target = summoner->getAttackerForHelper();
                if (!target && summoner->CanHaveThreatList() && !summoner->getThreatManager().isThreatListEmpty())
                    target = summoner->getThreatManager().getHostilTarget();
                if (target && (creature->IsFriendlyTo(summoner) || creature->IsHostileTo(target)))
                    creature->AI()->AttackStart(target);
            }
        }
    }

    if (!creature->HasReactState(REACT_PASSIVE) && !creature->getVictim())
    {
        sLog->outDebug(LOG_FILTER_GENERAL, "DoZoneInCombat called for creature that has empty threat list (creature entry = %u)", creature->GetEntry());
        return;
    }

    map->ApplyOnEveryPlayer([&](Player* player)
    {
        if (player->isGameMaster())
            return;

        if (player->isAlive())
        {
            creature->SetInCombatWith(player);
            player->SetInCombatWith(creature);
            creature->AddThreat(player, 0.0f);
        }

        /* Causes certain things to never leave the threat list (Priest Lightwell, etc):
        for (Unit::ControlList::const_iterator itr = player->m_Controlled.begin(); itr != player->m_Controlled.end(); ++itr)
        {
        creature->SetInCombatWith(*itr);
        (*itr)->SetInCombatWith(creature);
        creature->AddThreat(*itr, 0.0f);
        }*/
    });
}

void CreatureAI::DoAttackerAreaInCombat(Unit* attacker, float range, Unit* pUnit)
{
    if (!attacker)
        attacker = me;

    if (!pUnit)
        pUnit = me;

    Map *map = pUnit->GetMap();
    if (!map->IsDungeon())
        return;

    if (!pUnit->CanHaveThreatList() || pUnit->getThreatManager().isThreatListEmpty())
        return;

    map->ApplyOnEveryPlayer([&](Player* player)
    {
        if (player->isAlive() && attacker->GetDistance(player) <= range)
        {
            pUnit->SetInCombatWith(player);
            player->SetInCombatWith(pUnit);
            pUnit->AddThreat(player, 0.0f);
        }
    });
}

void CreatureAI::DoAttackerGroupInCombat(Player* attacker)
{
    if (attacker)
    {
        if (Group* group = attacker->GetGroup())
        {
            for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
            {
                Player* player = itr->getSource();

                if (player && player->isAlive() && !player->isInCombat() && player->GetMapId() == me->GetMapId())
                {
                    me->SetInCombatWith(player);
                    player->SetInCombatWith(me);
                    me->AddThreat(player, 0.0f);
                }
            }
        }
    }
}

void CreatureAI::DoAggroPulse(const uint32 diff)
{
    if (inFightAggroCheck_Timer <= diff)
    {
        std::list<HostileReference*> t_list = me->getThreatManager().getThreatList();
        if (!t_list.empty())
        {
            for (std::list<HostileReference*>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
            {
                if (auto player = Player::GetPlayer(*me, (*itr)->getUnitGuid()))
                {
                    if (player->isAlive())
                    {
                        me->SetInCombatWithZone();
                        break;
                    }
                }
            }
        }
        inFightAggroCheck_Timer = TIME_INTERVAL_LOOK;
    }
    else
        inFightAggroCheck_Timer -= diff;
}

// scripts does not take care about MoveInLineOfSight loops
// MoveInLineOfSight can be called inside another MoveInLineOfSight and cause stack overflow
void CreatureAI::MoveInLineOfSight_Safe(Unit* who)
{
    if (m_MoveInLineOfSight_locked)
        return;

    m_MoveInLineOfSight_locked = true;
    MoveInLineOfSight(who);
    m_MoveInLineOfSight_locked = false;
}

bool CreatureAI::CanSeeEvenInPassiveMode()
{
    return m_canSeeEvenInPassiveMode;
}

void CreatureAI::SetCanSeeEvenInPassiveMode(bool canSeeEvenInPassiveMode)
{
    m_canSeeEvenInPassiveMode = canSeeEvenInPassiveMode;
}

void CreatureAI::MoveInLineOfSight(Unit* who)
{
    if (me->getVictim())
        return;

    if (me->GetCreatureType() == CREATURE_TYPE_NON_COMBAT_PET) // non-combat pets should just stand there and look good;)
        return;

    if (me->canStartAttack(who, false))
        AttackStart(who);
}

void CreatureAI::EnterEvadeMode()
{
    if (!_EnterEvadeMode())
        return;

    sLog->outDebug(LOG_FILTER_UNITS, "Creature %u enters evade mode.", me->GetEntry());

    if (!me->GetVehicle()) // otherwise me will be in evade mode forever
    {
        if (Unit* owner = me->GetCharmerOrOwner())
        {
            me->GetMotionMaster()->Clear(false);
            me->GetMotionMaster()->MoveFollow(owner, me->GetFollowDistance(), me->GetFollowAngle(), MOTION_SLOT_ACTIVE);
        }
        else
        {
            if (!me->HasUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED) || !me->isInCombat())
                me->AddUnitState(UNIT_STATE_EVADE);

            me->GetMotionMaster()->MoveTargetedHome();
        }
    }

    if (!me->IsDespawn())
    {
        me->m_Events.KillAllEvents(true);

        if (me->IsVisible())
            me->KillAllDelayedCombats();
    }

    Reset();

    if (me->IsVehicle()) // use the same sequence of addtoworld, aireset may remove all summons!
        me->GetVehicleKit()->Reset(true);
}

void CreatureAI::DespawnOnRespawn(uint32 uiTimeToDespawn)
{
    me->AddDelayedEvent(100, [this, uiTimeToDespawn]() -> void
    {
        me->DespawnOrUnsummon();
        me->SetRespawnTime(uiTimeToDespawn);
    });
}

void CreatureAI::SetFlyMode(bool fly)
{
    me->SetCanFly(fly);
    me->SetDisableGravity(fly);
    if (fly)
        me->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_HOVER);
    else
        me->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_HOVER);
}

AISpellInfoType::AISpellInfoType() : target(AITARGET_SELF), condition(AICOND_COMBAT)
, cooldown(AI_DEFAULT_COOLDOWN), realCooldown(0), maxRange(0.0f)
{
}

AISpellInfoType* GetAISpellInfo(uint32 i) { return &CreatureAI::AISpellInfo[i]; }
