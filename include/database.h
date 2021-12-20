#pragma once
#include <iostream>
#include <map>
#include <mutex>

class ObjectAskue;


class Database
{
    std::map<int, ObjectAskue> _db{};
    std::mutex _mx_map;
public:
    void setObject(ObjectAskue& askue);
};

void showAskue(const ObjectAskue& askue);