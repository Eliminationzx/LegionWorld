////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef SCRIPTED_COMESTIC_AI_HPP_SCRIPTED_AI
# define SCRIPTED_COMESTIC_AI_HPP_SCRIPTED_AI

#include "Creature.h"
#include "CreatureAI.h"
#include "Common.h"

namespace MS {
namespace AI
{

struct CosmeticAI : public CreatureAI
{
public:
    explicit CosmeticAI(Creature* creature) : CreatureAI(creature)
    {
        _emptyWarned = true;
    }

    virtual ~CosmeticAI()
    { }

    void AddDelayedEvent(uint32 timeout, std::function<void()> && function)
    {
        _emptyWarned = false;
        _timedDelayedOperations.push_back(std::pair<uint32, std::function<void()>>(timeout, function));
    }

    void SetAIObstacleManagerEnabled(bool p_Enabled)
    {
        if (p_Enabled)
            me->SetFlag(UNIT_FIELD_NPC_FLAGS + 1, UNIT_NPC_FLAG2_AI_OBSTACLE);
        else
            me->RemoveFlag(UNIT_FIELD_NPC_FLAGS + 1, UNIT_NPC_FLAG2_AI_OBSTACLE);
    }

    virtual void LastOperationCalled() { }

public:
    void UpdateAI(uint32 diff) override
    {
        while (!_delayedOperations.empty())
        {
            _delayedOperations.front()();
            _delayedOperations.pop();
        }

        for (auto i = _timedDelayedOperations.begin(); i != _timedDelayedOperations.end(); i++)
        {
            i->first -= diff;

            if (i->first < 0)
            {
                i->second();
                i->second = nullptr;
            }
        }

        uint32 timedDelayedOperationCountToRemove = (uint32)std::count_if(std::begin(_timedDelayedOperations), std::end(_timedDelayedOperations), [](std::pair<int32, std::function<void()>> const& p_Pair) -> bool
        {
            return p_Pair.second == nullptr;
        });

        for (uint32 i = 0; i < timedDelayedOperationCountToRemove; i++)
        {
            auto itr = std::find_if(std::begin(_timedDelayedOperations), std::end(_timedDelayedOperations), [](std::pair<int32, std::function<void()>> const& p_Pair) -> bool
            {
                return p_Pair.second == nullptr;
            });

            if (itr != std::end(_timedDelayedOperations))
                _timedDelayedOperations.erase(itr);
        }

        if (_timedDelayedOperations.empty() && !_emptyWarned)
        {
            _emptyWarned = true;
            LastOperationCalled();
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (type == POINT_MOTION_TYPE)
        {
            if (_onPointReached.find(id) != _onPointReached.end())
            {
                _delayedOperations.push([this, id]() -> void
                {
                    if (_onPointReached.find(id) != _onPointReached.end())
                        _onPointReached[id]();
                });
            }
        }
    }

public:
    std::map<uint32, std::function<void()>> _onPointReached;
    std::queue<std::function<void()>> _delayedOperations;
    std::vector<std::pair<int32, std::function<void()>>> _timedDelayedOperations;
    bool _emptyWarned;
};

}   ///< namespace AI
}   ///< namespace MS

#endif  ///< SCRIPTED_COMESTIC_AI_HPP_SCRIPTED_AI
