
if (USE_SCRIPTPCH)
  set(Draenor_STAT_PCH_HDR PrecompiledHeaders/DraenorPCH.h)
  set(Draenor_STAT_PCH_SRC PrecompiledHeaders/DraenorPCH.cpp)
endif ()

set(Draenor_STAT_SRCS
  ${Draenor_STAT_SRCS}
    
  dark_portal.cpp
  frostfire_ridge.cpp
  shadowmoon_valley.cpp
  world_bossess_draenor.cpp

  IronDocks/instance_iron_docks.cpp
  IronDocks/boss_fleshrender_nokgar.cpp
  IronDocks/boss_grimrail_enforcers.cpp
  IronDocks/boss_oshir.cpp
  IronDocks/boss_skulloc.cpp
  
  Auchindoun/auchindoun.cpp
  Auchindoun/auchindoun.hpp
  Auchindoun/boss_azakael.cpp
  Auchindoun/boss_kaathar.cpp
  Auchindoun/boss_nyami.cpp
  Auchindoun/boss_teronogor.cpp
  Auchindoun/instance_auchindoun.cpp

  BloodmaulSlagMines/bloodmaul_slag_mines.cpp
  BloodmaulSlagMines/bloodmaul_slag_mines.h
  BloodmaulSlagMines/bloodmaul_slag_mines_instance.cpp
  BloodmaulSlagMines/boss_forgemaster_gogduh.cpp
  BloodmaulSlagMines/boss_gugrokk.cpp
  BloodmaulSlagMines/boss_roltall.cpp
  BloodmaulSlagMines/boss_slave_watcher_crushto.cpp

  UpperBlackrockSpire/instance_upper_blackrock_spire.cpp
  UpperBlackrockSpire/boss_orebender_gorashan.cpp
  UpperBlackrockSpire/boss_kyrak.cpp
  UpperBlackrockSpire/boss_commander_tharbek.cpp
  UpperBlackrockSpire/boss_ragewing_untamed.cpp
  UpperBlackrockSpire/boss_warlord_zaela.cpp
  UpperBlackrockSpire/upper_blackrock_spire.cpp

  ShadowmoonBurialGrounds/instance_shadowmoon_burial_grounds.cpp
  ShadowmoonBurialGrounds/boss_sadana_bloodfury.cpp
  ShadowmoonBurialGrounds/boss_nhallish.cpp
  ShadowmoonBurialGrounds/boss_bonemaw.cpp
  ShadowmoonBurialGrounds/boss_nerzhul.cpp

  Skyreach/instance_skyreach.cpp
  Skyreach/boss_ranjit.cpp
  Skyreach/boss_araknath.cpp
  Skyreach/boss_rukhran.cpp
  Skyreach/boss_high_sage_viryx.cpp

  TheEverbloom/instance_the_everbloom.cpp
  TheEverbloom/boss_witherbark.cpp
  TheEverbloom/boss_ancient_protectors.cpp
  TheEverbloom/boss_xeritac.cpp
  TheEverbloom/boss_archmage_sol.cpp
  TheEverbloom/boss_yalnu.cpp

  Highmaul/boss_brackenspore.cpp
  Highmaul/boss_imperator_margok.cpp
  Highmaul/boss_kargath_bladefist.cpp
  Highmaul/boss_koragh.cpp
  Highmaul/boss_tectus.cpp
  Highmaul/boss_the_butcher.cpp
  Highmaul/boss_twin_ogron.cpp
  Highmaul/highmaul.cpp
  Highmaul/highmaul.hpp
  Highmaul/instance_highmaul.cpp

  BlackrockFoundry/instance_blackrock_foundry.cpp

  HellfireCitadel/instance_hellfire_citadel.cpp
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
  ${CMAKE_SOURCE_DIR}/src/server/game/Garrison
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
  ${CMAKE_CURRENT_SOURCE_DIR}/PrecompiledHeaders
  ${MYSQL_INCLUDE_DIR}
  ${OPENSSL_INCLUDE_DIR}
)

add_library(Draenor STATIC
  ${Draenor_STAT_SRCS}
  ${Draenor_STAT_PCH_SRC}
)

set_target_properties(Draenor PROPERTIES LINKER_LANGUAGE CXX)

message("  -> Prepared: Draenor")

# Generate precompiled header
if (USE_SCRIPTPCH)
  add_cxx_pch(Draenor ${Draenor_STAT_PCH_HDR} ${Draenor_STAT_PCH_SRC})
endif()
