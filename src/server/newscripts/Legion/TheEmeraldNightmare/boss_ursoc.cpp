/*
    https://uwow.biz/
*/

#include "the_emerald_nightmare.h"

enum Says
{
    SAY_AGGRO               = 0,
    SAY_FOCUSED_GAZE_EMOTE  = 1,
    SAY_FOCUSED_GAZE        = 2,
    SAY_CACOPHONY           = 3,
    SAY_DEATH               = 4,
    SAY_BLOOD_FRENZY        = 5,
};

enum Spells
{
    SPELL_EMPTY_ENERGY              = 205270,
    SPELL_FEROCIOUS_YELL            = 204990,
    SPELL_ENERGIZE                  = 205272, //2.941176470588235
    SPELL_BERSERK                   = 26662,
    SPELL_OVERWHELM_AURA            = 197946,
    SPELL_OVERWHELM_DUMMY           = 197947,
    SPELL_OVERWHELM_DMG             = 197943,
    SPELL_REND_FLESH_AURA           = 197948,
    SPELL_REND_FLESH_DUMMY          = 197949,
    SPELL_REND_FLESH_DMG            = 197942,
    SPELL_FOCUSED_GAZE              = 198006,
    SPELL_BARRELING_MOMENTUM        = 198002,
    SPELL_B_MOMENTUM_FILTER         = 198004,
    SPELL_B_MOMENTUM_PING           = 198007,
    SPELL_B_MOMENTUM_CHARGE         = 198009,
    SPELL_B_MOMENTUM_AT             = 198093,
    SPELL_B_MOMENTUM_DMG            = 198099,
    SPELL_B_MOMENTUM_AOE            = 198109,
    SPELL_B_MOMENTUM_DUMMY          = 198102,
    SPELL_MOMENTUM_DEBUFF           = 198108,
    SPELL_TRAMPLING_SLAM            = 199237,
    SPELL_BLOOD_FRENZY              = 198388,
    SPELL_ROARING_CACOPHONY_AURA    = 197986,
    SPELL_ROARING_CACOPHONY_DUMMY_1 = 197988,
    SPELL_ROARING_CACOPHONY_DUMMY_2 = 197989,
    SPELL_ROARING_CACOPHONY_DMG     = 197969,
    SPELL_ECHOING_DISCHORD          = 198392,

    //Heroic+
    SPELL_ROARING_CACOPHONY_SUM     = 197982, //Sum npc 100576
    SPELL_NIGHTMARISH_CACOPHONY     = 197980,
    SPELL_NIGHTMARE_IMAGE_AT        = 197960, //Dmg 205611
    SPELL_NIGHTMARE_IMAGE_MORPH     = 204969,
};

enum eEvents
{
    EVENT_FOCUSED_GAZE              = 1,
    EVENT_REND_FLESH                = 2,
    EVENT_OVERWHELM                 = 3,
    EVENT_ROARING_CACOPHONY         = 4,
    EVENT_ROARING_CACOPHONY_2       = 5,

    EVENT_ACTIVATE_IMAGE, //Mythic
    ACTION_BARRELING_MOMENTUM,
};

enum eOther
{
    DATA_PLAYER_HITCOUNT            = 1,
    DATA_MOMENTUM_TARGET            = 2,
};

//100497
class boss_ursoc : public CreatureScript
{
public:
    boss_ursoc() : CreatureScript("boss_ursoc") {}

    struct boss_ursocAI : public BossAI
    {
        boss_ursocAI(Creature* creature) : BossAI(creature, DATA_URSOC)
        {
            introDone = false;
        }

        bool introDone;
        bool focused;
        bool lowHp;
        uint8 playerHitCount;
        uint32 berserkTimer;
        ObjectGuid m_TargetGUID;

        void Reset() override
        {
            _Reset();
            summons.DespawnAll();
            me->SetPower(POWER_ENERGY, 100);
            DoCast(me, SPELL_EMPTY_ENERGY, true);
            focused = false;
            lowHp = false;
        }

        void EnterCombat(Unit* /*who*/) override
        //44:17
        {
            _EnterCombat();
            DoCast(me, SPELL_FEROCIOUS_YELL, true);
            DoCast(me, SPELL_ENERGIZE, true);

            berserkTimer = 5 * MINUTE * IN_MILLISECONDS;
            events.ScheduleEvent(EVENT_OVERWHELM, 10000);
            events.ScheduleEvent(EVENT_REND_FLESH, 13000);
            events.ScheduleEvent(EVENT_ROARING_CACOPHONY, 18000);
            Talk(SAY_AGGRO);
        }

        void JustDied(Unit* /*killer*/) override
        {
            _JustDied();
            Talk(SAY_DEATH);
        }

        void MoveInLineOfSight(Unit* who) override
        {  
            if (who->GetTypeId() != TYPEID_PLAYER)
                return;

            if (!introDone && me->IsWithinDistInMap(who, 100.0f))
            {
                introDone = true;
                Conversation* conversation = new Conversation;
                if (!conversation->CreateConversation(sObjectMgr->GetGenerator<HighGuid::Conversation>()->Generate(), 3605, who, nullptr, who->GetPosition()))
                    delete conversation;
            }
        }

        void SpellHit(Unit* caster, const SpellInfo* spell) override
        {
            switch (spell->Id)
            {
                case SPELL_OVERWHELM_DUMMY:
                    DoCastVictim(SPELL_OVERWHELM_DMG, true);
                    break;
                case SPELL_REND_FLESH_DUMMY:
                    DoCastVictim(SPELL_REND_FLESH_DMG);
                    break;
                case SPELL_ROARING_CACOPHONY_DUMMY_2:
                    DoCast(SPELL_ROARING_CACOPHONY_DMG);
                    if (GetDifficultyID() == DIFFICULTY_HEROIC_RAID || GetDifficultyID() == DIFFICULTY_MYTHIC_RAID)
                    {
                        EntryCheckPredicate pred(NPC_NIGHTMARE_IMAGE);
                        summons.DoAction(ACTION_1, pred);
                        DoCast(me, SPELL_ROARING_CACOPHONY_SUM, true);
                    }
                    Talk(SAY_CACOPHONY);
                    break;
                case SPELL_B_MOMENTUM_PING:
                    playerHitCount = 0;
                    DoCast(me, SPELL_B_MOMENTUM_AT, true);
                    DoCast(caster, SPELL_TRAMPLING_SLAM, true);
                    //DoCast(caster, SPELL_B_MOMENTUM_CHARGE);
                    //>Hack
                    me->SetPower(POWER_ENERGY, 0);
                    me->GetMotionMaster()->MoveCharge(caster->GetPositionX(), caster->GetPositionY(), caster->GetPositionZ(), 40.0f, SPELL_B_MOMENTUM_CHARGE);
                    //<
                    focused = false;
                    break;
            }
        }

        void SpellHitTarget(Unit* target, const SpellInfo* spell) override
        {
            switch (spell->Id)
            {
                case SPELL_B_MOMENTUM_FILTER:
                    m_TargetGUID = target->GetGUID();
                    DoCast(target, SPELL_FOCUSED_GAZE, true);
                    DoCast(target, SPELL_B_MOMENTUM_DUMMY, true);
                    Talk(SAY_FOCUSED_GAZE_EMOTE, target->GetGUID());
                    Talk(SAY_FOCUSED_GAZE);
                    break;
            }
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type == POINT_MOTION_TYPE)
                if (id == SPELL_B_MOMENTUM_CHARGE)
                    DoCast(me, SPELL_B_MOMENTUM_AOE, true);
        }

        void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType dmgType) override
        {
            if (me->HealthBelowPct(31) && !lowHp)
            {
                lowHp = true;
                DoCast(me, SPELL_BLOOD_FRENZY, true);
                Talk(SAY_BLOOD_FRENZY);
            }
        }

        void DoAction(int32 const action) override
        {
            if (action == ACTION_BARRELING_MOMENTUM && !focused)
            {
                focused = true;
                events.ScheduleEvent(EVENT_FOCUSED_GAZE, 500);
            }
        }

        uint32 GetData(uint32 type) const override
        {
            switch (type)
            {
                case DATA_PLAYER_HITCOUNT:
                    return playerHitCount;
            }
            return 0;
        }

        void SetData(uint32 type, uint32 data) override
        {
            switch (type)
            {
                case DATA_PLAYER_HITCOUNT:
                    playerHitCount++;
                    break;
                default:
                    break;
            }
        }

        ObjectGuid GetGUID(int32 type) override
        {
            switch (type)
            {
                case DATA_MOMENTUM_TARGET:
                    return m_TargetGUID;
                default:
                    return ObjectGuid::Empty;
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            if (berserkTimer)
            {
                if (berserkTimer <= diff)
                {
                    berserkTimer = 0;
                    DoCast(me, SPELL_BERSERK, true);
                }
                else
                    berserkTimer -= diff;
            }

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_REND_FLESH:
                        DoCast(me, SPELL_REND_FLESH_AURA, true);
                        break;
                    case EVENT_FOCUSED_GAZE:
                        m_TargetGUID.Clear();
                        DoCast(me, SPELL_B_MOMENTUM_FILTER, true);
                        break;
                    case EVENT_OVERWHELM:
                        DoCast(me, SPELL_OVERWHELM_AURA, true);
                        break;
                    case EVENT_ROARING_CACOPHONY:
                        DoCast(me, SPELL_ROARING_CACOPHONY_DUMMY_2, true);
                        events.ScheduleEvent(EVENT_ROARING_CACOPHONY_2, 20000);
                        if (GetDifficultyID() == DIFFICULTY_MYTHIC_RAID)
                            events.ScheduleEvent(EVENT_ACTIVATE_IMAGE, 40000);
                        break;
                    case EVENT_ROARING_CACOPHONY_2:
                        DoCast(me, SPELL_ROARING_CACOPHONY_AURA, true);
                        break;
                    case EVENT_ACTIVATE_IMAGE:
                    {
                        GuidList* summonList = me->GetSummonList(NPC_NIGHTMARE_IMAGE);
                        if (!summonList->empty())
                            if (ObjectGuid firstGUID = summonList->front())
                                if (Creature* image = ObjectAccessor::GetCreature(*me, firstGUID))
                                    image->AI()->DoAction(ACTION_2);
                        events.ScheduleEvent(EVENT_ACTIVATE_IMAGE, 40000);
                        break;
                    }
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_ursocAI (creature);
    }
};

//100576
class npc_ursoc_nightmare_image : public CreatureScript
{
public:
    npc_ursoc_nightmare_image() : CreatureScript("npc_ursoc_nightmare_image") {}

    struct npc_ursoc_nightmare_imageAI : public ScriptedAI
    {
        npc_ursoc_nightmare_imageAI(Creature* creature) : ScriptedAI(creature)
        {
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
        }

        EventMap events;

        void Reset() override {}

        void DoAction(int32 const action) override
        {
            switch (action)
            {
                case ACTION_1:
                    me->RemoveAurasDueToSpell(SPELL_NIGHTMARE_IMAGE_MORPH);
                    DoCast(SPELL_NIGHTMARISH_CACOPHONY);
                    events.ScheduleEvent(EVENT_1, 4000);
                    break;
                case ACTION_2:
                    events.ScheduleEvent(EVENT_2, 500);
                    break;
            }
        }

        void IsSummonedBy(Unit* summoner) override
        {
            DoCast(me, SPELL_NIGHTMARE_IMAGE_MORPH, true);
            events.ScheduleEvent(EVENT_3, 3000);
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim() && me->isInCombat())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_1:
                        DoCast(me, SPELL_NIGHTMARE_IMAGE_MORPH, true);
                        break;
                    case EVENT_2:
                        me->RemoveAreaObject(SPELL_NIGHTMARE_IMAGE_AT);
                        me->RemoveAurasDueToSpell(SPELL_NIGHTMARE_IMAGE_MORPH);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                        me->SetReactState(REACT_AGGRESSIVE);
                        DoZoneInCombat(me, 100.0f);
                        break;
                    case EVENT_3:
                        DoCast(me, SPELL_NIGHTMARE_IMAGE_AT, true);
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_ursoc_nightmare_imageAI(creature);
    }
};

//205272
class spell_ursoc_periodic_energize : public SpellScriptLoader
{
public:
    spell_ursoc_periodic_energize() : SpellScriptLoader("spell_ursoc_periodic_energize") { }

    class spell_ursoc_periodic_energize_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_ursoc_periodic_energize_AuraScript);

        uint8 PowerCount;

        void OnTick(AuraEffect const* aurEff)
        {
            Creature* caster = GetCaster()->ToCreature();
            if (!caster || !caster->isInCombat())
                return;

            PowerCount = caster->GetPower(POWER_ENERGY);

            if (PowerCount < 100)
                caster->SetPower(POWER_ENERGY, PowerCount + 3);
            else
                caster->AI()->DoAction(ACTION_BARRELING_MOMENTUM);
        }

        void Register() override
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_ursoc_periodic_energize_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_ursoc_periodic_energize_AuraScript();
    }
};

//199237
class spell_ursoc_trampling_slam : public SpellScriptLoader
{
    public:
        spell_ursoc_trampling_slam() : SpellScriptLoader("spell_ursoc_trampling_slam") { }

        class spell_ursoc_trampling_slam_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ursoc_trampling_slam_SpellScript);

            void DealDamage()
            {
                Unit* caster = GetCaster();
                Unit* target = GetHitUnit();

                if (!caster || !target)
                    return;
                
                float distance = caster->GetExactDist2d(target);

                if (distance >= 0.0f && distance <= 60.0f)
                    SetHitDamage(GetHitDamage() * (1 - (distance / 60)));
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_ursoc_trampling_slam_SpellScript::DealDamage);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_ursoc_trampling_slam_SpellScript();
        }
};

//198109
class spell_ursoc_barreling_impact : public SpellScriptLoader
{
    public:
        spell_ursoc_barreling_impact() : SpellScriptLoader("spell_ursoc_barreling_impact") { }

        class spell_ursoc_barreling_impact_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ursoc_barreling_impact_SpellScript);

            void DealDamage()
            {
                if (Unit* caster = GetCaster())
                    if (caster->IsAIEnabled)
                    {
                        uint8 plrCount = caster->GetMap()->GetPlayersCountExceptGMs();
                        uint8 hitCount = caster->GetAI()->GetData(DATA_PLAYER_HITCOUNT);
                        if (hitCount)
                        {
                            uint32 perc = float(float(hitCount) / float(plrCount) * 100.0f);
                            if (perc >= 100)
                                perc = 90;
                            int32 damage = CalculatePct(GetHitDamage(), 100 - perc);
                            SetHitDamage(damage);
                        }
                    }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_ursoc_barreling_impact_SpellScript::DealDamage);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_ursoc_barreling_impact_SpellScript();
        }
};

//198099
class spell_ursoc_momentum_filter : public SpellScriptLoader
{
    public:
        spell_ursoc_momentum_filter() : SpellScriptLoader("spell_ursoc_momentum_filter") { }

        class spell_ursoc_momentum_filter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ursoc_momentum_filter_SpellScript);

            void FilterTargets(WorldObject*& target)
            {
                if (Unit* caster = GetCaster())
                {
                    ObjectGuid guid = caster->GetAI()->GetGUID(DATA_MOMENTUM_TARGET);
                    if (Player* pTarget = ObjectAccessor::GetPlayer(*caster, guid))
                    {
                        if (!(Player*)target->IsInBetween(caster, pTarget, 10.0f))
                        {
                            target = nullptr;
                        }
                    }
                }
            }

            void HandleOnHit()
            {
                if (GetCaster() && GetHitUnit())
                {
                    GetCaster()->CastSpell(GetHitUnit(), SPELL_MOMENTUM_DEBUFF, true);
                    GetCaster()->GetAI()->SetData(DATA_PLAYER_HITCOUNT, 1);
                }
            }

            void Register() override
            {
                OnObjectTargetSelect += SpellObjectTargetSelectFn(spell_ursoc_momentum_filter_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_TARGET_ENEMY);
                OnObjectTargetSelect += SpellObjectTargetSelectFn(spell_ursoc_momentum_filter_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_TARGET_ENEMY);
                OnHit += SpellHitFn(spell_ursoc_momentum_filter_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_ursoc_momentum_filter_SpellScript();
        }
};

void AddSC_boss_ursoc()
{
    new boss_ursoc();
    new npc_ursoc_nightmare_image();
    new spell_ursoc_periodic_energize();
    new spell_ursoc_trampling_slam();
    new spell_ursoc_barreling_impact();
    new spell_ursoc_momentum_filter();
}
