#include <iostream>
#include <exception>

#include "BookmarkSyncAgent.hpp"
#include "DataStoreFactory.hpp"


BookmarkSyncAgent::BookmarkSyncAgent()
{
    constructDataStores();
}

void BookmarkSyncAgent::runAgent()
{
    fetchDataStoresToDatabase();
    putDatabaseToDataStores();
}

void BookmarkSyncAgent::constructDataStores()
{
    if(m_DataStores.size() > 0){
        throw std::runtime_error("constructDataStores() was already called!");
    }
    
    std::vector<std::string> names = {
        "Egon",
        "Franz"
    };
    
    for(std::string& i : names){
        try{
            IDataStore* ds = DataStoreFactory::create(i);
            if(ds){
                std::cout << "DataStore created successfully: " << i << std::endl;
            }else{
                std::cout << "Unknown DataStore requested: " << i << std::endl;
            }
        }catch(std::exception& ex){
            std::cout << ex.what() << std::endl;
        }
    }
}

void BookmarkSyncAgent::fetchDataStoresToDatabase()
{
    
}

void BookmarkSyncAgent::putDatabaseToDataStores()
{
}




