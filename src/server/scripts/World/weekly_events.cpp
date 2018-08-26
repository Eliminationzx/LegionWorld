
#include "GameEventMgr.h"

static uint8 const maxWeeklyEvents = 8;

uint8 eventTab[maxWeeklyEvents] =
{
    90, ///< Northrend Timewalking Dungeon Event
    91, ///< Outland Timewalking Dungeon Event
    92, ///< Cataclysm Timewalking Dungeon Event
    93, ///< Pet Battle Bonus Event
    94, ///< Battleground Bonus Event
    95, ///< Arena Skirmish Bonus Event
    96, ///< World Quest Bonus Event
    97, ///< Legion Dungeon Event
};

uint32 auraTab[maxWeeklyEvents] =
{
    0,      ///< Northrend Timewalking Dungeon Event
    0,      ///< Outland Timewalking Dungeon Event
    0,      ///< Cataclysm Timewalking Dungeon Event
    186406, ///< Pet Battle Bonus Event
    186403, ///< Battleground Bonus Event
    186501, ///< Arena Skirmish Bonus Event
    255788, ///< World Quest Bonus Event
    225787, ///< Legion Dungeon Event
};

class PlayerScript_Weekly_Event_Bonus : public PlayerScript
{
public:
    PlayerScript_Weekly_Event_Bonus() :PlayerScript("PlayerScript_Weekly_Event_Bonus") { }

    void OnLogin(Player* player) override
    {
        for (uint8 i = 0; i < maxWeeklyEvents; ++i)
            if (sGameEventMgr->IsActiveEvent(eventTab[i]))
                if (auraTab[i])
                {
                    player->AddAura(auraTab[i], player);
                    break;
                }
    }

    void OnLogout(Player* player) override
    {
        for (uint8 i = 0; i < maxWeeklyEvents + 3; ++i)
            player->RemoveAura(auraTab[i]);
    }
};

void AddSC_weekly_events_cripts()
{
    //new PlayerScript_Weekly_Event_Bonus(); @TODO dissable atm as wrong & outdated & shitty code
};
