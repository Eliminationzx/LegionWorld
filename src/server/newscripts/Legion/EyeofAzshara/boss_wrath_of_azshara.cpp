/*
    http://uwow.biz
    Dungeon : Eye of Azshara 100-110
    Encounter: Wrath of Azshara
    Normal: 83%, Heroic: 100%, Mythic: 100%
    Need implemented: EVENT_CRY_OF_WRATH
*/

#include "eye_of_azshara.h"

enum Says
{
    SAY_START           = 1,
    SAY_AGGRO           = 2,
    SAY_DELUGE          = 3,
    SAY_ARCANE_EMOTE    = 4,
    SAY_ARCANE          = 5,
    SAY_CRY_EMOTE       = 6,
    SAY_CRY             = 7,
    SAY_STORMS          = 8,
    SAY_DEATH           = 9,
};

enum Spells
{
    //Pre-event spells
    SPELL_SURGING_WATERS        = 192632,
    SPELL_TIDAL_WAVE            = 192940,
    SPELL_DAMAGE_SELF_20PCT     = 193500,
    SPELL_TEMPEST_ATTUNEMENT    = 193491,
    SPELL_STORM_CONDUIT         = 193196,

    SPELL_MYSTIC_TORNADO        = 192680,
    SPELL_MASSIVE_DELUGE        = 192620,
    SPELL_ARCANE_BOMB           = 192705,
    SPELL_ARCANE_BOMB_VEH_SEAT  = 192706,
    SPELL_ARCANE_BOMB_VISUAL    = 192711,
    SPELL_ARCANE_BOMB_DMG       = 192708,
    SPELL_CRY_OF_WRATH          = 192985,
    SPELL_RAGING_STORMS         = 192696,
    SPELL_HEAVING_SANDS         = 197164,

    //Heroic
    SPELL_CRUSHING_DEPTHS       = 197365,
    SPELL_MAGIC_RESONANCE       = 196665,
    SPELL_FROST_RESONANCE       = 196666,

    SPELL_MYSTIC_TORNADO_AT     = 192673,
    SPELL_LIGHTNING_STRIKES_1   = 192989, //2s tick
    SPELL_LIGHTNING_STRIKES_2   = 192990, //7s tick
};

enum eEvents
{
    EVENT_MYSTIC_TORNADO        = 1,
    EVENT_MASSIVE_DELUGE        = 2,
    EVENT_ARCANE_BOMB           = 3,
    EVENT_CRY_OF_WRATH          = 4,
    EVENT_RAGING_STORMS         = 5,
    //Heroic
    EVENT_CRUSHING_DEPTHS       = 6,
};

Position const nagPos[4] =
{
    {-3511.70f, 4479.83f, 1.61f, 5.00f},
    {-3474.94f, 4281.46f, 1.89f, 1.65f},
    {-3382.71f, 4354.07f, 1.39f, 2.83f},
    {-3415.23f, 4442.80f, 1.09f, 3.79f},
};

//96028
class boss_wrath_of_azshara : public CreatureScript
{
public:
    boss_wrath_of_azshara() : CreatureScript("boss_wrath_of_azshara") {}

    struct boss_wrath_of_azsharaAI : public BossAI
    {
        boss_wrath_of_azsharaAI(Creature* creature) : BossAI(creature, DATA_WRATH_OF_AZSHARA) 
        {
            SetCombatMovement(false);
            nagaDiedCount = 0;
            SummonNagas();
            me->SetHealth(me->CountPctFromMaxHealth(21));
            me->setRegeneratingHealth(false);
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
            me->SetVisible(false);
        }

        bool lowHP;
        uint8 nagaDiedCount;
        uint16 checkVictimTimer;

        void Reset() override
        {
            _Reset();
            lowHP = false;
            checkVictimTimer = 2000;

            if (!me->HasAura(SPELL_SURGING_WATERS))
                DoCast(me, SPELL_SURGING_WATERS, true);
            DoCast(me, SPELL_HEAVING_SANDS, true);

            me->SetHealth(me->CountPctFromMaxHealth(21));
            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MAGIC_RESONANCE);
            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FROST_RESONANCE);
        }

        void EnterCombat(Unit* /*who*/) override
        {
            Talk(SAY_AGGRO);
            _EnterCombat();
            DoCast(me, SPELL_TIDAL_WAVE, true);

            events.ScheduleEvent(EVENT_MYSTIC_TORNADO, 9000);
            events.ScheduleEvent(EVENT_MASSIVE_DELUGE, 12000);
            events.ScheduleEvent(EVENT_ARCANE_BOMB, 23000);

            if (GetDifficultyID() != DIFFICULTY_NORMAL)
                events.ScheduleEvent(EVENT_CRUSHING_DEPTHS, 20000);
        }

        void JustDied(Unit* /*killer*/) override
        {
            Talk(SAY_DEATH);
            _JustDied();
            instance->DoUpdateAchievementCriteria(CRITERIA_TYPE_COMPLETE_DUNGEON_ENCOUNTER, 1814);
            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MAGIC_RESONANCE);
            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FROST_RESONANCE);
        }

        void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType dmgType) override
        {
            if (me->HealthBelowPct(11) && !lowHP)
            {
                lowHP = true;
                events.ScheduleEvent(EVENT_CRY_OF_WRATH, 0);
            }
        }

        void SpellHitTarget(Unit* target, const SpellInfo* spell) override
        {
            switch (spell->Id)
            {
                case SPELL_ARCANE_BOMB:
                {
                    if (target->GetTypeId() == TYPEID_PLAYER)
                        Talk(SAY_ARCANE_EMOTE, target->ToPlayer()->GetGUID());
                    Talk(SAY_ARCANE);
                    break;
                }
                case SPELL_ARCANE_BOMB_VEH_SEAT:
                {
                    if (Creature* sum = target->SummonCreature(NPC_ARCANE_BOMB, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 18000))
                    {
                        //sum->EnterVehicle(target);
                        sum->CastSpell(sum, SPELL_ARCANE_BOMB_VISUAL, true);
                        sum->CastSpell(sum, SPELL_ARCANE_BOMB_DMG);
                    }
                    break;
                }
            }

            if (GetDifficultyID() != DIFFICULTY_NORMAL)
            {
                switch (spell->Id)
                {
                    case 192619: //Massive Deluge
                    case SPELL_RAGING_STORMS:
                        DoCast(target, SPELL_FROST_RESONANCE, true);
                        break;
                    case SPELL_ARCANE_BOMB_DMG:
                        DoCast(target, SPELL_MAGIC_RESONANCE, true);
                        break;
                }
            }
        }

        void SummonNagas()
        {
            me->SummonCreature(NPC_MYSTIC_SSAVEH, nagPos[0]);
            me->SummonCreature(NPC_RITUALIST_LESHA, nagPos[1]);
            me->SummonCreature(NPC_CHANNELER_VARISZ, nagPos[2]);
            me->SummonCreature(NPC_BINDER_ASHIOI, nagPos[3]);
        }

        void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
        {
            switch (summon->GetEntry())
            {
                case NPC_MYSTIC_SSAVEH:
                case NPC_RITUALIST_LESHA:
                case NPC_CHANNELER_VARISZ:
                case NPC_BINDER_ASHIOI:
                    nagaDiedCount++;
                    //DoCast(me, SPELL_DAMAGE_SELF_20PCT, true);
                    break;
            }
            if (nagaDiedCount == 4)
            {
                me->RemoveAurasDueToSpell(SPELL_STORM_CONDUIT);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                me->SetReactState(REACT_DEFENSIVE);
                me->SetHealth(me->CountPctFromMaxHealth(21));
                Talk(SAY_START);
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (checkVictimTimer <= diff)
            {
                if (me->getVictim())
                    if (!me->IsWithinMeleeRange(me->getVictim()))
                        events.ScheduleEvent(EVENT_RAGING_STORMS, 500);

                checkVictimTimer = 2000;
            }
            else checkVictimTimer -= diff;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_MYSTIC_TORNADO:
                        DoCast(me, SPELL_MYSTIC_TORNADO, true);
                        events.ScheduleEvent(EVENT_MYSTIC_TORNADO, 25000);
                        break;
                    case EVENT_MASSIVE_DELUGE:
                        DoCast(SPELL_MASSIVE_DELUGE);
                        Talk(SAY_DELUGE);
                        events.ScheduleEvent(EVENT_MASSIVE_DELUGE, 48000);
                        break;
                    case EVENT_ARCANE_BOMB:
                        DoCast(SPELL_ARCANE_BOMB);
                        events.ScheduleEvent(EVENT_ARCANE_BOMB, 30000);
                        break;
                    case EVENT_CRY_OF_WRATH:
                        DoCast(SPELL_CRY_OF_WRATH);
                        Talk(SAY_CRY_EMOTE);
                        Talk(SAY_CRY);
                        break;
                    case EVENT_RAGING_STORMS:
                        DoCast(SPELL_RAGING_STORMS);
                        Talk(SAY_STORMS);
                        break;
                    case EVENT_CRUSHING_DEPTHS:
                        //Talk();
                        if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                            DoCast(SPELL_CRUSHING_DEPTHS);
                        events.ScheduleEvent(EVENT_CRUSHING_DEPTHS, 40000);
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_wrath_of_azsharaAI (creature);
    }
};

//98173, 100248, 100249, 100250
class npc_wrath_of_azshara_nagas : public CreatureScript
{
public:
    npc_wrath_of_azshara_nagas() : CreatureScript("npc_wrath_of_azshara_nagas") {}

    struct npc_wrath_of_azshara_nagasAI : public ScriptedAI
    {
        npc_wrath_of_azshara_nagasAI(Creature* creature) : ScriptedAI(creature) {}

        EventMap events;
        bool activateConduit;

        void Reset() override
        {
            events.Reset();
        }

        void IsSummonedBy(Unit* summoner) override
        {
            activateConduit = false;
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
            DoCast(me, SPELL_TEMPEST_ATTUNEMENT, true);
        }

        void EnterCombat(Unit* /*who*/) override
        {
            me->CastStop();
            events.ScheduleEvent(EVENT_1, 6000);
            events.ScheduleEvent(EVENT_2, 9000);

            if (me->GetEntry() != 98173)
                events.ScheduleEvent(EVENT_3, 18000);
        }

        void JustDied(Unit* /*killer*/) override
        {
            Talk(0);
        }

        void DoAction(int32 const action) override
        {
            if (activateConduit)
                return;

            activateConduit = true;
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
            me->SetReactState(REACT_AGGRESSIVE);
            me->InterruptNonMeleeSpells(false, SPELL_TEMPEST_ATTUNEMENT);
            DoCast(me, SPELL_STORM_CONDUIT, true);
        }

        void JustReachedHome() override
        {
            DoCast(me, SPELL_STORM_CONDUIT, true);
        }

        void SpellHitTarget(Unit* target, const SpellInfo* spell) override
        {
            if (spell->Id == SPELL_ARCANE_BOMB_VEH_SEAT)
                if (Creature* sum = target->SummonCreature(NPC_ARCANE_BOMB, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 18000))
                {
                    sum->CastSpell(sum, SPELL_ARCANE_BOMB_VISUAL, true);
                    sum->CastSpell(sum, SPELL_ARCANE_BOMB_DMG);
                }
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_1:
                        DoCast(196516);
                        events.ScheduleEvent(EVENT_1, 6000);
                        break;                   
                    case EVENT_2:
                        if (me->GetEntry() == NPC_BINDER_ASHIOI)
                            DoCast(196515);
                        if (me->GetEntry() == NPC_MYSTIC_SSAVEH)
                            DoCast(196870);                        
                        if (me->GetEntry() == NPC_CHANNELER_VARISZ)
                            DoCast(197105);                        
                        if (me->GetEntry() == NPC_RITUALIST_LESHA)
                            DoCast(196027);
                        events.ScheduleEvent(EVENT_2, 18000);
                        break;                    
                    case EVENT_3:
                        DoCast(SPELL_ARCANE_BOMB);
                        events.ScheduleEvent(EVENT_3, 18000);
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_wrath_of_azshara_nagasAI(creature);
    }
};

//97673
class npc_wrath_of_azshara_mystic_tornado : public CreatureScript
{
public:
    npc_wrath_of_azshara_mystic_tornado() : CreatureScript("npc_wrath_of_azshara_mystic_tornado") {}

    struct npc_wrath_of_azshara_mystic_tornadoAI : public ScriptedAI
    {
        npc_wrath_of_azshara_mystic_tornadoAI(Creature* creature) : ScriptedAI(creature) 
        {
            me->SetReactState(REACT_PASSIVE);
        }

        EventMap events;
        uint16 randMoveTimer;

        void Reset() override {}

        void IsSummonedBy(Unit* summoner) override
        {
            if (!summoner->isInCombat())
            {
                me->DespawnOrUnsummon();
                return;
            }

            DoCast(me, SPELL_MYSTIC_TORNADO_AT, true);
            randMoveTimer = 10000;
        }

        void UpdateAI(uint32 diff) override
        {
            events.Update(diff);

            if (randMoveTimer <= diff)
            {
                randMoveTimer = 10000;
                float angle = (float)rand_norm() * static_cast<float>(2 * M_PI);
                float distance = 15 * (float)rand_norm();
                float x = me->GetHomePosition().GetPositionX() + distance * std::cos(angle);
                float y = me->GetHomePosition().GetPositionY() + distance * std::sin(angle);
                float z = me->GetHomePosition().GetPositionZ();
                Trinity::NormalizeMapCoord(x);
                Trinity::NormalizeMapCoord(y);

                me->GetMotionMaster()->MovePoint(1, x, y, z);
            }
            else randMoveTimer -= diff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_wrath_of_azshara_mystic_tornadoAI(creature);
    }
};

//97691
class npc_wrath_of_azshara_arcane_bomb : public CreatureScript
{
public:
    npc_wrath_of_azshara_arcane_bomb() : CreatureScript("npc_wrath_of_azshara_arcane_bomb") {}

    struct npc_wrath_of_azshara_arcane_bombAI : public ScriptedAI
    {
        npc_wrath_of_azshara_arcane_bombAI(Creature* creature) : ScriptedAI(creature) 
        {
            instance = me->GetInstanceScript();
            me->SetReactState(REACT_PASSIVE);
            me->SetSpeed(MOVE_FLIGHT, 1.5f);
        }

        uint16 moveTimer;
        InstanceScript* instance;

        void Reset() override {}

        void IsSummonedBy(Unit* summoner) override
        {
            moveTimer = 500;
        }

        void UpdateAI(uint32 diff) override
        {
            if (moveTimer)
            {
                if (moveTimer <= diff)
                {
                    moveTimer = 1000;
                    if (Unit* owner = me->GetAnyOwner())
                    {
                        if (owner->HasAura(SPELL_ARCANE_BOMB_VEH_SEAT))
                            me->GetMotionMaster()->MovePoint(1, owner->GetPositionX(), owner->GetPositionY(), owner->GetPositionZ(), false);
                        else
                        {
                            moveTimer = 0;
                            me->GetMotionMaster()->Clear();
                        }
                    }
                }
                else
                    moveTimer -= diff;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_wrath_of_azshara_arcane_bombAI(creature);
    }
};

void AddSC_boss_wrath_of_azshara()
{
    new boss_wrath_of_azshara();
    new npc_wrath_of_azshara_nagas();
    new npc_wrath_of_azshara_mystic_tornado();
    new npc_wrath_of_azshara_arcane_bomb();
}