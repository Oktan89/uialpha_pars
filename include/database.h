#pragma once
#include <iostream>
#include <map>
#include <mutex>


class ObjectAskue;
class hBox;

class Database
{
    std::map<int, ObjectAskue> _db{};
    std::mutex _mx_map;
    hBox *_box;
public:
    void setObject(ObjectAskue& askue);
    void setBox(hBox *box);
    ObjectAskue &getObject(int key);
};

void showAskue(const ObjectAskue& askue);
