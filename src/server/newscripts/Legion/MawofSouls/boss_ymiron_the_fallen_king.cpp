/*
    http://uwow.biz
    Dungeon : Maw of Souls 100-110
    Encounter: Ymiron the fallen king
    Normal: 100%, Heroic: 100%, Mythic: 100%
*/

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "maw_of_souls.h"

enum Says
{
    SAY_INTRO           = 0,
    SAY_INTRO_2         = 1,
    SAY_AGGRO           = 2,
    SAY_SCREAMS         = 3,
    SAY_WINDS           = 4,
    SAY_BANE            = 5,
    SAY_ARISE           = 6,
    SAY_DEATH           = 7,
};

enum Spells
{
    SPELL_ZERO_REGEN            = 118357,
    SPELL_POWER                 = 167922,
    SPELL_KNEELING_VISUAL       = 197227, //State
    SPELL_SOUL_SIPHON           = 194665, //Hit Target NPC 102375
    SPELL_DARK_SLASH            = 193211,
    SPELL_SCREAMS_OF_DEAD       = 193364,
    SPELL_WINDS_OF_NORTHREND    = 193977,
    SPELL_BANE                  = 193460,
    SPELL_BANE_AT               = 193465,
    SPELL_BANE_DMG              = 193513,
    SPELL_BANE_NOVA             = 200194,
    SPELL_ARISE_FALLEN          = 193566, //Heroic+
    SPELL_ARISE_FALLEN_SUM      = 193594,

    //Trash
    SPELL_STATE                 = 196025,
    SPELL_INSTAKILL             = 117624,

    SPELL_BREAK_CHAINS          = 201375,

    SPELL_VIGOR                 = 203816, //Heroic+
    SPELL_MOD_SCALE             = 123978,
};

enum eEvents
{
    EVENT_DARK_SLASH            = 1,
    EVENT_SCREAMS_OF_DEAD       = 2,
    EVENT_WINDS_OF_NORTHREND    = 3,
    EVENT_BANE                  = 4,
    EVENT_ARISE_FALLEN          = 5,
    EVENT_INTRO                 = 6,

    //Trash
    EVENT_TRASH_INTRO           = 1,
    EVENT_TRASH_INTRO_2         = 2,
    EVENT_FRACTURE              = 3,
    EVENT_BARBED                = 4,
    EVENT_SWIRLING              = 5,
};

Position const trashPos[9] = 
{
    {7390.38f, 7285.93f, 43.86f, 0.32f},
    {7381.98f, 7295.84f, 43.85f, 0.21f},
    {7393.05f, 7312.54f, 43.86f, 5.58f},
    {7387.01f, 7299.40f, 43.86f, 5.83f},
    {7393.36f, 7306.04f, 43.91f, 5.58f},
    {7384.61f, 7286.07f, 43.86f, 0.51f},
    {7386.75f, 7292.16f, 43.86f, 0.05f},
    {7386.51f, 7304.95f, 43.92f, 5.69f},
    {7360.30f, 7301.24f, 43.68f, 1.87f}
};

//96756
class boss_ymiron_the_fallen_king : public CreatureScript
{
public:
    boss_ymiron_the_fallen_king() : CreatureScript("boss_ymiron_the_fallen_king") {}

    struct boss_ymiron_the_fallen_kingAI : public BossAI
    {
        boss_ymiron_the_fallen_kingAI(Creature* creature) : BossAI(creature, DATA_YMIRON)
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_ATTACKABLE_1);

            for (uint8 i = 0; i < 8; ++i)
                me->SummonCreature(NPC_RUNECARVER_INTRO, trashPos[i]);

            instance->instance->SummonCreature(NPC_SEACURSED_SLAVER, trashPos[8]);
        }

        bool intro = true;
        uint8 phase = 0;
        uint32 timer = 0;

        void Reset() override
        {
            _Reset();
            me->RemoveAurasDueToSpell(SPELL_POWER);
            DoCast(me, SPELL_ZERO_REGEN, true);
            me->SetMaxPower(POWER_MANA, 100);
            me->SetPower(POWER_MANA, 80);
        }

        void EnterCombat(Unit* /*who*/) override
        {
            Talk(SAY_AGGRO);
            _EnterCombat();
            DoCast(me, SPELL_POWER, true);
            events.ScheduleEvent(EVENT_SCREAMS_OF_DEAD, 6000);
            events.ScheduleEvent(EVENT_WINDS_OF_NORTHREND, 16000);
            events.ScheduleEvent(EVENT_BANE, 22000);
            me->SetWalk(false);
            me->RemoveAurasDueToSpell(SPELL_KNEELING_VISUAL);
        }

        void JustDied(Unit* /*killer*/) override
        {
            Talk(SAY_DEATH);
            _JustDied();
            if (GameObject* go = instance->instance->GetGameObject(instance->GetGuidData(DATA_YMIRON_GORN)))
                go->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
        }

        void MoveInLineOfSight(Unit* who) override
        {
            if (me->getVictim() || !who || (who->IsPlayer() && who->ToPlayer()->isGameMaster()))
                return;

            if (!timer && !phase && intro && who->IsPlayer() && me->GetDistance(who) < 60.0f)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetWalk(true);
                timer = 1000;
                return;
            }

            if (!intro && me->GetDistance(who) < 8.0f)
                AttackStart(who);
        }

        void DoAction(int32 const action) override
        {
            if (action == ACTION_1)
                events.ScheduleEvent(EVENT_DARK_SLASH, 500);
        }

        void OnAreaTriggerCast(Unit* caster, Unit* target, uint32 spellId, uint32 createATSpellId) override
        {
            if (spellId == SPELL_BANE_DMG)
                DoCast(target, SPELL_BANE_NOVA, true);
        }

        void OnApplyOrRemoveAura(uint32 spellId, AuraRemoveMode mode, bool apply) override
        {
            if (apply || !me->isInCombat() || mode != AURA_REMOVE_BY_EXPIRE)
                return;

            //Heroic+
            if (spellId == SPELL_BANE && GetDifficultyID() != DIFFICULTY_NORMAL)
				events.ScheduleEvent(EVENT_ARISE_FALLEN, 2000);
        }

        void SpellFinishCast(const SpellInfo* spell) override
        {
            if (spell->Id == SPELL_ARISE_FALLEN)
            {
                std::list<AreaTrigger*> listAT;
                me->GetAreaObjectList(listAT, SPELL_BANE_AT);
                if (!listAT.empty())
                {
                    uint8 atCount = listAT.size();

                    for (uint8 i = 0; i < atCount; ++i)
                    {
                        Map::PlayerList const& players = me->GetMap()->GetPlayers();
                        for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                        {
                            if (Player* player = itr->getSource())
                            {
                                if (player->isAlive() && player->getHostileRefManager().HasTarget(me))
                                {
                                    if (listAT.empty())
                                        return;

                                    AreaTrigger* at = Trinity::Containers::SelectRandomContainerElement(listAT);

                                    Position targetPos;
                                    player->GetRandomNearPosition(targetPos, 5.0f);
                                    me->PlayOrphanSpellVisual(at->GetPosition(), 0.0f, targetPos, 50259, 1.7f, ObjectGuid::Empty, true);

                                    AddDelayedEvent(2000, [=] () -> void
                                    {
                                        if (me && me->isAlive() && me->isInCombat())
                                            me->CastSpell(targetPos, SPELL_ARISE_FALLEN_SUM, true);
                                    });
                                    at->Despawn();
                                    listAT.remove(at);
                                }
                            }
                        }
                    }
                }
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (timer && phase < 7)
            {
                switch (phase) 
                {
                    case 0:
                        if (timer <= diff)
                        {
                            ++phase;
                            timer = 5000;
                            me->GetMotionMaster()->MoveJump(7406.41f, 7280.58f, 44.26f, 10.0f, 5.0f);
                        }
                        else timer -= diff;
                        break;
                    case 1:
                        if (timer <= diff)
                        {
                            ++phase;
                            timer = 5000;
                            me->GetMotionMaster()->MovePoint(0, 7408.16f, 7293.06f, 43.78f);
                        }
                        else timer -= diff;
                        break;
                    case 2:
                        if (timer <= diff)
                        {
                            ++phase;
                            timer = 5000;
                            me->GetMotionMaster()->MovePoint(0, 7397.77f, 7297.18f, 43.78f);
                        }
                        else timer -= diff;
                        break;
                    case 3:
                        if (timer <= diff)
                        {
                            ++phase;
                            timer = 10000;
                            DoCast(me, SPELL_SOUL_SIPHON, true);
                            Talk(SAY_INTRO);
                        }
                        else timer -= diff;
                        break;
                    case 4:
                        if (timer <= diff)
                        {
                            ++phase;
                            timer = 5000;
                            me->GetMotionMaster()->MovePoint(0, 7396.58f, 7276.02f, 43.79f);
                            Talk(SAY_INTRO_2);
                        }
                        else timer -= diff;
                        break;
                    case 5:
                        if (timer <= diff)
                        {
                            ++phase;
                            timer = 1000;
                            if (GameObject* GO = me->FindNearestGameObject(247041, 30.0f))
                                me->SetFacingToObject(GO);
                            DoCast(SPELL_KNEELING_VISUAL);
                        }
                        else timer -= diff;
                        break;
                    case 6:
                        if (timer <= diff)
                        {
                            ++phase;
                            timer = 0;
                            intro = false;
                            me->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                            me->SetReactState(REACT_AGGRESSIVE);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_ATTACKABLE_1);
                            me->SetWalk(false); 
                        }
                        else timer -= diff;
                        break;
                }
            }

            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (CheckHomeDistToEvade(diff, 40.0f, 7384.99f, 7299.08f, 43.78f))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_DARK_SLASH:
                        DoCastVictim(SPELL_DARK_SLASH);
                        break;
                    case EVENT_SCREAMS_OF_DEAD:
                        DoCast(SPELL_SCREAMS_OF_DEAD);
                        events.ScheduleEvent(EVENT_SCREAMS_OF_DEAD, 23000);
                        Talk(SAY_SCREAMS);
                        break;
                    case EVENT_WINDS_OF_NORTHREND:
                        Talk(SAY_WINDS);
                        DoCast(SPELL_WINDS_OF_NORTHREND);
                        events.ScheduleEvent(EVENT_WINDS_OF_NORTHREND, 24000);
                        break;
                    case EVENT_BANE:
                        Talk(SAY_BANE);
                        DoCast(SPELL_BANE);
                        events.ScheduleEvent(EVENT_BANE, 50000);
                        break;
                    case EVENT_ARISE_FALLEN:
                        Talk(SAY_ARISE);
                        DoCast(SPELL_ARISE_FALLEN);
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_ymiron_the_fallen_kingAI (creature);
    }
};

//98246
class npc_ymiron_risen_warrior : public CreatureScript
{
public:
    npc_ymiron_risen_warrior() : CreatureScript("npc_ymiron_risen_warrior") {}

    struct npc_ymiron_risen_warriorAI : public ScriptedAI
    {
        npc_ymiron_risen_warriorAI(Creature* creature) : ScriptedAI(creature)
        {
            me->SetReactState(REACT_PASSIVE);
        }

        EventMap events;

        void Reset() {}

        void IsSummonedBy(Unit* summoner) //37:57
        {
            AddDelayedEvent(100, [=] () -> void
            {
                if (me)
                {
                    me->SendPlaySpellVisualKit(2000, 59548, 4);
                    DoCast(me, SPELL_MOD_SCALE, true);
                }
            });

            //38:00
            AddDelayedEvent(3000, [=] () -> void
            {
                if (me && me->isAlive() && me->isInCombat())
                {
                    me->SetReactState(REACT_AGGRESSIVE);
                    DoZoneInCombat(me, 100.0f);
                }
            });

            events.ScheduleEvent(EVENT_1, 5000); //38:02, 38:08, 38:14
        }

        void UpdateAI(uint32 diff)
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
                        DoCast(me, SPELL_VIGOR, true);
                        events.ScheduleEvent(EVENT_1, 6000);
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ymiron_risen_warriorAI(creature);
    }
};

//97163
class npc_ymiron_cursed_falke : public CreatureScript
{
public:
    npc_ymiron_cursed_falke() : CreatureScript("npc_ymiron_cursed_falke") {}

    struct npc_ymiron_cursed_falkeAI : public ScriptedAI
    {
        npc_ymiron_cursed_falkeAI(Creature* creature) : ScriptedAI(creature) {}

        bool flyEvent = false;

        void Reset() 
        {
            if (me->GetDistance(7248.78f, 7287.24f, 25.57f) < 25.0f)
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
            else
                flyEvent = true;
        }

        void EnterCombat(Unit* /*who*/) override
        {
            me->RemoveAurasDueToSpell(SPELL_STATE);
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (id == 1)
                me->DespawnOrUnsummon();
        }

        void MoveInLineOfSight(Unit* who) override
        {  
            if (!who->IsPlayer() || who->ToPlayer()->isGameMaster())
                return;

            if (!flyEvent && me->IsWithinDistInMap(who, 45.0f))
            {
                flyEvent = true;
                me->SetReactState(REACT_PASSIVE);
                me->RemoveAurasDueToSpell(SPELL_STATE);
                me->GetMotionMaster()->MovePoint(1, 7257.08f, 7081.54f, 26.41f);
                return;
            }
            ScriptedAI::MoveInLineOfSight(who);
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_ymiron_cursed_falkeAI (creature);
    }
};

//97043
class npc_ymiron_seacursed_slaver : public CreatureScript
{
public:
    npc_ymiron_seacursed_slaver() : CreatureScript("npc_ymiron_seacursed_slaver") {}

    struct npc_ymiron_seacursed_slaverAI : public ScriptedAI
    {
        npc_ymiron_seacursed_slaverAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = me->GetInstanceScript();

            if (me->GetDistance(7272.66f, 7307.91f, 30.79f) < 1.0f)
            {
                trashIntro = true;
                if (Creature* shieldmaiden = me->SummonCreature(NPC_SHIELDMAIDEN, 7266.52f, 7283.81f, 27.31f, 1.75f))
                {
                    shieldmaiden->SetReactState(REACT_PASSIVE);
                    shieldmaiden->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
                    prisonerGUID = shieldmaiden->GetGUID();
                }
            }

            if (me->GetDistance(trashPos[8]) < 1.0f)
            {
                trashIntro = true;
                if (Creature* runecarver = me->SummonCreature(NPC_RUNECARVER, 7355.52f, 7315.79f, 48.0f, 4.29f))
                {
                    runecarver->SetReactState(REACT_PASSIVE);
                    runecarver->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
                    prisonerGUID = runecarver->GetGUID();
                }
            }
        }

        InstanceScript* instance;
        EventMap _events;
        bool trashIntro = false;
        ObjectGuid prisonerGUID;

        void Reset() override
        {
            _events.Reset();
        }

        void EnterCombat(Unit* /*who*/) override
        {
            _events.ScheduleEvent(EVENT_FRACTURE, 15000);
            _events.ScheduleEvent(EVENT_BARBED, 9000);
            _events.ScheduleEvent(EVENT_SWIRLING, 17000);
        }

        void MoveInLineOfSight(Unit* who) override
        {  
            if (!who || !who->IsPlayer() || who->ToPlayer()->isGameMaster())
                return;

            if (trashIntro && me->IsWithinDistInMap(who, 25.0f))
            {
                trashIntro = false;

                if (Creature* prisoner = Creature::GetCreature(*me, prisonerGUID))
                {
                    if (prisoner->isAlive() && !prisoner->isInCombat() && me->GetDistance(prisoner) < 30.0f)
                    {
                        Talk(1);
                        me->StopAttack();
                        me->StopMoving();
                        me->SetFacingTo(prisoner);
                        DoCast(prisoner, SPELL_BREAK_CHAINS, true);

                        AddDelayedEvent(2000, [this, prisoner] () -> void
                        {
                            if (me && me->isAlive())
                            {
                                me->SetReactState(REACT_AGGRESSIVE);
                                me->AI()->DoZoneInCombat(me, 30.0f);
                            }

                            if (prisoner && prisoner->isAlive() && !prisoner->isInCombat())
                            {
                                if (GameObject* go = prisoner->FindNearestGameObject(245064, 20.0f))
                                    go->SetGoState(GO_STATE_ACTIVE);
                                prisoner->SetHomePosition(7265.72f, 7287.86f, 26.69f, 1.69f);
                                prisoner->GetMotionMaster()->MovePoint(1, 7265.72f, 7287.86f, 26.69f);
                                prisoner->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
                                prisoner->SetReactState(REACT_AGGRESSIVE);
                                prisoner->AI()->DoZoneInCombat(prisoner, 30.0f);
                            }
                        });
                        return;
                    }
                }
            }
            ScriptedAI::MoveInLineOfSight(who);
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            _events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_FRACTURE:
                        DoCast(198752);
                        _events.ScheduleEvent(EVENT_FRACTURE, 15000);
                        break;
                    case EVENT_BARBED:
                        DoCast(194674);
                        _events.ScheduleEvent(EVENT_BARBED, 9000);
                        break;
                    case EVENT_SWIRLING:
                        DoCast(201567);
                        _events.ScheduleEvent(EVENT_SWIRLING, 17000);
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_ymiron_seacursed_slaverAI (creature);
    }
};

//167922
class spell_ymiron_power_regen : public SpellScriptLoader
{
    public:
        spell_ymiron_power_regen() : SpellScriptLoader("spell_ymiron_power_regen") { }
 
        class spell_ymiron_power_regen_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ymiron_power_regen_AuraScript)

            uint8 power{};
            uint8 powerCounter{};

            void OnPeriodic(AuraEffect const* aurEff)
            {
                if (Creature* ymiron = GetCaster()->ToCreature())
                {
                    power = ymiron->GetPower(POWER_MANA);
                    if (power < 100)
                    {
                        if (powerCounter < 2)
                        {
                            powerCounter++;
                            ymiron->SetPower(POWER_MANA, power + 7);
                        }
                        else
                        {
                            powerCounter = 0;
                            ymiron->SetPower(POWER_MANA, power + 6);
                        }
                    }
                    else
                    {
                        ymiron->SetPower(POWER_MANA, 0);
                        ymiron->AI()->DoAction(ACTION_1); //EVENT_DARK_SLASH
                    }
                }
            }
 
            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_ymiron_power_regen_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };
 
        AuraScript* GetAuraScript() const override
        {
            return new spell_ymiron_power_regen_AuraScript();
        }
};

//193460
class spell_ymiron_bane : public SpellScriptLoader
{
    public:                                                      
        spell_ymiron_bane() : SpellScriptLoader("spell_ymiron_bane") {}

        class spell_ymiron_bane_AuraScript : public AuraScript 
        {
            PrepareAuraScript(spell_ymiron_bane_AuraScript) 

            void OnPereodic(AuraEffect const* aurEff) 
            {
                PreventDefaultAction();

                if (!GetCaster())
                    return;

                Position pos;

                if (aurEff->GetTickNumber() == 1)
                {
                    for (int8 i = 0; i < 4; ++i)
                    {
                        GetCaster()->GetNearPosition(pos, frand(20.0f, 40.0f), frand(0.0f, 6.28f));
                        GetCaster()->CastSpell(pos, GetSpellInfo()->Effects[EFFECT_0]->TriggerSpell, true);
                    }
                }
                else
                {
                    GetCaster()->GetNearPosition(pos, frand(20.0f, 40.0f), frand(0.0f, 6.28f));
                    GetCaster()->CastSpell(pos, GetSpellInfo()->Effects[EFFECT_0]->TriggerSpell, true);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_ymiron_bane_AuraScript::OnPereodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_ymiron_bane_AuraScript();
        }
};

//191560
class spell_ymiron_helheim_teleport : public SpellScriptLoader
{
    public:
        spell_ymiron_helheim_teleport() : SpellScriptLoader("spell_ymiron_helheim_teleport") {}

        class spell_ymiron_helheim_teleport_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ymiron_helheim_teleport_SpellScript);

            void HandleOnHit()
            {
                if (Player* player = GetHitUnit()->ToPlayer())
                    player->TeleportTo(1492, 2931.86f, 870.70f, 517.5f, 4.73f);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_ymiron_helheim_teleport_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_ymiron_helheim_teleport_SpellScript();
        }
};

void AddSC_boss_ymiron_the_fallen_king()
{
    new boss_ymiron_the_fallen_king();
    new npc_ymiron_risen_warrior();
    new npc_ymiron_cursed_falke();
    new npc_ymiron_seacursed_slaver();
    new spell_ymiron_power_regen();
    new spell_ymiron_bane();
    new spell_ymiron_helheim_teleport();
}