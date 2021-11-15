#include <iostream>
#include <exception>
#include "BookmarkSyncAgent.hpp"

int main(int argc, const char * argv[]) {
    int rval = 0; // assume everthing went well
    
    try{
        BookmarkSyncAgent agent;
        agent.runAgent();
    }catch(std::invalid_argument& ex){
        std::cout << ex.what() << std::endl;
        rval = -1;
    }catch(std::runtime_error& ex){
        std::cout << ex.what() << std::endl;
        rval = -2;
    }catch(std::exception& ex){
        std::cout << ex.what() << std::endl;
        rval = -3;
    }catch (...) {
        std::cout << "unhandled exception occured" << std::endl;
        rval = -4;
    }
    
    return rval;
}
