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

#include "ObjectPosSelector.h"

UsedPosType operator~(UsedPosType uptype)
{
    return uptype == USED_POS_PLUS ? USED_POS_MINUS : USED_POS_PLUS;
}

ObjectPosSelector::UsedPos::UsedPos(float sign_, float size_, float dist_) : sign(sign_), size(size_), dist(dist_)
{
}

ObjectPosSelector::ObjectPosSelector(float x, float y, float size, float dist) : m_center_x(x), m_center_y(y), m_size(size), m_dist(dist)
{
    m_anglestep = acos(m_dist / (m_dist + 2 * m_size));

    m_nextUsedPos[USED_POS_PLUS] = m_UsedPosLists[USED_POS_PLUS].end();
    m_nextUsedPos[USED_POS_MINUS] = m_UsedPosLists[USED_POS_MINUS].end();

    m_smallStepAngle[USED_POS_PLUS] = 0;
    m_smallStepAngle[USED_POS_MINUS] = 0;

    m_smallStepOk[USED_POS_PLUS] = false;
    m_smallStepOk[USED_POS_MINUS] = false;

    m_smallStepNextUsedPos[USED_POS_PLUS] = nullptr;
    m_smallStepNextUsedPos[USED_POS_MINUS] = nullptr;
}

ObjectPosSelector::UsedPosList::value_type const* ObjectPosSelector::nextUsedPos(UsedPosType uptype)
{
    UsedPosList::const_iterator itr = m_nextUsedPos[uptype];
    if (itr != m_UsedPosLists[uptype].end())
        ++itr;

    if (itr == m_UsedPosLists[uptype].end())
    {
        if (!m_UsedPosLists[~uptype].empty())
            return &*m_UsedPosLists[~uptype].rbegin();
        return nullptr;
    }

    return &*itr;
}

float ObjectPosSelector::GetAngle(UsedPos const& usedPos) const
{
    return acos(m_dist / (usedPos.dist + usedPos.size + m_size));
}

void ObjectPosSelector::AddUsedPos(float size, float angle, float dist)
{
    if (angle >= 0)
        m_UsedPosLists[USED_POS_PLUS].insert(std::make_pair(angle, UsedPos(1.0f, size, dist)));
    else
        m_UsedPosLists[USED_POS_MINUS].insert(std::make_pair(-angle, UsedPos(-1.0f, size, dist)));
}

void ObjectPosSelector::InitializeAngle()
{
    m_nextUsedPos[USED_POS_PLUS] = m_UsedPosLists[USED_POS_PLUS].begin();
    m_nextUsedPos[USED_POS_MINUS] = m_UsedPosLists[USED_POS_MINUS].begin();

    m_smallStepAngle[USED_POS_PLUS] = 0;
    m_smallStepAngle[USED_POS_MINUS] = 0;

    m_smallStepOk[USED_POS_PLUS] = true;
    m_smallStepOk[USED_POS_MINUS] = true;
}

bool ObjectPosSelector::FirstAngle(float& angle)
{
    if (m_UsedPosLists[USED_POS_PLUS].empty() && !m_UsedPosLists[USED_POS_MINUS].empty())
        return NextAngleFor(*m_UsedPosLists[USED_POS_MINUS].begin(), 1.0f, USED_POS_PLUS, angle);
    if (m_UsedPosLists[USED_POS_MINUS].empty() && !m_UsedPosLists[USED_POS_PLUS].empty())
        return NextAngleFor(*m_UsedPosLists[USED_POS_PLUS].begin(), -1.0f, USED_POS_MINUS, angle);

    return false;
}

bool ObjectPosSelector::NextAngle(float& angle)
{
    while (m_nextUsedPos[USED_POS_PLUS] != m_UsedPosLists[USED_POS_PLUS].end() || m_nextUsedPos[USED_POS_MINUS] != m_UsedPosLists[USED_POS_MINUS].end() || m_smallStepOk[USED_POS_PLUS] || m_smallStepOk[USED_POS_MINUS])
    {
        // calculate next possible angle
        if (NextPosibleAngle(angle))
            return true;
    }

    return false;
}

bool ObjectPosSelector::NextUsedAngle(float& angle)
{
    while (m_nextUsedPos[USED_POS_PLUS] != m_UsedPosLists[USED_POS_PLUS].end() || m_nextUsedPos[USED_POS_MINUS] != m_UsedPosLists[USED_POS_MINUS].end())
    {
        // calculate next possible angle
        if (!NextPosibleAngle(angle))
            return true;
    }

    return false;
}

bool ObjectPosSelector::NextPosibleAngle(float& angle)
{
    // ++ direction less updated
    if (m_nextUsedPos[USED_POS_PLUS] != m_UsedPosLists[USED_POS_PLUS].end() &&
        (m_nextUsedPos[USED_POS_MINUS] == m_UsedPosLists[USED_POS_MINUS].end() || m_nextUsedPos[USED_POS_PLUS]->first <= m_nextUsedPos[USED_POS_MINUS]->first))
    {
        bool ok;
        if (m_smallStepOk[USED_POS_PLUS])
            ok = NextSmallStepAngle(1.0f, USED_POS_PLUS, angle);
        else
            ok = NextAngleFor(*m_nextUsedPos[USED_POS_PLUS], 1.0f, USED_POS_PLUS, angle);

        if (!ok)
            ++m_nextUsedPos[USED_POS_PLUS];                 // increase. only at fail (original or checked)
        return ok;
    }
    // -- direction less updated
    if (m_nextUsedPos[USED_POS_MINUS] != m_UsedPosLists[USED_POS_MINUS].end())
    {
        bool ok;
        if (m_smallStepOk[USED_POS_MINUS])
            ok = NextSmallStepAngle(-1.0f, USED_POS_MINUS, angle);
        else
            ok = NextAngleFor(*m_nextUsedPos[USED_POS_MINUS], -1.0f, USED_POS_MINUS, angle);

        if (!ok)
            ++m_nextUsedPos[USED_POS_MINUS];
        return ok;
    }
    // both list empty
    if (m_smallStepOk[USED_POS_PLUS] && (!m_smallStepOk[USED_POS_MINUS] || m_smallStepAngle[USED_POS_PLUS] <= m_smallStepAngle[USED_POS_MINUS]))
        return NextSmallStepAngle(1.0f, USED_POS_PLUS, angle);
    // -- direction less updated
    if (m_smallStepOk[USED_POS_MINUS])
        return NextSmallStepAngle(-1.0f, USED_POS_MINUS, angle);

    // no angles
    return false;
}

bool ObjectPosSelector::CheckAngle(UsedPosList::value_type const& nextUsedPos, float sign, float angle) const
{
    float angle_step2 = GetAngle(nextUsedPos.second);

    float next_angle = nextUsedPos.first;
    if (nextUsedPos.second.sign * sign < 0) // last node from diff. list (-pi+alpha)
        next_angle = 2 * M_PI - next_angle; // move to positive

    return fabs(angle) + angle_step2 <= next_angle;
}

bool ObjectPosSelector::CheckOriginal() const
{
    return (m_UsedPosLists[USED_POS_PLUS].empty() || CheckAngle(*m_UsedPosLists[USED_POS_PLUS].begin(), 1.0f, 0)) &&
        (m_UsedPosLists[USED_POS_MINUS].empty() || CheckAngle(*m_UsedPosLists[USED_POS_MINUS].begin(), -1.0f, 0));
}

bool ObjectPosSelector::IsNonBalanced() const
{
    return m_UsedPosLists[USED_POS_PLUS].empty() != m_UsedPosLists[USED_POS_MINUS].empty();
}

bool ObjectPosSelector::NextAngleFor(UsedPosList::value_type const& usedPos, float sign, UsedPosType uptype,
    float& angle)
{
    float angle_step = GetAngle(usedPos.second);

    // next possible angle
    angle = usedPos.first * usedPos.second.sign + angle_step * sign;

    UsedPosList::value_type const* nextNode = nextUsedPos(uptype);
    if (nextNode)
    {
        // if next node permit use selected angle, then do it
        if (!CheckAngle(*nextNode, sign, angle))
        {
            m_smallStepOk[uptype] = false;
            return false;
        }
    }

    // possible more points
    m_smallStepOk[uptype] = true;
    m_smallStepAngle[uptype] = angle;
    m_smallStepNextUsedPos[uptype] = nextNode;

    return true;
}

bool ObjectPosSelector::NextSmallStepAngle(float sign, UsedPosType uptype, float& angle)
{
    // next possible angle
    angle = m_smallStepAngle[uptype] + m_anglestep * sign;

    if (fabs(angle) > M_PI)
    {
        m_smallStepOk[uptype] = false;
        return false;
    }

    if (m_smallStepNextUsedPos[uptype])
    {
        if (fabs(angle) >= m_smallStepNextUsedPos[uptype]->first)
        {
            m_smallStepOk[uptype] = false;
            return false;
        }

        // if next node permit use selected angle, then do it
        if (!CheckAngle(*m_smallStepNextUsedPos[uptype], sign, angle))
        {
            m_smallStepOk[uptype] = false;
            return false;
        }
    }

    // possible more points
    m_smallStepAngle[uptype] = angle;
    return true;
}
