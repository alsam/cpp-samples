#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <boost/filesystem.hpp> 

enum class NetworkClass {
    None,
    Wired,
    Wireless,
};

NetworkClass checkNetworkStatus()
{
    using namespace boost::filesystem;

    const std::string targetPath { "/sys/class/net/" };

    auto checkStatus = [&targetPath](std::string const& fname, std::regex const& reg) -> bool
    {
        std::smatch what;
        std::string status;
        if (std::regex_match(fname, what, reg))
        {
            std::ifstream ifs(targetPath + fname + "/carrier");
            if (ifs.good())
            {
                ifs >> status;
                return (status == std::string("1"));
            }
        }
        return false;
    };

    const std::regex wiredWildcard { "eth.*" };
    const std::regex wirelessWildcard { "wlan.*" };

    directory_iterator endItr; // default ctor yields past-the-end
    for (directory_iterator i(targetPath); i != endItr; ++i)
    {
        std::string fname = i->path().filename().string();
        if (checkStatus(fname, wiredWildcard))
        {
            return NetworkClass::Wired;
        }
        if (checkStatus(fname, wirelessWildcard))
        {
            return NetworkClass::Wireless;
        }
    }

    return NetworkClass::None;
}

int main()
{
    NetworkClass status = checkNetworkStatus();
}
