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
        
        executeSQL("constructDataStores",
                   "SELECT Name, Class, Arguments FROM DataStores",
                   constructDataStoresCallback,
                   reinterpret_cast<void*>(&m_DataStores)
                   );
    }
    
    void fetchDataStoresToDatabase() const
    {
        for(auto& i : m_DataStores) {
            bulkUpdateBookmarkFlags(i->name(), IDataStore::FLAG_OUTDATED);
        }
    
        
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
    
    static int constructDataStoresCallback(void *rval, int argc, char **argv, char **azColName)
    {
        if (argc != 3){
            throw std::out_of_range("Unexpected number of values returned from tabe: DataSources");
        }
        
        std::string name = std::string(argv[0]);
        std::string classname = std::string(argv[1]);
        std::string arguments = std::string(argv[2]);
        
        IDataStore* ds = DataStoreFactory::create(classname, name, arguments);
        if(NULL == ds) {
            throw std::invalid_argument(classname);
        }
        std::cout << "DataStore constructed successfully: " << classname << std::endl;
        
        std::vector<IDataStore*>* datastores = reinterpret_cast<std::vector<IDataStore*>*>(rval);
        datastores->push_back(ds);
        
        return 0;
    }
};


#endif /* BookmarkSyncAgent_p_h */
