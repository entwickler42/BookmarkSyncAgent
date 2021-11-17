#include "BookmarkSyncAgent_p.hpp"

BookmarkSyncAgent::BookmarkSyncAgent()
{
    d = new BookmarkSyncAgentPrivate();
}

BookmarkSyncAgent::~BookmarkSyncAgent()
{
    delete d;
}

void BookmarkSyncAgent::runAgent()
{
    d->openDatabase();
    d->constructDataStores();
    d->fetchDataStoresToDatabase();
    d->putDatabaseToDataStores();
    d->destroyDataStores();
    d->closeDatabase();
}
