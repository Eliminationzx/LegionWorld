#include "ScriptMgr.h"
#include "ObjectMgr.h"
#include "Chat.h"
#include "WordFilterMgr.h"

class command_arena : public CommandScript
{
public:
    command_arena() : CommandScript("command_arena") { }

    ChatCommand* GetCommands() const override
    {
        static ChatCommand arenaDelCommandTable[] =
        {
            { "1x1",            SEC_GAMEMASTER,     false, &HandleArenaDel1x1Command,           "", NULL },
            { "2x2",            SEC_GAMEMASTER,     false, &HandleArenaDel2x2Command,           "", NULL },
            { "3x3",            SEC_GAMEMASTER,     false, &HandleArenaDel3x3Command,           "", NULL },
            { "rbg",            SEC_GAMEMASTER,     false, &HandleArenaDelRbgCommand,           "", NULL },
            { NULL,             0,                  false, NULL,                                "", NULL }
        };

        static ChatCommand arenaCommandTable[] =
        {
            { "del",            SEC_GAMEMASTER,     false, NULL,                                "", arenaDelCommandTable },
            { "closeseason",    SEC_ADMINISTRATOR,  false, &HandleCloseSeasonCommand,           "", NULL },
            { NULL,             0,                  false, NULL,                                "", NULL }
        };

        static ChatCommand antifloodCommandTable[] =
        {
            { "add",            SEC_GAMEMASTER,     false, &HandleAntiFloodAddPhrase,           "", NULL },
            { "update",         SEC_GAMEMASTER,     false, &HandleAntiFloodUpdatePharase,       "", NULL },
            { "list",           SEC_GAMEMASTER,     false, &HandleAntiFloodList,                "", NULL },
            { "addfromlfg",     SEC_GAMEMASTER,     false, &HandleAntiFloodAddFromLfgPhrase,    "", NULL },
            { NULL,             0,                  false, NULL,                                "", NULL }
        };

        static ChatCommand customCommandTable[] =
        {
            { "arenasliver",    SEC_GAMEMASTER,     false, &HandleArenaSliverCommand,           "", NULL },
            { "unarenasliver",  SEC_GAMEMASTER,     false, &HandleUnArenaSliverCommand,         "", NULL },
            { "lootcleanid",    SEC_GAMEMASTER,     false, &HandleLootCleanIdCommand,           "", NULL },
            { "unlootcleanid",  SEC_GAMEMASTER,     false, &HandleUnLootCleanIdCommand,         "", NULL },
            { "recoveryitem",   SEC_ADMINISTRATOR,  false, &HandleRecoveryItemCommand,          "", NULL },
            { "unrecoveryitem", SEC_ADMINISTRATOR,  false, &HandleUnRecoveryItemCommand,        "", NULL },
            { "listname",       SEC_GAMEMASTER,     false, &HandleListChangeName,               "", NULL },
            { "modmoney",       SEC_ADMINISTRATOR,  false, &HandleModMoneyCommand,              "", NULL },
            { "antiflood",      SEC_GAMEMASTER,     false, NULL,                                "", antifloodCommandTable},
            { NULL,             0,                  false, NULL,                                "", NULL }
        };

        // static ChatCommand logCommandTable[] =
        // {
            // { "addchar",        SEC_GAMEMASTER,     false, &HandleAddCharCommand,               "", NULL },
            // { "removechar",     SEC_GAMEMASTER,     false, &HandleRemoveCharCommand,            "", NULL },
            // { NULL,             0,                  false, NULL,                                "", NULL }
        // };

        static ChatCommand commandTable[] =
        {
            { "custom",         SEC_GAMEMASTER,     false, NULL,                                "", customCommandTable },
            { "arena",          SEC_GAMEMASTER,     false, NULL,                                "", arenaCommandTable },
            // { "log",            SEC_GAMEMASTER,   false, NULL,                                  "", logCommandTable },
            { NULL,             0,                  false, NULL,                                "", NULL }
        };

        return commandTable;
    }

    static bool HandleArenaDel1x1Command(ChatHandler* handler, const char* args)
    {
        return DelTeam(handler, args, 7);
    }

    static bool HandleArenaDel2x2Command(ChatHandler* handler, const char* args)
    {
        return DelTeam(handler, args, 0);
    }

    static bool HandleArenaDel3x3Command(ChatHandler* handler, const char* args)
    {
        return DelTeam(handler, args, 1);
    }

    static bool HandleArenaDelRbgCommand(ChatHandler* handler, const char* args)
    {
        return DelTeam(handler, args, 3);
    }

    static bool HandleArenaSliverCommand(ChatHandler* handler, const char* args)
    {
        Player* target;
        ObjectGuid target_guid;
        std::string target_name;
        if (!handler->extractPlayerTarget((char*)args, &target, &target_guid, &target_name))
            return false;

        CharacterDatabase.PQuery("insert into `character_reward` (`owner_guid`,`type`)value ('%u','8');", target_guid);
        std::string nameLink = handler->playerLink(target_name);
        handler->PSendSysMessage(20023, nameLink.c_str());
        return true;
    }

    static bool HandleUnArenaSliverCommand(ChatHandler* handler, const char* args)
    {
        Player* target;
        ObjectGuid target_guid;
        std::string target_name;
        if (!handler->extractPlayerTarget((char*)args, &target, &target_guid, &target_name))
            return false;

        CharacterDatabase.PQuery("DELETE FROM `character_reward` WHERE `type` = 8 AND `owner_guid` = '%u';", target_guid);
        std::string nameLink = handler->playerLink(target_name);
        handler->PSendSysMessage(20025, nameLink.c_str());
        return true;
    }

    static bool HandleUnLootCleanIdCommand(ChatHandler* handler, const char* args)
    {
        Player* target;
        ObjectGuid target_guid;
        std::string target_name;
        if (!handler->extractPlayerTarget((char*)args, &target, &target_guid, &target_name))
            return false;

        char* tail = strtok(NULL, "");

        // get from tail next item str
        while (char* itemStr = strtok(tail, " "))
        {
            // and get new tail
            tail = strtok(NULL, "");

            // parse item str
            char* itemIdStr = strtok(itemStr, ":");
            char* itemCountStr = strtok(NULL, " ");

            uint32 item_id = atoi(itemIdStr);
            if (!item_id)
                return false;
            ItemTemplate const* item_proto = sObjectMgr->GetItemTemplate(item_id);
            if (!item_proto)
                return false;

            uint32 item_count = itemCountStr ? atoi(itemCountStr) : 1;

            handler->PSendSysMessage(20026, item_id, item_count);
            CharacterDatabase.PQuery("DELETE FROM `character_reward` WHERE `type` = 9 AND `owner_guid` = '%u' AND `id` = %u AND `count` = %u;", target_guid, item_id, item_count);
        }
        return true;
    }

    static bool HandleLootCleanIdCommand(ChatHandler* handler, const char* args)
    {
        Player* target;
        ObjectGuid target_guid;
        std::string target_name;
        if (!handler->extractPlayerTarget((char*)args, &target, &target_guid, &target_name))
            return false;

        char* tail = strtok(NULL, "");

        // get from tail next item str
        while (char* itemStr = strtok(tail, " "))
        {
            // and get new tail
            tail = strtok(NULL, "");

            // parse item str
            char* itemIdStr = strtok(itemStr, ":");
            char* itemCountStr = strtok(NULL, " ");

            uint32 item_id = atoi(itemIdStr);
            if (!item_id)
                return false;
            ItemTemplate const* item_proto = sObjectMgr->GetItemTemplate(item_id);
            if (!item_proto)
                return false;

            uint32 item_count = itemCountStr ? atoi(itemCountStr) : 1;

            //sLog->outError("HandleLootCleanIdCommand item %u, target_guid %u, item_count %u", item_id, target_guid, item_count);

            handler->PSendSysMessage(20024, item_id, item_count);
            CharacterDatabase.PQuery("insert into `character_reward` (`owner_guid`,`type`,`id`,`count`)value ('%u','9','%u','%u');", target_guid, item_id, item_count);
        }

        std::string nameLink = handler->playerLink(target_name);
        handler->PSendSysMessage(20022, nameLink.c_str());
        return true;
    }

    static bool HandleUnRecoveryItemCommand(ChatHandler* handler, const char* args)
    {
        Player* target;
        ObjectGuid target_guid;
        std::string target_name;
        if (!handler->extractPlayerTarget((char*)args, &target, &target_guid, &target_name))
            return false;

        char* tail = strtok(NULL, "");

        // get from tail next item str
        while (char* itemStr = strtok(tail, " "))
        {
            // and get new tail
            tail = strtok(NULL, "");

            // parse item str
            char* itemIdStr = strtok(itemStr, ":");
            char* itemCountStr = strtok(NULL, " ");

            uint32 item_id = atoi(itemIdStr);
            if (!item_id)
                return false;
            ItemTemplate const* item_proto = sObjectMgr->GetItemTemplate(item_id);
            if (!item_proto)
                return false;

            uint32 item_count = itemCountStr ? atoi(itemCountStr) : 1;

            handler->PSendSysMessage(20026, item_id, item_count);
            CharacterDatabase.PQuery("DELETE FROM `character_reward` WHERE `type` = 10 AND `owner_guid` = '%u' AND `id` = %u AND `count` = %u;", target_guid, item_id, item_count);
        }
        return true;
    }

    static bool HandleListChangeName(ChatHandler* handler, const char* args)
    {
        Player* target;
        ObjectGuid target_guid;
        std::string target_name;
        if (!handler->extractPlayerTarget((char*)args, &target, &target_guid, &target_name))
            return false;

        QueryResult result;
        if(target)
            result = CharacterDatabase.PQuery("SELECT * FROM character_rename WHERE guid = %u", target->GetGUID().GetCounter());
        else if(target_guid)
            result = CharacterDatabase.PQuery("SELECT * FROM character_rename WHERE guid = %u", target_guid);
        else
            result = CharacterDatabase.PQuery("SELECT * FROM character_rename WHERE newname = '%s' or oldname = '%s'", target_name.c_str(), target_name.c_str());

        if(result)
        {
            do
            {
                Field * fetch = result->Fetch();
                ObjectGuid::LowType guid = fetch[0].GetUInt32();
                uint32 account = fetch[1].GetUInt32();
                std::string newname = fetch[2].GetString();
                std::string oldname = fetch[3].GetString();
                std::string date = fetch[4].GetString();

                handler->PSendSysMessage(20029, account, guid, newname.c_str(), oldname.c_str(), date.c_str());
            } while ( result->NextRow() );
        }
        else
            handler->PSendSysMessage("Cant find change name.");
        return true;
    }

    static bool HandleRecoveryItemCommand(ChatHandler* handler, const char* args)
    {
        Player* target;
        ObjectGuid target_guid;
        std::string target_name;
        if (!handler->extractPlayerTarget((char*)args, &target, &target_guid, &target_name))
            return false;

        char* tail = strtok(NULL, "");

        // get from tail next item str
        while (char* itemStr = strtok(tail, " "))
        {
            // and get new tail
            tail = strtok(NULL, "");

            // parse item str
            char* itemIdStr = strtok(itemStr, ":");
            char* itemCountStr = strtok(NULL, " ");

            uint32 item_id = atoi(itemIdStr);
            if (!item_id)
                return false;
            ItemTemplate const* item_proto = sObjectMgr->GetItemTemplate(item_id);
            if (!item_proto)
                return false;

            uint32 item_count = itemCountStr ? atoi(itemCountStr) : 1;

            //sLog->outError("HandleLootCleanIdCommand item %u, target_guid %u, item_count %u", item_id, target_guid, item_count);

            handler->PSendSysMessage(20024, item_id, item_count);
            CharacterDatabase.PQuery("insert into `character_reward` (`owner_guid`,`type`,`id`,`count`)value ('%u','10','%u','%u');", target_guid, item_id, item_count);
        }

        std::string nameLink = handler->playerLink(target_name);
        handler->PSendSysMessage(20022, nameLink.c_str());
        return true;
    }

    static bool DelTeam(ChatHandler* handler, const char* args, uint8 type)
    {
        Player* target;
        ObjectGuid target_guid;
        std::string target_name;
        if (!handler->extractPlayerTarget((char*)args, &target, &target_guid, &target_name))
            return false;

        //sLog->outString("giud: %u", target_guid);
        QueryResult bracketsResult = CharacterDatabase.PQuery("SELECT * FROM character_brackets_info WHERE guid = '%u' AND `bracket` = '%u'", target_guid.GetGUIDLow(), type);
        if (bracketsResult)
        {
            CharacterDatabase.PQuery("DELETE FROM character_brackets_info WHERE guid = '%u' AND `bracket` = '%u'", target_guid.GetGUIDLow(), type);
            handler->PSendSysMessage("Player: \"%s\" was deleted bracket %u stats.", target_name.c_str(), type);
            return true;
        }

        handler->PSendSysMessage("Cant find player bracket %u stats.", type);
        handler->SetSentErrorMessage(true);
        return false;
    }

    static bool HandleCloseSeasonCommand(ChatHandler* handler, const char* args)
    {
        bool debugOnly = false;
        if(!debugOnly)
            sWorld->SendWorldText(LANG_ARENA_CLOSESEASON_START);
        sLog->outArena("Окончание сезона Весна 2018");
        // by default:
        // 0 type - 1 - 3
        // 2 type - 4 - 12
        // 3 type - 13 - 25
        // 4 type - 26 - 50
        // 2c2
        {
            QueryResult teamResult = CharacterDatabase.Query("SELECT ch.guid, ch.name, oldWins, oldGames, oldRating, ch.account FROM `character_brackets_info` cbi LEFT JOIN characters ch ON ch.guid = cbi.guid WHERE `bracket` = 0 ORDER BY oldRating DESC");
            if (teamResult && teamResult->GetRowCount())
            {
                uint32 teamNumber = 1;
                do
                {
                    Field* Fields = teamResult->Fetch();
                    uint32 guid = uint32(Fields[0].GetUInt64());
                    std::string name = Fields[1].GetString();
                    uint32 wins = Fields[2].GetUInt32();
                    uint32 games = Fields[3].GetUInt32();
                    uint32 rating = Fields[4].GetUInt32();
                    uint32 account = Fields[5].GetUInt32();

                    uint32 titleId = 0;
                    uint32 achievId = 0;
                    uint32 tokenCount = 0;
                    if(!debugOnly)
                    {
                        if (teamNumber == 1)
                        {
                            achievId = 10096;
                            titleId = 453;
                            tokenCount = 1000;
                        }
                        if (teamNumber >= 2 && teamNumber <= 3)
                        {
                            achievId = 2091;
                            titleId = 42;
                            tokenCount = 500;
                        }
                        if (teamNumber >= 4 && teamNumber <= 5)
                        {
                            achievId = 2092;
                            titleId = 43;
                            tokenCount = 200;
                        }
                        if (teamNumber >= 6 && teamNumber <= 10)
                        {
                            achievId = 2093;
                            titleId = 44;
                            tokenCount = 150;
                        }
                        if (teamNumber >= 11 && teamNumber <= 18)
                        {
                            achievId = 2090;
                            titleId = 45;
                            tokenCount = 100;
                        }

                        if (tokenCount)
                        {
                            if (QueryResult result = LoginDatabase.PQuery("SELECT battlenet_account FROM account where id = %u", account))
                            {
                                uint32 battlenetID = (*result)[0].GetUInt32();
                                LoginDatabase.PQuery("UPDATE `battlenet_accounts` SET `balans` = `balans` + '%u' WHERE `id` = '%u';", tokenCount, battlenetID);
                                sLog->outArena("UPDATE `battlenet_accounts` SET `balans` = `balans` + '%u' WHERE `id` = '%u';", tokenCount, battlenetID);
                            }
                        }
                        if (titleId)
                        {
                            CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','2','%u','0')", guid, titleId);
                            sLog->outArena("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','2','%u','0')", guid, titleId);
                        }
                        if (achievId)
                        {
                            CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','1','%u','0')", guid, achievId);
                            sLog->outArena("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','1','%u','0')", guid, achievId);
                        }
                    }

                    if (tokenCount)
                        sLog->outArena("За %u место в топе 2на2 получил %s(%u) %u токенов, %u achievId, %u titleId, побед %u, игр %u, рейтинг %u", teamNumber, name.c_str(), guid, tokenCount, achievId, titleId, wins, games, rating);
                    teamNumber++;
                }
                while (teamResult->NextRow());
            }
        }

        // by default:
        // 1 - top 1-3
        // 2 - top 4-6
        // 3 - top offlokie 3%
        // 4 - top offlokie 10%
        // 5 - top offlokie 35%
        // 3c3
        // {
            // if(QueryResult arenaWinner = CharacterDatabase.PQuery("SELECT ch.guid, ch.name, oldWins, oldGames, oldRating, ch.account FROM `character_brackets_info` cbi LEFT JOIN characters ch ON ch.guid = cbi.guid WHERE `bracket` = 1 and oldRating > 1000 AND oldWins > 49 ORDER BY oldRating DESC"))
            // {
                // uint32 count = 1;
                // uint32 playerCount = arenaWinner->GetRowCount();
                // uint32 firstWinTypeCount = 3;
                // uint32 secondWinTypeCount = 6;
                // uint32 thirdWinTypeCount  = uint32(playerCount * 0.03f);
                // if(thirdWinTypeCount < 6)
                    // thirdWinTypeCount = 6;
                // uint32 fothWinTypeCount  = uint32(playerCount * 0.1f);
                // if(fothWinTypeCount < 6)
                    // fothWinTypeCount = 6;
                // uint32 fifthWinTypeCount  = uint32(playerCount * 0.35f);
                // if(fifthWinTypeCount < 6)
                    // fifthWinTypeCount = 6;

                // sLog->outArena("Количество чаров %u первая %u, вторая %u, третья %u, четвертая %u, петая %u", playerCount, firstWinTypeCount, secondWinTypeCount, thirdWinTypeCount, fothWinTypeCount, fifthWinTypeCount);

                // do
                // {
                    // Field* Fields = arenaWinner->Fetch();
                    // uint32 guid = uint32(Fields[0].GetUInt64());
                    // std::string name = Fields[1].GetString();
                    // uint32 wins = Fields[2].GetUInt32();
                    // uint32 games = Fields[3].GetUInt32();
                    // uint32 rating = Fields[4].GetUInt32();
                    // uint32 account = Fields[5].GetUInt32();

                    // //Season achivement Гладиатор / Gladiator
                    // if(count <= firstWinTypeCount)
                    // {
                        // if(!debugOnly)
                        // {
                            // if (QueryResult result = LoginDatabase.PQuery("SELECT battlenet_account FROM account where id = %u", account))
                            // {
                                // uint32 battlenetID = (*result)[0].GetUInt32();
                                // LoginDatabase.PQuery("UPDATE `battlenet_accounts` SET `balans` = `balans` + '%u' WHERE `id` = '%u';", 800, battlenetID);
                                // sLog->outArena("UPDATE `battlenet_accounts` SET `balans` = `balans` + '%u' WHERE `id` = '%u';", 800, battlenetID);
                            // }
                            // // CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','3','%u','1')", guid, 85785);
                            // // CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','3','%u','200')", guid, 38186);
                            // CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','1','%u','0')", guid, 11012);
                            // CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','2','%u','0')", guid, 502);
                        // }

                        // sLog->outArena("За %u место в топе 3на3(0.1) получил %s(%u) token=800, achiv=11012, title=502, побед %u, игр %u, рейтинг %u", count, name.c_str(), guid, wins, games, rating);
                    // }

                    // //Гладиатор / Gladiator
                    // if(count <= secondWinTypeCount)
                    // {
                        // if(!debugOnly && (count > firstWinTypeCount))
                        // {
                            // if (QueryResult result = LoginDatabase.PQuery("SELECT battlenet_account FROM account where id = %u", account))
                            // {
                                // uint32 battlenetID = (*result)[0].GetUInt32();
                                // LoginDatabase.PQuery("UPDATE `battlenet_accounts` SET `balans` = `balans` + '%u' WHERE `id` = '%u';", 1200, battlenetID);
                                // sLog->outArena("UPDATE `battlenet_accounts` SET `balans` = `balans` + '%u' WHERE `id` = '%u';", 1200, battlenetID);
                            // }
                            // CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','1','%u','0')", guid, 3436);
                            // CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','2','%u','0')", guid, 167);
                            // sLog->outArena("За %u место в топе 3на3(0.5) получил %s(%u) efir=600, achiv=3436, title=167, побед %u, игр %u, рейтинг %u", count, name.c_str(), guid, wins, games, rating);
                        // }

                        // if(!debugOnly)
                        // {
                            // CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','1','%u','0')", guid, 2091);
                            // CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','2','%u','0')", guid, 42);
                        // }

                        // sLog->outArena("За %u место в топе 3на3(0.5) получил %s(%u) efir=250, achiv=2091, title=42, побед %u, игр %u, рейтинг %u", count, name.c_str(), guid, wins, games, rating);
                    // }

                    // //Дуэлянт / Duelist
                    // if(count <= thirdWinTypeCount)
                    // {
                        // if(!debugOnly)
                        // {
                            // CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','1','%u','0')", guid, 2092);
                            // CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','2','%u','0')", guid, 43);
                        // }

                        // sLog->outArena("За %u место в топе 3на3(3.0) получил %s(%u) achiv=2092, title=43, побед %u, игр %u, рейтинг %u", count, name.c_str(), guid, wins, games, rating);
                    // }

                    // //Фаворит / Rival
                    // if(count <= fothWinTypeCount)
                    // {
                        // if(!debugOnly)
                        // {
                            // CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','1','%u','0')", guid, 2093);
                            // CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','2','%u','0')", guid, 44);
                        // }

                        // sLog->outArena("За %u место в топе 3на3(10.0) получил %s(%u) achiv=2093, title=44, побед %u, игр %u, рейтинг %u", count, name.c_str(), guid, wins, games, rating);
                    // }

                    // //Претендент / Challenger
                    // if(count <= fifthWinTypeCount)
                    // {
                        // if(!debugOnly)
                        // {
                            // CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','1','%u','0')", guid, 2090);
                            // CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','2','%u','0')", guid, 45);
                        // }

                        // sLog->outArena("За %u место в топе 3на3(35.0) получил %s(%u) achiv=2090, title=45, побед %u, игр %u, рейтинг %u", count, name.c_str(), guid, wins, games, rating);
                    // }
                    // count++;
                // }
                // while (arenaWinner->NextRow());
            // }
        // }

        // by default:
        // 1 - top offlokie 0.1%
        // 2 - top offlokie 0.5%
        // 3 - top offlokie 3%
        // 4 - top offlokie 10%
        // 5 - top offlokie 35%
        // 5c5
        // {
            // if(QueryResult arenaWinner = CharacterDatabase.PQuery("SELECT ch.guid, ch.name, oldWins, oldGames, oldRating FROM `character_brackets_info` cbi LEFT JOIN characters ch ON ch.guid = cbi.guid WHERE `bracket` = 2 and oldRating > 1000 AND oldWins > 49 ORDER BY oldRating DESC"))
            // {
                // uint32 count = 1;
                // uint32 playerCount = arenaWinner->GetRowCount();
                // uint32 firstWinTypeCount = uint32(playerCount * 0.001f);
                // if(firstWinTypeCount < 5)
                    // firstWinTypeCount = 5;
                // uint32 secondWinTypeCount = uint32(playerCount * 0.005f);
                // if(secondWinTypeCount < 5)
                    // secondWinTypeCount = 5;
                // uint32 thirdWinTypeCount  = uint32(playerCount * 0.03f);
                // if(thirdWinTypeCount < 5)
                    // thirdWinTypeCount = 5;
                // uint32 fothWinTypeCount  = uint32(playerCount * 0.1f);
                // if(fothWinTypeCount < 5)
                    // fothWinTypeCount = 5;
                // uint32 fifthWinTypeCount  = uint32(playerCount * 0.35f);
                // if(fifthWinTypeCount < 5)
                    // fifthWinTypeCount = 5;

                // do
                // {
                    // Field* Fields = arenaWinner->Fetch();
                    // uint32 guid = uint32(Fields[0].GetUInt64());
                    // std::string name = Fields[1].GetString();
                    // uint32 wins = Fields[2].GetUInt32();
                    // uint32 games = Fields[3].GetUInt32();
                    // uint32 rating = Fields[4].GetUInt32();

                    // //Season achivement Гладиатор / Gladiator
                    // if(count <= firstWinTypeCount)
                    // {
                        // if(!debugOnly)
                        // {
                            // CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','3','%u','1')", guid, 85785);
                            // CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','3','%u','200')", guid, 38186);
                            // CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','1','%u','0')", guid, 6938);
                            // CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','2','%u','0')", guid, 281);
                        // }

                        // sLog->outArena("За %u место в топе 5на5(0.1) получил %s(%u) efir=200, item=85785, achiv=6938, title=281, побед %u, игр %u, рейтинг %u", count, name.c_str(), guid, wins, games, rating);
                    // }

                    // //Гладиатор / Gladiator
                    // if(count <= secondWinTypeCount)
                    // {
                        // if(!debugOnly)
                        // {
                            // CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','3','%u','250')", guid, 38186);
                            // CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','1','%u','0')", guid, 2091);
                            // CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','2','%u','0')", guid, 42);
                        // }

                        // sLog->outArena("За %u место в топе 5на5(0.5) получил %s(%u) efir=250, achiv=2091, title=42, побед %u, игр %u, рейтинг %u", count, name.c_str(), guid, wins, games, rating);
                    // }

                    // //Дуэлянт / Duelist
                    // if(count <= thirdWinTypeCount)
                    // {
                        // if(!debugOnly)
                        // {
                            // CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','1','%u','0')", guid, 2092);
                            // CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','2','%u','0')", guid, 43);
                        // }

                        // sLog->outArena("За %u место в топе 5на5(3.0) получил %s(%u) achiv=2092, title=43, побед %u, игр %u, рейтинг %u", count, name.c_str(), guid, wins, games, rating);
                    // }

                    // //Фаворит / Rival
                    // if(count <= fothWinTypeCount)
                    // {
                        // if(!debugOnly)
                        // {
                            // CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','1','%u','0')", guid, 2093);
                            // CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','2','%u','0')", guid, 44);
                        // }

                        // sLog->outArena("За %u место в топе 5на5(10.0) получил %s(%u) achiv=2093, title=44, побед %u, игр %u, рейтинг %u", count, name.c_str(), guid, wins, games, rating);
                    // }

                    // //Претендент / Challenger
                    // if(count <= fifthWinTypeCount)
                    // {
                        // if(!debugOnly)
                        // {
                            // CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','1','%u','0')", guid, 2090);
                            // CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','2','%u','0')", guid, 45);
                        // }

                        // sLog->outArena("За %u место в топе 5на5(35.0) получил %s(%u) achiv=2090, title=45, побед %u, игр %u, рейтинг %u", count, name.c_str(), guid, wins, games, rating);
                    // }
                    // count++;
                // }
                // while (arenaWinner->NextRow());
            // }
        // }

        // by default:
        // 1 - top offlokie 0.5%
        // RBG
        // {
            // if(QueryResult arenaWinner = CharacterDatabase.PQuery("SELECT ch.guid, ch.name, oldWins, oldGames, oldRating, ch.race, ch.account FROM `character_brackets_info` cbi LEFT JOIN characters ch ON ch.guid = cbi.guid WHERE `bracket` = 3 and oldRating > 1000 AND oldWins > 49 ORDER BY oldRating DESC"))
            // {
                // uint32 count = 1;
                // uint32 playerCount = arenaWinner->GetRowCount();
                // uint32 firstWinTypeCount = uint32(playerCount * 0.005f);
                // if(firstWinTypeCount < 10)
                    // firstWinTypeCount = 10;

                // do
                // {
                    // Field* Fields = arenaWinner->Fetch();
                    // uint32 guid = uint32(Fields[0].GetUInt64());
                    // std::string name = Fields[1].GetString();
                    // uint32 wins = Fields[2].GetUInt32();
                    // uint32 games = Fields[3].GetUInt32();
                    // uint32 rating = Fields[4].GetUInt32();
                    // uint32 team = Player::TeamForRace(Fields[5].GetUInt8());
                    // uint32 account = Fields[6].GetUInt32();
                    // uint32 achivement = team == HORDE ? 6941 : 6942;

                    // //Season achivement Герой Орды / Hero of the Horde / Hero of the Alliance / Герой Альянса
                    // if(count <= firstWinTypeCount)
                    // {
                        // if(!debugOnly)
                        // {
                            // if (QueryResult result = LoginDatabase.PQuery("SELECT battlenet_account FROM account where id = %u", account))
                            // {
                                // uint32 battlenetID = (*result)[0].GetUInt32();
                                // LoginDatabase.PQuery("UPDATE `battlenet_accounts` SET `balans` = `balans` + '%u' WHERE `id` = '%u';", 500, battlenetID);
                                // sLog->outArena("UPDATE `battlenet_accounts` SET `balans` = `balans` + '%u' WHERE `id` = '%u';", 500, battlenetID);
                            // }
                            // CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','1','%u','0')", guid, achivement);
                        // }

                        // sLog->outArena("За %u место в топе РБГ(0.5) получил %s(%u) efir=250, achiv=%u побед %u, игр %u, рейтинг %u", count, name.c_str(), guid, achivement, wins, games, rating);
                    // }
                    // count++;
                // }
                // while (arenaWinner->NextRow());
            // }
        // }

        if(!debugOnly)
            sWorld->SendWorldText(LANG_ARENA_CLOSESEASON_END);
        return true;
    }

    // static bool HandleAddCharCommand(ChatHandler* handler, const char* args)
    // {
        // std::string name = args;
        // if (ObjectGuid guid = sObjectMgr->GetPlayerGUIDByName(name))
        // {
            // sObjectMgr->AddCharToDupeLog(guid);
            // handler->PSendSysMessage("Added.");
        // }

        // return true;
    // }

    // static bool HandleRemoveCharCommand(ChatHandler* handler, const char* args)
    // {
        // std::string name = args;
        // if (ObjectGuid guid = sObjectMgr->GetPlayerGUIDByName(name))
        // {
            // sObjectMgr->RemoveCharFromDupeList(guid);
            // handler->PSendSysMessage("Removed.");
        // }

        // return true;
    // }
    
    static bool HandleModMoneyCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        Player* target;
        ObjectGuid target_guid;
        std::string target_name;
        if (!handler->extractPlayerTarget((char*)args, &target, &target_guid, &target_name))
            return false;

        char* money = strtok(NULL, "");
        if (!money)
            return false;
        
        int64 addmoney = atoi(money);
        addmoney = addmoney * 10000; // only gold

        if (target) // if online
        {
            int64 moneyuser = static_cast<int64>(target->GetMoney());

            if (addmoney < 0)
            {
                int64 newmoney = int64(moneyuser) + addmoney;

                if (newmoney <= 0)
                {
                    handler->PSendSysMessage(LANG_YOU_TAKE_ALL_MONEY, handler->GetNameLink(target).c_str());
                    if (handler->needReportToTarget(target))
                        (ChatHandler(target)).PSendSysMessage(LANG_YOURS_ALL_MONEY_GONE, handler->GetNameLink().c_str());

                    target->SetMoney(0);
                }
                else
                {
                    if (newmoney > MAX_MONEY_AMOUNT)
                        newmoney = MAX_MONEY_AMOUNT;

                    handler->PSendSysMessage(LANG_YOU_TAKE_MONEY, abs(addmoney), handler->GetNameLink(target).c_str());
                    if (handler->needReportToTarget(target))
                        (ChatHandler(target)).PSendSysMessage(LANG_YOURS_MONEY_TAKEN, handler->GetNameLink().c_str(), abs(addmoney));
                    target->SetMoney(newmoney);
                }
            }
            else
            {
                handler->PSendSysMessage(LANG_YOU_GIVE_MONEY, uint32(addmoney), handler->GetNameLink(target).c_str());
                if (handler->needReportToTarget(target))
                    (ChatHandler(target)).PSendSysMessage(LANG_YOURS_MONEY_GIVEN, handler->GetNameLink().c_str(), uint32(addmoney));

                if (addmoney >=MAX_MONEY_AMOUNT)
                    target->SetMoney(MAX_MONEY_AMOUNT);
                else
                    target->ModifyMoney(int64(addmoney));
            }
            sLog->outDebug(LOG_FILTER_GENERAL, handler->GetTrinityString(LANG_NEW_MONEY), moneyuser, uint32(addmoney), target->GetMoney());
        }
        else if (target_guid)
        {
            
            QueryResult result = CharacterDatabase.PQuery("SELECT money FROM characters WHERE (`guid`= '%u')", target_guid);
            if(result)
            {
               // do
                {
                    Field * fetch = result->Fetch();
                    uint64 now_money = fetch[0].GetUInt64();
                    
                    int64 newmoney = int64(now_money) + addmoney;
                    if (addmoney < 0)
                    {
                        if (newmoney <= 0)
                        {
                            handler->PSendSysMessage(LANG_YOU_TAKE_ALL_MONEY, target_name.c_str());
                            CharacterDatabase.PQuery("UPDATE `characters` SET `money`= 0 WHERE (`guid`= '%u')", target_guid);
                        }
                        else
                        {
                            if (newmoney > MAX_MONEY_AMOUNT)
                                newmoney = MAX_MONEY_AMOUNT;
                            
                            handler->PSendSysMessage(LANG_YOU_TAKE_MONEY, abs(addmoney), target_name.c_str());
                            CharacterDatabase.PQuery("UPDATE `characters` SET `money` = " SI64FMTD " WHERE (`guid`= '%u')", newmoney, target_guid);
                        }
                    }
                    else
                    {
                        handler->PSendSysMessage(LANG_YOU_GIVE_MONEY, uint32(addmoney), target_name.c_str());
                        if (newmoney > MAX_MONEY_AMOUNT)
                            newmoney = MAX_MONEY_AMOUNT;
                        CharacterDatabase.PQuery("UPDATE `characters` SET `money` = " SI64FMTD " WHERE (`guid`= '%u')", newmoney, target_guid);
                    }
                        
                    handler->PSendSysMessage("The operation was conducted in offline-mod. Success");
                }// while ( result->NextRow() );
            }
            else
                handler->PSendSysMessage("Unknown error on offline operation! (Char is delete?????)");
                // CharacterDatabase.PQuery("UPDATE `characters` SET `money`= money + %d WHERE (`guid`= '%u')", addmoney, target_guid);
        }
        else
        {
            handler->PSendSysMessage("Unknown error. Write to Vict for fix it.");
        }
        return true;
    }

    static bool HandleAntiFloodAddPhrase(ChatHandler* handler, const char* args)
    {
        char* penaltyCh = strtok((char*)args, ", ");

        if (!penaltyCh)
            return false;

        int penalty = atoi(penaltyCh);
        char* text = strtok(NULL, "");
        if (!text)
            return false;

        size_t hash = sWordFilterMgr->FilterMsgAndGetHash(text);
        if (sWordFilterMgr->GetIdOfBadSentence(hash) && sWordFilterMgr->GetSourceMaskForBadSentence(hash) == (BS_LFG + BS_MAIL + BS_PM))
        {
            handler->PSendSysMessage("Just exist!");
            return true;
        }

        sWordFilterMgr->AddBadSentence(text, hash, BS_LFG + BS_MAIL + BS_PM, penalty);
        handler->PSendSysMessage("Succesfully added penalty %d for phrase \"%s\" with id = %u", penalty, text, sWordFilterMgr->GetLastIdBadSentences());
        return true;
    }

    static bool HandleAntiFloodUpdatePharase(ChatHandler* handler, const char* args)
    {
        char* idCh = strtok((char*)args, " ");
        if (!idCh)
            return false;

        int id = atoi(idCh);
        if (id <= 0)
            return false;

        char* penaltyCh = strtok(NULL, "");
        if (!penaltyCh)
            return false;

        int32 penalty = atoi(penaltyCh);

        sWordFilterMgr->UpdatePenaltyForBadSentenceById(id, penalty);

        handler->PSendSysMessage("Success!");
        return true;
    }

    static bool HandleAntiFloodList(ChatHandler* handler, const char* args)
    {
        int page = atoi(args);
        if (!page)
            page = 1;

        auto list = sWordFilterMgr->GetBadSentenceList(page);
        for (auto info : list)
        {
            std::stringstream ss;
            for (auto mask : { BS_PM , BS_MAIL, BS_LFG })
                if (info.sourceMask & mask)
                {
                    switch (mask)
                    {
                    case BS_PM:
                        ss << "PM ";
                        break;
                    case BS_MAIL:
                        ss << "MAIL ";
                        break;
                    case BS_LFG:
                        ss << "LFG ";
                        break;
                    }
                }
            handler->PSendSysMessage("Id: %u Text: \"%s\" Penalty: %d Mask: %s", info.id, info.text.c_str(), info.penalty, ss.str().c_str());
        }

        handler->PSendSysMessage("Max number: %u (Max page: %.0f)", sWordFilterMgr->GetLastIdBadSentences(), ceil(float(sWordFilterMgr->GetLastIdBadSentences()) / 25.0f));
        return true;

    }

    static bool HandleAntiFloodAddFromLfgPhrase(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        Player* target;
        ObjectGuid target_guid;
        std::string target_name;
        if (!handler->extractPlayerTarget((char*)args, &target, &target_guid, &target_name))
            return false;

        if (!target)
            return false;

        char* penaltych = strtok(NULL, "");
        if (!penaltych)
            return false;
        
        int32 penalty = atoi(penaltych);
        
        std::string& text = target->lastLfgPhrase;
        if (text.empty())
            return false;

        auto hash = sWordFilterMgr->FilterMsgAndGetHash(text);
        if (sWordFilterMgr->GetIdOfBadSentence(hash) && sWordFilterMgr->GetSourceMaskForBadSentence(hash) == (BS_LFG + BS_MAIL + BS_PM))
        {
            handler->PSendSysMessage("Just exist!");
            return true;
        }

        if (sWordFilterMgr->AddBadSentence(text, hash, BS_LFG+BS_MAIL+BS_PM , penalty))
            handler->PSendSysMessage("Succesfully add bad phrase: \"%s\" with penalty %d by id %u", text.c_str(), penalty, sWordFilterMgr->GetIdOfBadSentence(hash));
        else
            handler->PSendSysMessage("Error this fras just exist with penalty %u by id %u", penalty, sWordFilterMgr->GetIdOfBadSentence(hash));


        if (penalty < 0 && target->GetSession()->_hwid != 0)
        {
            target->GetSession()->_countPenaltiesHwid = -1;
            std::stringstream reasonStr;
            reasonStr << "-Флуд №" << sWordFilterMgr->GetIdOfBadSentence(hash) << " Realm: " << sWorld->GetRealmName().c_str();
            LoginDatabase.PQuery("REPLACE INTO hwid_penalties VALUES (" UI64FMTD ", -1, \"%s\")", target->GetSession()->_hwid, reasonStr.str().c_str());
        }

        return true;
    }

};

void AddSC_command_arena()
{
    new command_arena();
}
