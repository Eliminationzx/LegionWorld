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

#ifndef TRINITY_WAYPOINTMANAGER_H
#define TRINITY_WAYPOINTMANAGER_H

struct WaypointData
{
    uint32 id;
    float x, y, z, orientation;
    bool run;
    float speed;
    uint32 delay;
    uint8 delay_chance;
    uint32 event_id;
    uint8 event_chance;
};

typedef std::vector<WaypointData*> WaypointPath;
typedef std::unordered_map<uint32, WaypointPath> WaypointPathContainer;

class WaypointMgr
{
    public:
        static WaypointMgr* instance()
        {
            static WaypointMgr instance;
            return &instance;
        }

        // Attempts to reload a single path from database
        void ReloadPath(uint32 id);

        // Loads all paths from database, should only run on startup
        void Load();

        // Returns the path from a given id
        WaypointPath const* GetPath(uint32 id) const
        {
            WaypointPathContainer::const_iterator itr = _waypointStore.find(id);
            if (itr != _waypointStore.end())
                return &itr->second;

            return nullptr;
        }

        // Returns the path from a given id
        WaypointPath const* GetPathScript(uint32 id) const
        {
            WaypointPathContainer::const_iterator itr = _waypointScriptStore.find(id);
            if (itr != _waypointScriptStore.end())
                return &itr->second;

            return nullptr;
        }

    private:
        WaypointMgr();
        ~WaypointMgr();

        WaypointPathContainer _waypointStore;
        WaypointPathContainer _waypointScriptStore;
};

#define sWaypointMgr WaypointMgr::instance()

#endif

