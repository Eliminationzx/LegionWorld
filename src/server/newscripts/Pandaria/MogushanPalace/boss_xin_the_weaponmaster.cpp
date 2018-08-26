/*
    Dungeon : Mogu'shan palace 87-89
    Xin the weaponmaster
    Jade servers
*/

#include "mogu_shan_palace.h"

class boss_xin_the_weaponmaster : public CreatureScript
{
    public:
        boss_xin_the_weaponmaster() : CreatureScript("boss_xin_the_weaponmaster") { }

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_xin_the_weaponmaster_AI(creature);
        }

        enum eEvents
        {
            EVENT_RING_OF_FIRE          = 1,
            EVENT_HEURT                 = 2,
            EVENT_INCITING_ROAR         = 3,
            EVENT_SWORD_THROWER         = 4,
            EVENT_SWORD_THROWER_STOP    = 5,
            EVENT_AXES_ACTIVATE         = 6,
            EVENT_AXES_DESACTIVATE      = 7
        };

        enum eSpells
        {
            SPELL_HEURT         = 119684,
            SPELL_INCITING_ROAR = 122959,
        };

        struct boss_xin_the_weaponmaster_AI : public BossAI
        {
            boss_xin_the_weaponmaster_AI(Creature* creature) : BossAI(creature, DATA_XIN_THE_WEAPONMASTER), summons(me)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            SummonList summons;
            bool onegem;
            bool twogem;

            void Reset() override
            {
                if (instance)
                    instance->SetData(TYPE_ACTIVATE_SWORD, 0);
                
                onegem = true;
                twogem = true;

                summons.DespawnAll();
                events.Reset();
            }

            void EnterCombat(Unit* who) override
            {
                events.ScheduleEvent(EVENT_RING_OF_FIRE, 3000);
                events.ScheduleEvent(EVENT_HEURT, urand(10000, 15000));
                events.ScheduleEvent(EVENT_INCITING_ROAR, urand(15000, 25000));
                events.ScheduleEvent(EVENT_SWORD_THROWER, 30000);
                events.ScheduleEvent(EVENT_AXES_ACTIVATE, 15000);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType dmgType) override
            {
                if (me->HealthBelowPctDamaged(66, damage) && onegem)
                {
                    onegem = false;
                    me->SummonCreature(CREATURE_LAUNCH_SWORD, otherPos[2]);
                }

                if (me->HealthBelowPctDamaged(33, damage) && twogem)
                {
                    twogem = false;
                    me->SummonCreature(CREATURE_LAUNCH_SWORD, otherPos[3]);
                }
            }

            void JustSummoned(Creature* summoned) override
            {
                summons.Summon(summoned);
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
                        case EVENT_RING_OF_FIRE:
                            if (instance)
                                instance->SetData(TYPE_ACTIVATE_ANIMATED_STAFF, 0);
                            events.ScheduleEvent(EVENT_RING_OF_FIRE, 20000);
                            break;
                        case EVENT_HEURT:
                            me->CastSpell(me, SPELL_HEURT, false);
                            events.ScheduleEvent(EVENT_HEURT, urand(10000, 15000));
                            break;
                        case EVENT_INCITING_ROAR:
                            me->CastSpell(me, SPELL_INCITING_ROAR, false);
                            events.ScheduleEvent(EVENT_INCITING_ROAR, 30000);
                            break;
                        case EVENT_SWORD_THROWER:
                            if (instance)
                                instance->SetData(TYPE_ACTIVATE_SWORD, 1);
                            events.ScheduleEvent(EVENT_SWORD_THROWER_STOP, 10000);
                            break;
                        case EVENT_SWORD_THROWER_STOP:
                            if (instance)
                                instance->SetData(TYPE_ACTIVATE_SWORD, 0);
                            events.ScheduleEvent(EVENT_SWORD_THROWER, 20000);
                            break;
                        case EVENT_AXES_ACTIVATE:
                            if (instance)
                                instance->SetData(TYPE_ACTIVATE_ANIMATED_AXE, 1);
                            events.ScheduleEvent(EVENT_AXES_DESACTIVATE, 10000);
                            break;
                        case EVENT_AXES_DESACTIVATE:
                            if (instance)
                                instance->SetData(TYPE_ACTIVATE_ANIMATED_AXE, 0);
                            events.ScheduleEvent(EVENT_AXES_ACTIVATE, 15000);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };
};

class mob_animated_staff : public CreatureScript
{
    public:
        mob_animated_staff() : CreatureScript("mob_animated_staff") { }

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new mob_animated_staff_AI(creature);
        }

        enum eSpells
        {
            SPELL_RING_OF_FIRE_0 = 119544,
            SPELL_RING_OF_FIRE_1 = 119590,
        };

        enum eActions
        {
            ACTION_ACTIVATE,
        };

        enum eEvents
        {
            EVENT_SUMMON_RING_OF_FIRE = 1,
            EVENT_UNSUMMON = 2,
            EVENT_SUMMON_RING_TRIGGER = 3,
        };

        enum eCreatures
        {
            CREATURE_RING_OF_FIRE = 61499,
        };

        struct mob_animated_staff_AI : public ScriptedAI
        {
            mob_animated_staff_AI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetDisplayId(42195);
                me->SetVirtualItem(0, 76364);
                me->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
            }

            EventMap events;
            float _x;
            float _y;
            float point;

            void Reset() override
            {
                _x = 0.f;
                _y = 0.f;
                point = 0.f;

                me->AddAura(SPELL_PERMANENT_FEIGN_DEATH, me);

                Position home = me->GetHomePosition();
                me->GetMotionMaster()->MovePoint(0, home);
            }

            void EnterCombat(Unit* unit) override
            {
            }

            void DoAction(const int32 action) override
            {
                switch (action)
                {
                    case ACTION_ACTIVATE:
                        me->RemoveAura(SPELL_PERMANENT_FEIGN_DEATH);
                        events.ScheduleEvent(EVENT_SUMMON_RING_OF_FIRE, 500);
                        break;
                }
            }

            void JustSummoned(Creature* summoned) override
            {
                if (summoned->GetEntry() == CREATURE_RING_OF_FIRE)
                {
                    summoned->setFaction(14);
                    summoned->SetReactState(REACT_PASSIVE);
                    summoned->AddAura(SPELL_RING_OF_FIRE_0, summoned);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SUMMON_RING_OF_FIRE:
                        {
                            events.ScheduleEvent(EVENT_UNSUMMON, 9000);
                            Unit* target = NULL;
                            std::list<Unit*> units;

                            Map::PlayerList const& PlayerList = me->GetMap()->GetPlayers();
                            if (!PlayerList.isEmpty())
                                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                                    if (Player* plr = i->getSource())
                                        if (plr->isAlive() && !plr->isGameMaster())
                                            units.push_back(plr);

                            if (units.empty())
                                return;

                            target = Trinity::Containers::SelectRandomContainerElement(units);
                            if (!target)
                                return;

                            me->GetMotionMaster()->MovePoint(0, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ());
                            point = 0.0f;
                            _x = target->GetPositionX();
                            _y = target->GetPositionY();
                            events.ScheduleEvent(EVENT_SUMMON_RING_TRIGGER, 100);
                            break;
                        }
                        case EVENT_UNSUMMON:
                            Reset();
                            break;
                        case EVENT_SUMMON_RING_TRIGGER:
                        {
                            if (point >= 11)
                            {
                                if (TempSummon* tmp = me->SummonCreature(CREATURE_RING_OF_FIRE, _x, _y, me->GetMap()->GetHeight(_x, _y, me->GetPositionZ()), 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 10000))
                                {
                                    tmp->RemoveAura(SPELL_RING_OF_FIRE_0);
                                    tmp->CastSpell(tmp, SPELL_RING_OF_FIRE_1, false);
                                }
                                return;
                            }

                            float x = _x + 5.0f * cos(point * M_PI / 5);
                            float y = _y + 5.0f * sin(point * M_PI / 5);
                            me->SummonCreature(CREATURE_RING_OF_FIRE, x, y, me->GetMap()->GetHeight(x, y, me->GetPositionZ()), 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 10000);
                            ++point;
                            events.ScheduleEvent(EVENT_SUMMON_RING_TRIGGER, 400);
                            break;
                        }
                    }
                }
            }
        };
};

class OnlyTriggerInFrontPredicate
{
    public:
        OnlyTriggerInFrontPredicate(Unit* caster) : _caster(caster) {}

        bool operator()(WorldObject* target)
        {
            return target->GetEntry() != 59481 || !_caster->isInFront(target, M_PI / 5) || target->GetGUID() == _caster->GetGUID();
        }

    private:
        Unit* _caster;
};

class spell_dart : public SpellScriptLoader
{
    public:
        spell_dart() : SpellScriptLoader("spell_dart") { }

        class spell_dart_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dart_SpellScript);

            bool Validate(SpellInfo const* spell) override
            {
                return true;
            }

            // set up initial variables and check if caster is creature
            // this will let use safely use ToCreature() casts in entire script
            bool Load() override
            {
                return true;
            }

            void SelectTarget(std::list<WorldObject*>& targetList)
            {
                if (!GetCaster())
                    return;

                if (targetList.empty())
                {
                    FinishCast(SPELL_FAILED_NO_VALID_TARGETS);
                    return;
                }

                //Select the two targets.
                std::list<WorldObject*> targets = targetList;
                targetList.remove_if(OnlyTriggerInFrontPredicate(GetCaster()));

                //See if we intersect with any players.
                for (std::list<WorldObject*>::iterator object = targets.begin(); object != targets.end(); ++object)
                    if ((*object)->GetTypeId() == TYPEID_PLAYER)
                        for (std::list<WorldObject*>::iterator itr = targetList.begin(); itr != targetList.end(); ++itr)
                            if ((*object)->IsInBetween(GetCaster(), *itr, 2.0f))
                            {
                                const SpellInfo* damageSpell = sSpellMgr->GetSpellInfo(SPELL_THROW_DAMAGE);
                                GetCaster()->DealDamage((*object)->ToPlayer(), damageSpell->Effects[0]->BasePoints, 0, SPELL_DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, damageSpell);
                            }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dart_SpellScript::SelectTarget, EFFECT_0, TARGET_SRC_CASTER);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dart_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dart_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dart_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_CONE_ENEMY_104);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dart_SpellScript();
        }
};

void AddSC_boss_xin_the_weaponmaster()
{
    new boss_xin_the_weaponmaster();
    new mob_animated_staff();
    new spell_dart();
}
