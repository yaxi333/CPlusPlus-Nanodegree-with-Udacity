#include <string>

#include "format.h"
#include <cmath>
#include <iostream>

using std::string;

string Format::ElapsedTime(long seconds) {
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    int second =  static_cast<int>(seconds % 60);
    std::string res;
    res = std::to_string(hours) + ":" + std::to_string(minutes) + ":" + std::to_string(second);
    return res;
}