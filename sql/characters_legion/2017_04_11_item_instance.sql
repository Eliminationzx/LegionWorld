ALTER TABLE `item_instance`
ADD COLUMN `isdonateitem`  tinyint(1) UNSIGNED NOT NULL DEFAULT 0 AFTER `challengeKeyIsCharded`;