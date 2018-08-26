DELETE FROM spell_script_names WHERE spell_id in (229174,229175,229176,192767,192768,192769,192770,192771,192772,192773,192774,192775,192776,192761,192762,192763,192764,192765,192766,191464,237820,237821,237822,237823,237824,237825,237826,237827,237828);
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
('229174', 'spell_vantus_rune_trial_of_valor'),
('229175', 'spell_vantus_rune_trial_of_valor'),
('229176', 'spell_vantus_rune_trial_of_valor'),
('192767', 'spell_vantus_rune_the_nighthold'),
('192768', 'spell_vantus_rune_the_nighthold'),
('192769', 'spell_vantus_rune_the_nighthold'),
('192770', 'spell_vantus_rune_the_nighthold'),
('192771', 'spell_vantus_rune_the_nighthold'),
('192772', 'spell_vantus_rune_the_nighthold'),
('192773', 'spell_vantus_rune_the_nighthold'),
('192774', 'spell_vantus_rune_the_nighthold'),
('192775', 'spell_vantus_rune_the_nighthold'),
('192776', 'spell_vantus_rune_the_nighthold'),
('192761', 'spell_vantus_rune_the_emerald_nightmare'),
('192762', 'spell_vantus_rune_the_emerald_nightmare'),
('192763', 'spell_vantus_rune_the_emerald_nightmare'),
('192764', 'spell_vantus_rune_the_emerald_nightmare'),
('192765', 'spell_vantus_rune_the_emerald_nightmare'),
('192766', 'spell_vantus_rune_the_emerald_nightmare'),
('191464', 'spell_vantus_rune_the_emerald_nightmare'),
('237820', 'spell_vantus_rune_tomb_of_sargeras'),
('237821', 'spell_vantus_rune_tomb_of_sargeras'),
('237822', 'spell_vantus_rune_tomb_of_sargeras'),
('237823', 'spell_vantus_rune_tomb_of_sargeras'),
('237824', 'spell_vantus_rune_tomb_of_sargeras'),
('237825', 'spell_vantus_rune_tomb_of_sargeras'),
('237826', 'spell_vantus_rune_tomb_of_sargeras'),
('237827', 'spell_vantus_rune_tomb_of_sargeras'),
('237828', 'spell_vantus_rune_tomb_of_sargeras');

DELETE FROM spell_group_stack_rules WHERE group_id = 1002;
INSERT INTO `spell_group_stack_rules` (`group_id`, `stack_rule`) VALUES
('1002', '1');

DELETE FROM spell_group WHERE id in (1002);
INSERT INTO `spell_group` (`id`, `spell_id`) VALUES
('1002', '237820'),
('1002', '237821'),
('1002', '237822'),
('1002', '237823'),
('1002', '237824'),
('1002', '237825'),
('1002', '237826'),
('1002', '237827'),
('1002', '237828'),
('1002', '192761'),
('1002', '192762'),
('1002', '192763'),
('1002', '192764'),
('1002', '192765'),
('1002', '192766'),
('1002', '191464'),
('1002', '192767'),
('1002', '192768'),
('1002', '192769'),
('1002', '192770'),
('1002', '192771'),
('1002', '192772'),
('1002', '192773'),
('1002', '192774'),
('1002', '192775'),
('1002', '192776'),
('1002', '229174'),
('1002', '229175'),
('1002', '229176');