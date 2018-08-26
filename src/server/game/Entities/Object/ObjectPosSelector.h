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

#ifndef _OBJECT_POS_SELECTOR_H
#define _OBJECT_POS_SELECTOR_H

#include <Common.h>

enum UsedPosType
{
    USED_POS_PLUS,
    USED_POS_MINUS
};

inline UsedPosType operator ~(UsedPosType uptype);

struct ObjectPosSelector
{
    struct UsedPos
    {
        UsedPos(float sign_, float size_, float dist_);

        float sign;
        float size;                                         // size of point
        float dist;                                         // dist to central point (including central point size)
    };

    typedef std::multimap<float, UsedPos> UsedPosList;       // abs(angle)->Node

    ObjectPosSelector(float x, float y, float size, float dist);

    void AddUsedPos(float size, float angle, float dist);
    void InitializeAngle();

    bool FirstAngle(float& angle);
    bool NextAngle(float& angle);
    bool NextUsedAngle(float& angle);

    bool NextPosibleAngle(float& angle);

    bool CheckAngle(UsedPosList::value_type const& nextUsedPos, float sign, float angle) const;

    bool CheckOriginal() const;

    bool IsNonBalanced() const;

    bool NextAngleFor(UsedPosList::value_type const& usedPos, float sign, UsedPosType uptype, float& angle);

    bool NextSmallStepAngle(float sign, UsedPosType uptype, float& angle);

    // next used post for m_nextUsedPos[uptype]
    UsedPosList::value_type const* nextUsedPos(UsedPosType uptype);

    // angle from used pos to next possible free pos
    float GetAngle(UsedPos const& usedPos) const;

    float m_center_x;
    float m_center_y;
    float m_size;                                           // size of object in center
    float m_dist;                                           // distance for searching pos (including central object size)
    float m_anglestep;

    UsedPosList m_UsedPosLists[2];
    UsedPosList::const_iterator m_nextUsedPos[2];

    // field for small step from first after next used pos until next pos
    float m_smallStepAngle[2];
    bool  m_smallStepOk[2];
    UsedPosList::value_type const* m_smallStepNextUsedPos[2];
};
#endif
