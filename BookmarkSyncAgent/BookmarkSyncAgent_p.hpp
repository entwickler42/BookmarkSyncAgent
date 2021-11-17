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
    
    void fetchDataStoresToDatabase() const
    {
        for(auto& i : m_DataStores) {
            bulkUpdateBookmarkFlags(i->name(), IDataStore::FLAG_OUTDATED);
        }
        
        bulkUpdateBookmarkFlags("DataStoreSafari", IDataStore::FLAG_OUTDATED);
        bulkUpdateBookmarkFlags("DataStoreChrome", IDataStore::FLAG_OUTDATED);
        
        // insertOrUpdateBookmarksIntoTable("DataStoreSafari", "bookmarks.folder1.1", "protocol.domain.website.1", IDataStore::FLAG_UPTODATE);
        // insertOrUpdateBookmarksIntoTable("DataStoreSafari", "bookmarks.folder1.2", "protocol.domain.website.2", IDataStore::FLAG_UPTODATE);
        insertOrUpdateBookmarksIntoTable("DataStoreSafari", "bookmarks.folder1.3", "protocol.domain.website.3", IDataStore::FLAG_OUTDATED);
        insertOrUpdateBookmarksIntoTable("DataStoreSafari", "bookmarks.folder2.1", "protocol.domain.website.4", IDataStore::FLAG_UPTODATE);
        insertOrUpdateBookmarksIntoTable("DataStoreSafari", "bookmarks.folder2.2", "protocol.domain.website.5", IDataStore::FLAG_UPTODATE);
        insertOrUpdateBookmarksIntoTable("DataStoreSafari", "bookmarks.folder2.3", "protocol.domain.website.6", IDataStore::FLAG_UPTODATE);

        insertOrUpdateBookmarksIntoTable("DataStoreChrome", "bookmarks.folder1.1", "protocol.domain.website.1", IDataStore::FLAG_UPTODATE);
        insertOrUpdateBookmarksIntoTable("DataStoreChrome", "bookmarks.folder1.2", "protocol.domain.website.2", IDataStore::FLAG_UPTODATE);
        insertOrUpdateBookmarksIntoTable("DataStoreChrome", "bookmarks.folder1.3", "protocol.domain.website.3", IDataStore::FLAG_UPTODATE);
        insertOrUpdateBookmarksIntoTable("DataStoreChrome", "bookmarks.folder2.1", "protocol.domain.website.4", IDataStore::FLAG_UPTODATE);
        insertOrUpdateBookmarksIntoTable("DataStoreChrome", "bookmarks.folder2.2", "protocol.domain.website.5", IDataStore::FLAG_UPTODATE);
        insertOrUpdateBookmarksIntoTable("DataStoreChrome", "bookmarks.folder2.3", "protocol.domain.website.6", IDataStore::FLAG_UPTODATE);
    }
    
    void putDatabaseToDataStores() const
    {
        Bookmarks bookmarks = fetchCombinedBookmarks();
    }
    
private:
    sqlite3 *m_Database = NULL;
    std::vector<IDataStore*> m_DataStores;
    
    void throwAndFreeOnError(const std::string& origin, int sqlCode, char* sqlError) const
    {
        if(sqlCode != SQLITE_OK){
            std::ostringstream msg;
            msg << "Sqlite3 error at " << origin << ": " << sqlError;
            sqlite3_free(sqlError);
            throw std::runtime_error(msg.str());
        }
    }
    
    void executeSQL(const std::string& origin, const std::string& sql, int (*callback)(void*,int,char**,char**) = 0, void* rval = 0) const
    {
        int sql_code;
        char* sql_errmsg;
        
        sql_code = sqlite3_exec(m_Database, sql.c_str(), callback, rval, &sql_errmsg);
        throwAndFreeOnError(origin, sql_code, sql_errmsg);
    }
    
    void bulkUpdateBookmarkFlags(const std::string& dataSource, int32_t flags) const
    {
        std::ostringstream update;
        
        update << "UPDATE Bookmarks SET flags =" << std::to_string(flags) << " where DataStoreName = " << "'" << dataSource << "'";
        
        executeSQL("bulkUpdateBookmarkFlags", update.str());
    }
    
    void insertOrUpdateBookmarksIntoTable(const std::string& dataStoreName, const std::string& path, const std::string& url, int32_t flags) const
    {
        std::ostringstream insert;
        
        insert << "INSERT OR REPLACE INTO ";
        insert << "Bookmarks(DataStoreName, Flags, Path, Url) ";
        insert << "VALUES(" << "'" << dataStoreName << "'" << "," << std::to_string(flags) << "," << "'" << path << "'" << "," << "'" << url << "'" << ")";
        
        executeSQL("insertOrUpdateBookmarksIntoTable", insert.str());
    }
    
    std::vector<DataStoreDefinition> fetchDataStoresDefinition() const
    {
        std::vector<DataStoreDefinition> definitions;
        
        executeSQL("fetchDataStoresDefinition",
                   "SELECT Name, Class, Arguments FROM DataStores",
                   fetchDataStoresDefinitionCallback,
                   reinterpret_cast<void*>(&definitions)
                   );
        
        return definitions;
    }
    
    Bookmarks fetchCombinedBookmarks() const
    {
        Bookmarks bookmarks;
        
        executeSQL("selectCombinedBookmarks",
                   "SELECT DISTINCT Path, Url FROM Bookmarks WHERE Path NOT IN (SELECT Path FROM Bookmarks WHERE Flags = 1)",
                   fetchCombinedBookmarksCallback,
                   reinterpret_cast<void*>(&bookmarks)
                   );
        
        return bookmarks;
    }
    
    static int fetchCombinedBookmarksCallback(void *rval, int argc, char **argv, char **azColName)
    {
        if (argc != 2){
            throw std::out_of_range("Unexpected number of values returned from tabe: DataSources");
        }
            
        Bookmarks* bookmarks = reinterpret_cast<Bookmarks*>(rval);        
        bookmarks->push_back({
            std::string(argv[0]),
            std::string(argv[1])
        });
        
        return 0;
    }
    
    static int fetchDataStoresDefinitionCallback(void *rval, int argc, char **argv, char **azColName)
    {
        if (argc != 3){
            throw std::out_of_range("Unexpected number of values returned from tabe: DataSources");
        }

        std::vector<DataStoreDefinition>* definitions = reinterpret_cast<std::vector<DataStoreDefinition>* >(rval);
        definitions->push_back({
            std::string(argv[0]),
            std::string(argv[1]),
            std::string(argv[2])
        });
        
        return 0;
    }
};


#endif /* BookmarkSyncAgent_p_h */
