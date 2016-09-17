#include <set>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

struct zowner
{
    std::string           owner;
    std::set<std::string> items;
};

zowner load(const std::string &file)
{
    boost::property_tree::ptree pt;
    read_xml(file, pt);

    zowner co;

    co.owner = pt.get<std::string>("html.body.h1");

    //BOOST_FOREACH(
    //   boost::property_tree::ptree::value_type &v,
    //   pt.get_child("html.body.table"))
    //   co.items.insert(v.second.data());

    return co;
}

int main()
{
    auto tree = load("http_head_doesnt_parse_with_pugixml.xml");
    //auto tree = load("http_head.xml");
}
