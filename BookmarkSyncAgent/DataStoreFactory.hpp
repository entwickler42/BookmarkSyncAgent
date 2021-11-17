//
//  DataStoreFactory.hpp
#ifndef DataStoreFactory_hpp
#define DataStoreFactory_hpp

#include <string>
#include "IDataStore.hpp"

class DataStoreFactory
{
public:
    static IDataStore* create(const std::string& classname, const std::string& name, const std::string& arguments);
    
protected:
    DataStoreFactory() {}
    virtual ~DataStoreFactory() {}
};

#endif /* DataStoreFactory_hpp */
