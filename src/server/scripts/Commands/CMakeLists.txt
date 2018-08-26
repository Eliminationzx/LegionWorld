# Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

if (USE_SCRIPTPCH)
  set(command_STAT_PCH_HDR PrecompiledHeaders/CommandPCH.h)
  set(command_STAT_PCH_SRC PrecompiledHeaders/CommandPCH.cpp)
endif ()

set(command_STAT_SRCS
  ${command_STAT_SRCS}
  cs_account.cpp
  cs_achievement.cpp
  cs_ban.cpp
  cs_battlenet_account.cpp
  cs_bf.cpp
  cs_cast.cpp
  cs_character.cpp
  cs_character.cpp
  cs_cheat.cpp
  cs_debug.cpp
  cs_disable.cpp
  cs_event.cpp
  cs_eo.cpp
  cs_gm.cpp
  cs_go.cpp
  cs_gobject.cpp
  cs_guild.cpp
  cs_honor.cpp
  cs_instance.cpp
  cs_learn.cpp
  cs_lfg.cpp
  cs_list.cpp
  cs_lookup.cpp
  cs_message.cpp
  cs_misc.cpp
  cs_modify.cpp
  cs_npc.cpp
  cs_quest.cpp
  cs_reload.cpp
  cs_reset.cpp
  cs_tele.cpp
  cs_ticket.cpp
  cs_scenario.cpp  
  cs_server.cpp
  cs_titles.cpp
  cs_wp.cpp
  cs_battlepay.cpp
#  cs_pdump.cpp
#  cs_channel.cpp
#  cs_pet.cpp
#  cs_aura.cpp
#  cs_unaura.cpp
#  cs_nameannounce.cpp
#  cs_gmnameannounce.cpp
#  cs_announce.cpp
#  cs_gmannounce.cpp
#  cs_notify.cpp
#  cs_gmnotify.cpp
#  cs_appear.cpp
#  cs_summon.cpp
#  cs_groupsummon.cpp
#  cs_commands.cpp
#  cs_demorph.cpp
#  cs_die.cpp
#  cs_revive.cpp
#  cs_dismount.cpp
#  cs_guid.cpp
#  cs_help.cpp
#  cs_itemmove.cpp
#  cs_cooldown.cpp
#  cs_unlearn.cpp
#  cs_distance.cpp
#  cs_recall.cpp
#  cs_save.cpp
#  cs_saveall.cpp
#  cs_kick.cpp
#  cs_start.cpp
#  cs_taxicheat.cpp
#  cs_linkgrave.cpp
#  cs_neargrave.cpp
#  cs_explorecheat.cpp
#  cs_hover.cpp
#  cs_levelup.cpp
#  cs_showarea.cpp
#  cs_hidearea.cpp
#  cs_additem.cpp
#  cs_additemset.cpp
#  cs_bank.cpp
#  cs_wchange.cpp
#  cs_maxskill.cpp
#  cs_setskill.cpp
#  cs_whispers.cpp
#  cs_pinfo.cpp
#  cs_respawn.cpp
#  cs_send.cpp
#  cs_mute.cpp
#  cs_unmute.cpp
#  cs_movegens.cpp
#  cs_cometome.cpp
#  cs_damage.cpp
#  cs_combatstop.cpp
#  cs_flusharenapoints.cpp
#  cs_repairitems.cpp
#  cs_waterwalk.cpp
#  cs_freeze.cpp
#  cs_unfreeze.cpp
#  cs_listfreeze.cpp
#  cs_possess.cpp
#  cs_unpossess.cpp
#  cs_bindsight.cpp
#  cs_unbindsight.cpp
#  cs_playall.cpp
)

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/dep
  ${CMAKE_SOURCE_DIR}/dep/cds
  ${CMAKE_SOURCE_DIR}/dep/cppformat
  ${CMAKE_SOURCE_DIR}/dep/recastnavigation/Detour
  ${CMAKE_SOURCE_DIR}/dep/recastnavigation/Detour/Include
  ${CMAKE_SOURCE_DIR}/dep/recastnavigation/Recast
  ${CMAKE_SOURCE_DIR}/dep/recastnavigation/Recast/Include
  ${CMAKE_SOURCE_DIR}/dep/g3dlite/include
  ${CMAKE_SOURCE_DIR}/dep/SFMT
  ${CMAKE_SOURCE_DIR}/dep/zlib
  ${CMAKE_SOURCE_DIR}/src/server/shared
  ${CMAKE_SOURCE_DIR}/src/server/shared/Configuration
  ${CMAKE_SOURCE_DIR}/src/server/shared/Cryptography
  ${CMAKE_SOURCE_DIR}/src/server/shared/Database
  ${CMAKE_SOURCE_DIR}/src/server/shared/DataStores
  ${CMAKE_SOURCE_DIR}/src/server/shared/Debugging
  ${CMAKE_SOURCE_DIR}/src/server/shared/Dynamic/LinkedReference
  ${CMAKE_SOURCE_DIR}/src/server/shared/Dynamic
  ${CMAKE_SOURCE_DIR}/src/server/shared/Logging
  ${CMAKE_SOURCE_DIR}/src/server/shared/Packets
  ${CMAKE_SOURCE_DIR}/src/server/shared/Threading
  ${CMAKE_SOURCE_DIR}/src/server/shared/Utilities
  ${CMAKE_SOURCE_DIR}/src/server/ipc
  ${CMAKE_SOURCE_DIR}/src/server/collision
  ${CMAKE_SOURCE_DIR}/src/server/collision/Management
  ${CMAKE_SOURCE_DIR}/src/server/collision/Models
  ${CMAKE_SOURCE_DIR}/src/server/collision/Maps
  ${CMAKE_SOURCE_DIR}/src/server/shared
  ${CMAKE_SOURCE_DIR}/src/server/shared/Database
  ${CMAKE_SOURCE_DIR}/src/server/game/Accounts
  ${CMAKE_SOURCE_DIR}/src/server/game/Anticheat
  ${CMAKE_SOURCE_DIR}/src/server/game/Achievements
  ${CMAKE_SOURCE_DIR}/src/server/game/Addons
  ${CMAKE_SOURCE_DIR}/src/server/game/AI
  ${CMAKE_SOURCE_DIR}/src/server/game/AI/CoreAI
  ${CMAKE_SOURCE_DIR}/src/server/game/AI/ScriptedAI
  ${CMAKE_SOURCE_DIR}/src/server/game/AI/SmartScripts
  ${CMAKE_SOURCE_DIR}/src/server/game/AuctionHouse
  ${CMAKE_SOURCE_DIR}/src/server/game/Battlefield
  ${CMAKE_SOURCE_DIR}/src/server/game/Battlefield/Zones
  ${CMAKE_SOURCE_DIR}/src/server/game/Battlegrounds
  ${CMAKE_SOURCE_DIR}/src/server/game/Battlegrounds/Zones
  ${CMAKE_SOURCE_DIR}/src/server/game/BattlePet
  ${CMAKE_SOURCE_DIR}/src/server/game/BattlePay
  ${CMAKE_SOURCE_DIR}/src/server/game/BrawlersGuild
  ${CMAKE_SOURCE_DIR}/src/server/game/Calendar
  ${CMAKE_SOURCE_DIR}/src/server/game/Challenge
  ${CMAKE_SOURCE_DIR}/src/server/game/Chat
  ${CMAKE_SOURCE_DIR}/src/server/game/Chat/Channels
  ${CMAKE_SOURCE_DIR}/src/server/game/Conditions
  ${CMAKE_SOURCE_DIR}/src/server/shared/Configuration
  ${CMAKE_SOURCE_DIR}/src/server/game/Combat
  ${CMAKE_SOURCE_DIR}/src/server/game/DataStores
  ${CMAKE_SOURCE_DIR}/src/server/game/DungeonFinding
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/AreaTrigger
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/Conversation
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/Corpse
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/Creature
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/DynamicObject
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/EventObject
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/Item
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/Item/Container
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/GameObject
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/Object
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/Object/Updates
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/Pet
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/Player
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/Transport
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/Unit
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/Vehicle
  ${CMAKE_SOURCE_DIR}/src/server/game/Events
  ${CMAKE_SOURCE_DIR}/src/server/game/Globals
  ${CMAKE_SOURCE_DIR}/src/server/game/Grids
  ${CMAKE_SOURCE_DIR}/src/server/game/Grids/Cells
  ${CMAKE_SOURCE_DIR}/src/server/game/Grids/Notifiers
  ${CMAKE_SOURCE_DIR}/src/server/game/Groups
  ${CMAKE_SOURCE_DIR}/src/server/game/Guilds
  ${CMAKE_SOURCE_DIR}/src/server/game/Handlers
  ${CMAKE_SOURCE_DIR}/src/server/game/Instances
  ${CMAKE_SOURCE_DIR}/src/server/game/Loot
  ${CMAKE_SOURCE_DIR}/src/server/game/Mails
  ${CMAKE_SOURCE_DIR}/src/server/game/Miscellaneous
  ${CMAKE_SOURCE_DIR}/src/server/game/Maps
  ${CMAKE_SOURCE_DIR}/src/server/game/Movement
  ${CMAKE_SOURCE_DIR}/src/server/game/Movement/MovementGenerators
  ${CMAKE_SOURCE_DIR}/src/server/game/Movement/Spline
  ${CMAKE_SOURCE_DIR}/src/server/game/Movement/Waypoints
  ${CMAKE_SOURCE_DIR}/src/server/game/OutdoorPvP
  ${CMAKE_SOURCE_DIR}/src/server/game/Pools
  ${CMAKE_SOURCE_DIR}/src/server/game/PrecompiledHeaders
  ${CMAKE_SOURCE_DIR}/src/server/game/Quests
  ${CMAKE_SOURCE_DIR}/src/server/game/Reputation
  ${CMAKE_SOURCE_DIR}/src/server/game/Scenarios  
  ${CMAKE_SOURCE_DIR}/src/server/game/Scripting
  ${CMAKE_SOURCE_DIR}/src/server/game/Server
  ${CMAKE_SOURCE_DIR}/src/server/game/Server/Protocol
  ${CMAKE_SOURCE_DIR}/src/server/game/Server/Packets
  ${CMAKE_SOURCE_DIR}/src/server/game/Skills
  ${CMAKE_SOURCE_DIR}/src/server/game/Spells
  ${CMAKE_SOURCE_DIR}/src/server/game/Spells/Auras
  ${CMAKE_SOURCE_DIR}/src/server/game/Texts
  ${CMAKE_SOURCE_DIR}/src/server/game/Tickets
  ${CMAKE_SOURCE_DIR}/src/server/game/Tools
  ${CMAKE_SOURCE_DIR}/src/server/game/Warden
  ${CMAKE_SOURCE_DIR}/src/server/game/Warden/Modules
  ${CMAKE_SOURCE_DIR}/src/server/game/Weather
  ${CMAKE_SOURCE_DIR}/src/server/game/World
  ${CMAKE_SOURCE_DIR}/src/server/game/GroupFinder
  ${CMAKE_CURRENT_SOURCE_DIR}/PrecompiledHeaders
  ${MYSQL_INCLUDE_DIR}
  ${OPENSSL_INCLUDE_DIR}
)

add_library(Command STATIC
  ${command_STAT_SRCS}
  ${command_STAT_PCH_SRC}
)

set_target_properties(Command PROPERTIES LINKER_LANGUAGE CXX)

message("  -> Prepared: Commands")

# Generate precompiled header
if (USE_SCRIPTPCH)
  add_cxx_pch(Command ${command_STAT_PCH_HDR} ${command_STAT_PCH_SRC})
endif()
