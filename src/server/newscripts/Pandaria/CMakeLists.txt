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
  set(Pandaria_STAT_PCH_HDR PrecompiledHeaders/PandariaPCH.h)
  set(Pandaria_STAT_PCH_SRC PrecompiledHeaders/PandariaPCH.cpp)
endif ()

set(Pandaria_STAT_SRCS
  ${Pandaria_STAT_SRCS}
  boss_sha_of_anger.cpp
  boss_galion.cpp
  boss_oondasta.cpp
  boss_nalak.cpp

  MogushanPalace/instance_mogu_shan_palace.cpp
  MogushanPalace/mogu_shan_palace.cpp
  MogushanPalace/boss_trial_of_the_king.cpp
  MogushanPalace/boss_gekkan.cpp
  MogushanPalace/boss_xin_the_weaponmaster.cpp

  TempleJadeSerpent/instance_temple_of_jade_serpent.cpp
  TempleJadeSerpent/boss_wise_mari.cpp
  TempleJadeSerpent/boss_lorewalker_stonestep.cpp
  TempleJadeSerpent/boss_liu_flameheart.cpp
  TempleJadeSerpent/boss_sha_of_doubt.cpp

  StormstoutBrewery/instance_stormstout_brewery.cpp
  StormstoutBrewery/stormstout_brewery.cpp
  StormstoutBrewery/stormstout_brewery.h
  StormstoutBrewery/boss_ook_ook.cpp
  StormstoutBrewery/boss_hoptallus.cpp
  StormstoutBrewery/boss_yan_zhu.cpp

  GateSettingSun/instance_gate_setting_sun.cpp
  GateSettingSun/gate_setting_sun.cpp
  GateSettingSun/gate_setting_sun.h
  GateSettingSun/boss_saboteur_kiptilak.cpp
  GateSettingSun/boss_striker_gadok.cpp
  GateSettingSun/boss_commander_rimok.cpp
  GateSettingSun/boss_raigonn.cpp

  ShadopanMonastery/instance_shadopan_monastery.cpp
  ShadopanMonastery/shadopan_monastery.cpp
  ShadopanMonastery/shadopan_monastery.h
  ShadopanMonastery/boss_gu_cloudstrike.cpp
  ShadopanMonastery/boss_master_snowdrift.cpp
  ShadopanMonastery/boss_sha_of_violence.cpp
  ShadopanMonastery/boss_taran_zhu.cpp
  
  SiegeNiuzaoTemple/instance_siege_of_the_niuzoa_temple.cpp
  SiegeNiuzaoTemple/siege_of_the_niuzoa_temple.cpp
  SiegeNiuzaoTemple/siege_of_the_niuzoa_temple.h
  SiegeNiuzaoTemple/boss_jinbak.cpp
  SiegeNiuzaoTemple/boss_commander_vojak.cpp
  SiegeNiuzaoTemple/boss_general_pavalak.cpp
  SiegeNiuzaoTemple/boss_wing_leader_neronok.cpp
  

  MogushanVault/instance_mogu_shan_vault.cpp
  MogushanVault/mogu_shan_vault.cpp
  MogushanVault/mogu_shan_vault.h
  MogushanVault/boss_stone_guard.cpp
  MogushanVault/boss_feng.cpp
  MogushanVault/boss_garajal.cpp
  MogushanVault/boss_spirit_kings.cpp
  MogushanVault/boss_elegon.cpp
  MogushanVault/boss_will_of_emperor.cpp
  
  HeartFear/instance_heart_of_fear.cpp
  HeartFear/heart_of_fear.h
  HeartFear/boss_vizier_zorlok.cpp
  HeartFear/boss_lord_tayak.cpp
  HeartFear/boss_garalon.cpp
  HeartFear/boss_lord_meljarak.cpp
  HeartFear/boss_unsok.cpp
  HeartFear/boss_empress_shekzeer.cpp

  TerraceEndlessSpring/instance_terrace_of_endless_spring.cpp
  TerraceEndlessSpring/terrace_of_endless_spring.h
  TerraceEndlessSpring/boss_protectors.cpp
  TerraceEndlessSpring/boss_tsulong.cpp
  TerraceEndlessSpring/boss_lei_shi.cpp
  TerraceEndlessSpring/boss_sha_of_fear.cpp

  ThroneOfThunder/instance_throne_of_thunder.cpp
  ThroneOfThunder/throne_of_thunder.h
  ThroneOfThunder/boss_jinrokh.cpp
  ThroneOfThunder/boss_horridon.cpp
  ThroneOfThunder/boss_council_of_elders.cpp
  ThroneOfThunder/boss_tortos.cpp
  ThroneOfThunder/boss_megaera.cpp
  ThroneOfThunder/boss_jikun.cpp
  ThroneOfThunder/boss_durumu.cpp
  ThroneOfThunder/boss_primordius.cpp
  ThroneOfThunder/boss_dark_animus.cpp
  ThroneOfThunder/boss_iron_qon.cpp
  ThroneOfThunder/boss_twin_consorts.cpp
  ThroneOfThunder/boss_lei_shen.cpp
  ThroneOfThunder/boss_ra_den.cpp

  SiegeofOrgrimmar/instance_siege_of_orgrimmar.cpp
  SiegeofOrgrimmar/siege_of_orgrimmar.cpp
  SiegeofOrgrimmar/siege_of_orgrimmar.h
  SiegeofOrgrimmar/boss_immerseus.cpp
  SiegeofOrgrimmar/boss_fallen_protectors.cpp
  SiegeofOrgrimmar/boss_norushen.cpp
  SiegeofOrgrimmar/boss_sha_of_pride.cpp
  SiegeofOrgrimmar/boss_galakras.cpp
  SiegeofOrgrimmar/boss_iron_juggernaut.cpp
  SiegeofOrgrimmar/boss_korkron_dark_shaman.cpp
  SiegeofOrgrimmar/boss_general_nazgrim.cpp
  SiegeofOrgrimmar/boss_malkorok.cpp
  SiegeofOrgrimmar/boss_spoils_of_pandaria.cpp
  SiegeofOrgrimmar/boss_thok_the_bloodthirsty.cpp
  SiegeofOrgrimmar/boss_siegecrafter_blackfuse.cpp
  SiegeofOrgrimmar/boss_paragons_of_the_klaxxi.cpp
  SiegeofOrgrimmar/boss_garrosh_hellscream.cpp
  
  wandering_island.cpp
  timeless_isle.cpp

  valley_of_the_four_winds.cpp
  krasarang_wilds.cpp
  kun_lai_summit.cpp
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

add_library(Pandaria STATIC
  ${Pandaria_STAT_SRCS}
  ${Pandaria_STAT_PCH_SRC}
)

message("  -> Prepared: Pandaria")

# Generate precompiled header
if (USE_SCRIPTPCH)
  add_cxx_pch(Pandaria ${Pandaria_STAT_PCH_HDR} ${Pandaria_STAT_PCH_SRC})
endif()
