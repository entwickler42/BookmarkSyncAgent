#include "DataStoreFactory.hpp"

#include <functional>
#include <map>

class DebugDataStore : public IDataStore
{
public:
    std::string name()
    {
        return "foo";
    }
    
    Bookmarks fetchData()
    {
        Bookmarks v;
        
        return v;
    }
    
    void putData(const Bookmarks& bookmarks)
    {}
};


IDataStore* DataStoreFactory::create(const std::string& classname, const std::string& name, const std::string& arguments)
{
    IDataStore* data_store = 0;
    
    std::map<std::string, std::function<IDataStore*(void)>> types {
        { "DataStoreChrome", []() { return new DebugDataStore(); } },
        { "DataStoreSafari", []() { return new DebugDataStore(); } }
    };
    
    if(types.find(classname) != types.end()){
        data_store = types[classname]();
        data_store->setName(name);
        data_store->setArguments(arguments);
    }
    
    return data_store;
}
