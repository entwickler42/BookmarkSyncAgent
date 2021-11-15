//
//  DataStoreFactory.hpp
//  BookmarkSyncAgent
//
//  Created by Stefan Brueck on 15.11.21.
//

#ifndef DataStoreFactory_hpp
#define DataStoreFactory_hpp

#include <stdio.h>
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
