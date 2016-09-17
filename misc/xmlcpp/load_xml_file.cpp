#include "pugixml.hpp"

#include <iostream>

// [pugixml quick start guide](http://pugixml.org/docs/quickstart.html)

int main()
{
// tag::code[]
    pugi::xml_document doc;

    pugi::xml_parse_result result = doc.load_file("maven.xml");

    std::cout << "Load result: " << result.description() << "\n";
    std::cout << "version name: " << doc.child("metadata").child("version").child_value() << std::endl;
    std::cout << "lastUpdated: " << doc.child("metadata").child("versioning").child("lastUpdated").child_value() << std::endl;
// end::code[]
}

