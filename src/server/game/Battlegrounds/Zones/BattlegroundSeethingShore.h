
#ifndef __BATTLEGROUNDSS_H
#define __BATTLEGROUNDSS_H

#include "Battleground.h"

struct BattlegoundSeethingShoreScore final : BattlegroundScore
{
    friend class BattlegroundSeethingShore;

protected:
    BattlegoundSeethingShoreScore(ObjectGuid playerGuid, TeamId team);

    void UpdateScore(uint32 type, uint32 value) override;
    void BuildObjectivesBlock(std::vector<int32>& stats) override;

    uint32 Captures = 0;
};

class BattlegroundSeethingShore : public Battleground
{
    std::array<Transport*, MAX_TEAMS> _gunship;
    std::unordered_map<uint32, bool> _azeriteFissureIds;
    uint32 _spawnTimer;
    uint32 _spawnTimerDelay;
    bool _isInformedNearVictory;
    bool _startDelay;
public:
    BattlegroundSeethingShore();
    ~BattlegroundSeethingShore() override;

    void AddPlayer(Player* player) override;
    void StartingEventCloseDoors() override;
    void StartingEventOpenDoors() override;
    void RemovePlayer(Player* player, ObjectGuid guid, uint32 team) override;
    bool SetupBattleground() override;
    void Reset() override;
    void RelocateDeadPlayers(ObjectGuid guideGuid) override;
    bool UpdatePlayerScore(Player* player, uint32 type, uint32 value, bool doAddHonor = true) override;
    void ActivateRandomAzeriteFissure();
    void OnGameObjectCreate(GameObject* object) override;
    void OnCreatureRemove(Creature* creature) override;
    void CastActivates(Creature* controller);
    void FillInitialWorldStates(WorldPackets::WorldState::InitWorldStates& packet) override;
    void EventPlayerClickedOnFlag(Player* source, GameObject* object) override;
    uint32 GetMaxScore() const override;
    void PostUpdateImpl(uint32 diff) override;

    uint8 _activeAzerriteFissureCounter{0};
    std::list<ObjectGuid> _azeritesToActivate;
};

#endif
