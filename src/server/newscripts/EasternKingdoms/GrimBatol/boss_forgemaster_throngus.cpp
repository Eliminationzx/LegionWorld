#include "grim_batol.h"

//todo: реализовать адов и mighty stomp

enum ScriptTexts
{
    SAY_AGGRO   = 0,
    SAY_KILL    = 1,
    SAY_DEATH   = 2,
    SAY_SHIELD  = 3,
    SAY_MACE    = 5,
    SAY_DUAL    = 4
};

enum Equipment
{
    EQUIPMENT_ID_SHIELD     = 40400,
    EQUIPMENT_ID_SWORD      = 65094,
    EQUIPMENT_ID_MACE       = 65090 
};

enum Spells
{
    SPELL_MIGHTY_STOMP          = 74984,
    SPELL_PICK_WEAPON           = 75000,
    SPELL_DUAL_BLADES           = 74981,
    SPELL_ENCUMBERED            = 75007,
    SPELL_IMPALING_SLAM_0       = 75057,
    SPELL_IMPALING_SLAM         = 75056,
    SPELL_DISORIENTING_ROAR     = 74976,
    SPELL_BURNING_SHIELD        = 90819,
    SPELL_PERSONAL_PHALANX      = 74908,
    SPELL_FLAMING_ARROW         = 45101,
    SPELL_FLAMING_ARROW_VISUAL  = 74944,
    SPELL_CAVE_IN_DUMMY         = 74987,
    SPELL_CAVE_IN               = 74990,
    SPELL_FIRE_PATCH_AURA       = 90752,
    SPELL_LAVA_PATCH_DMG        = 90754,
    SPELL_BURNING_BLADES        = 90759,
    SPELL_LAVAL_MARRE           = 90743
};

enum Adds
{
    NPC_CAVE_IN_STALKER     = 40228,
    NPC_TWILIGHT_ARCHER     = 40197,
    NPC_FIRE_PATCH          = 48711
};

enum Events
{
    EVENT_PICK_WEAPON           = 1,
    EVENT_DISORIENTING_ROAR     = 2,
    EVENT_IMPALING_SLAM         = 3,
    EVENT_FLAMING_ARROW         = 4,
    EVENT_CAVE_IN_DUMMY         = 8,
    EVENT_CAVE_IN_DMG           = 9,
    EVENT_ARCHER_SHOOT          = 10,
    EVENT_STOMP                 = 11
};

enum Others
{
    ACTION_MACE         = 1,
    ACTION_DUAL_BLADES  = 2,
    ACTION_SHIELD       = 3
};


const Position twilightarcherPos[13] =
{
    {-542.994f, -605.236f, 300.201f, 1.68049f},
    {-543.59f, -605.413f, 283.784f, 1.50377f},
    {-521.237f, -605.435f, 300.76f, 1.63886f},
    {-483.862f, -588.658f, 297.574f, 2.38106f},
    {-482.655f, -588.461f, 280.966f, 2.34571f},
    {-471.266f, -575.324f, 295.906f, 2.30254f},
    {-525.377f, -455.312f, 285.288f, 4.66187f},
    {-544.49f, -454.961f, 295.831f, 4.79539f},
    {-522.164f, -455.31f, 299.791f, 4.77575f},
    {-468.703f, -489.004f, 300.462f, 3.78616f},
    {-470.907f, -484.791f, 282.203f, 3.87255f},
    {-485.052f, -474.621f, 296.525f, 3.92361f},
    {-481.352f, -477.21f, 280.714f, 3.72334f}
};

class boss_forgemaster_throngus : public CreatureScript
{
    public:
        boss_forgemaster_throngus() : CreatureScript("boss_forgemaster_throngus") {}

        struct boss_forgemaster_throngusAI : public ScriptedAI
        {
            boss_forgemaster_throngusAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            EventMap events;
            uint32 uiLastWeapon;
            bool mace;
            bool blades;
            bool shield;

            void Reset() override
            {
                if (!instance)
                    return;

                SetEquipmentSlots(false, 0, 0, 0);
                events.Reset();
                DespawnAllSummons();

                mace = false;
                blades = false;
                shield = false;

                if (instance)
                    instance->SetData(DATA_FORGEMASTER_THRONGUS, NOT_STARTED);
            }

            void EnterCombat(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);
                events.ScheduleEvent(EVENT_PICK_WEAPON, 10000);
                if (instance)
                    instance->SetData(DATA_FORGEMASTER_THRONGUS, IN_PROGRESS);
            }
            
            void JustDied(Unit* /*killer*/) override
            {
                Talk(SAY_DEATH);
                if (instance)
                    instance->SetData(DATA_FORGEMASTER_THRONGUS, DONE);
            }

            void DespawnAllSummons()
            {
                std::list<Creature*> list;
                list.clear();
                me->GetCreatureListWithEntryInGrid(list, 40228, 200.0f);
                me->GetCreatureListWithEntryInGrid(list, 48711, 200.0f);
                me->GetCreatureListWithEntryInGrid(list, 40197, 200.0f);
                if (!list.empty())
                    for (std::list<Creature*>::const_iterator itr = list.begin(); itr != list.end(); itr++)
                        (*itr)->DespawnOrUnsummon();
            }

            void KilledUnit(Unit* victim) override
            {
                if (!victim)
                    return;

                if (victim->IsPlayer())
                    Talk(SAY_KILL);
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
                    case EVENT_PICK_WEAPON:
                        events.DelayEvents(3000);
                        DoCast(SPELL_PICK_WEAPON);
                        me->RemoveAura(SPELL_BURNING_BLADES);
                        events.ScheduleEvent(EVENT_PICK_WEAPON, 32500);
                        break;
                    case EVENT_IMPALING_SLAM:
                        if (mace)
                        {
                            if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                            {
                                DoCast(target, SPELL_IMPALING_SLAM);
                                DoCast(target, SPELL_IMPALING_SLAM_0);
                            }
                            events.ScheduleEvent(EVENT_IMPALING_SLAM, 12000);
                        }
                        break;
                    case EVENT_DISORIENTING_ROAR:
                        if (blades)
                        {
                            DoCast(SPELL_DISORIENTING_ROAR);
                            events.ScheduleEvent(EVENT_DISORIENTING_ROAR, 12000);
                        }
                        break;
                    case EVENT_STOMP:
                        if (!shield)
                        {
                            DoCast(SPELL_MIGHTY_STOMP);
                            events.ScheduleEvent(EVENT_STOMP, 15000);
                        }
                        break;
                    }
                }
                DoMeleeAttackIfReady();
            }

            void DoAction(const int32 action) override
            {
                switch (action)
                {
                case ACTION_MACE:
                    mace = true;
                    blades = false;
                    shield = false;
                    DoCast(me, SPELL_ENCUMBERED, true);
                    DoCast(SPELL_LAVAL_MARRE);
                    Talk(SAY_MACE);
                    SetEquipmentSlots(false, EQUIPMENT_ID_MACE, 0, 0);
                    events.ScheduleEvent(EVENT_IMPALING_SLAM, 7000);
                    events.ScheduleEvent(EVENT_STOMP, urand(5000, 9000));
                    break;
                case ACTION_DUAL_BLADES:
                    blades = true;
                    mace = false;
                    shield = false;
                    DoCast(me, SPELL_DUAL_BLADES, true);
                    Talk(SAY_DUAL);
                    SetEquipmentSlots(false, EQUIPMENT_ID_SWORD, EQUIPMENT_ID_SWORD, 0);
                    events.ScheduleEvent(EVENT_DISORIENTING_ROAR, 7000);
                    events.ScheduleEvent(EVENT_STOMP, 9000);
                    if (IsHeroic())
                        DoCast(SPELL_BURNING_BLADES);
                    break;
                case ACTION_SHIELD:
                    if (IsHeroic())
                        DoCast(SPELL_BURNING_SHIELD);
                    shield = true;
                    mace = false;
                    blades = false;
                    DoCast(me, SPELL_PERSONAL_PHALANX, true);
                    Talk(SAY_SHIELD);
                    SetEquipmentSlots(false, 0, EQUIPMENT_ID_SHIELD, 0);
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_forgemaster_throngusAI(creature);
        }
};

class npc_throngus_fire_patch : public CreatureScript
{
    public:
        npc_throngus_fire_patch() : CreatureScript("npc_throngus_fire_patch") {}

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_throngus_fire_patchAI(pCreature);
        }

        struct npc_throngus_fire_patchAI : public Scripted_NoMovementAI
        {
            npc_throngus_fire_patchAI(Creature *c) : Scripted_NoMovementAI(c) {}      

            void Reset() override
            {
                DoCast(SPELL_FIRE_PATCH_AURA);
            }
        };
};

class spell_throngus_pick_weapon : public SpellScriptLoader
{
    public:
        spell_throngus_pick_weapon() : SpellScriptLoader("spell_throngus_pick_weapon") {}

        class spell_throngus_pick_weapon_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_throngus_pick_weapon_SpellScript);

            void HandleScriptEffect(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);

                Unit* caster = GetCaster();
                if (!caster)
                    return;

                switch (urand(0, 2))
                {
                case 0:
                    caster->GetAI()->DoAction(ACTION_DUAL_BLADES);
                    break;
                case 1:
                    caster->GetAI()->DoAction(ACTION_MACE);
                    break;
                case 2:
                    caster->GetAI()->DoAction(ACTION_SHIELD);
                    break;
                }                
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_throngus_pick_weapon_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_throngus_pick_weapon_SpellScript();
        }
};

struct npc_twilight_archer : public ScriptedAI
{
    npc_twilight_archer(Creature* creature) : ScriptedAI(creature)
    {
        creature->SetReactState(REACT_PASSIVE);
    }

    EventMap events;

    void IsSummonedBy(Unit* summoner)
    {
        DoZoneInCombat();
        events.ScheduleEvent(EVENT_ARCHER_SHOOT, urand(1700, 2500));
    }

    void UpdateAI(uint32 diff)
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_ARCHER_SHOOT:
                if (auto nearPlayer = SelectTarget(SELECT_TARGET_NEAREST, 0, 100.0f, true))
                {
                    DoCast(nearPlayer, SPELL_FLAMING_ARROW_VISUAL, true);
                    DoCast(nearPlayer, SPELL_FLAMING_ARROW, true);
                }
                events.ScheduleEvent(EVENT_ARCHER_SHOOT, urand(1700, 2500));
                break;
            }
        }
    }
};

struct npc_cave_in_stalker : public ScriptedAI
{
    npc_cave_in_stalker(Creature* creature) : ScriptedAI(creature)
    {
        creature->SetReactState(REACT_PASSIVE);
    }

    void IsSummonedBy(Unit* summoner)
    {
        DoCast(SPELL_CAVE_IN);
        DoCast(SPELL_CAVE_IN_DUMMY);
    }
};

void AddSC_boss_forgemaster_throngus()
{
    new boss_forgemaster_throngus();
    new npc_throngus_fire_patch();
    new spell_throngus_pick_weapon();
    RegisterCreatureAI(npc_twilight_archer);
    RegisterCreatureAI(npc_cave_in_stalker);
}