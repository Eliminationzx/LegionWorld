#include "QuestData.h"
#include "MapManager.h"

std::pair<uint32, uint32> quest_by_class[12]
{
    {42607, 42609},  // Воин
    {42844, 39696},  // Пал
    {44090, 42519},  // Хант
    {43007, 42139},  // Разбойник
    {44100, 43270},  // Жрец
    {44550, 43264},  // ДК
    {42114, 42383},  // Шаман
    {41141, 42663},  // Маг
    {44099, 42608},  // Лок
    {42186, 42187},  // Монк
    {42516, 42583},  // Друид
    {42666, 42671},  // ДХ
};

class npc_quest_giver : public CreatureScript
{
    public:
    npc_quest_giver () : CreatureScript("npc_quest_giver") {}

    bool OnGossipHello(Player *pPlayer, Creature* _creature) override
    {
        pPlayer->ADD_GOSSIP_ITEM_EXTENDED(0, " ==Удаление гарнизона при забагивании после смены фракции ==", GOSSIP_SENDER_MAIN, 4, "ВНИМАНИЕ: Это действие ПОЛНОСТЬЮ удаляет Ваш гарнизон! Данная процедура не может быть возвращена или отменена в дальнейшем", 0, false);
        pPlayer->ADD_GOSSIP_ITEM(0, "1. Друид. Выдача квеста \"Посев семян \" (41255)", GOSSIP_SENDER_MAIN, 1);
        pPlayer->ADD_GOSSIP_ITEM(0, "2. Все классы. Засчитывание квеста \"Явитесь, защитники!\"", GOSSIP_SENDER_MAIN, 2);
        pPlayer->ADD_GOSSIP_ITEM(0, "3. Рыцарь смерти. Засчитывание квеста \"Возвращение в мардум\"", GOSSIP_SENDER_MAIN, 3);
        
        pPlayer->PlayerTalkClass->SendGossipMenu(100010, _creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* pPlayer, Creature* _creature, uint32 /*uiSender*/, uint32 uiAction) override
    {
        pPlayer->PlayerTalkClass->ClearMenus();
        
        switch(uiAction)
        {
            case 1:
                if (pPlayer->getClass() == CLASS_DRUID)
                    AddQuestHelper(pPlayer, 41255);
                break;
            case 2:
                if (pPlayer->IsQuestRewarded(quest_by_class[pPlayer->getClass() -1].first))
                    pPlayer->CompleteQuest(quest_by_class[pPlayer->getClass() -1].second);
                break;
            case 3:
                pPlayer->CompleteQuest(41033);
                break;
            case 4:
            {
                if (pPlayer->getLevel() <= 101)
                    return false;
                ObjectGuid::LowType lowGuid = pPlayer->GetGUIDLow();
                sMapMgr->SetUnloadGarrison(lowGuid);
                uint32 id = pPlayer->GetTeam() == HORDE ? 71 : 2;
                pPlayer->SaveToDB();
                pPlayer->GetSession()->KickPlayer();
                
                _creature->AddDelayedEvent(1000, [this, lowGuid, id] () -> void
                {
                
                    SQLTransaction trans = CharacterDatabase.BeginTransaction();
                    
                    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHARACTER_GARRISON_ONLY);
                    stmt->setUInt64(0, lowGuid);
                    trans->Append(stmt);
                    
                    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHARACTER_GARRISON_BLUEPRINTS);
                    stmt->setUInt64(0, lowGuid);
                    trans->Append(stmt);
                    
                    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHARACTER_GARRISON_BUILDINGS);
                    stmt->setUInt64(0, lowGuid);
                    trans->Append(stmt);
                    
                    // stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHARACTER_GARRISON_FOLLOWERS);
                    // stmt->setUInt64(0, lowGuid);
                    // trans->Append(stmt);
                    
                    // stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHARACTER_GARRISON_MISSIONS);
                    // stmt->setUInt64(0, lowGuid);
                    // trans->Append(stmt);
                    
                    // stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHARACTER_GARRISON_SHIPMENTS);
                    // stmt->setUInt64(0, lowGuid);
                    // trans->Append(stmt);
                    
                    // stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHARACTER_GARRISON_TALENTS);
                    // stmt->setUInt64(0, lowGuid);
                    // trans->Append(stmt);
                    
                    trans->PAppend("insert into `character_reward` (`id`, `type`, `owner_guid`)value ('%u','10', '%u')", id, lowGuid);
                    
                    CharacterDatabase.CommitTransaction(trans);
                });
                
            }
        }
        
        
        pPlayer->CLOSE_GOSSIP_MENU();
        return true;
    }
    
    void AddQuestHelper(Player* player, uint32 entry)
    {
        Quest const* quest = sQuestDataStore->GetQuestTemplate(entry);
        if (quest && player->CanAddQuest(quest, true) && !player->IsQuestRewarded(quest->GetQuestId()))
        {
            player->AddQuest(quest, NULL);
            if (player->CanCompleteQuest(entry))
                player->CompleteQuest(entry);
        }
    }
};



void AddSC_npc_quest_giver()
{
    new npc_quest_giver();
}