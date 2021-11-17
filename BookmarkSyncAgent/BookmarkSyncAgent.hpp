#ifndef BookmarkSyncAgent_hpp
#define BookmarkSyncAgent_hpp

class BookmarkSyncAgentPrivate;

class BookmarkSyncAgent final
{
public:
    BookmarkSyncAgent();
    ~BookmarkSyncAgent();
    
    void runAgent();
    
private:
    BookmarkSyncAgentPrivate* d;
};

#endif /* BookmarkSyncAgent_hpp */
