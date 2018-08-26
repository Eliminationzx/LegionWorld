UPDATE `creature_template` SET `ScriptName`='npc_stalactite_stalker' WHERE (`entry`='43159');

UPDATE `creature_template` SET `npcflag`='129', `unit_flags2`='0' WHERE (`entry`='87065');
UPDATE `creature_template` SET `AIName`='' WHERE (`entry`='44819');
UPDATE `creature_template` SET `AIName`='' WHERE (`entry`='54431');
UPDATE `creature_template` SET `AIName`='' WHERE (`entry`='54445');
UPDATE `creature_template` SET `AIName`='' WHERE (`entry`='54123');

DELETE FROM creature_text WHERE entry in (49045);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextID`, `comment`) VALUES
(49045, 0, 0, 'А-а-а-а!', 12, 0, 100, 0, 0, 0, 45530, 'Ауг to Бесноватый кроколиск');
