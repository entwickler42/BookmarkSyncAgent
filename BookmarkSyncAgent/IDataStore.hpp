#ifndef IDataStore_h
#define IDataStore_h

#include "Bookmark.hpp"
#include <map>



class IDataStore
{
public:
    enum DataStoreFlags {
        FLAG_NONE = 0x00,
        FLAG_OUTDATED = 0x01,
        FLAG_UPTODATE = 0x02
    };
    
    virtual ~IDataStore()
    {}
    
    virtual std::string name() = 0;
    
    virtual Bookmarks fetchData() = 0;
    virtual void putData(const Bookmarks& bookmarks) = 0;
};




#endif /* IDataStore_h */
