#ifndef IDataStore_h
#define IDataStore_h

#include "Bookmark.h"
#include <map>

class IDataStore
{
public:
    virtual ~IDataStore()
    {}
    
    virtual std::string name() = 0;
    
    virtual Bookmarks fetchData() = 0;
    virtual void putData(const Bookmarks& bookmarks) = 0;
};




#endif /* IDataStore_h */
