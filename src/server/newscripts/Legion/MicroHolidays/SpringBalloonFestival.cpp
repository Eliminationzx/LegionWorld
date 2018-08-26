/*
    http://uwow.biz
*/

#include "MoveSplineInit.h"

enum eSay
{
    // GENERAL
    SAY_NEED_PLAYERS              = 0,

    // DALARAN
    SAY_LINE_CHENS_1              = 0,
    SAY_LINE_CHENS_2,
    SAY_LINE_CHENS_3,
    SAY_LINE_CHENS_4,
    SAY_LINE_CHENS_5,
    SAY_LINE_CHENS_6,
    SAY_LINE_CHENS_7,
    SAY_LINE_CHENS_8,
    SAY_LINE_CHENS_9,
    SAY_LINE_CHENS_10,
    SAY_LINE_CHENS_11,

    // ULDUM
    SAY_LINE_SCHNOTTZ_1           = 0,
    SAY_LINE_SCHNOTTZ_2,
    SAY_LINE_SCHNOTTZ_3,
    SAY_LINE_SCHNOTTZ_4,
    SAY_LINE_SCHNOTTZ_5,
    SAY_LINE_SCHNOTTZ_6,
    SAY_LINE_SCHNOTTZ_7,
    SAY_LINE_SCHNOTTZ_8,
    SAY_LINE_BALLOON              = 0,

    // PANDARIA
    SAY_LINE_LIN_CLOUDWALKER_1    = 0,
    SAY_LINE_LIN_CLOUDWALKER_2,
    SAY_LINE_LIN_CLOUDWALKER_3,
    SAY_LINE_LIN_CLOUDWALKER_4,
    SAY_LINE_LIN_CLOUDWALKER_5,
    SAY_LINE_LIN_CLOUDWALKER_6,
    SAY_LINE_LIN_CLOUDWALKER_7,
    SAY_LINE_LIN_CLOUDWALKER_8,
    SAY_LINE_LIN_CLOUDWALKER_9,
    SAY_LINE_LIN_CLOUDWALKER_10,
    SAY_LINE_LIN_CLOUDWALKER_11,
    SAY_LINE_LIN_CLOUDWALKER_12,
    SAY_LINE_LIN_CLOUDWALKER_13,
    SAY_LINE_LIN_CLOUDWALKER_14,
    SAY_LINE_LIN_CLOUDWALKER_15,
    SAY_LINE_LIN_CLOUDWALKER_16,

    // AZSUNA
    SAY_LINE_BOSCOE_1             = 0,
    SAY_LINE_BOSCOE_2,
    SAY_LINE_BOSCOE_3,
    SAY_LINE_BOSCOE_4,
    SAY_LINE_BOSCOE_5,
    SAY_LINE_BOSCOE_6,
    SAY_LINE_BOSCOE_7,
    SAY_LINE_BOSCOE_8,
    SAY_LINE_BOSCOE_9,
    SAY_LINE_RUNGLE_1             = 0,
    SAY_LINE_RUNGLE_2,
    SAY_LINE_RUNGLE_3,
    SAY_LINE_RUNGLE_4,
    SAY_LINE_RUNGLE_5,
    SAY_LINE_RUNGLE_6,

    // SURAMAR
    SAY_LINE_ZANG_1               = 0,
    SAY_LINE_ZANG_2,
    SAY_LINE_ZANG_3,
    SAY_LINE_ZANG_4,
    SAY_LINE_ZANG_5,
    SAY_LINE_ZANG_6,
    SAY_LINE_ZANG_7,
    SAY_LINE_ZANG_8,
    SAY_LINE_ZANG_9,
    SAY_LINE_ZANG_10,
    SAY_LINE_ZANG_11,
    SAY_LINE_ZANG_12,
    SAY_LINE_ZANG_13,
    SAY_LINE_ZANG_14,
    SAY_LINE_ZANG_15,
    SAY_LINE_ZANG_16,
    SAY_LINE_ZANG_17,
    SAY_LINE_ZANG_18,

    // HIGHMOUNTAINS
    SAY_LINE_HEMET_NESINGWARY_1    = 0,
    SAY_LINE_HEMET_NESINGWARY_2,
    SAY_LINE_HEMET_NESINGWARY_3,
    SAY_LINE_HEMET_NESINGWARY_4,
    SAY_LINE_HEMET_NESINGWARY_5,
    SAY_LINE_HEMET_NESINGWARY_6,
    SAY_LINE_HEMET_NESINGWARY_7,
    SAY_LINE_HEMET_NESINGWARY_8,
    SAY_LINE_HEMET_NESINGWARY_9,
    SAY_LINE_HEMET_NESINGWARY_10,
    SAY_LINE_HEMET_NESINGWARY_11,
    SAY_LINE_HEMET_NESINGWARY_12,
    SAY_LINE_HEMET_NESINGWARY_13,

    // STORMHEIM
    SAY_LINE_EMI_LAN_1             = 0,
    SAY_LINE_EMI_LAN_2,
    SAY_LINE_EMI_LAN_3,
    SAY_LINE_EMI_LAN_4,
    SAY_LINE_EMI_LAN_5,
    SAY_LINE_EMI_LAN_6,
    SAY_LINE_EMI_LAN_7,
    SAY_LINE_EMI_LAN_8,
    SAY_LINE_EMI_LAN_9,
    SAY_LINE_EMI_LAN_10,
    SAY_LINE_EMI_LAN_11,
};

enum eCreatures
{
    // DALARAN
    NPC_CHENS_BALLOON               = 118469,
    NPC_CHENS                       = 118470,
    NPC_CHENS_BALLON_VH             = 118365,
    NPC_CHENS_VH                    = 118354,

    // ULDUM
    NPC_SCHNOTTZ                    = 118482,
    NPC_SCHNOTTZ_VH                 = 118340,
    NPC_SCHNOTTZ_BALLON             = 118481,
    NPC_SCHNOTTZ_BALLON_VH          = 118297,

    // PANDARIA
    NPC_LIN_CLOUDWALKER             = 118474,
    NPC_LIN_CLOUDWALKER_VH          = 118355,
    NPC_CLOUDWALKER_EXPRESS         = 118473,
    NPC_CLOUDWALKER_EXPRESS_VH      = 118366,

    // AZSUNA
    NPC_SKY_CHARIOT                 = 118466,
    NPC_SKY_CHARIOT_VH              = 118363,
    NPC_BOSCOE                      = 118467,
    NPC_BOSCOE_VH                   = 118351,
    NPC_RUNGLE                      = 118468,
    NPC_RUNGLE_VH                   = 118352,

    // SURAMAR
    NPC_CLOUDWALKER_EXPRESS_S       = 118435,
    NPC_CLOUDWALKER_EXPRESS_S_VH    = 118359,
    NPC_ZANG_CLOUDWALKER            = 118436,
    NPC_ZANG_CLOUDWALKER_VH         = 118357,

    // HIGHMOUNTAINS
    NPC_HEMET_NESINGWARY            = 94409,
    NPC_HEMET_NESINGWARY_VH         = 118353,
    NPC_THE_KILLEMJARO              = 118364,
    NPC_THE_KILLEMJARO_VH           = 118446,

    // STORMHEIM
    NPC_EMI_LAN                     = 118443,
    NPC_EMI_LAN_VH                  = 118356,
    NPC_EMI_LANS_SKYLOUNGE          = 118442,
    NPC_EMI_LANS_SKYLOUNGE_VH       = 118367
};

enum eSpells
{
    // GENERAL
    SPELL_BALLOON_FIND_PLAYERS              = 234944,
    SPELL_WAINTING_FOR_BALLOON              = 234941,
    SPELL_RIDE_BALLOON                      = 234947,

    // ULDUM
    SPELL_SCHNOTTZ_HAS_TO_GO_OR             = 236349,
    SPELL_SCHNOTTZ_HAS_TO_GO                = 236346,

    // AZSUNA
    SPELL_JUNIOR_BALLONER_PREPAREDNESS_KIT  = 236323
};

struct npc_chens : ScriptedAI
{
    npc_chens(Creature* creature) : ScriptedAI(creature) {}

    uint32 timerAuraCheck = 1000;
    EventMap events;

    void sGossipSelect(Player* player, uint32 /*sender*/, uint32 action) override
    {
        if (action = 1)
        {
            Talk(SAY_NEED_PLAYERS);
            player->CastSpell(player, SPELL_WAINTING_FOR_BALLOON);
            player->CLOSE_GOSSIP_MENU();
            DoCast(SPELL_BALLOON_FIND_PLAYERS);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        if (timerAuraCheck <= diff)
        {
            std::list<Player*> playerList;
            GetPlayerListInGrid(playerList, me, 60.0f);
            for (auto players : playerList)
            {
                if (!players->IsInDist2d(me, 20.0f) && players->HasAura(SPELL_WAINTING_FOR_BALLOON))
                {
                    players->RemoveAurasDueToSpell(SPELL_WAINTING_FOR_BALLOON);
                }
            }
            timerAuraCheck = 1000;
        } else timerAuraCheck -= diff;
    }
};

struct npc_scgnottz : ScriptedAI
{
    npc_scgnottz(Creature* creature) : ScriptedAI(creature) {}

    uint32 timerAuraCheck = 1000;
    EventMap events;

    void sGossipSelect(Player* player, uint32 /*sender*/, uint32 action) override
    {
        if (action = 1)
        {
            Talk(SAY_NEED_PLAYERS);
            player->CastSpell(player, SPELL_WAINTING_FOR_BALLOON);
            player->CLOSE_GOSSIP_MENU();
        }
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        if (timerAuraCheck <= diff)
        {
            std::list<Player*> playerList;
            GetPlayerListInGrid(playerList, me, 60.0f);
            for (auto players : playerList)
            {
                if (!players->IsInDist2d(me, 20.0f) && players->HasAura(SPELL_WAINTING_FOR_BALLOON))
                {
                    players->RemoveAurasDueToSpell(SPELL_WAINTING_FOR_BALLOON);
                }
            }
            timerAuraCheck = 1000;
        }
        else timerAuraCheck -= diff;
    }
};

struct npc_lin_cloudwalker : ScriptedAI
{
    npc_lin_cloudwalker(Creature* creature) : ScriptedAI(creature) {}

    uint32 timerAuraCheck = 1000;
    EventMap events;

    void sGossipSelect(Player* player, uint32 /*sender*/, uint32 action) override
    {
        if (action = 1)
        {
            Talk(SAY_NEED_PLAYERS);
            player->CastSpell(player, SPELL_WAINTING_FOR_BALLOON);
            player->CLOSE_GOSSIP_MENU();
        }
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        if (timerAuraCheck <= diff)
        {
            std::list<Player*> playerList;
            GetPlayerListInGrid(playerList, me, 60.0f);
            for (auto players : playerList)
            {
                if (!players->IsInDist2d(me, 20.0f) && players->HasAura(SPELL_WAINTING_FOR_BALLOON))
                {
                    players->RemoveAurasDueToSpell(SPELL_WAINTING_FOR_BALLOON);
                }
            }
            timerAuraCheck = 1000;
        }
        else timerAuraCheck -= diff;
    }
};

struct npc_rungle : ScriptedAI
{
    npc_rungle(Creature* creature) : ScriptedAI(creature) {}

    uint32 timerAuraCheck = 1000;
    EventMap events;

    void sGossipSelect(Player* player, uint32 /*sender*/, uint32 action) override
    {
        if (action = 1)
        {
            Talk(SAY_NEED_PLAYERS);
            player->CastSpell(player, SPELL_WAINTING_FOR_BALLOON);
            player->CLOSE_GOSSIP_MENU();
        }
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        if (timerAuraCheck <= diff)
        {
            std::list<Player*> playerList;
            GetPlayerListInGrid(playerList, me, 60.0f);
            for (auto players : playerList)
            {
                if (!players->IsInDist2d(me, 20.0f) && players->HasAura(SPELL_WAINTING_FOR_BALLOON))
                {
                    players->RemoveAurasDueToSpell(SPELL_WAINTING_FOR_BALLOON);
                }
            }
            timerAuraCheck = 1000;
        }
        else timerAuraCheck -= diff;
    }
};

struct npc_zang_cloudwalker : ScriptedAI
{
    npc_zang_cloudwalker(Creature* creature) : ScriptedAI(creature) {}

    uint32 timerAuraCheck = 1000;
    EventMap events;

    void sGossipSelect(Player* player, uint32 /*sender*/, uint32 action) override
    {
        if (action = 1)
        {
            Talk(SAY_NEED_PLAYERS);
            player->CastSpell(player, SPELL_WAINTING_FOR_BALLOON);
            player->CLOSE_GOSSIP_MENU();
        }
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        if (timerAuraCheck <= diff)
        {
            std::list<Player*> playerList;
            GetPlayerListInGrid(playerList, me, 60.0f);
            for (auto players : playerList)
            {
                if (!players->IsInDist2d(me, 20.0f) && players->HasAura(SPELL_WAINTING_FOR_BALLOON))
                {
                    players->RemoveAurasDueToSpell(SPELL_WAINTING_FOR_BALLOON);
                }
            }
            timerAuraCheck = 1000;
        }
        else timerAuraCheck -= diff;
    }
};

struct npc_hemet_nesingwary : ScriptedAI
{
    npc_hemet_nesingwary(Creature* creature) : ScriptedAI(creature) {}

    uint32 timerAuraCheck = 1000;
    EventMap events;

    void sGossipSelect(Player* player, uint32 /*sender*/, uint32 action) override
    {
        if (action = 1)
        {
            Talk(SAY_NEED_PLAYERS);
            player->CastSpell(player, SPELL_WAINTING_FOR_BALLOON);
            player->CLOSE_GOSSIP_MENU();
        }
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        if (timerAuraCheck <= diff)
        {
            std::list<Player*> playerList;
            GetPlayerListInGrid(playerList, me, 60.0f);
            for (auto players : playerList)
            {
                if (!players->IsInDist2d(me, 20.0f) && players->HasAura(SPELL_WAINTING_FOR_BALLOON))
                {
                    players->RemoveAurasDueToSpell(SPELL_WAINTING_FOR_BALLOON);
                }
            }
            timerAuraCheck = 1000;
        }
        else timerAuraCheck -= diff;
    }
};

struct npc_emi_lan : ScriptedAI
{
    npc_emi_lan(Creature* creature) : ScriptedAI(creature) {}

    uint32 timerAuraCheck = 1000;
    EventMap events;

    void sGossipSelect(Player* player, uint32 /*sender*/, uint32 action) override
    {
        if (action = 1)
        {
            Talk(SAY_NEED_PLAYERS);
            player->CastSpell(player, SPELL_WAINTING_FOR_BALLOON);
            player->CLOSE_GOSSIP_MENU();
        }
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        if (timerAuraCheck <= diff)
        {
            std::list<Player*> playerList;
            GetPlayerListInGrid(playerList, me, 60.0f);
            for (auto players : playerList)
            {
                if (!players->IsInDist2d(me, 20.0f) && players->HasAura(SPELL_WAINTING_FOR_BALLOON))
                {
                    players->RemoveAurasDueToSpell(SPELL_WAINTING_FOR_BALLOON);
                }
            }
            timerAuraCheck = 1000;
        }
        else timerAuraCheck -= diff;
    }
};

struct npc_chens_vh : ScriptedAI
{
    npc_chens_vh(Creature* creature) : ScriptedAI(creature) {}

    EventMap events;

    void Reset() override
    {
        events.ScheduleEvent(EVENT_1, 5000);
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_1:
                Talk(SAY_LINE_CHENS_1);
                events.ScheduleEvent(EVENT_2, 6000);
                break;
            case EVENT_2:
                Talk(SAY_LINE_CHENS_2);
                events.ScheduleEvent(EVENT_3, 7000);
                break;
            case EVENT_3:
                Talk(SAY_LINE_CHENS_3);
                events.ScheduleEvent(EVENT_4, 8000);
                break;
            case EVENT_4:
                Talk(SAY_LINE_CHENS_4);
                events.ScheduleEvent(EVENT_5, 8000);
                break;
            case EVENT_5:
                Talk(SAY_LINE_CHENS_5);
                events.ScheduleEvent(EVENT_6, 8000);
                break;
            case EVENT_6:
                Talk(SAY_LINE_CHENS_6);
                events.ScheduleEvent(EVENT_7, 6000);
                break;
            case EVENT_7:
                Talk(SAY_LINE_CHENS_7);
                events.ScheduleEvent(EVENT_8, 9000);
                break;
            case EVENT_8:
                Talk(SAY_LINE_CHENS_8);
                events.ScheduleEvent(EVENT_9, 35000); // to player
                break;
            case EVENT_9:
                Talk(SAY_LINE_CHENS_9);
                events.ScheduleEvent(EVENT_10, 33000); // to player
                break;
            case EVENT_10:
                Talk(SAY_LINE_CHENS_10);
                events.ScheduleEvent(EVENT_11, 35000); // to player
                break;
            case EVENT_11:
                Talk(SAY_LINE_CHENS_11);
                break;
            }
        }
    }
};

struct npc_scgnottz_vh : ScriptedAI
{
    npc_scgnottz_vh(Creature* creature) : ScriptedAI(creature) {}

    EventMap events;

    void Reset() override
    {
        events.ScheduleEvent(EVENT_1, 5000);
    }

    void DoAction(int32 const action) override
    {
        switch (action)
        {
        case ACTION_1:
            Talk(SAY_LINE_SCHNOTTZ_8);
            me->DespawnOrUnsummon(1000);
            break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_1:
                Talk(SAY_LINE_SCHNOTTZ_1);
                events.ScheduleEvent(EVENT_2, 6000);
                break;
            case EVENT_2:
                Talk(SAY_LINE_SCHNOTTZ_2);
                events.ScheduleEvent(EVENT_3, 5000);
                break;
            case EVENT_3:
                Talk(SAY_LINE_SCHNOTTZ_3);
                events.ScheduleEvent(EVENT_4, 6000);
                break;
            case EVENT_4:
                Talk(SAY_LINE_SCHNOTTZ_4);
                events.ScheduleEvent(EVENT_5, 5000);
                break;
            case EVENT_5:
                Talk(SAY_LINE_SCHNOTTZ_5);
                events.ScheduleEvent(EVENT_6, 6000);
                break;
            case EVENT_6:
                Talk(SAY_LINE_SCHNOTTZ_6);
                events.ScheduleEvent(EVENT_7, 8000);
                break;
            case EVENT_7:
                Talk(SAY_LINE_SCHNOTTZ_7);
                break;
            }
        }
    }
};

struct npc_lin_cloudwalker_vh : ScriptedAI
{
    npc_lin_cloudwalker_vh(Creature* creature) : ScriptedAI(creature) {}

    EventMap events;

    void Reset() override
    {
        events.ScheduleEvent(EVENT_1, 4000);
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_1:
                Talk(SAY_LINE_LIN_CLOUDWALKER_1);
                events.ScheduleEvent(EVENT_2, 7000);
                break;
            case EVENT_2:
                Talk(SAY_LINE_LIN_CLOUDWALKER_2);
                events.ScheduleEvent(EVENT_3, 8000);
                break;
            case EVENT_3:
                Talk(SAY_LINE_LIN_CLOUDWALKER_3);
                events.ScheduleEvent(EVENT_4, 4000);
                break;
            case EVENT_4:
                Talk(SAY_LINE_LIN_CLOUDWALKER_4);
                events.ScheduleEvent(EVENT_5, 6000);
                break;
            case EVENT_5:
                Talk(SAY_LINE_LIN_CLOUDWALKER_5);
                events.ScheduleEvent(EVENT_6, 4000);
                break;
            case EVENT_6:
                Talk(SAY_LINE_LIN_CLOUDWALKER_6);
                events.ScheduleEvent(EVENT_7, 6000);
                break;
            case EVENT_7:
                Talk(SAY_LINE_LIN_CLOUDWALKER_7);
                events.ScheduleEvent(EVENT_8, 11000);
                break;
            case EVENT_8:
                Talk(SAY_LINE_LIN_CLOUDWALKER_8);
                events.ScheduleEvent(EVENT_9, 8000);
                break;
            case EVENT_9:
                Talk(SAY_LINE_LIN_CLOUDWALKER_9);
                events.ScheduleEvent(EVENT_10, 3000);
                break;
            case EVENT_10:
                Talk(SAY_LINE_LIN_CLOUDWALKER_10);
                events.ScheduleEvent(EVENT_11, 2000);
                break;
            case EVENT_11:
                Talk(SAY_LINE_LIN_CLOUDWALKER_11);
                events.ScheduleEvent(EVENT_12, 6000);
                break;
            case EVENT_12:
                Talk(SAY_LINE_LIN_CLOUDWALKER_12);
                events.ScheduleEvent(EVENT_13, 6000);
                break;
            case EVENT_13:
                Talk(SAY_LINE_LIN_CLOUDWALKER_13);
                events.ScheduleEvent(EVENT_14, 36000); // to player
                break;
            case EVENT_14:
                Talk(SAY_LINE_LIN_CLOUDWALKER_14);
                events.ScheduleEvent(EVENT_15, 35000); // to player
                break;
            case EVENT_15:
                Talk(SAY_LINE_LIN_CLOUDWALKER_15);
                events.ScheduleEvent(EVENT_16, 35000); // to player
                break;
            case EVENT_16:
                Talk(SAY_LINE_LIN_CLOUDWALKER_16);
                break;
            }
        }
    }
};

struct npc_boscoe_vh : ScriptedAI
{
    npc_boscoe_vh(Creature* creature) : ScriptedAI(creature) {}

    EventMap events;

    void Reset() override
    {
        events.ScheduleEvent(EVENT_1, 4000);
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_1:
                Talk(SAY_LINE_BOSCOE_1);
                events.ScheduleEvent(EVENT_2, 7000);
                break;
            case EVENT_2:
                Talk(SAY_LINE_BOSCOE_2);
                events.ScheduleEvent(EVENT_3, 12000);
                break;
            case EVENT_3:
                Talk(SAY_LINE_BOSCOE_3);
                events.ScheduleEvent(EVENT_4, 11000);
                break;
            case EVENT_4:
                Talk(SAY_LINE_BOSCOE_4);
                events.ScheduleEvent(EVENT_5, 12000);
                break;
            case EVENT_5:
                Talk(SAY_LINE_BOSCOE_5);
                events.ScheduleEvent(EVENT_6, 9000);
                break;
            case EVENT_6:
                Talk(SAY_LINE_BOSCOE_6);
                events.ScheduleEvent(EVENT_7, 8000);
                break;
            case EVENT_7:
                Talk(SAY_LINE_BOSCOE_7);
                events.ScheduleEvent(EVENT_8, 8000);
                break;
            case EVENT_8:
                Talk(SAY_LINE_BOSCOE_8);
                events.ScheduleEvent(EVENT_9, 14000);
                break;
            case EVENT_9:
                Talk(SAY_LINE_BOSCOE_9);
                break;
            }
        }
    }
};


struct npc_rungle_vh : ScriptedAI
{
    npc_rungle_vh(Creature* creature) : ScriptedAI(creature) {}

    EventMap events;

    void Reset() override
    {
        events.ScheduleEvent(EVENT_1, 19000);
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_1:
                Talk(SAY_LINE_RUNGLE_1);
                events.ScheduleEvent(EVENT_2, 11000);
                break;
            case EVENT_2:
                Talk(SAY_LINE_RUNGLE_2);
                events.ScheduleEvent(EVENT_3, 13000);
                break;
            case EVENT_3:
                Talk(SAY_LINE_RUNGLE_3);
                events.ScheduleEvent(EVENT_4, 10000);
                break;
            case EVENT_4:
                Talk(SAY_LINE_RUNGLE_4);
                events.ScheduleEvent(EVENT_5, 26000);
                break;
            case EVENT_5:
                Talk(SAY_LINE_RUNGLE_5);
                break;
            }
        }
    }
};

struct npc_zang_cloudwalker_vh : ScriptedAI
{
    npc_zang_cloudwalker_vh(Creature* creature) : ScriptedAI(creature) {}

    EventMap events;

    void Reset() override
    {
        events.ScheduleEvent(EVENT_1, 4000);
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_1:
                Talk(SAY_LINE_ZANG_1);
                events.ScheduleEvent(EVENT_2, 11000);
                break;
            case EVENT_2:
                Talk(SAY_LINE_ZANG_2);
                events.ScheduleEvent(EVENT_3, 6000);
                break;
            case EVENT_3:
                Talk(SAY_LINE_ZANG_3);
                events.ScheduleEvent(EVENT_4, 6000);
                break;
            case EVENT_4:
                Talk(SAY_LINE_ZANG_4); // to player
                events.ScheduleEvent(EVENT_5, 11000);
                break;
            case EVENT_5:
                Talk(SAY_LINE_ZANG_5);
                events.ScheduleEvent(EVENT_6, 6000);
                break;
            case EVENT_6:
                Talk(SAY_LINE_ZANG_6); // to player
                events.ScheduleEvent(EVENT_7, 13000);
                break;
            case EVENT_7:
                Talk(SAY_LINE_ZANG_7); // to player
                events.ScheduleEvent(EVENT_8, 12000);
                break;
            case EVENT_8:
                Talk(SAY_LINE_ZANG_8);
                events.ScheduleEvent(EVENT_9, 7000);
                break;
            case EVENT_9:
                Talk(SAY_LINE_ZANG_9);
                events.ScheduleEvent(EVENT_10, 8000);
                break;
            case EVENT_10:
                Talk(SAY_LINE_ZANG_10);
                events.ScheduleEvent(EVENT_11, 7000);
                break;
            case EVENT_11:
                Talk(SAY_LINE_ZANG_11); // to player
                events.ScheduleEvent(EVENT_12, 16000);
                break;
            case EVENT_12:
                Talk(SAY_LINE_ZANG_12);
                events.ScheduleEvent(EVENT_13, 11000);
                break;
            case EVENT_13:
                Talk(SAY_LINE_ZANG_13);
                events.ScheduleEvent(EVENT_14, 7000);
                break;
            case EVENT_14:
                Talk(SAY_LINE_ZANG_14); // to player
                events.ScheduleEvent(EVENT_15, 6000);
                break;
            case EVENT_15:
                Talk(SAY_LINE_ZANG_15);
                events.ScheduleEvent(EVENT_16, 11000);
                break;
            case EVENT_16:
                Talk(SAY_LINE_ZANG_16); // to player
                events.ScheduleEvent(EVENT_17, 16000);
                break;
            case EVENT_17:
                Talk(SAY_LINE_ZANG_17);
                events.ScheduleEvent(EVENT_18, 12000);
                break;
            case EVENT_18:
                Talk(SAY_LINE_ZANG_18);
                break;
            }
        }
    }
};

struct npc_hemet_nesingwary_vh : ScriptedAI
{
    npc_hemet_nesingwary_vh(Creature* creature) : ScriptedAI(creature) {}

    EventMap events;

    void Reset() override
    {
        events.ScheduleEvent(EVENT_1, 5000);
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_1:
                Talk(SAY_LINE_HEMET_NESINGWARY_1);
                events.ScheduleEvent(EVENT_2, 7000);
                break;
            case EVENT_2:
                Talk(SAY_LINE_HEMET_NESINGWARY_2);
                events.ScheduleEvent(EVENT_3, 8000);
                break;
            case EVENT_3:
                Talk(SAY_LINE_HEMET_NESINGWARY_3);
                events.ScheduleEvent(EVENT_4, 9000);
                break;
            case EVENT_4:
                Talk(SAY_LINE_HEMET_NESINGWARY_4);
                events.ScheduleEvent(EVENT_5, 7000);
                break;
            case EVENT_5:
                Talk(SAY_LINE_HEMET_NESINGWARY_5);
                events.ScheduleEvent(EVENT_6, 6000);
                break;
            case EVENT_6:
                Talk(SAY_LINE_HEMET_NESINGWARY_6);
                events.ScheduleEvent(EVENT_7, 9000);
                break;
            case EVENT_7:
                Talk(SAY_LINE_HEMET_NESINGWARY_7);
                events.ScheduleEvent(EVENT_8, 11000);
                break;
            case EVENT_8:
                Talk(SAY_LINE_HEMET_NESINGWARY_8);
                events.ScheduleEvent(EVENT_9, 10000);
                break;
            case EVENT_9:
                Talk(SAY_LINE_HEMET_NESINGWARY_9);
                events.ScheduleEvent(EVENT_10, 12000);
                break;
            case EVENT_10:
                Talk(SAY_LINE_HEMET_NESINGWARY_10);
                events.ScheduleEvent(EVENT_11, 3000);
                break;
            case EVENT_11:
                Talk(SAY_LINE_HEMET_NESINGWARY_11);
                events.ScheduleEvent(EVENT_12, 60000);
                break;
            case EVENT_12:
                Talk(SAY_LINE_HEMET_NESINGWARY_12);
                events.ScheduleEvent(EVENT_13, 76000);
                break;
            case EVENT_13:
                Talk(SAY_LINE_HEMET_NESINGWARY_13);
                break;
            }
        }
    }
};

struct npc_emi_lan_vh : ScriptedAI
{
    npc_emi_lan_vh(Creature* creature) : ScriptedAI(creature) {}

    EventMap events;

    void Reset() override
    {
        events.ScheduleEvent(EVENT_1, 5000);
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_1:
                Talk(SAY_LINE_EMI_LAN_1);
                events.ScheduleEvent(EVENT_2, 8000);
                break;
            case EVENT_2:
                Talk(SAY_LINE_EMI_LAN_2);
                events.ScheduleEvent(EVENT_3, 8000);
                break;
            case EVENT_3:
                Talk(SAY_LINE_EMI_LAN_3);
                events.ScheduleEvent(EVENT_4, 7000);
                break;
            case EVENT_4:
                Talk(SAY_LINE_EMI_LAN_4);
                events.ScheduleEvent(EVENT_5, 7000);
                break;
            case EVENT_5:
                Talk(SAY_LINE_EMI_LAN_5);
                events.ScheduleEvent(EVENT_6, 7000);
                break;
            case EVENT_6:
                Talk(SAY_LINE_EMI_LAN_6);
                events.ScheduleEvent(EVENT_7, 9000);
                break;
            case EVENT_7:
                Talk(SAY_LINE_EMI_LAN_7);
                events.ScheduleEvent(EVENT_8, 5000);
                break;
            case EVENT_8:
                Talk(SAY_LINE_EMI_LAN_8);
                events.ScheduleEvent(EVENT_9, 9000);
                break;
            case EVENT_9:
                Talk(SAY_LINE_EMI_LAN_9);
                events.ScheduleEvent(EVENT_10, 6000);
                break;
            case EVENT_10:
                Talk(SAY_LINE_EMI_LAN_10);
                events.ScheduleEvent(EVENT_11, 6000);
                break;
            case EVENT_11:
                Talk(SAY_LINE_EMI_LAN_11);
                break;
            }
        }
    }
};

struct npc_scgnottz_ballon : ScriptedAI
{
    npc_scgnottz_ballon(Creature* creature) : ScriptedAI(creature) {}

    EventMap events;
    std::list<ObjectGuid> listPlayers;
    uint32 vote;

    void Reset() override
    {
        vote = 0;
        events.ScheduleEvent(EVENT_1, 43000);
    }

    void PassengerBoarded(Unit* who, int8 /*seatId*/, bool apply)
    {
        if (who && who->IsPlayer())
            listPlayers.push_back(who->GetGUID());
    }

    void DoAction(int32 const action) override
    {
        switch (action)
        {
        case ACTION_1:
            vote++;
            if (vote == 3)
            {
                Talk(SAY_LINE_BALLOON);
                if (auto schnottz = me->FindNearestCreature(NPC_SCHNOTTZ_VH, 5.0f))
                    schnottz->GetAI()->DoAction(ACTION_1);
            }
            break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_1:
                if (!listPlayers.empty())
                {
                    for (auto& target : listPlayers)
                    {
                        if (Player* player = ObjectAccessor::GetPlayer(*me, target))
                            player->AddAura(SPELL_SCHNOTTZ_HAS_TO_GO_OR, player);
                    }
                }
                break;
            }
        }
    }
};

struct npc_sky_chariot : ScriptedAI
{
    npc_sky_chariot(Creature* creature) : ScriptedAI(creature) {}

    EventMap events;
    std::list<ObjectGuid> listPlayers;

    void Reset() override
    {
        events.ScheduleEvent(EVENT_1, 85000);
    }

    void PassengerBoarded(Unit* who, int8 /*seatId*/, bool apply)
    {
        if (who && who->IsPlayer())
            listPlayers.push_back(who->GetGUID());
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_1:
                if (!listPlayers.empty())
                {
                    for (auto& target : listPlayers)
                    {
                        if (Player* player = ObjectAccessor::GetPlayer(*me, target))
                        {
                            DoCast(65785);
                            DoCast(SPELL_JUNIOR_BALLONER_PREPAREDNESS_KIT);
                            player->CastSpell(player, 126408, false);
                            player->AddAura(SPELL_JUNIOR_BALLONER_PREPAREDNESS_KIT, player);
                            me->DespawnOrUnsummon(2000);
                        }
                    }
                }
                break;
            }
        }
    }
};

//236346
class spell_schnottz_has_to_go : public SpellScript
{
    PrepareSpellScript(spell_schnottz_has_to_go);

    void HandleOnCast()
    {
        if (Unit* caster = GetCaster())
            if (auto vehicle = caster->FindNearestCreature(NPC_SCHNOTTZ_BALLON_VH, 5.0f))
                vehicle->GetAI()->DoAction(ACTION_1);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_schnottz_has_to_go::HandleOnCast);
    }
};

void AddSC_SpringBalloonFestival()
{
    RegisterCreatureAI(npc_chens);
    RegisterCreatureAI(npc_scgnottz);
    RegisterCreatureAI(npc_lin_cloudwalker);
    RegisterCreatureAI(npc_rungle);
    RegisterCreatureAI(npc_zang_cloudwalker);
    RegisterCreatureAI(npc_hemet_nesingwary);
    RegisterCreatureAI(npc_emi_lan);
    RegisterCreatureAI(npc_chens_vh);
    RegisterCreatureAI(npc_scgnottz_vh);
    RegisterCreatureAI(npc_lin_cloudwalker_vh);
    RegisterCreatureAI(npc_boscoe_vh);
    RegisterCreatureAI(npc_rungle_vh);
    RegisterCreatureAI(npc_zang_cloudwalker_vh);
    RegisterCreatureAI(npc_hemet_nesingwary_vh);
    RegisterCreatureAI(npc_emi_lan_vh);
    RegisterCreatureAI(npc_scgnottz_ballon);
    RegisterCreatureAI(npc_sky_chariot);
    RegisterSpellScript(spell_schnottz_has_to_go);
}