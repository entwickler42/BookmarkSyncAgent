//
//  DataStoreFactory.hpp
#ifndef DataStoreFactory_hpp
#define DataStoreFactory_hpp

#include <string>
#include "IDataStore.h"

class DataStoreFactory
{
public:
    static IDataStore* create(const std::string& name);
    
protected:
    DataStoreFactory() {}
    virtual ~DataStoreFactory() {}
};

#endif /* DataStoreFactory_hpp */
