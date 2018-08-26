#include <ScriptPCH.h>
#include <ScriptMgr.h>
#include "GameEventMgr.h"
#include "Player.h"
#include "QuestData.h"
#include "Bag.h"
#include "Channel.h"
#include "ChatLink.h"
#include "WordFilterMgr.h"


#define BadColor() {ChatHandler(player).PSendSysMessage("You can't use color on chat! 000"); msg = ""; return false; }

class playerScriptPvpMisc : public PlayerScript
{
public:
    playerScriptPvpMisc() : PlayerScript("playerScriptPvpMisc") { }

    void OnLogin(Player* player) override
    {
        player->AddDelayedEvent(100, [player]() -> void
        {
            if (!player)
                return;

            if (player->getLevel() == MAX_LEVEL)
                for (uint32 questID : {44891 /* 2v2 Weekly Quest UI*/, 44908 /*3v3 Weekly Quest UI*/, 44909 /*10v10 Weekly Quest UI*/})
                {
                    auto quest = sQuestDataStore->GetQuestTemplate(questID);
                    if (!player->IsQuestRewarded(questID))
                        player->AddQuest(quest, nullptr);
                }
        });
    }
};

class playerScriptCheckArts : public PlayerScript
{
public:
    playerScriptCheckArts() : PlayerScript("playerScriptCheckArts") {}

    void OnLogin(Player* player) override
    {
        player->AddDelayedEvent(10000, [player]() -> void
        {
            if (!player)
                return;

            if (player->AllArtifacts.empty())
                return;

            if (!player->m_mailsLoaded)
                player->_LoadMail();

            for (std::pair<ObjectGuid, uint32> ArtIt : player->AllArtifacts)
            {
                if (!player->HasItemCount(ArtIt.second))
                {
                    if (!player->SearchItemOnMail(ArtIt.second))
                    {
                        ItemPosCountVec dest;
                        InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, ArtIt.second, 1);
                        if (msg != EQUIP_ERR_OK)
                        {
                            ChatHandler(player).PSendSysMessage("You don't have free slot for create your artifact with entry %d . Free up space in bags for him and make a re login", ArtIt.second);
                            continue;
                        }

                        PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_ITEM_INFO);
                        stmt->setUInt64(0, ArtIt.first.GetGUIDLow());

                        if (PreparedQueryResult artifactsResult = CharacterDatabase.Query(stmt))
                        {
                            Field* art_fields = artifactsResult->Fetch();

                            ItemTemplate const* itemProto = sObjectMgr->GetItemTemplate(ArtIt.second);
                            Item* item = NewItemOrBag(itemProto);

                            if (item->LoadFromDB(ArtIt.first.GetGUIDLow(), player->GetGUID(), art_fields, ArtIt.second, player->getLevel()))
                            {
                                player->StoreItem(dest, item, true);
                                ChatHandler(player).PSendSysMessage("Your artifact with entry %d was return, but for his correctly work you need to make a re login (relog)", ArtIt.second);
                                continue; // all are ok! Will check next item;
                            }
                            else
                            {
                                item->FSetState(ITEM_REMOVED);

                                SQLTransaction temp = SQLTransaction(NULL);
                                item->SaveToDB(temp);                               // it also deletes item object !
                            }
                        }

                        uint32 space = 0;
                        player->AddItem(ArtIt.second, 1, &space, ArtIt.first);
                        ChatHandler(player).PSendSysMessage("Your artifact with entry %d was return, but for his correctly work you need to make a re login (relog)", ArtIt.second);
                    }
                    else
                        ChatHandler(player).PSendSysMessage("Notification: Your artifact with entry %d is waiting for you by mail! ", ArtIt.second);
                }
            }
        });
    }
};

class player_informated_about_rates : public PlayerScript
{
public:
    player_informated_about_rates() : PlayerScript("player_informated_about_rates") {}

    void OnLogin(Player* player) override
    {
        if (!sWorld->getRate(RATE_XP_CHANGE_MAX) || !sWorld->getRate(RATE_XP_CHANGE_STEP))
            return;
        
        if (sWorld->getBoolConfig(CONFIG_FUN_OPTION_ENABLED))
            return;
        
        player->AddDelayedEvent(10000, [player] () -> void
        {
            if (!player)
                return;
            
            if (Creature* announcer = player->FindNearestCreature(230007, 30.0f, true))
                announcer->AI()->Talk(0, player->GetGUID());
        });
            
    }
};

class player_fun_welcome_info : public PlayerScript
{
public:
    player_fun_welcome_info() : PlayerScript("player_fun_welcome_info") {}

    void OnLogin(Player* player, bool firstLogin) override
    {
        if (!sWorld->getBoolConfig(CONFIG_FUN_OPTION_ENABLED) || !firstLogin)
            return;
        
        player->AddDelayedEvent(5000, [player] () -> void
        {
            if (!player)
                return;
        
            if (Creature* announcer = player->SummonCreature(230008, player->GetPositionX() + irand(-7, 7), player->GetPositionY() + irand(-7, 7), player->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 63000, player->GetGUID()))
            {
                // announcer->AddPlayerInPersonnalVisibilityList(player->GetGUID());
                announcer->CastSpell(announcer, 70618);
                announcer->AddDelayedEvent(2000, [announcer] () -> void
                {
                    if (Unit* owner = announcer->GetAnyOwner())
                        announcer->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST, owner->GetFollowAngle());
                });
                announcer->AddDelayedEvent(60000, [announcer] () -> void
                {
                    announcer->CastSpell(announcer, 70618);
                    announcer->DespawnOrUnsummon(1500);
                });
            }
        });
    }
};

class player_learn_warforge: public PlayerScript
{
public:
    player_learn_warforge() : PlayerScript("player_learn_warforge") { }

    void OnSpellLearned(Player* player, uint32 spellID) override
    {
        if (spellID == 163024)
        {

            Item* pItem = Item::CreateItem(112324, 1, player);
            if (!pItem)
                return;

            MailSender sender(MAIL_NORMAL, player->GetGUIDLow(), MAIL_STATIONERY_GM);
            
            SQLTransaction trans = CharacterDatabase.BeginTransaction();
            MailDraft("Nightmarish Hitching Post", "Thank you for your purchase!").AddItem(pItem).SendMailTo(trans, MailReceiver(player, player->GetGUIDLow()), sender);

            CharacterDatabase.CommitTransaction(trans);
        }
    }
};

 class System_Flood_And_RSP : public PlayerScript
{
public:
    System_Flood_And_RSP() : PlayerScript("System_Flood_And_RSP") {}
    
    void OnWhoListCall(Player* player, const std::set<ObjectGuid>& playersGuids) override
    {
        PlayerFloodInfo& info = sWordFilterMgr->GetFloodInfo(player->GetSession()->GetAccountId());
        info.players.insert(playersGuids.begin(), playersGuids.end());
    }
 
    void OnChat(Player* player, uint32 type, uint32 lang, std::string& msg) override
    {
        PenaltyPlayer(player, CheckMessage(player, msg, lang, type));
    }
 
    void OnChat(Player* player, uint32 type, uint32 lang, std::string& msg, Player* receiver) override
    {
        PenaltyPlayer(player, CheckMessage(player, msg, lang, type, NULL, (receiver ? receiver->GetGUID() : ObjectGuid::Empty)));
    }
 
    void OnChat(Player* player, uint32 type, uint32 lang, std::string& msg, Group* group) override
    {
        PenaltyPlayer(player, CheckMessage(player, msg, lang, type));
    }
 
    void OnChat(Player* player, uint32 type, uint32 lang, std::string& msg, Guild* guild) override
    {
        PenaltyPlayer(player, CheckMessage(player, msg, lang, type));
    }
 
    void OnChat(Player* player, uint32 type, uint32 lang, std::string& msg, Channel* channel) override
    {
       PenaltyPlayer(player, CheckMessage(player, msg, lang, type, channel));
    }
    
    void OnSendMail(Player* player, std::string& subject, std::string& body, ObjectGuid receiver) override
    {
        std::string unionMessage = body + subject;
            
        auto check = CheckMessage(player, unionMessage, 0, CHAT_MSG_BN_WHISPER, NULL, receiver);
        if ((check.first && check.second) || unionMessage.empty())
        {
            subject = "";
            body = "";
            if (check.first && check.second)
                PenaltyPlayer(player, check);
        }
    }

    bool CheckOnColor(Player* player, std::string& msg)
    {
       // ChatLink* link = nullptr;
        std::unique_ptr<ChatLink> link = nullptr;

        size_t pos = 0;
        uint32 id = 0;
        if ((pos = msg.find("|Hitem:")) != std::string::npos)
        {
            size_t end_pos = msg.find(":", pos + 7);
            if (end_pos - (pos + 7) >= 1 && end_pos != std::string::npos)
            {
                id = atoul(msg.substr(pos + 7, (end_pos - (pos + 7))).c_str());
                if (id)
                    link.reset(new ItemChatLink(id));
                else
                    BadColor(); // macro
            }
            else
                BadColor(); // macro
                
        }
        else if ((pos = msg.find("|Hquest:")) != std::string::npos)
        {
            size_t end_pos = msg.find(":", pos + 8);
            if (end_pos - (pos + 8) >= 1 && end_pos != std::string::npos)
            {
                id = atoul(msg.substr(pos + 8, (end_pos - (pos + 8))).c_str());
                if (id)
                    link.reset(new QuestChatLink(id));
                else
                    BadColor(); // macro
            }
            else
                BadColor(); // macro
        }
        else if ((pos = msg.find("|Htrade:")) != std::string::npos)  // |cffffd000|Htrade:Player-8-00000030:spell:202|h[name]|h|r
        {
            size_t start_pos = msg.find(":", pos + 8);
            if (start_pos == std::string::npos)
                BadColor();

            size_t end_pos = msg.find(":", start_pos + 1);
            if (end_pos - (start_pos + 1) >= 1 && end_pos != std::string::npos)
            {
                id = atoul(msg.substr(start_pos + 1, (end_pos - (start_pos + 1))).c_str());
                if (id)
                    link.reset(new TradeChatLink(id));
                else
                    BadColor(); // macro
            }
            else
                BadColor(); // macro
        }
        else if ((pos = msg.find("|Htalent:")) != std::string::npos)
        {
            size_t end_pos = msg.find("|", pos + 9);
            if (end_pos - (pos + 9) >= 1 && end_pos != std::string::npos)
            {
                id = atoul(msg.substr(pos + 9, (end_pos - (pos + 9))).c_str());
                if (id)
                    link.reset(new TalentChatLink(id));
                else
                    BadColor(); // macro
            }
            else
                BadColor(); // macro
        }
        else if ((pos = msg.find("|Hspell:")) != std::string::npos)
        {
            size_t end_pos = msg.find(":", pos + 8);
            if (end_pos - (pos + 8) >= 1 && end_pos != std::string::npos)
            {
                id = atoul(msg.substr(pos + 8, (end_pos - (pos + 8))).c_str());
                if (id)
                    link.reset(new SpellChatLink(id));
                else
                    BadColor(); // macro
            }
            else
                BadColor(); // macro
        }
        else if ((pos = msg.find("|Henchant:")) != std::string::npos)
        {
            size_t end_pos = msg.find("|", pos + 10);
            if (end_pos - (pos + 10) >= 1 && end_pos != std::string::npos)
            {
                id = atoul(msg.substr(pos + 10, (end_pos - (pos + 10))).c_str());
                if (id)
                    link.reset(new EnchantmentChatLink(id));
                else
                    BadColor(); // macro
            }
            else
                BadColor(); // macro
        }
        else if ((pos = msg.find("|Hachievement:")) != std::string::npos)
        {
            size_t end_pos = msg.find(":", pos + 14);
            if (end_pos - (pos + 14) >= 1 && end_pos != std::string::npos)
            {
                id = atoul(msg.substr(pos + 14, (end_pos - (pos + 14))).c_str());
                if (id)
                    link.reset(new AchievementChatLink(id));
                else
                    BadColor(); // macro
            }
            else
                BadColor(); // macro
        }

        if (!link)
            return true;

        if ((pos = msg.find("[", pos)) == std::string::npos)
        {
            ChatHandler(player).PSendSysMessage("You can't use color on chat! 00");
            msg = "";
            return false;
        }

        size_t pos_end = msg.find("]", pos);
        if (pos_end == std::string::npos)
        {
            ChatHandler(player).PSendSysMessage("You can't use color on chat! 01");
            msg = "";
            return false;
        }

        if (pos_end - (pos + 1) <= 0)
        {
            ChatHandler(player).PSendSysMessage("You can't use color on chat! 1");
            msg = "";
            return false;
        }

        std::string substr = msg.substr(pos + 1, (pos_end - (pos + 1)));
        if (substr.empty())
        {
            ChatHandler(player).PSendSysMessage("You can't use color on chat! 2");
            msg = "";
            return false;
        }

        if (!link->ValidateName((char*)substr.c_str(), msg.c_str()))
        {
            ChatHandler(player).PSendSysMessage("You can't use color on chat! 3");
            msg = "";
            return false;
        }
        return true;
    }
    
    std::pair<uint32, int32> CheckMessage(Player* player, std::string& msg, uint32 lang, uint32 type, Channel* channel = NULL, ObjectGuid receiver = ObjectGuid::Empty)
    {        
        if (player->isGameMaster())
            return {0, 0};

        if (!player->CanSpeak())
            return { 0, 0 };

        if (!IsValidType(type)) //don't check some system message and etc
            return{ 0, 0 };

        if (lang != LANG_ADDON)
            if (!CheckOnColor(player, msg))
                return{ 0, 0 };

        if (lang == LANG_ADDON || (type == CHAT_MSG_RAID && !player->InBattleground()))
            return{ 0, 0 };


        PlayerFloodInfo& info = sWordFilterMgr->GetFloodInfo(player->GetSession()->GetAccountId());


        //< Lfg and "often write" part
        if (channel && channel->IsLFG())
            player->lastLfgPhrase = msg;

        if (sWorld->getBoolConfig(CONFIG_ANTI_FLOOD_LFG) && type)       
            if (IsOftenSay(player, channel))
            {
                msg = "";
                return{ 0,0 };
            }
        //>

        if(!sWorld->getBoolConfig(CONFIG_ANTI_FLOOD_PM))
            return {0, 0};

        
        bool isValid = true;
        size_t hash = sWordFilterMgr->FilterMsgAndGetHash(msg.c_str(), player->GetSession()->GetSessionDbLocaleIndex(), &isValid);
        if (!isValid)
        {
            msg = "";
            ChatHandler(player).PSendSysMessage("You can use only correct symbols!");
            return{ 0, 0 };
        }

        if (msg.size() <= 5)
            return{ 0, 0 };

        uint32 reqMask = GetMaskFor(type);

        if (!hash)
            return{ 0, 0 };

        if (sWordFilterMgr->GetSourceMaskForBadSentence(hash)) // if exist mask, then exist msg in db
        {
            if (sWordFilterMgr->GetPenaltyForBadSentence(hash) && sWordFilterMgr->GetSourceMaskForBadSentence(hash) & reqMask) // just exist this phrase ?
                return{ sWordFilterMgr->GetIdOfBadSentence(hash), sWordFilterMgr->GetPenaltyForBadSentence(hash) };

            return { 0, 0 };
        }

        if (type == CHAT_MSG_YELL && player->getLevel() < MAX_LEVEL)
        {
            bool isPotentialBadZone = false;
            for (auto zone : {1519, 1637, 7502})
                if (player->GetZoneId() == zone)
                {
                    isPotentialBadZone = true;
                    break;
                }

            if (!isPotentialBadZone)
                return{ 0, 0 };
        }        
        else if(receiver.IsEmpty() || (type != CHAT_MSG_BN_WHISPER && info.players.find(receiver) == info.players.end()) || (type != CHAT_MSG_WHISPER && type != CHAT_MSG_BN_WHISPER) || msg.size() <= 10)
            return{ 0,0 };

        if (!receiver.IsEmpty())
            info.players.erase(receiver);

        if (sWordFilterMgr->GetPenaltyForBadSentence(hash) && sWordFilterMgr->GetSourceMaskForBadSentence(hash) & reqMask) // just exist this phrase ?
            return{ sWordFilterMgr->GetIdOfBadSentence(hash), sWordFilterMgr->GetPenaltyForBadSentence(hash) };

        float mod = 1;
        if (type == CHAT_MSG_BN_WHISPER)
        {
            if (msg.size() >= 10)
                info.mailPhrases.insert(msg);
        }
        else if (type == CHAT_MSG_YELL)
            mod = 3;

        bool needPenalty = false;

        if (++info.phrasesCheck[hash] >= sWorld->getIntConfig(CONFIG_ANTI_FLOOD_COUNT_OF_MESSAGES) * mod)
        {
            needPenalty = true;
        }
        else if (type == CHAT_MSG_BN_WHISPER && info.mailPhrases.size() >= sWorld->getIntConfig(CONFIG_ANTI_FLOOD_COUNT_OF_MESSAGES))
        {
            std::multiset<size_t> wordsCount{}; // try find similar words on another phareses
            for (const auto& text : info.mailPhrases)
            {
                Tokenizer tokens(text, ' ', 0, false);
                for (auto& word : tokens)
                {
                    if (strlen(word) < 3)
                        continue;

                    size_t wordHash = sWordFilterMgr->FilterMsgAndGetHash(word, player->GetSession()->GetSessionDbLocaleIndex());
                    if (wordHash)
                    {
                        if (info.mailFoundedBadWords.find(wordHash) != info.mailFoundedBadWords.end())
                        {
                            needPenalty = true;
                            break;
                        }
                        wordsCount.insert(wordHash);
                    }
                }

                if (needPenalty)
                    break;
            }

            if (!needPenalty)
            {
                uint32 similarWords = 0;
                for (auto& val : wordsCount)
                    if (wordsCount.count(val) >= info.mailPhrases.size()) // on ever phrase
                    {
                        ++similarWords;
                        info.mailFoundedBadWords.insert(val);
                    }

                uint8 requiredSimilarWords = ceil(3.0f * float(sWorld->getIntConfig(CONFIG_ANTI_FLOOD_COUNT_OF_MESSAGES)) / float(info.mailPhrases.size())); // if there are so many phrases, then one word in each phrase will be enough
                if (requiredSimilarWords == 0)
                    requiredSimilarWords = 1;

                if (similarWords >= requiredSimilarWords)
                    needPenalty = true;
            }
        }
        

        if (needPenalty)
        {
            sWordFilterMgr->AddBadSentence(msg, hash, reqMask);

            if (sWordFilterMgr->GetPenaltyForBadSentence(hash) && sWordFilterMgr->GetSourceMaskForBadSentence(hash) & reqMask) // just exist this phrase ?
                return{ sWordFilterMgr->GetIdOfBadSentence(hash), sWordFilterMgr->GetPenaltyForBadSentence(hash) };
        }
 
        return{ 0,0 };
    }
 
    void PenaltyPlayer(Player* player, std::pair<uint32, int32> pair)
    {
        uint32 id = pair.first;
        if (!id)
            return;

        int32 penaltyTime = pair.second;
        if (!penaltyTime)
            return;

        std::stringstream reasonStr;
        reasonStr << "-Флуд №" << id << " Realm: " << sWorld->GetRealmName().c_str();
        time_t rawtime;
        time(&rawtime);
        reasonStr << "Time: " << ctime(&rawtime);
        
        if (penaltyTime > 0)
        {
            PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_MUTE_TIME);

            uint32 notSpeakTime = penaltyTime;

            int64 muteTime = time(NULL) + notSpeakTime * MINUTE;
            player->GetSession()->m_muteTime = muteTime;
            stmt->setInt64(0, muteTime);
            ChatHandler(player).PSendSysMessage(LANG_YOUR_CHAT_DISABLED, notSpeakTime, reasonStr.str().c_str());

            stmt->setUInt32(1, player->GetSession()->GetAccountId());
            LoginDatabase.Execute(stmt);

            LoginDatabase.PQuery("INSERT INTO account_muted VALUES (%u, UNIX_TIMESTAMP(), UNIX_TIMESTAMP() + %u, '%s', '%s', 1)", player->GetSession()->GetAccountId(), notSpeakTime * 60, player->GetSession()->GetPlayer()->GetName(), reasonStr.str().c_str());

        }
        else
        {
            sWorld->BanAccount(BAN_ACCOUNT, player->GetSession()->GetAccountName(), "-1", reasonStr.str().c_str(), "Server");
            if (player->GetSession()->_countPenaltiesHwid >= 0 && player->GetSession()->_hwid != 0)
                LoginDatabase.PQuery("REPLACE INTO hwid_penalties VALUES (" UI64FMTD ", %i, \"%s\")", player->GetSession()->_hwid, ++player->GetSession()->_countPenaltiesHwid, reasonStr.str().c_str());
        }

    }

    bool IsValidType(uint32 type)
    {
        if (type == CHAT_MSG_SAY || type == CHAT_MSG_YELL || type == CHAT_MSG_WHISPER || type == CHAT_MSG_EMOTE ||
            type == CHAT_MSG_TEXT_EMOTE || type == CHAT_MSG_CHANNEL || type == CHAT_MSG_RAID || type == CHAT_MSG_RAID_LEADER || type == CHAT_MSG_BN_WHISPER)
            return true;
        return false;

    }

    bool IsOftenSay(Player* player, Channel* channel)
    {
        PlayerFloodInfo& info = sWordFilterMgr->GetFloodInfo(player->GetSession()->GetAccountId());

        if (info.lastSayTime)
        {
            uint32 _time = (channel && channel->IsLFG()) ? 5000 : 1000;
            if (GetMSTimeDiffToNow(info.lastSayTime) < _time)
            {
                ChatHandler(player).PSendSysMessage("You cannot say for another %u second", ((_time / 1000) - (GetMSTimeDiffToNow(info.lastSayTime) / 1000)));
                // msg = "";

                player->GetSession()->m_muteTime = time(NULL) + _time / 1000;

                return true; // without some penalties
            }
        }

        info.lastSayTime = getMSTime();

        return false;
    }

    uint32 GetMaskFor(uint32 type)
    {
        if (type == CHAT_MSG_BN_WHISPER)
            return BS_MAIL;

        if (type == CHAT_MSG_WHISPER)
            return BS_PM;

        return BS_LFG; // lfg, say and other
    }
};

 
void AddSC_player_special_scripts()
{
    new playerScriptPvpMisc();
    new playerScriptCheckArts();
    new player_informated_about_rates();
    new player_fun_welcome_info();
    new player_learn_warforge();
    new System_Flood_And_RSP();
};
