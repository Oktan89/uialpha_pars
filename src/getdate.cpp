#include <chrono>
#include "getdate.h"

void Getdate::update() const // нужно предусмотреть многопоточность
{
    std::lock_guard<std::mutex> loc(_m_date);
    time_now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    _tm = std::localtime(&time_now);
}

// Получить сконфигурированную строку с датой и временем
// hour- H
// min - M
// sec - S
// year- y
// mon - m
// day - d
// exemple: {'H',':', 'M', ':', 'S'} return 22:01:36
std::string Getdate::getdate_time(std::initializer_list<char> lst) const
{
    std::string s_date{};
    update();

    for (auto i = lst.begin(); i != lst.end(); ++i)
    {
        switch (*i)
        {
        case 'H':
            s_date += ((_tm->tm_hour>=10 ? std::to_string(_tm->tm_hour) : "0" + std::to_string(_tm->tm_hour)));
            break;
        case 'M':
            s_date += ((_tm->tm_min>=10 ? std::to_string(_tm->tm_min) : "0" + std::to_string(_tm->tm_min)));
            break;
        case 'S':
            s_date += ((_tm->tm_sec>=10 ? std::to_string(_tm->tm_sec) : "0" + std::to_string(_tm->tm_sec)));
            break;
        case 'y':
            s_date += (std::to_string(_tm->tm_year % 100));
            break;
        case 'm':
            s_date += ((_tm->tm_mon+1 >= 10 ? std::to_string(_tm->tm_mon + 1) : "0" + std::to_string(_tm->tm_mon + 1)));;
            break;
        case 'd':
            s_date += ((_tm->tm_mday>=10 ? std::to_string(_tm->tm_mday) : "0" + std::to_string(_tm->tm_mday)));
            break;
        default:
            s_date += *i;
            break;
        }
    }
    return s_date;
}

Getdate *Getdate::GetObjectDate()
{
    std::lock_guard<std::mutex> loc(_m_date);

    if(_date == nullptr)
    {
        _date = new Getdate();
    } 
    
    return _date;
}

/*void Getdate::Destroy()
{
    delete _date;
}*/

std::tm Getdate::getStructTmTimeNow() const
{
    update();
    return *_tm;
}

Getdate * Getdate::_date = nullptr;
std::mutex Getdate::_m_date;