#pragma once
#include <windows.h>

class ConsoleCP
{
    UINT oldin;
    UINT oldout;

public:
    ConsoleCP(UINT cp)
    {
        oldin = GetConsoleCP();
        oldout = GetConsoleOutputCP();
        SetConsoleCP(cp);
        SetConsoleOutputCP(cp);
    }

    // поскольку мы изменили свойства внешнего объекта — консоли, нам нужно
    // вернуть всё как было (если программа вылетит, пользователю не повезло)
    ~ConsoleCP()
    {
        SetConsoleCP(oldin); 
        SetConsoleOutputCP(oldout);
    }
};