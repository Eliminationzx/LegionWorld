
#include "ScriptPCH.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "Spell.h"

const Position mfPos[22] =
{
    // Snarler spawn
    { -3924.37f, 6303.53f, 17.59f, 1.88f },
    { -4011.98f, 6416.34f, 14.75f, 3.73f },
    { -4097.92f, 6458.10f, 14.80f, 3.19f },
    { -4170.17f, 6503.14f, 13.41f, 1.55f },
    { -4266.80f, 6521.71f, 14.39f, 2.68f },
    { -4318.40f, 6601.40f, 9.853f, 1.47f },
    { -4056.26f, 6664.42f, 13.22f, 2.99f },
    { -4009.05f, 6561.04f, 17.15f, 4.37f },
    { -3932.42f, 6584.56f, 12.91f, 3.70f },
    { -3838.57f, 6461.64f, 11.91f, 3.92f },
    { -4268.83f, 6678.51f, 9.731f, 4.84f },
    // Dreadhowl spawn
    { -4225.82f, 6556.37f, 14.61f, 5.84f },
    { -4141.07f, 6523.72f, 16.81f, 6.06f },
    { -4073.94f, 6580.90f, 16.70f, 0.27f },
    { -3957.37f, 6617.45f, 12.66f, 0.43f },
    { -3865.21f, 6524.91f, 18.89f, 2.94f },
    { -3872.48f, 6498.26f, 17.90f, 3.39f },
    { -3914.52f, 6398.61f, 13.61f, 4.04f },
    { -4038.38f, 6514.68f, 13.36f, 3.01f },
    { -4344.90f, 6583.72f, 10.64f, 1.75f },
    { -4193.76f, 6122.50f, 13.00f, 6.06f },
    { -4082.68f, 6121.38f, 17.41f, 5.37f }
};

Position const SpawnRingsAT[57] =
{
    { -4278.502f, 6305.27f, 55.3745f },
    { -4387.158f, 6218.971f, 85.1059f },
    { -4384.887f, 6059.405f, 30.4877f },
    { -4464.976f, 6074.34f, 56.51786f },
    { -4416.028f, 6263.413f, 30.60862f },
    { -4411.45f, 6469.171f, 37.86602f },
    { -4417.354f, 6368.175f, 34.5676f },
    { -4061.708f, 6240.754f, 55.67439f },
    { -3976.156f, 6239.41f, 59.68959f },
    { -4298.453f, 6435.71f, 99.74864f },
    { -4000.738f, 6319.748f, 61.96915f },
    { -4123.092f, 6420.391f, 79.5131f },
    { -4200.064f, 6404.912f, 49.83862f },
    { -4153.389f, 6421.354f, 49.73269f },
    { -4177.096f, 6412.299f, 53.74761f },
    { -3994.031f, 6130.03f, 143.9004f },
    { -4152.132f, 6029.343f, 112.4916f },
    { -4276.29f, 6004.999f, 98.62133f },
    { -4327.917f, 6013.772f, 118.3816f },
    { -4282.986f, 6089.893f, 74.05066f },
    { -4204.17f, 6195.492f, 80.76868f },
    { -4127.677f, 6190.193f, 87.08089f },
    { -4125.719f, 6353.675f, 70.76109f },
    { -4219.653f, 6288.33f, 56.897f },
    { -4088.217f, 6675.956f, 53.79656f },
    { -4162.233f, 6763.257f, 25.24334f },
    { -4231.226f, 6586.085f, 93.67531f },
    { -4265.304f, 6513.369f, 93.58659f },
    { -4163.147f, 6539.726f, 78.76099f },
    { -3860.811f, 6494.958f, 97.87495f },
    { -4015.245f, 6472.497f, 85.551f },
    { -3931.849f, 6370.7f, 73.34687f },
    { -3871.005f, 6321.215f, 78.78794f },
    { -3888.948f, 6244.342f, 71.50654f },
    { -4311.99f, 5950.87f, 104.305f },
    { -4237.936f, 5942.774f, 117.577f },
    { -4196.168f, 5890.585f, 127.7626f },
    { -4142.12f, 5947.434f, 119.1986f },
    { -3812.991f, 6369.193f, 118.9729f },
    { -3861.29f, 6380.367f, 55.15146f },
    { -3794.465f, 6427.009f, 69.57449f },
    { -3781.519f, 6438.263f, 61.80006f },
    { -3754.766f, 6475.822f, 64.21665f },
    { -3833.854f, 6670.972f, 60.10976f },
    { -4319.019f, 6655.865f, 24.96414f },
    { -3760.503f, 6175.529f, 123.6823f },
    { -4149.393f, 6684.266f, 31.54233f },
    { -3951.658f, 6719.8f, 37.38012f },
    { -4147.766f, 6777.004f, 32.39946f },
    { -3792.443f, 6895.78f, 26.00098f },
    { -3740.898f, 6846.799f, 56.73854f },
    { -3777.356f, 6723.497f, 24.15144f },
    { -3880.603f, 6715.769f, 22.8216f },
    { -4068.458f, 5933.453f, 115.6255f },
    { -3969.863f, 5989.153f, 113.2228f },
    { -3887.325f, 6097.111f, 116.1136f },
    { -3817.255f, 6127.589f, 126.3984f }
};

enum Spells
{
    SPELL_DARKMOON_DEATHMATCH_TELE_1    = 108919,
    SPELL_DARKMOON_DEATHMATCH_TELE_2    = 113212,
    SPELL_DARKMOON_DEATHMATCH_TELE_3    = 113213,
    SPELL_DARKMOON_DEATHMATCH_TELE_4    = 113216,
    SPELL_DARKMOON_DEATHMATCH_TELE_5    = 113219,
    SPELL_DARKMOON_DEATHMATCH_TELE_6    = 113224,
    SPELL_DARKMOON_DEATHMATCH_TELE_7    = 113227,
    SPELL_DARKMOON_DEATHMATCH_TELE_8    = 113228,
    SPELL_RING_TOSS_HIT                 = 101699,
    SPELL_RING_TOSS_MISS                = 101697,
    SPELL_RING_TOSS_MISS_2              = 101698,
    SPELL_LANDING_RESULT_KILL_CREDIT    = 100962,
    SPELL_BULLSEYE                      = 62173,
    SPELL_GREAT_SHOT                    = 62175,
    SPELL_POOR_SHOT                     = 62179,
    SPELL_CANNONBALL                    = 62244,
    SPELL_TONK_TARGET                   = 62265,
    SPELL_DARKMOON_MAGIC_WINGS          = 102116
};

enum Creatures
{
    NPC_DUBENKO                         = 54490,
    NPC_TONK_TARGET                     = 33081,
    NPC_DARKMOON_FAIRE_CANNON_TARGET    = 33068,
    NPC_DARKMOON_FAIRE_CANNON           = 15218
};

// 71992 - Moonfang <Darkmoon Den Mother>
class boss_darkmoon_moonfang_mother : public CreatureScript
{
    enum eSay
    {
        SAY_SUMM_SNARLER = 0,
        SAY_SUMM_DREADHOWL = 1,
        SAY_SUMM_MOTHER = 2
    };

    enum eCreatures
    {
        NPC_MOONFANG_SNARLER = 56160,
        NPC_MOONFANG_DREADHOWL = 71982
    };

    enum eSpells
    {
        SPELL_LEAP_FOR_THE_KILL = 144546,
        SPELL_FANGS_OF_THE_MOON = 144700,
        SPELL_MOONFANG_TEARS = 144702,
        SPELL_CALL_THE_PACK = 144602,
        SPELL_MOONFANG_CURSE = 144590
    };

public:
    boss_darkmoon_moonfang_mother() : CreatureScript("boss_darkmoon_moonfang_mother") { }

    struct boss_darkmoon_moonfang_motherAI : public ScriptedAI
    {
        boss_darkmoon_moonfang_motherAI(Creature* creature) : ScriptedAI(creature), summons(me)
        {
            me->SetVisible(false);
            prevEvent1 = true;
            prevEvent2 = false;
            sDiedCount = 0;
        }

        EventMap events;
        SummonList summons;

        bool prevEvent1;
        bool prevEvent2;
        uint8 sDiedCount;

        void Reset() override
        {
            events.Reset();
            summons.DespawnAll();

            if (prevEvent1)
            {
                SummonMoonfang();
                ZoneTalk(SAY_SUMM_SNARLER);
            }
        }

        void SummonMoonfang()
        {
            if (prevEvent1)
            {
                for (uint8 i = 0; i < 11; i++)
                {
                    me->SummonCreature(NPC_MOONFANG_SNARLER, mfPos[i], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
                }
            }

            if (prevEvent2)
            {
                for (uint8 i = 11; i < 22; i++)
                {
                    me->SummonCreature(NPC_MOONFANG_DREADHOWL, mfPos[i], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
                }
            }
        }

        void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
        {
            if (prevEvent1 || prevEvent2)
                sDiedCount++;

            if (sDiedCount == 11)
            {
                prevEvent1 = false;
                prevEvent2 = true;
                ZoneTalk(SAY_SUMM_DREADHOWL);
                SummonMoonfang();
            }
            if (sDiedCount == 22)
            {
                prevEvent2 = false;
                me->SetVisible(true);
                ZoneTalk(SAY_SUMM_MOTHER);
            }
        }

        void EnterCombat(Unit* /*who*/) override
        {
            events.ScheduleEvent(EVENT_1, 0);       // cast leap
            events.ScheduleEvent(EVENT_2, 10000);   // cast stuns the target
            events.ScheduleEvent(EVENT_3, 8000);    // cast tears AOE
            events.ScheduleEvent(EVENT_4, 64000);   // summon moonfangs
            events.ScheduleEvent(EVENT_5, 180000);  // cast mind control
        }

        void JustSummoned(Creature* summon) override
        {
            summons.Summon(summon);
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim() && me->isInCombat())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_1:
                    if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 1, 40.0f, true))
                        DoCast(pTarget, SPELL_LEAP_FOR_THE_KILL);
                    events.ScheduleEvent(EVENT_1, 12000);
                    break;
                case EVENT_2:
                    if (Unit* pTarget = me->getVictim())
                        DoCast(pTarget, SPELL_FANGS_OF_THE_MOON);
                    events.ScheduleEvent(EVENT_2, 10000);
                    break;
                case EVENT_3:
                    DoCast(SPELL_MOONFANG_TEARS);
                    events.ScheduleEvent(EVENT_3, 22000);
                    break;
                case EVENT_4:
                    DoCast(SPELL_CALL_THE_PACK);
                    events.ScheduleEvent(EVENT_4, 64000);
                    break;
                case EVENT_5:
                    DoCast(SPELL_MOONFANG_CURSE);
                    events.ScheduleEvent(EVENT_5, 180000);
                    break;
                default:
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };
    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_darkmoon_moonfang_motherAI(creature);
    }
};

// Darkmoon Faire Gnolls - 54444, 54466, 54549
class npc_darkmoon_faire_gnolls : public CreatureScript
{
    enum Spells
    {
        SPELL_WHACK = 102022,
        SPELL_GNOLL_KILL_CREDIT = 101835
    };

public:
    npc_darkmoon_faire_gnolls() : CreatureScript("npc_darkmoon_faire_gnolls") { }

    struct npc_darkmoon_faire_gnollsAI : public Scripted_NoMovementAI
    {
        npc_darkmoon_faire_gnollsAI(Creature* c) : Scripted_NoMovementAI(c)
        {
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }

        EventMap events;

        void Reset() override
        {
            me->DespawnOrUnsummon(3000);
        }

        void SpellHit(Unit* pCaster, const SpellInfo* Spell) override
        {
            if (Spell->Id == SPELL_WHACK)
            {
                DoCast(pCaster, SPELL_GNOLL_KILL_CREDIT);
                me->Kill(me);
            }
        }
    };

    CreatureAI* GetAI(Creature *creature) const override
    {
        return new npc_darkmoon_faire_gnollsAI(creature);
    }
};

// Darkmoon Faire Gnoll Holder - 54547
class npc_darkmoon_faire_gnoll_holder : public CreatureScript
{
    enum Events
    {
        EVENT_SUMMON_GNOLL = 1,
    };

    enum Spells
    {
        SPELL_SUMMON_GNOLL = 102036,
        SPELL_SUMMON_GNOLL_BABY = 102043,
        SPELL_SUMMON_GNOLL_BONUS = 102044
    };

public:
    npc_darkmoon_faire_gnoll_holder() : CreatureScript("npc_darkmoon_faire_gnoll_holder") { }

    struct npc_darkmoon_faire_gnoll_holderAI : public Scripted_NoMovementAI
    {
        npc_darkmoon_faire_gnoll_holderAI(Creature* c) : Scripted_NoMovementAI(c)
        {
            me->SetReactState(REACT_PASSIVE);
        }

        EventMap events;

        void Reset() override
        {
            events.ScheduleEvent(EVENT_SUMMON_GNOLL, 3000);
        }

        void UpdateAI(uint32 diff) override
        {
            events.Update(diff);

            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_SUMMON_GNOLL:
                {
                    std::list<Creature*> creatures;
                    me->GetCreatureListWithEntryInGrid(creatures, 54546, 40.0f);
                    if (!creatures.empty())
                    {
                        Trinity::Containers::RandomResizeList(creatures, 1);
                        uint32 Spells[3] =
                        {
                            SPELL_SUMMON_GNOLL,
                            SPELL_SUMMON_GNOLL_BABY,
                            SPELL_SUMMON_GNOLL_BONUS,
                        };
                        uint8 rand = urand(0, 3);
                        if (Creature* pTarget = creatures.front())
                            DoCast(pTarget, Spells[rand], true);
                    }
                    events.ScheduleEvent(EVENT_SUMMON_GNOLL, 2000);
                    break;
                }
                }
            }
        }
    };

    CreatureAI* GetAI(Creature *creature) const override
    {
        return new npc_darkmoon_faire_gnoll_holderAI(creature);
    }
};

// Rinling - 14841
class npc_darkmoon_faire_rinling : public CreatureScript
{
    enum Events
    {
        EVENT_SHOTEVENT_ACTIVE = 1,
        EVENT_SHOTEVENT_PAUSE = 2
    };

    enum Npcs
    {
        NPC_DARKMOON_FAIRE_TARGET = 24171,
    };

    enum Spells
    {
        SPELL_GOSSIP_CREATE_RIFLE = 101991,
        SPELL_INDICATOR = 43313,
        SPELL_INDICATOR_QUICK_SHOT = 101010
    };

public:
    npc_darkmoon_faire_rinling() : CreatureScript("npc_darkmoon_faire_rinling") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        player->CLOSE_GOSSIP_MENU();

        if (action == 1)
            creature->CastSpell(player, SPELL_GOSSIP_CREATE_RIFLE, true);

        return true;
    }

    struct npc_darkmoon_faire_rinlingAI : public Scripted_NoMovementAI
    {
        npc_darkmoon_faire_rinlingAI(Creature* c) : Scripted_NoMovementAI(c)
        {
            me->SetReactState(REACT_PASSIVE);
        }

        EventMap events;

        void Reset() override
        {
            events.ScheduleEvent(EVENT_SHOTEVENT_ACTIVE, 3000);
        }

        void UpdateAI(uint32 diff) override
        {
            events.Update(diff);

            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_SHOTEVENT_ACTIVE:
                {
                    std::list<Creature*> creatures;
                    me->GetCreatureListWithEntryInGrid(creatures, NPC_DARKMOON_FAIRE_TARGET, 15.0f);
                    if (!creatures.empty())
                    {
                        Trinity::Containers::RandomResizeList(creatures, 1);

                        if (Creature* pTarget = creatures.front())
                        {
                            pTarget->CastSpell(pTarget, SPELL_INDICATOR, true);
                            pTarget->CastSpell(pTarget, SPELL_INDICATOR_QUICK_SHOT, true);
                        }
                    }
                    events.ScheduleEvent(EVENT_SHOTEVENT_PAUSE, 3000);
                    break;
                }
                case EVENT_SHOTEVENT_PAUSE:
                {
                    std::list<Creature*> creatures;
                    me->GetCreatureListWithEntryInGrid(creatures, NPC_DARKMOON_FAIRE_TARGET, 15.0f);
                    if (!creatures.empty())
                    {
                        for (std::list<Creature*>::iterator iter = creatures.begin(); iter != creatures.end(); ++iter)
                            (*iter)->RemoveAura(SPELL_INDICATOR);
                    }
                    events.ScheduleEvent(EVENT_SHOTEVENT_ACTIVE, 3000);
                    break;
                }
                }
            }
        }
    };

    CreatureAI* GetAI(Creature *creature) const override
    {
        return new npc_darkmoon_faire_rinlingAI(creature);
    }
};

class spell_darkmoon_cannon_prep : public AuraScript
{
    PrepareAuraScript(spell_darkmoon_cannon_prep);

    void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* pCaster = GetCaster())
            pCaster->SetControlled(true, UNIT_STATE_ROOT);
    }

    void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* pCaster = GetCaster())
        {
            pCaster->SetControlled(false, UNIT_STATE_ROOT);

            if (Creature* CanonTrigger = pCaster->FindNearestCreature(NPC_DARKMOON_FAIRE_CANNON, 30.0f))
                CanonTrigger->CastSpell(pCaster, SPELL_DARKMOON_MAGIC_WINGS, true);
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_darkmoon_cannon_prep::OnApply, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_darkmoon_cannon_prep::OnRemove, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
    }
};

//62244
class spell_darkmoon_cannonball : public AuraScript
{
    PrepareAuraScript(spell_darkmoon_cannonball);

    void HandlePeriodicTick(AuraEffect const* /*aurEff*/)
    {
        Unit* pCaster = GetCaster();
        if (!pCaster)
            return;

        if (pCaster->IsInWater())
            if (Creature* CanonTrigger = pCaster->FindNearestCreature(NPC_DARKMOON_FAIRE_CANNON_TARGET, 30.0f))
            {
                pCaster->CastSpell(pCaster, GetSpellInfo()->Effects[EFFECT_0]->BasePoints, true);

                if (pCaster->IsInRange(CanonTrigger, 0.0f, 1.0f))
                {
                    for (uint8 i = 0; i < 5; ++i)
                        pCaster->CastSpell(pCaster, SPELL_LANDING_RESULT_KILL_CREDIT, true);

                    pCaster->CastSpell(pCaster, SPELL_BULLSEYE, true);
                }
                else if (pCaster->IsInRange(CanonTrigger, 1.0f, 4.0f))
                {
                    for (uint8 i = 0; i < 3; ++i)
                        pCaster->CastSpell(pCaster, SPELL_LANDING_RESULT_KILL_CREDIT, true);

                    pCaster->CastSpell(pCaster, SPELL_GREAT_SHOT, true);
                }
                else if (pCaster->IsInRange(CanonTrigger, 4.0f, 15.0f))
                {
                    pCaster->CastSpell(pCaster, SPELL_POOR_SHOT, true);
                }

                pCaster->RemoveAura(SPELL_CANNONBALL);
            }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_darkmoon_cannonball::HandlePeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// Cannon - 102292
class spell_darkmoon_faire_cannon : public SpellScript
{
    PrepareSpellScript(spell_darkmoon_faire_cannon);

    void HandleHitTarget(SpellEffIndex /*effIndex*/)
    {
        PreventHitDefaultEffect(EFFECT_0);

        Unit* caster = GetCaster()->GetOwner();
        Unit* target = GetHitUnit()->ToCreature();

        if (!caster || !target)
            return;

        if (target->isAlive() && target->GetEntry() == NPC_TONK_TARGET)
            caster->CastSpell(caster, SPELL_TONK_TARGET, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_darkmoon_faire_cannon::HandleHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Ring Toss - 101695
class spell_darkmoon_ring_toss : public SpellScript
{
    PrepareSpellScript(spell_darkmoon_ring_toss);

    void HandleHitTarget(SpellEffIndex /*effIndex*/)
    {
        PreventHitDefaultEffect(EFFECT_0);

        Unit* caster = GetCaster();
        Unit* target = GetHitUnit()->ToCreature();
        if (!caster || !target)
            return;

        if (target->GetEntry() == NPC_DUBENKO)
        {
            float distance = target->GetDistance2d(GetExplTargetDest()->GetPositionX(), GetExplTargetDest()->GetPositionY());

            if (distance < 0.5f)
                caster->CastSpell(target, SPELL_RING_TOSS_HIT, true);

            else if (distance < 2.0f)
                caster->CastSpell(target, SPELL_RING_TOSS_MISS, true);

            else if (distance < 4.0f)
                caster->CastSpell(target, SPELL_RING_TOSS_MISS_2, true);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_darkmoon_ring_toss::HandleHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

class spell_darkmoon_deathmatch_teleport : public SpellScript
{
    PrepareSpellScript(spell_darkmoon_deathmatch_teleport);

    void HandleScript(SpellEffIndex effIndex)
    {
        Player* pTarget = GetHitUnit()->ToPlayer();
        if (!pTarget)
            return;

        uint32 Spells[8] =
        {
            SPELL_DARKMOON_DEATHMATCH_TELE_1,
            SPELL_DARKMOON_DEATHMATCH_TELE_2,
            SPELL_DARKMOON_DEATHMATCH_TELE_3,
            SPELL_DARKMOON_DEATHMATCH_TELE_4,
            SPELL_DARKMOON_DEATHMATCH_TELE_5,
            SPELL_DARKMOON_DEATHMATCH_TELE_6,
            SPELL_DARKMOON_DEATHMATCH_TELE_7,
            SPELL_DARKMOON_DEATHMATCH_TELE_8
        };
        uint8 rand = urand(0, 8);

        pTarget->CastSpell(pTarget, Spells[rand]);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_darkmoon_deathmatch_teleport::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

class spell_darkmoon_summon_rings : public SpellScript
{
    PrepareSpellScript(spell_darkmoon_summon_rings);

    void HandleScript(SpellEffIndex effIndex)
    {
        Player* caster = GetHitUnit()->ToPlayer();

        if (!caster)
            return;

        for (uint8 i = 0; i < 57; ++i)
            caster->CastSpell(SpawnRingsAT[i], 170815, false);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_darkmoon_summon_rings::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

//46668
class spell_darkmoon_carousel_whee : public AuraScript
{
    PrepareAuraScript(spell_darkmoon_carousel_whee);

    uint32 update;

    bool Validate(SpellInfo const* /*spell*/) override
    {
        update = 0;
        return true;
    }

    void OnUpdate(uint32 diff)
    {
        update += diff;

        if (update >= 5000)
        {
            Player* _player = GetCaster()->ToPlayer();

            if (!_player)
                return;

            if (_player->IsOnVehicle())
            {
                if (_player->GetMapId() == 974)
                {
                    if (GameObject* Go = GetClosestGameObjectWithEntry(_player, 216666, 15.0f))
                    {
                        if (Aura* aura = GetAura())
                        {
                            uint32 currentMaxDuration = aura->GetMaxDuration();
                            uint32 newMaxDurantion = currentMaxDuration + (5 * MINUTE * IN_MILLISECONDS);
                            newMaxDurantion = newMaxDurantion <= (60 * MINUTE * IN_MILLISECONDS) ? newMaxDurantion : (60 * MINUTE * IN_MILLISECONDS);

                            aura->SetMaxDuration(newMaxDurantion);
                            aura->SetDuration(newMaxDurantion);
                        }
                    }
                }
            }
            update = 0;
        }
    }

    void Register() override
    {
        OnAuraUpdate += AuraUpdateFn(spell_darkmoon_carousel_whee::OnUpdate);
    }
};

void AddSC_darkmoon()
{
    new boss_darkmoon_moonfang_mother();
    new npc_darkmoon_faire_gnolls();
    new npc_darkmoon_faire_gnoll_holder();
    new npc_darkmoon_faire_rinling();
    RegisterAuraScript(spell_darkmoon_cannon_prep);
    RegisterAuraScript(spell_darkmoon_cannonball);
    RegisterSpellScript(spell_darkmoon_faire_cannon);
    RegisterSpellScript(spell_darkmoon_ring_toss);
    RegisterSpellScript(spell_darkmoon_deathmatch_teleport);
    RegisterSpellScript(spell_darkmoon_summon_rings);
    RegisterAuraScript(spell_darkmoon_carousel_whee);
}
