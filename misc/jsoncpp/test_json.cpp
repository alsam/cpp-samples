#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include "json/json.h"

// see [Parsing JSON using jsoncpp](http://codereview.stackexchange.com/questions/122602/parsing-json-using-jsoncpp)
// for code review

class JsonRoot
{
private:
    Json::Value root_;

public:

    JsonRoot(std::string const&);

    Json::Value const& operator[](std::string const& index) const {
        return root_[index];
    }
};

JsonRoot::JsonRoot(std::string const& path)
{
    std::ifstream json(path);
    if (!json) {
        std::perror(path.c_str());
        std::exit(-1);
    }
    json >> root_;
}

int main(int argc, char** argv)
{
    try {
        if (argc < 2) {
            std::cout << "give me json file\n";
            std::exit(-2);
        }
        JsonRoot json(argv[1]);
        Json::Value v = json["query"]["results"];
        std::cout << "v: " << v << std::endl;
        if (v.isArray()) { // represented as std::map
            for (int i = 0; i < v.size(); ++i) {
                std::cout << "v[ " << i << " ] = " << v[i] << std::endl;
            }
            for (auto const& z : v) {
                std::cout << "z: " << z << "\n";
                auto const& mem_names = z.getMemberNames();
                for (auto const& nm : mem_names) {
                    std::cout << "nm : " << nm << std::endl;
                    auto val = z[nm];
                    std::cout << "val : " << val << std::endl;
                }
            }
        }
        return EXIT_SUCCESS;
    } catch (std::exception const& e) {
        std::cerr << e.what() << "\n";
    }
}

#if 0
int main()
{
    bool alive = true;
    while (alive) {
        Json::Value root;
        Json::Reader reader;
        std::string test = "test.json";
        bool parsingSuccessful = reader.parse( test, root, false );
        if ( !parsingSuccessful )
        {
            // report to the user the failure and their locations in the document.
            std::cout  << reader.getFormatedErrorMessages()
                   << "\n";
        }
    
        std::string encoding = root.get("encoding", "UTF-8" ).asString();
        std::cout << encoding << "\n";
        alive = false;

    }
    return 0;
}

int main()
{
    // For convenience, use `writeString()` with a specialized builder.
    Json::StreamWriterBuilder wbuilder;
    wbuilder["indentation"] = "\t";
    std::string document = Json::writeString(wbuilder, root);
    // Here, using a specialized Builder, we discard comments and
    // record errors as we parse.
    Json::CharReaderBuilder rbuilder;
    rbuilder["collectComments"] = false;
    std::string errs;
    bool ok = Json::parseFromStream(rbuilder, std::cin, &root, &errs);
}
#endif

