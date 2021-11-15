#ifndef BookmarkSyncAgent_hpp
#define BookmarkSyncAgent_hpp

#include <vector>
#include "IDataStore.h"

class BookmarkSyncAgent
{
public:
    BookmarkSyncAgent();
    void runAgent();
    
protected:
    void constructDataStores();
    void fetchDataStoresToDatabase();
    void putDatabaseToDataStores();
    
private:
    std::vector<IDataStore> m_DataStores;
};

#endif /* BookmarkSyncAgent_hpp */
