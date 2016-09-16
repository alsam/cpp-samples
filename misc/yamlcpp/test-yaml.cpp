#include <iostream>
#include <string>
#include "yaml-cpp/yaml.h"

using namespace std;

// https://docs.pivotal.io/pivotalcf/1-7/devguide/deploy-apps/manifest.html

int main()
{
    YAML::Node config = YAML::LoadFile("app.yaml");

    auto apps = config["applications"];
    std::cout << "apps:\n" << apps << std::endl;
    auto flame = apps[1];
    std::cout << "flame:\n" << flame << std::endl;
    auto chost = flame["host"];
    std::cout << "chost: " << chost << std::endl;
    std::string host = chost.as<std::string>();
    std::cout << "Host: " << host << std::endl;

    return 0;
}

