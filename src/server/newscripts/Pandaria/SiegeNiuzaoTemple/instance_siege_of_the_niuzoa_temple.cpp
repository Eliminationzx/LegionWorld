/*
    Dungeon : Stormstout Brewery 85-87
    Instance General Script
*/

#include "siege_of_the_niuzoa_temple.h"

class instance_siege_of_the_niuzoa_temple : public InstanceMapScript
{
public:
    instance_siege_of_the_niuzoa_temple() : InstanceMapScript("instance_siege_of_the_niuzoa_temple", 1011) { }

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_siege_of_the_niuzoa_temple_InstanceMapScript(map);
    }

    struct instance_siege_of_the_niuzoa_temple_InstanceMapScript : public InstanceScript
    {
        ObjectGuid jinbakGuid;
        ObjectGuid vojakGuid;
        ObjectGuid pavalakGuid;
        ObjectGuid neronokGuid;
        ObjectGuid puddleGuid;

        instance_siege_of_the_niuzoa_temple_InstanceMapScript(Map* map) : InstanceScript(map)
        {}

        void Initialize() override
        {
            jinbakGuid.Clear();
            vojakGuid.Clear();
            pavalakGuid.Clear();
            neronokGuid.Clear();
            puddleGuid.Clear();
        }
        
        void OnCreatureCreate(Creature* creature) override
        {
            switch (creature->GetEntry())
            {
                case NPC_JINBAK:
                    jinbakGuid = creature->GetGUID();
                    break;
                case NPC_VOJAK:
                    vojakGuid = creature->GetGUID();
                    break;
                case NPC_PAVALAK:
                    pavalakGuid = creature->GetGUID();
                    break;
                case NPC_NERONOK:
                    neronokGuid = creature->GetGUID();
                    break;
                case NPC_PUDDLE:
                    puddleGuid = creature->GetGUID();
                    break;
            }
        }

        void SetData(uint32 type, uint32 data) override
        {}

        uint32 GetData(uint32 type) const override
        {
            return 0;
        }

        ObjectGuid GetGuidData(uint32 type) const override
        {
            switch (type)
            {
                case NPC_JINBAK:    return jinbakGuid;
                case NPC_VOJAK:     return vojakGuid;
                case NPC_PAVALAK:   return pavalakGuid;
                case NPC_NERONOK:   return neronokGuid;
                case NPC_PUDDLE:    return puddleGuid;
            }

            return ObjectGuid::Empty;
        }

        void Update(uint32 diff) override
        {
            // Challenge
            InstanceScript::Update(diff);
        }
    };

};

void AddSC_instance_siege_of_the_niuzoa_temple()
{
    new instance_siege_of_the_niuzoa_temple();
}
