/*
    http://uwow.biz
    Dungeon : Violet Hold Legion 100-110
    Encounter: Mindflayer Kaahrj
    Normal: 100%, Heroic: 100%, Mythic: 100%
*/

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "violet_hold_legion.h"

enum Says
{
    SAY_AGGRO           = 0,
   // SAY_ETERNAL         = 4, 6, 10
   // SAY_RANDOM          = 2, 8 
   // SAY_       ???    = 12 (Твоя душа будет блуждать по бесконечной спирали.) , 14 (Древние зовут тебя. Иди... иди... иди..), 16 (Безглазый змей смотрит с бесконечных небес.)
    SAY_DEATH           = 18,
};

enum Spells
{
    SPELL_SHADOW_CRASH              = 201920,
    SPELL_SHADOW_CRASH_AT           = 201120,
    SPELL_DOOM                      = 201148,
    SPELL_HYSTERIA                  = 201146,
    SPELL_ETERNAL_DARKNESS_SUM      = 201153,

    //Summons
    SPELL_ETERNAL_DARKNESS_DMG      = 201172,

    //Heroic
    SPELL_COLLAPSING_SHADOWS_AT     = 201256, //201255 dmg
    SPELL_COLLAPSING_SHADOWS_MOD    = 201287, //Mod force speed ?
};

enum eEvents
{
    EVENT_SHADOW_CRASH          = 1,
    EVENT_DOOM                  = 2,
    EVENT_HYSTERIA              = 3,
    EVENT_ETERNAL_DARKNESS      = 4,
};

//101950
class boss_mindflayer_kaahrj : public CreatureScript
{
public:
    boss_mindflayer_kaahrj() : CreatureScript("boss_mindflayer_kaahrj") {}

    struct boss_mindflayer_kaahrjAI : public BossAI
    {
        boss_mindflayer_kaahrjAI(Creature* creature) : BossAI(creature, DATA_KAAHRJ), timer(0)
        {
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
            removeloot = false;
        }

        bool removeloot;
        uint32 timer;

        void Reset() override
        {
            _Reset();

            if (instance->GetData(DATA_MAIN_EVENT_PHASE) == IN_PROGRESS)
                me->SetReactState(REACT_DEFENSIVE);
            
            timer = 13000;
        }

        void EnterCombat(Unit* /*who*/) override
        //49:10
        {
            TalkandSendWhisperToAll(SAY_AGGRO);
            _EnterCombat();

            events.ScheduleEvent(EVENT_SHADOW_CRASH, 4000); //49:14, 49:26, 49:50, 50:03
            events.ScheduleEvent(EVENT_DOOM, 8000);         //49:18, 49:30, 49:38, 49:54, 50:06
            events.ScheduleEvent(EVENT_HYSTERIA, 13000);    //49:23, 49:35, 49:59, 50:11
            events.ScheduleEvent(EVENT_ETERNAL_DARKNESS, 30000); //49:41, 50:17, 50:55
        }

        void JustDied(Unit* /*killer*/) override
        {
            TalkandSendWhisperToAll(SAY_DEATH);
            _JustDied();

            if (removeloot)
                me->RemoveFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
        }

        void DoAction(int32 const action) override
        {
            if (action == ACTION_REMOVE_LOOT)
                removeloot = true;
        }

        void OnAreaTriggerDespawn(uint32 spellId, Position pos, bool duration) override
        {
            if (duration && spellId == SPELL_SHADOW_CRASH_AT)
                if (GetDifficultyID() != DIFFICULTY_NORMAL)
                    me->CastSpell(pos, SPELL_COLLAPSING_SHADOWS_AT, true);
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            if (timer <= diff)
            {
                if (urand(1, 2) == 1)
                    TalkandSendWhisperToAll(2);
                else
                    TalkandSendWhisperToAll(8);
            } else timer -= diff;
            
            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_SHADOW_CRASH:
                        DoCast(SPELL_SHADOW_CRASH);
                        events.ScheduleEvent(EVENT_SHADOW_CRASH, 12000);
                        break;
                    case EVENT_DOOM:
                        DoCastVictim(SPELL_DOOM);
                        events.ScheduleEvent(EVENT_DOOM, 12000);
                        break;
                    case EVENT_HYSTERIA:
                        if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 60.0f, true))
                            DoCast(pTarget, SPELL_HYSTERIA);
                        events.ScheduleEvent(EVENT_HYSTERIA, 12000);
                        break;
                    case EVENT_ETERNAL_DARKNESS:
                        DoCast(SPELL_ETERNAL_DARKNESS_SUM);
                        uint8 rand = urand(1, 3);
                        if (rand == 1)
                            TalkandSendWhisperToAll(4);
                        if (rand == 2)
                            TalkandSendWhisperToAll(6);
                        if (rand == 3)
                            TalkandSendWhisperToAll(10);
                        events.ScheduleEvent(EVENT_ETERNAL_DARKNESS, 36000);
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
        
        void TalkandSendWhisperToAll(uint32 text)
        {
            timer += 13000; //это должно быть тут (рандомная фраза продлевает таймер. от других фраз в том числе)
            Talk(text);
            text += 1;
            Map::PlayerList const& players = me->GetMap()->GetPlayers();

            for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
            {
                if (Player* player = itr->getSource())
                    Talk(text, player->GetGUID());
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_mindflayer_kaahrjAI (creature);
    }
};

//101994
class npc_kaahrj_faceless_tendril : public CreatureScript
{
public:
    npc_kaahrj_faceless_tendril() : CreatureScript("npc_kaahrj_faceless_tendril") {}

    struct npc_kaahrj_faceless_tendrilAI : public ScriptedAI
    {
        npc_kaahrj_faceless_tendrilAI(Creature* creature) : ScriptedAI(creature), eDarknessTimer(0)
        {
            me->SetReactState(REACT_PASSIVE);
        }

        uint16 eDarknessTimer;

        void Reset() override
        {
            eDarknessTimer = 500;
        }

        void IsSummonedBy(Unit* summoner) override
        {
            DoZoneInCombat(me, 120.0f);
        }

        void UpdateAI(uint32 diff) override
        {
            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (eDarknessTimer <= diff)
            {
                eDarknessTimer = 3000;
                DoCast(me, SPELL_ETERNAL_DARKNESS_DMG, true);
            }
            else
                eDarknessTimer -= diff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_kaahrj_faceless_tendrilAI(creature);
    }
};

void AddSC_boss_mindflayer_kaahrj()
{
    new boss_mindflayer_kaahrj();
    new npc_kaahrj_faceless_tendril();
}