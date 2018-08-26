#include "grim_batol.h"

class instance_grim_batol : public InstanceMapScript
{
public:
    instance_grim_batol() : InstanceMapScript("instance_grim_batol", 670) { }

    InstanceScript* GetInstanceScript(InstanceMap* pMap) const
    {
        return new instance_grim_batol_InstanceMapScript(pMap);
    }

    struct instance_grim_batol_InstanceMapScript : public InstanceScript
    {
        instance_grim_batol_InstanceMapScript(Map* pMap) : InstanceScript(pMap)
        {
            SetBossNumber(MAX_ENCOUNTER);
        }
        
        ObjectGuid uiGeneralUmbrissGUID;
        ObjectGuid uiForgemasterThrongusGUID;
        ObjectGuid uiDrahgaShadowburnerGUID;
        ObjectGuid uiErudaxGUID;
        
        uint32 m_auiEncounter[MAX_ENCOUNTER];
        
        std::string str_data;
        
        void Initialize()
        {
            for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                m_auiEncounter[i] = NOT_STARTED;
                
            uiGeneralUmbrissGUID.Clear();
            uiForgemasterThrongusGUID.Clear();
            uiDrahgaShadowburnerGUID.Clear();
            uiErudaxGUID.Clear();
        }
        
        bool IsEncounterInProgress() const
        {
            for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                if (m_auiEncounter[i] == IN_PROGRESS)
                    return true;
            return false;
        }
        
        void OnCreatureCreate(Creature* creature)
        {
            switch(creature->GetEntry())
            {
                case NPC_GENERAL_UMBRISS:
                    uiGeneralUmbrissGUID = creature->GetGUID();
                    break;
                case NPC_FORGEMASTER_THRONGUS:
                    uiForgemasterThrongusGUID = creature->GetGUID();
                    break;
                case NPC_DRAHGA_SHADOWBURNER:
                    uiDrahgaShadowburnerGUID = creature->GetGUID();
                    break;
                case NPC_ERUDAX:
                    uiErudaxGUID = creature->GetGUID();
                    break;
            }
        }
        
        void SetData(uint32 type, uint32 data)
        {
            switch(type)
            {
                case DATA_GENERAL_UMBRISS:
                    m_auiEncounter[0] = data;
                    break;
                case DATA_FORGEMASTER_THRONGUS:
                    m_auiEncounter[1] = data;
                    break;
                case DATA_DRAHGA_SHADOWBURNER:
                    m_auiEncounter[2] = data;
                    break;
                case DATA_ERUDAX:
                    m_auiEncounter[3] = data;
                    break;
                }

                if (data == DONE)
                    SaveToDB();
        }
        
        uint32 GetData(uint32 type) const override
        {
            switch(type)
            {
                case DATA_GENERAL_UMBRISS:
                    return m_auiEncounter[0];
                case DATA_FORGEMASTER_THRONGUS:
                    return m_auiEncounter[1];
                case DATA_DRAHGA_SHADOWBURNER:
                    return m_auiEncounter[2];
                case DATA_ERUDAX:
                    return m_auiEncounter[3];
            }
            return 0;
        }
        
        ObjectGuid GetGuidData(uint32 identifier) const
        {
            switch(identifier)
            {
                case DATA_GENERAL_UMBRISS:
                    return uiGeneralUmbrissGUID;
                case DATA_FORGEMASTER_THRONGUS:
                    return uiForgemasterThrongusGUID;
                case DATA_DRAHGA_SHADOWBURNER:
                    return uiDrahgaShadowburnerGUID;
                case DATA_ERUDAX:
                    return uiErudaxGUID;
            }
            return ObjectGuid::Empty;
        }
        
        std::string GetSaveData()
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << "G B " << m_auiEncounter[0] << " " << m_auiEncounter[1] << " "
                << m_auiEncounter[2] << " " << m_auiEncounter[3];

            str_data = saveStream.str();

            OUT_SAVE_INST_DATA_COMPLETE;
            return str_data;
        }

        void Load(const char* in)
        {
            if (!in)
            {
                OUT_LOAD_INST_DATA_FAIL;
                return;
            }

            OUT_LOAD_INST_DATA(in);

            char dataHead1, dataHead2;
            uint16 data0, data1, data2, data3;

            std::istringstream loadStream(in);
            loadStream >> dataHead1 >> dataHead2 >> data0 >> data1 >> data2 >> data3;

            if (dataHead1 == 'G' && dataHead2 == 'B')
            {
                m_auiEncounter[0] = data0;
                m_auiEncounter[1] = data1;
                m_auiEncounter[2] = data2;
                m_auiEncounter[3] = data3;

                for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    if (m_auiEncounter[i] == IN_PROGRESS)
                        m_auiEncounter[i] = NOT_STARTED;

            } else OUT_LOAD_INST_DATA_FAIL;

            OUT_LOAD_INST_DATA_COMPLETE;
        }
    };
};

void AddSC_instance_grim_batol()
{
    new instance_grim_batol();
}
