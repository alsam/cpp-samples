#include <iostream>
#include <ctime>
#include <sunset.h>

// NY

// #define LATITUDE 40.7142700 // °
// #define LONGITUDE -74.0059700 // °
// #define TIMEZONE -5

// Moscow

#define LATITUDE 55.4544 // °
#define LONGITUDE 37.3763 // °
#define TIMEZONE 3

int main(int argc, char **argv)
{
    SunSet sun;
    
    auto rightnow = std::time(nullptr);
    struct tm *tad = std::localtime(&rightnow);
    sun.setPosition(LATITUDE, LONGITUDE, TIMEZONE);
    sun.setTZOffset(TIMEZONE);
    long sunrise = static_cast<long>(sun.calcSunrise());
    long sunset = static_cast<long>(sun.calcSunset());

    std::cout << "NY year "
              << tad->tm_year + 1900
              << " mon " << tad->tm_mon + 1
              << " day " << tad->tm_mday
              << " Sunrise at " << (sunrise/60) << ":" << (sunrise%60)
              << "am, Sunset at " << (sunset/60) << ":" << (sunset%60) << "pm\n";
}
