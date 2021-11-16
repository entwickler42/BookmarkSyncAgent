#include <iostream>
#include <exception>
#include "BookmarkSyncAgent.hpp"

int main(int argc, const char * argv[]) {
    int rval = 0; // assume everthing went well
    
    try{
        BookmarkSyncAgent agent;
        agent.runAgent();
    }catch(std::invalid_argument& ex){
        std::cout << "invalid argument: " << ex.what() << std::endl;
        rval = -1;
    }catch(std::runtime_error& ex){
        std::cout << "runtime error: " << ex.what() << std::endl;
        rval = -2;
    }catch(std::out_of_range& ex){
        std::cout << "out of range: " << ex.what() << std::endl;
        rval = -3;
    }catch(std::exception& ex){
        std::cout << "exception: " << ex.what() << std::endl;
        rval = -4;
    }catch (...) {
        std::cout << "unhandled exception: " << std::endl;
        rval = -5;
    }
    
    return rval;
}
