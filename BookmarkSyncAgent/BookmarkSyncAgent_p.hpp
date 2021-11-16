#ifndef BookmarkSyncAgent_p_h
#define BookmarkSyncAgent_p_h

#include <iostream>
#include <exception>
#include <vector>
#include <sstream>
#include <sqlite3.h>

#include "IDataStore.hpp"
#include "BookmarkSyncAgent.hpp"
#include "DataStoreFactory.hpp"

#ifndef DBPATH
#define DBPATH "/Users/stb/dev/BookmarkSync/Database/database.db"
#endif

struct DataStoreDefinition
{
    std::string name;
    std::string classname;
    std::string arguments;
};

class BookmarkSyncAgentPrivate
{
public:
    ~BookmarkSyncAgentPrivate()
    {
        destroyDataStores();
        closeDatabase();
    }
    
    void openDatabase()
    {
        assert(NULL == m_Database);
        
        if (sqlite3_open(DBPATH, &m_Database)){
            std::ostringstream msg;
            msg << "Can't open database: " << DBPATH << std::endl
            << "sqllite3: " << sqlite3_errmsg(m_Database) << std::endl;
            throw std::runtime_error(msg.str());
        }
    }
    
    void closeDatabase()
    {
        if(NULL != m_Database){
            sqlite3_close(m_Database);
            m_Database = NULL;
        }
    }
    
    std::vector<DataStoreDefinition> fetchDataStoresDefinition()
    {
        std::vector<DataStoreDefinition> definitions;
        char *sql_error = 0;
        int sql_code;
    
        sql_code = sqlite3_exec(
                                m_Database,
                                "SELECT Name, Class, Arguments FROM DataStores",
                                fetchDataStoresDefinitionCallback,
                                reinterpret_cast<void*>(&definitions),
                                &sql_error
                                );
        
        if(sql_code != SQLITE_OK){
            std::ostringstream msg;
            msg << "SQL error: " << sql_error;
            sqlite3_free(sql_error);
            throw std::runtime_error(msg.str());
        }
        
        return definitions;
    }
    
    void destroyDataStores()
    {
        for(int i=0; i<m_DataStores.size(); i++){
            delete m_DataStores[i];
        }
        m_DataStores.clear();
    }
    
    void constructDataStores()
    {
        destroyDataStores();
        
        for(DataStoreDefinition& i : fetchDataStoresDefinition()){
            IDataStore* ds = DataStoreFactory::create(i.classname);
            if(NULL == ds) {
                throw std::invalid_argument(i.classname);
            }
            std::cout << "DataStore constructed successfully: " << i.name << std::endl;
            m_DataStores.push_back(ds);
        }
    }
    
    void fetchDataStoresToDatabase()
    {}
    
    void putDatabaseToDataStores()
    {}
    
    std::vector<IDataStore*> m_DataStores;
    
private:
    sqlite3 *m_Database = NULL;
    
    static int fetchDataStoresDefinitionCallback(void *rval, int argc, char **argv, char **azColName)
    {
        if (argc != 3){
            throw std::out_of_range("Unexpected number of values returned from tabe: DataSources");
        }

        std::vector<DataStoreDefinition>* definitions = reinterpret_cast<std::vector<DataStoreDefinition>* >(rval);
        DataStoreDefinition d = {
            std::string(argv[0]),
            std::string(argv[1]),
            std::string(argv[2])
        };
        
        definitions->push_back(d);
        
        return 0;
    }
};


#endif /* BookmarkSyncAgent_p_h */
