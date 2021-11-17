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

class BookmarkSyncAgentPrivate final
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
        assert(NULL != m_Database);
        
        if(NULL != m_Database){
            sqlite3_close(m_Database);
            m_Database = NULL;
        }
    }
    
    std::vector<DataStoreDefinition> fetchDataStoresDefinition()
    {
        std::vector<DataStoreDefinition> definitions;
        char *sql_errmsg = 0;
        int sql_code;
    
        sql_code = sqlite3_exec(
                                m_Database,
                                "SELECT Name, Class, Arguments FROM DataStores",
                                fetchDataStoresDefinitionCallback,
                                reinterpret_cast<void*>(&definitions),
                                &sql_errmsg
                                );
        
        throwAndFreeOnError("fetchDataStoresDefinition", sql_code, sql_errmsg);
        
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
    {
        for(auto& i : m_DataStores) {
            bulkUpdateBookmarkFlags(i->name(), IDataStore::FLAG_OUTDATED);
        }
        
        insertOrUpdateBookmarksIntoTable("", "", "", IDataStore::FLAG_UPTODATE);
    }
    
    void putDatabaseToDataStores()
    {}
    
private:
    sqlite3 *m_Database = NULL;
    std::vector<IDataStore*> m_DataStores;
    
    void throwAndFreeOnError(const std::string& origin, int sqlCode, char* sqlError)
    {
        if(sqlCode != SQLITE_OK){
            std::ostringstream msg;
            msg << "Sqlite3 error at " << origin << ": " << sqlError;
            sqlite3_free(sqlError);
            throw std::runtime_error(msg.str());
        }
    }
    
    void executeSQL(const std::string& origin, const std::string& sql)
    {
        int sql_code;
        char* sql_errmsg;
        
        sql_code = sqlite3_exec(m_Database, sql.c_str(), NULL, NULL, &sql_errmsg);
        throwAndFreeOnError(origin, sql_code, sql_errmsg);
    }
    
    void bulkUpdateBookmarkFlags(const std::string& dataSource, int32_t flags)
    {
        std::ostringstream update;
        
        update << "UPDATE BOOKMARKS SET flags =" << std::to_string(flags) << " where DataSource = " << "'" << dataSource << "'";
        
        executeSQL("bulkUpdateBookmarkFlags", update.str());
    }
    
    void insertOrUpdateBookmarksIntoTable(const std::string& dataSource, const std::string& path, const std::string& url, int32_t flags)
    {
        std::ostringstream insert;
        
        insert << "INSERT OR REPLACE INTO ";
        insert << "Bookmarks(DatSource, Flags, Path, Url) ";
        insert << "VALUES(" << "'" << dataSource << "'" << "," << std::to_string(flags) << "," << "'" << path << "'" << "," << "'" << url << "'" << ")";
        
        executeSQL("insertOrUpdateBookmarksIntoTable", insert.str());
    }
    
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
