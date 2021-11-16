#ifndef BookmarkSyncAgent_hpp
#define BookmarkSyncAgent_hpp

class BookmarkSyncAgentPrivate;

class BookmarkSyncAgent
{
public:
    BookmarkSyncAgent();
    virtual ~BookmarkSyncAgent();
    
    void runAgent();
    
private:
    BookmarkSyncAgentPrivate* d;
};

#endif /* BookmarkSyncAgent_hpp */
