# Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.


file(GLOB_RECURSE sources_ArenaofAnnihilation Scenario/ArenaofAnnihilation/*.cpp Scenario/ArenaofAnnihilation/*.h)
source_group(ArenaofAnnihilation FILES ${sources_ArenaofAnnihilation})

file(GLOB_RECURSE sources_ABrewingStorm Scenario/ABrewingStorm/*.cpp Scenario/ABrewingStorm/*.h)
source_group(ABrewingStorm FILES ${sources_ABrewingStorm})

file(GLOB_RECURSE sources_ALittlePatience Scenario/ALittlePatience/*.cpp Scenario/ALittlePatience/*.h)
source_group(ALittlePatience FILES ${sources_ALittlePatience})

file(GLOB_RECURSE sources_AssoultOnZanvess Scenario/AssoultOnZanvess/*.cpp Scenario/AssoultOnZanvess/*.h)
source_group(AssoultOnZanvess FILES ${sources_AssoultOnZanvess})

file(GLOB_RECURSE sources_BattleOnTheHighSeas Scenario/BattleOnTheHighSeas/*.cpp Scenario/BattleOnTheHighSeas/*.h)
source_group(BattleOnTheHighSeas FILES ${sources_BattleOnTheHighSeas})

file(GLOB_RECURSE sources_BloodInTheSnow Scenario/BloodInTheSnow/*.cpp Scenario/BloodInTheSnow/*.h)
source_group(BloodInTheSnow FILES ${sources_BloodInTheSnow})

file(GLOB_RECURSE sources_BrewmoonFestival Scenario/BrewmoonFestival/*.cpp Scenario/BrewmoonFestival/*.h)
source_group(BrewmoonFestival FILES ${sources_BrewmoonFestival})

file(GLOB_RECURSE sources_BrokenIslands Scenario/BrokenIslands/*.cpp Scenario/BrokenIslands/*.h)
source_group(BrokenIslands FILES ${sources_BrokenIslands})

file(GLOB_RECURSE sources_CelestialTournament Scenario/CelestialTournament/*.cpp Scenario/CelestialTournament/*.h)
source_group(CelestialTournament FILES ${sources_CelestialTournament})

file(GLOB_RECURSE sources_CryptOfForgottenKings Scenario/CryptOfForgottenKings/*.cpp Scenario/CryptOfForgottenKings/*.h)
source_group(CryptOfForgottenKings FILES ${sources_CryptOfForgottenKings})

file(GLOB_RECURSE sources_DaggerInTheDark Scenario/DaggerInTheDark/*.cpp Scenario/DaggerInTheDark/*.h)
source_group(DaggerInTheDark FILES ${sources_DaggerInTheDark})

file(GLOB_RECURSE sources_DarkHeartOfPandaria Scenario/DarkHeartOfPandaria/*.cpp Scenario/DarkHeartOfPandaria/*.h)
source_group(DarkHeartOfPandaria FILES ${sources_DarkHeartOfPandaria})

file(GLOB_RECURSE sources_DominationPoint Scenario/DominationPoint/*.cpp Scenario/DominationPoint/*.h)
source_group(DominationPoint FILES ${sources_DominationPoint})

file(GLOB_RECURSE sources_FallofShanBu Scenario/FallofShanBu/*.cpp Scenario/FallofShanBu/*.h)
source_group(FallofShanBu FILES ${sources_FallofShanBu})

file(GLOB_RECURSE sources_GreenstoneVillage Scenario/GreenstoneVillage/*.cpp Scenario/GreenstoneVillage/*.h)
source_group(GreenstoneVillage FILES ${sources_GreenstoneVillage})

file(GLOB_RECURSE sources_LionsLanding Scenario/LionsLanding/*.cpp Scenario/LionsLanding/*.h)
source_group(LionsLanding FILES ${sources_LionsLanding})

file(GLOB_RECURSE sources_PursuingTheBlackHarvest Scenario/PursuingTheBlackHarvest/*.cpp Scenario/PursuingTheBlackHarvest/*.h)
source_group(PursuingTheBlackHarvest FILES ${sources_PursuingTheBlackHarvest})

file(GLOB_RECURSE sources_TheSecretsOfRagefire Scenario/TheSecretsOfRagefire/*.cpp Scenario/TheSecretsOfRagefire/*.h)
source_group(TheSecretsOfRagefire FILES ${sources_TheSecretsOfRagefire})

file(GLOB_RECURSE sources_ThunderForge Scenario/ThunderForge/*.cpp Scenario/ThunderForge/*.h)
source_group(ThunderForge FILES ${sources_ThunderForge})

file(GLOB_RECURSE sources_TrovesOfTheThunderKing Scenario/TrovesOfTheThunderKing/*.cpp Scenario/TrovesOfTheThunderKing/*.h)
source_group(TrovesOfTheThunderKing FILES ${sources_TrovesOfTheThunderKing})

file(GLOB_RECURSE sources_UngaIngoo Scenario/UngaIngoo/*.cpp Scenario/UngaIngoo/*.h)
source_group(UngaIngoo FILES ${sources_UngaIngoo})

file(GLOB_RECURSE sources_ProvingGrounds Scenario/ProvingGrounds/*.cpp Scenario/ProvingGrounds/*.h)
source_group(ProvingGrounds FILES ${sources_ProvingGrounds})

file(GLOB_RECURSE sources_BrokenShore Scenario/Artifacts/Paladin/BrokenShore/*.cpp Scenario/Artifacts/Paladin/BrokenShore/*.h)
source_group(BrokenShore FILES ${sources_BrokenShore})

file(GLOB_RECURSE sources_ShieldsRest Scenario/Artifacts/Paladin/ShieldsRest/*.cpp Scenario/Artifacts/Paladin/ShieldsRest/*.h)
source_group(ShieldsRest FILES ${sources_ShieldsRest})

file(GLOB_RECURSE sources_Vengeance Scenario/Artifacts/DemonHunter/Vengeance/*.cpp Scenario/Artifacts/DemonHunter/Vengeance/*.h)
source_group(DH_Vengeance_art_scenario FILES ${sources_Vengeance})

file(GLOB_RECURSE sources_Dreadblades Scenario/Artifacts/Rogue/Dreadblades/*.cpp Scenario/Artifacts/Rogue/Dreadblades/*.h)
source_group(Dreadblades FILES ${sources_Dreadblades})

file(GLOB_RECURSE sources_Kingslayers Scenario/Artifacts/Rogue/Kingslayers/*.cpp Scenario/Artifacts/Rogue/Kingslayers/*.h)
source_group(Kingslayers FILES ${sources_Kingslayers})

file(GLOB_RECURSE sources_Devourer Scenario/Artifacts/Rogue/FangsoftheDevourer/*.cpp)
source_group(Devourer FILES ${sources_Devourer})

file(GLOB_RECURSE sources_Warrior Scenario/Artifacts/Warrior/Intro/*.cpp)
source_group(Warrior FILES ${sources_Warrior})

file(GLOB_RECURSE sources_Warswords Scenario/Artifacts/Warrior/Warswords/*.cpp)
source_group(Warswords FILES ${sources_Warswords})

file(GLOB_RECURSE sources_EarthWarder Scenario/Artifacts/Warrior/EarthWarder/*.cpp)
source_group(EarthWarder FILES ${sources_EarthWarder})

file(GLOB_RECURSE sources_Tuure Scenario/Artifacts/Priest/Tuure/*.cpp)
source_group(Tuure FILES ${sources_Tuure})

file(GLOB_RECURSE sources_SilverHand Scenario/Artifacts/Paladin/SilverHand/*.cpp)
source_group(SilverHand FILES ${sources_SilverHand})

file(GLOB_RECURSE sources_Sharasdal Scenario/Artifacts/Shaman/Sharasdal/*.cpp)
source_group(Sharasdal FILES ${sources_Sharasdal})

file(GLOB_RECURSE sources_Monks Scenario/Artifacts/Monk/Intro/*.cpp)
source_group(Monk FILES ${sources_Monks})

file(GLOB_RECURSE sources_Sheylun Scenario/Artifacts/Monk/Shey-lun/*.cpp)
source_group(SheyLun FILES ${sources_Sheylun})

file(GLOB_RECURSE sources_JadeSerpent Scenario/Artifacts/Monk/TempleOfTheJadeSerpent/*.cpp)
source_group(SheyLun FILES ${sources_JadeSerpent})

file(GLOB_RECURSE sources_AssaultBrokenShore Scenario/AssaultBrokenShore/*.cpp)
source_group(AssaultBrokenShore FILES ${sources_AssaultBrokenShore})

file(GLOB_RECURSE sources_InvasionAzsuna Scenario/LegionInvasion/Azsuna/*.cpp)
source_group(InvasionAzsuna FILES ${sources_InvasionAzsuna})

file(GLOB_RECURSE sources_InvasionHM Scenario/LegionInvasion/HighMountain/*.cpp)
source_group(InvasionHM FILES ${sources_InvasionHM})

file(GLOB_RECURSE sources_InvasionValshara Scenario/LegionInvasion/Valshara/*.cpp)
source_group(InvasionValshara FILES ${sources_InvasionValshara})

file(GLOB_RECURSE sources_InvasionStormheim Scenario/LegionInvasion/Stormheim/*.cpp)
source_group(InvasionStormheim FILES ${sources_InvasionStormheim})

file(GLOB_RECURSE sources_BattlePetInstance Scenario/BattlePetInstance/*.cpp)
source_group(BattlePetInstance FILES ${sources_BattlePetInstance})

file(GLOB_RECURSE sources_ArmyTraining Scenario/ArmyTraining/*.cpp)
source_group(ArmyTraining FILES ${sources_ArmyTraining})

set(scripts_STAT_SRCS
  ${scripts_STAT_SRCS}
  ${sources_ArenaofAnnihilation}
  ${sources_ABrewingStorm}
  ${sources_ALittlePatience}
  ${sources_AssoultOnZanvess}
  ${sources_BattleOnTheHighSeas}
  ${sources_BloodInTheSnow}
  ${sources_BrewmoonFestival}
  ${sources_BrokenIslands}
  ${sources_CelestialTournament}
  ${sources_CryptOfForgottenKings}
  ${sources_DaggerInTheDark}
  ${sources_DarkHeartOfPandaria}
  ${sources_DominationPoint}
  ${sources_FallofShanBu}
  ${sources_GreenstoneVillage}
  ${sources_LionsLanding}
  ${sources_PursuingTheBlackHarvest}
  ${sources_TheSecretsOfRagefire}
  ${sources_ThunderForge}
  ${sources_TrovesOfTheThunderKing}
  ${sources_UngaIngoo}
  ${sources_ProvingGrounds}
  ${sources_BrokenShore}
  ${sources_ShieldsRest}
  ${sources_Vengeance}
  ${sources_Dreadblades}
  ${sources_Kingslayers}
  ${sources_Devourer}
  ${sources_Warrior}
  ${sources_Warswords}
  ${sources_EarthWarder}
  ${sources_Tuure}
  ${sources_SilverHand}
  ${sources_Sharasdal}
  ${sources_Monks}
  ${sources_Sheylun}
  ${sources_JadeSerpent}
  ${sources_AssaultBrokenShore}
  ${sources_InvasionAzsuna}
  ${sources_InvasionHM}
  ${sources_InvasionValshara}
  ${sources_InvasionStormheim}
  ${sources_BattlePetInstance}
  ${sources_ArmyTraining}
)

message("  -> Prepared: Scenario")
