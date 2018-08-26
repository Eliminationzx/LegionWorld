DELETE FROM `spell_aura_options` WHERE `ID` = '20517'; 
INSERT INTO `spell_aura_options` (`ID`, `SpellID`, `ProcCharges`, `ProcTypeMask`, `CumulativeAura`, `ProcChance`, `VerifiedBuild`) VALUES ('20517', '202425', '3', '65536', '0', '100', '23911'); 
DELETE FROM `hotfix_data` WHERE `Id` = '20517' AND `TableHash` = '4096770149' AND `RecordID` = '20517' AND `Timestamp` = '0'; 
INSERT INTO `hotfix_data` VALUES ('20517', '4096770149', '20517', '0', '0');

DELETE FROM `spell_shapeshift` WHERE `SpellID` = '2006'; 
DELETE FROM `spell_shapeshift` WHERE `SpellID` = '213634'; 
INSERT INTO `spell_shapeshift` (`ID`, `SpellID`, `ShapeshiftExclude1`, `ShapeshiftMask1`) VALUES 
('45', '2006', '0', '0'),
('4756', '213634', '0', '2147483648'); 
DELETE FROM `hotfix_data` WHERE `Id` = '20518' AND `TableHash` = '3163679255' AND `RecordID` = '4756'; 
DELETE FROM `hotfix_data` WHERE `Id` = '20519' AND `TableHash` = '3163679255' AND `RecordID` = '45'; 
INSERT INTO `hotfix_data` (`Id`, `TableHash`, `RecordID`) VALUES 
('20518', '3163679255', '4756'), 
('20519', '3163679255', '45');

DELETE FROM `spell` WHERE `ID` = '17940'; 
INSERT INTO `spell` (`ID`, `Name`) VALUES ('17940', 'Agony Soul Shard Driver');
DELETE FROM `spell_misc` WHERE `ID` = '500003'; 
INSERT INTO `spell_misc` (`ID`, `Attributes1`, `Attributes2`, `Attributes3`, `Attributes4`, `Attributes5`, `Attributes6`, `Attributes7`, `CastingTimeIndex`, `DurationIndex`, `ActiveIconFileDataID`, `SpellIconFileDataID`, `SpellID`) VALUES 
('500003', '763363584', '168', '540677', '269484032', '4', '393224', '4096', '1', '21', '136243', '136243' , '17940');
DELETE FROM `spell_effect` WHERE `ID` = '6'; 
DELETE FROM `spell_effect` WHERE `ID` = '7'; 
INSERT INTO `spell_effect` (`ID`, `SpellID`, `Effect`, `EffectAura`, `ImplicitTarget1`) VALUES ('6', '17940', '6', '4', '1');
INSERT INTO `spell_effect` (`ID`, `SpellID`, `Effect`, `EffectAura`, `EffectIndex`, `ImplicitTarget1`) VALUES ('7', '17940', '6', '4', '1', '1');
UPDATE `spell_effect` SET `EffectBasePoints`='1' WHERE (`ID`='6');
UPDATE `spell_effect` SET `EffectBasePoints`='0' WHERE (`ID`='6');

DELETE FROM `spell` WHERE `ID` = '62388'; 
INSERT INTO `spell` (`ID`, `Name`) VALUES ('62388', 'Teleport UNLOCK');
DELETE FROM `spell_effect` WHERE `ID` = '9'; 
INSERT INTO `spell_effect` (`ID`, `SpellID`, `Effect`, `EffectAura`, `ImplicitTarget1`) VALUES ('9', '62388', '6', '4', '1');
DELETE FROM `spell_misc` WHERE `ID` = '500004'; 
INSERT INTO `spell_misc` (`ID`, `Attributes1`, `Attributes3`, `Attributes4`, `Attributes14`, `CastingTimeIndex`, `DurationIndex`, `ActiveIconFileDataID`, `SpellIconFileDataID`, `SpellID`) VALUES 
('500004', '536871296', '4', '196608', '1', '1', '21', '136243', '136243', '62388');

DELETE FROM `spell_effect` WHERE `ID` = '299412'; 
DELETE FROM `spell_effect` WHERE `ID` = '299449'; 
INSERT INTO `spell_effect` (`ID`, `Effect`, `EffectBasePoints`, `EffectIndex`, `EffectAura`, `EffectChainAmplitude`, `PvpMultiplier`, `GroupSizeBasePointsCoefficient`, `EffectMiscValue1`, `ImplicitTarget1`, `SpellID`) VALUES 
('299412', '6', '3', '0', '31', '1', '1', '1', '0', '1', '203059'),
('299449', '6', '3', '1', '79', '1', '1', '1', '127', '1', '203059'); 
DELETE FROM `hotfix_data` WHERE `Id` = '20520' AND `TableHash` = '4030871717' AND `RecordID` = '299412' AND `Timestamp` = '0';
DELETE FROM `hotfix_data` WHERE `Id` = '20521' AND `TableHash` = '4030871717' AND `RecordID` = '299449' AND `Timestamp` = '0';
INSERT INTO `hotfix_data` (`Id`, `TableHash`, `RecordID`) VALUES 
('20520', '4030871717', '299412'),
('20521', '4030871717', '299449');

DELETE FROM `spell_categories` WHERE `ID` = '51114'; 
INSERT INTO `spell_categories` (`ID`, `StartRecoveryCategory`, `DefenseType`, `PreventionType`, `SpellID`) VALUES ('51114', '133', '1', '1', '199360'); 
DELETE FROM `spell_cooldowns` WHERE `ID` = '22915'; 
INSERT INTO `spell_cooldowns` (`ID`, `SpellID`) VALUES ('22915', '199360'); 
DELETE FROM `hotfix_data` WHERE `Id` = '20522' AND `TableHash` = '3689412649' AND `RecordID` = '51114'; 
INSERT INTO `hotfix_data` (`Id`, `TableHash`, `RecordID`) VALUES ('20522', '3689412649', '51114'); 
DELETE FROM `hotfix_data` WHERE `Id` = '20523' AND `TableHash` = '4193483863' AND `RecordID` = '22915'; 
INSERT INTO `hotfix_data` (`Id`, `TableHash`, `RecordID`) VALUES ('20523', '4193483863', '22915'); 

DELETE FROM `spell_categories` WHERE `ID` = '62964'; 
DELETE FROM `spell_cooldowns` WHERE `ID` = '28016'; 
DELETE FROM `spell_effect` WHERE `ID` = '372968'; 
DELETE FROM `hotfix_data` WHERE `Id` = '20525' AND `TableHash` = '3689412649' AND `RecordID` = '62964'; 
DELETE FROM `hotfix_data` WHERE `Id` = '20526' AND `TableHash` = '4193483863' AND `RecordID` = '28016'; 
DELETE FROM `hotfix_data` WHERE `Id` = '20527' AND `TableHash` = '4030871717' AND `RecordID` = '372968'; 
