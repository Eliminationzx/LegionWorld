/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
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
#ifndef CREATUREAIIMPL_H
#define CREATUREAIIMPL_H

#include "Common.h"
#include "CreatureAI.h"

template<class T>
const T& RAND(const T& v1, const T& v2)
{
    return (urand(0, 1)) ? v1 : v2;
}

template<class T>
const T& RAND(const T& v1, const T& v2, const T& v3)
{
    switch (urand(0, 2))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
    }
}

template<class T>
const T& RAND(const T& v1, const T& v2, const T& v3, const T& v4)
{
    switch (urand(0, 3))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
        case 3: return v4;
    }
}

template<class T>
const T& RAND(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5)
{
    switch (urand(0, 4))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
        case 3: return v4;
        case 4: return v5;
    }
}

template<class T>
const T& RAND(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5, const T& v6)
{
    switch (urand(0, 5))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
        case 3: return v4;
        case 4: return v5;
        case 5: return v6;
    }
}

template<class T>
const T& RAND(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5, const T& v6, const T& v7)
{
    switch (urand(0, 6))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
        case 3: return v4;
        case 4: return v5;
        case 5: return v6;
        case 6: return v7;
    }
}

template<class T>
const T& RAND(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5, const T& v6, const T& v7, const T& v8)
{
    switch (urand(0, 7))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
        case 3: return v4;
        case 4: return v5;
        case 5: return v6;
        case 6: return v7;
        case 7: return v8;
    }
}

template<class T>
const T& RAND(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5, const T& v6, const T& v7, const T& v8,
              const T& v9)
{
    switch (urand(0, 8))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
        case 3: return v4;
        case 4: return v5;
        case 5: return v6;
        case 6: return v7;
        case 7: return v8;
        case 8: return v9;
    }
}

template<class T>
const T& RAND(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5, const T& v6, const T& v7, const T& v8,
              const T& v9, const T& v10)
{
    switch (urand(0, 9))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
        case 3: return v4;
        case 4: return v5;
        case 5: return v6;
        case 6: return v7;
        case 7: return v8;
        case 8: return v9;
        case 9: return v10;
    }
}

template<class T>
const T& RAND(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5, const T& v6, const T& v7, const T& v8,
              const T& v9, const T& v10, const T& v11)
{
    switch (urand(0, 10))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
        case 3: return v4;
        case 4: return v5;
        case 5: return v6;
        case 6: return v7;
        case 7: return v8;
        case 8: return v9;
        case 9: return v10;
        case 10: return v11;
    }
}

template<class T>
const T& RAND(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5, const T& v6, const T& v7, const T& v8,
              const T& v9, const T& v10, const T& v11, const T& v12)
{
    switch (urand(0, 11))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
        case 3: return v4;
        case 4: return v5;
        case 5: return v6;
        case 6: return v7;
        case 7: return v8;
        case 8: return v9;
        case 9: return v10;
        case 10: return v11;
        case 11: return v12;
    }
}

template<class T>
const T& RAND(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5, const T& v6, const T& v7, const T& v8,
              const T& v9, const T& v10, const T& v11, const T& v12, const T& v13)
{
    switch (urand(0, 12))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
        case 3: return v4;
        case 4: return v5;
        case 5: return v6;
        case 6: return v7;
        case 7: return v8;
        case 8: return v9;
        case 9: return v10;
        case 10: return v11;
        case 11: return v12;
        case 12: return v13;
    }
}

template<class T>
const T& RAND(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5, const T& v6, const T& v7, const T& v8,
              const T& v9, const T& v10, const T& v11, const T& v12, const T& v13, const T& v14)
{
    switch (urand(0, 13))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
        case 3: return v4;
        case 4: return v5;
        case 5: return v6;
        case 6: return v7;
        case 7: return v8;
        case 8: return v9;
        case 9: return v10;
        case 10: return v11;
        case 11: return v12;
        case 12: return v13;
        case 13: return v14;
    }
}

template<class T>
const T& RAND(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5, const T& v6, const T& v7, const T& v8,
              const T& v9, const T& v10, const T& v11, const T& v12, const T& v13, const T& v14, const T& v15)
{
    switch (urand(0, 14))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
        case 3: return v4;
        case 4: return v5;
        case 5: return v6;
        case 6: return v7;
        case 7: return v8;
        case 8: return v9;
        case 9: return v10;
        case 10: return v11;
        case 11: return v12;
        case 12: return v13;
        case 13: return v14;
        case 14: return v15;
    }
}

template<class T>
const T& RAND(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5, const T& v6, const T& v7, const T& v8,
              const T& v9, const T& v10, const T& v11, const T& v12, const T& v13, const T& v14, const T& v15, const T& v16)
{
    switch (urand(0, 15))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
        case 3: return v4;
        case 4: return v5;
        case 5: return v6;
        case 6: return v7;
        case 7: return v8;
        case 8: return v9;
        case 9: return v10;
        case 10: return v11;
        case 11: return v12;
        case 12: return v13;
        case 13: return v14;
        case 14: return v15;
        case 15: return v16;
    }
}

enum c_events
{
    EVENT_1     = 1,
    EVENT_2,
    EVENT_3,
    EVENT_4,
    EVENT_5,
    EVENT_6,
    EVENT_7,
    EVENT_8,
    EVENT_9,
    EVENT_10,
    EVENT_11,
    EVENT_12,
    EVENT_13,
    EVENT_14,
    EVENT_15,
    EVENT_16,
    EVENT_17,
    EVENT_18,
    EVENT_19,
    EVENT_20,
    EVENT_21,
    EVENT_22,
    EVENT_23,
    EVENT_24,
    EVENT_25,
    EVENT_26,
    EVENT_27,
    EVENT_28,
    EVENT_29,
    EVENT_30,
    EVENT_31,
    EVENT_32,
    EVENT_33,
    EVENT_34,
    EVENT_35,
    EVENT_36,
    EVENT_37,
    EVENT_38,
    EVENT_39,
    EVENT_40,
    EVENT_41,
    EVENT_42,
    EVENT_43,
    EVENT_44,
    EVENT_45,
    EVENT_46,
    EVENT_47,
    EVENT_48,
    EVENT_49,
    EVENT_50
};

enum c_actions
{
    ACTION_NONE,

    ACTION_1,
    ACTION_2,
    ACTION_3,
    ACTION_4,
    ACTION_5,
    ACTION_6,
    ACTION_7,
    ACTION_8,
    ACTION_9,
    ACTION_10,
    ACTION_11,
    ACTION_12,
    ACTION_13,
    ACTION_14,
    ACTION_15
};

class EventMap : std::map<uint32, uint64>
{
    public:
        EventMap();

        // Returns current timer value, does not represent real dates/times
        uint32 GetTimer() const;

        // Removes all events and clears phase
        void Reset();

        void Update(uint32 time);

        uint32 GetPhaseMask() const;

        /**
        * @name IsInPhase
        * @brief Returns wether event map is in specified phase or not.
        * @param phase Wanted phase.
        * @return True, if phase of event map contains specified phase.
        */
        bool IsInPhase(uint8 phase);

        bool Empty() const;

        // Sets event phase, must be in range 1 - 8
        void SetPhase(uint32 phase);

        /**
        * @name AddPhase
        * @brief Activates the given phase (bitwise).
        * @param phase Phase which should be activated. Values: 1 - 8
        */
        void AddPhase(uint8 phase);

        /**
        * @name RemovePhase
        * @brief Deactivates the given phase (bitwise).
        * @param phase Phase which should be deactivated. Values: 1 - 8.
        */
        void RemovePhase(uint8 phase);

        void ScheduleEvent(uint32 eventId, Seconds time, uint32 group = 0, uint32 phase = 0);
        void ScheduleEvent(uint32 eventId, Minutes time, uint32 group = 0, uint32 phase = 0);
        /**
        * @name ScheduleEvent
        * @brief Creates new event entry in map.
        * @param eventId The id of the new event.
        * @param time The time in milliseconds until the event occurs.
        * @param group The group which the event is associated to. Has to be between 1 and 8. 0 means it has no group.
        * @param phase The phase in which the event can occur. Has to be between 1 and 8. 0 means it can occur in all phases.
        */
        void ScheduleEvent(uint32 eventId, uint32 time, uint32 group = 0, uint32 phase = 0);

        /**
        * @name RescheduleEvent
        * @brief Cancels the given event and reschedules it.
        * @param eventId The id of the event.
        * @param time The time in milliseconds until the event occurs.
        * @param group The group which the event is associated to. Has to be between 1 and 8. 0 means it has no group.
        * @param phase The phase in which the event can occur. Has to be between 1 and 8. 0 means it can occur in all phases.
        */
        void RescheduleEvent(uint32 eventId, uint32 time, uint32 groupId = 0, uint32 phase = 0);

        /// Check if specified event is scheduled
        bool HasEvent(uint32 eventId) const
        {
            for (const_iterator itr = begin(); itr != end(); ++itr)
                if ((itr->second & 0x0000FFFF) == eventId)
                    return true;

            return false;
        }

        uint32 GetEventTime(uint32 eventId) const
        {
            for (const_iterator itr = begin(); itr != end(); ++itr)
                if ((itr->second & 0x0000FFFF) == eventId)
                    return itr->first;

            return 0;
        }

        /**
        * @name RepeatEvent
        * @brief Repeats the mostly recently executed event.
        * @param time Time until the event occurs.
        */
        void RepeatEvent(uint32 time);

        // Removes first event
        void PopEvent();

        /**
        * @name ExecuteEvent
        * @brief Returns the next event to execute and removes it from map.
        * @return Id of the event to execute.
        */
        uint32 ExecuteEvent();

        // Gets next event id to execute
        uint32 GetEvent();

        void DelayEvent(uint32 eventID, uint32 delay);

        // Delay all events
        void DelayEvents(uint32 delay);

        // Delay all events having the specified Group
        void DelayEvents(uint32 delay, uint32 groupId);

        // Cancel events with specified id
        void CancelEvent(uint32 eventId);

        // Cancel events belonging to specified group
        void CancelEventGroup(uint32 groupId);

        // Returns time of next event to execute
        // To get how much time remains substract _time
        uint32 GetNextEventTime(uint32 eventId) const;

    private:
        uint32 _time;
        uint32 _phase;
};

enum AITarget
{
    AITARGET_SELF,
    AITARGET_VICTIM,
    AITARGET_ENEMY,
    AITARGET_ALLY,
    AITARGET_BUFF,
    AITARGET_DEBUFF,
};

enum AICondition
{
    AICOND_AGGRO,
    AICOND_COMBAT,
    AICOND_DIE,
    AICOND_EVADE,
};

#define AI_DEFAULT_COOLDOWN 5000

struct AISpellInfoType
{
    AISpellInfoType();
    AITarget target;
    AICondition condition;
    uint32 cooldown;
    uint32 realCooldown;
    float maxRange;
};

AISpellInfoType* GetAISpellInfo(uint32 i);

bool InstanceHasScript(WorldObject const* obj, char const* scriptName);

template<class AI, class T>
AI* GetInstanceAI(T* obj, char const* scriptName)
{
    if (InstanceHasScript(obj, scriptName))
        return new AI(obj);

    return nullptr;
}

inline void CreatureAI::SetGazeOn(Unit* target)
{
    if (me->IsValidAttackTarget(target))
    {
        AttackStart(target);
        me->SetReactState(REACT_PASSIVE);
    }
}

inline bool CreatureAI::UpdateVictimWithGaze()
{
    if (!me->isInCombat())
        return false;

    if (me->HasReactState(REACT_PASSIVE))
    {
        if (me->getVictim())
            return true;
        me->SetReactState(REACT_AGGRESSIVE);
    }

    if (Unit* victim = me->SelectVictim())
        AttackStart(victim);
    return me->getVictim();
}

inline bool CreatureAI::UpdateVictim()
{
    if (!me->isInCombat() || me->IsDespawn() || !me->IsInWorld())
        return false;

    if (me->GetReactState() == REACT_ATTACK_OFF)
        return false;

    if (!me->HasReactState(REACT_PASSIVE))
    {
        if (Unit* victim = me->SelectVictim())
            AttackStart(victim);
        return me->getVictim();
    }
    if (me->getThreatManager().isThreatListEmpty())
    {
        EnterEvadeMode();
        return false;
    }
    return true;
}

inline bool CreatureAI::IsInDisable()
{
    if (me->HasAuraType(SPELL_AURA_MOD_STUN) || me->HasAuraType(SPELL_AURA_MOD_FEAR) ||
        me->HasAuraType(SPELL_AURA_MOD_CHARM) || me->HasAuraType(SPELL_AURA_MOD_CONFUSE) ||
        me->HasAuraType(SPELL_AURA_MOD_ROOT) || me->HasAuraType(SPELL_AURA_MOD_FEAR_2) ||
        me->HasAuraType(SPELL_AURA_MOD_ROOTED))
        return true;

    return false;
}

inline bool CreatureAI::IsInControl()
{
    if (me->HasAuraType(SPELL_AURA_MOD_STUN) || me->HasAuraType(SPELL_AURA_MOD_FEAR) ||
        me->HasAuraType(SPELL_AURA_MOD_CHARM) || me->HasAuraType(SPELL_AURA_MOD_CONFUSE) ||
        me->HasAuraType(SPELL_AURA_MOD_FEAR_2) || me->HasAuraType(SPELL_AURA_MOD_PACIFY_SILENCE))
        return true;

    return false;
}

inline bool CreatureAI::_EnterEvadeMode()
{
    if (!me->isAlive())
        return false;

    // dont remove vehicle auras, passengers arent supposed to drop off the vehicle
    // NPC_HOLY_GUARDIAN or Mindbender or any summons
    switch(me->GetEntry())
    {
        case 46499:
        case 62982:
        case 67236:
        case 35814:
            break;
        default:
            if (!me->isAnySummons() && !me->isTrainingDummy())
                me->RemoveAllAurasExceptType(SPELL_AURA_CONTROL_VEHICLE);
            break;
    }

    // sometimes bosses stuck in combat?
    me->DeleteThreatList();
    me->ClearSaveThreatTarget();
    me->CombatStop(true);
    me->LoadCreaturesAddon();
    me->SetLootRecipient(nullptr);
    me->ResetPlayerDamageReq();

    if (me->IsInEvadeMode())
        return false;

    return true;
}

inline void UnitAI::DoCast(Unit* victim, uint32 spellId, bool triggered)
{
    if (!victim || (me->HasUnitState(UNIT_STATE_CASTING) && !triggered))
        return;

    me->CastSpell(victim, spellId, triggered);
}

inline void UnitAI::DoCastVictim(uint32 spellId, bool triggered)
{
    if (!me->getVictim() || (me->HasUnitState(UNIT_STATE_CASTING) && !triggered))
        return;

    me->CastSpell(me->getVictim(), spellId, triggered);
}

inline void UnitAI::DoCastAOE(uint32 spellId, bool triggered)
{
    if (!triggered && me->HasUnitState(UNIT_STATE_CASTING))
        return;

    me->CastSpell((Unit*)nullptr, spellId, triggered);
}

inline Creature* CreatureAI::DoSummon(uint32 entry, const Position& pos, uint32 despawnTime, TempSummonType summonType)
{
    return (Creature*)me->SummonCreature(entry, pos, summonType, despawnTime);
}

inline Creature* CreatureAI::DoSummon(uint32 entry, WorldObject* obj, float radius, uint32 despawnTime, TempSummonType summonType)
{
    Position pos;
    obj->GetRandomNearPosition(pos, radius);
    return (Creature*)me->SummonCreature(entry, pos, summonType, despawnTime);
}

inline Creature* CreatureAI::DoSummonFlyer(uint32 entry, WorldObject* obj, float flightZ, float radius, uint32 despawnTime, TempSummonType summonType)
{
    Position pos;
    obj->GetRandomNearPosition(pos, radius);
    pos.m_positionZ += flightZ;
    return (Creature*)me->SummonCreature(entry, pos, summonType, despawnTime);
}

#endif

