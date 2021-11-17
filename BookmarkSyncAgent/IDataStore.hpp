#ifndef IDataStore_h
#define IDataStore_h

#include <string>
#include <map>

#include "Bookmark.hpp"

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
    
    std::string name() { return _name; } const
    std::string arguments() { return _arguments; } const
    
    virtual void setName(std::string name) { _name = name; }
    virtual void setArguments(std::string arguments) { _arguments = arguments; }
    
    virtual Bookmarks fetchData() = 0;
    virtual void putData(const Bookmarks& bookmarks) = 0;
    
private:
    std::string _arguments;
    std::string _name;
};




#endif /* IDataStore_h */
