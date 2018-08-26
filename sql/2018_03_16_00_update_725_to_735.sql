UPDATE characters AS ch, character_currency AS cc SET ch.money = ch.money + (cc.total_count * 10000/15) WHERE ch.guid = cc.guid AND cc.currency = 1356;
UPDATE characters AS ch, character_currency AS cc SET ch.money = ch.money + (cc.total_count * 10000/3) WHERE ch.guid = cc.guid AND cc.currency = 1357;
DELETE FROM `character_currency` WHERE `currency` = '1356';
DELETE FROM `character_currency` WHERE `currency` = '1357';
DELETE FROM `character_currency` WHERE `currency` = '1508';
DELETE FROM item_instance WHERE itemEntry IN (151564,151565,151566,151567,151568);
DELETE FROM item_instance WHERE itemEntry IN (137561,137563,137560,137565,137564,137562,152649,141350,140225,140222,140224,153248,153202,146902,146901,146900,146899,146898,146897,147361);
DELETE FROM item_instance WHERE itemEntry IN (147786,147791,147792,147793,147794,147795,147796,147797,147798,147799,147800,147801,147837);

-- Use this when open Argus
TRUNCATE `item_bonus_tree_node`;
TRUNCATE `item_level_selector`

UPDATE `pvp_reward` SET `BaseLevel` = '915' WHERE `Type` = '0'; 
UPDATE `pvp_reward` SET `BaseLevel` = '915' WHERE `Type` = '1'; 
UPDATE `pvp_reward` SET `BaseLevel` = '915' WHERE `Type` = '2'; 
UPDATE `pvp_reward` SET `BaseLevel` = '915' WHERE `Type` = '3'; 
UPDATE `pvp_reward` SET `BaseLevel` = '915' WHERE `Type` = '4'; 
UPDATE `pvp_reward` SET `BaseLevel` = '915' WHERE `Type` = '6'; 
UPDATE `pvp_reward` SET `ElitLevel` = '940' WHERE `Type` = '2'; 
UPDATE `pvp_reward` SET `ElitLevel` = '940' WHERE `Type` = '3'; 
UPDATE `pvp_reward` SET `ElitLevel` = '940' WHERE `Type` = '4'; 

DELETE FROM creature WHERE guid IN (12672929);
DELETE FROM npc_vendor WHERE entry IN (121589);
insert into `npc_vendor` (`entry`, `slot`, `item`, `maxcount`, `incrtime`, `ExtendedCost`, `money`, `type`, `PlayerConditionID`, `IgnoreFiltering`, `RandomPropertiesSeed`, `RandomPropertiesID`, `BonusListID`, `ItemModifiers`, `Context`, `VerifiedBuild`) values
('121589','0','153059','0','0','6240','0','1','0','0','0','0','','','0','21021'),
('121589','0','153060','0','0','6240','0','1','0','0','0','0','','','0','21021'),
('121589','0','153061','0','0','6240','0','1','0','0','0','0','','','0','21021'),
('121589','0','153062','0','0','6240','0','1','0','0','0','0','','','0','21021'),
('121589','0','153063','0','0','6240','0','1','0','0','0','0','','','0','21021'),
('121589','0','153064','0','0','6240','0','1','0','0','0','0','','','0','21021'),
('121589','0','153065','0','0','6240','0','1','0','0','0','0','','','0','21021'),
('121589','0','153066','0','0','6240','0','1','0','0','0','0','','','0','21021'),
('121589','0','153067','0','0','6240','0','1','0','0','0','0','','','0','21021'),
('121589','0','153068','0','0','6240','0','1','0','0','0','0','','','0','21021'),
('121589','0','153205','0','0','6240','0','1','0','0','0','0','','','0','21021'),
('121589','0','153206','0','0','6240','0','1','0','0','0','0','','','0','21021'),
('121589','0','153207','0','0','6240','0','1','0','0','0','0','','','0','21021'),
('121589','0','153208','0','0','6240','0','1','0','0','0','0','','','0','21021'),
('121589','20','153209','0','0','6240','0','1','0','0','0','0','','','14','26365'),
('121589','0','153210','0','0','6240','0','1','0','0','0','0','','','0','21021'),
('121589','0','153211','0','0','6240','0','1','0','0','0','0','','','0','21021'),
('121589','0','153212','0','0','6240','0','1','0','0','0','0','','','0','21021'),
('121589','0','153213','0','0','6240','0','1','0','0','0','0','','','0','21021'),
('121589','0','153214','0','0','6240','0','1','0','0','0','0','','','0','21021'),
('121589','0','153215','0','0','6240','0','1','0','0','0','0','','','0','21021'),
('121589','0','153216','0','0','6240','0','1','0','0','0','0','','','0','21021');

INSERT INTO `item_bonus_tree_node` (`ID`, `ChildItemBonusListID`, `ItemContext`, `BonusTreeID`, `VerifiedBuild`) VALUES ('1834', '3408', '13', '524', '26124');
