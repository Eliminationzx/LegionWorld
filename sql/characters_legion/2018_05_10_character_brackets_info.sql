ALTER TABLE `character_brackets_info`   
  ADD COLUMN `oldRating` MEDIUMINT(9) DEFAULT 0 NOT NULL AFTER `rating`,
  ADD COLUMN `oldBest` MEDIUMINT(9) DEFAULT 0 NOT NULL AFTER `best`,
  ADD COLUMN `oldMmr` MEDIUMINT(9) DEFAULT 0 NOT NULL AFTER `mmr`,
  ADD COLUMN `oldGames` INT(11) DEFAULT 0 NOT NULL AFTER `games`,
  ADD COLUMN `oldWins` INT(11) DEFAULT 0 NOT NULL AFTER `wins`;
