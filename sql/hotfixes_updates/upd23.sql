CREATE TABLE `vignette`(  
  `ID` INT(10) UNSIGNED NOT NULL DEFAULT 0,
  `Name` TEXT NOT NULL,
  `UnkFloat1` FLOAT NOT NULL DEFAULT 0,
  `UnkFloat2` FLOAT NOT NULL DEFAULT 0,
  `QuestUnk` INT(10) NOT NULL DEFAULT 0,
  `QuestID` INT(10) NOT NULL DEFAULT 0,
  `Unk1` TINYINT(3) NOT NULL DEFAULT 0,
  `Unk2` TINYINT(3) NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=MYISAM CHARSET=utf8 COLLATE=utf8_general_ci;

DROP TABLE IF EXISTS `vignette_locale`;
CREATE TABLE `vignette_locale` (
  `ID` INT(10) UNSIGNED NOT NULL DEFAULT '0',
  `locale` VARCHAR(4) NOT NULL,
  `Name_lang` TEXT,
  `VerifiedBuild` SMALLINT(6) NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`,`locale`)
) ENGINE=MYISAM DEFAULT CHARSET=utf8;
