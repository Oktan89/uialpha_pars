#pragma once
#include <sstream>
#include <iostream>
#include <mutex>


struct pcout : public std::stringstream {
static inline std::mutex cout_mutex;

~pcout() {
    std::lock_guard<std::mutex> l {cout_mutex};
    std::cout << rdbuf();
    std::cout.flush();
    }
};

