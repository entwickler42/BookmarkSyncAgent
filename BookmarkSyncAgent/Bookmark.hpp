#ifndef Bookmark_h
#define Bookmark_h

#include <string>
#include <vector>

struct Bookmark
{
    std::string url;
    std::string path;
};

typedef std::vector<Bookmark> Bookmarks;

#endif /* Bookmark_h */
