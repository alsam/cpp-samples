/**
* \file
* The most simple example.
* install: `yaourt -S aur/libcurlpp`
* [curlpp examples](https://github.com/jpbarrette/curlpp/tree/master/examples)
* [curlpp download wnd write callback example](https://github.com/datacratic/curlpp/blob/master/examples/example11.cpp)
* [Link errors with curlpp](http://stackoverflow.com/questions/33420631/link-errors-with-curlpp)
*/

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

// curlpp specific headers
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

// pugixml
#include <pugixml.hpp>

// [clean html with C or C++ tidy](http://www.cppfun.com/clean-html-with-c-or-cpp-tidy.htm)
// [tidy 5.2.0-1](https://www.archlinux.org/packages/extra/x86_64/tidy/)
// `pacman -S extra/tidy`

#include <tidy.h>
#include <buffio.h>
 
std::string cleanhtml(const std::string &html)
{
    // init a tidy document
    TidyDoc tidy_doc=tidyCreate();
    TidyBuffer output_buffer= {0};
 
    // configure tidy
    // the flags tell tidy to output xml and disable warnings
    bool config_success=tidyOptSetBool(tidy_doc,TidyXmlOut,yes)
                        && tidyOptSetBool(tidy_doc,TidyQuiet,yes)
                        && tidyOptSetBool(tidy_doc,TidyNumEntities,yes)
                        && tidyOptSetBool(tidy_doc,TidyShowWarnings,no);
 
    int tidy_rescode=-1;
 
    // parse input
    if(config_success)
        tidy_rescode=tidyParseString(tidy_doc,html.c_str());
 
    // process html
    if(tidy_rescode>=0)
        tidy_rescode=tidySaveBuffer(tidy_doc,&output_buffer);
 
    if(tidy_rescode<0)
        throw("tidy has a error: "+tidy_rescode);
 
    std::string result=(char *)output_buffer.bp;
    tidyBufFree(&output_buffer);
    tidyRelease(tidy_doc);
 
    return result;
}


using namespace curlpp::options;

bool is_ready(false);
std::mutex m;
std::condition_variable cv;

struct simple_walker: pugi::xml_tree_walker {
    virtual bool for_each(pugi::xml_node& node) {
        for (int i = 0; i < depth(); ++i) std::cout << "  "; // indentation
        std::cout << /* node_types[node.type()] << */ ": name='" << node.name() << "', value='" << node.value() << "'\n";
        return true; // continue traversal
    }
};

void
getWebPage(std::string const& url, std::string& page)
{
    std::unique_lock<std::mutex> lk(m);
    try {
        // That's all that is needed to do cleanup of used resources (RAII style).
        curlpp::Cleanup myCleanup;

        // Our request to be sent.
        curlpp::Easy myRequest;

        // Set the URL.
        myRequest.setOpt(new curlpp::options::Url(url));

        // Send request and get a result.
        // By default the result goes to standard output.

        std::ostringstream os;
        curlpp::options::WriteStream ws(&os);
        myRequest.setOpt(ws);
        myRequest.perform();

        page = std::move(os.str());

        is_ready = true;
        cv.notify_one();

    } catch(curlpp::RuntimeError & e) {
        std::cout << e.what() << std::endl;
    }

    catch(curlpp::LogicError & e) {
        std::cout << e.what() << std::endl;
    }
}

std::vector<std::string>
dirList(std::string const& html)
{
    std::string cleaned_xml{cleanhtml(html)};
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(cleaned_xml.c_str());
    std::cout << "html.body.h1: " << doc.child("html").child("body").child("h1").child_value() << std::endl;
    auto table = doc.child("html").child("body").child("table");
    // tag::code[]
    for (auto it = table.begin(); it != table.end(); ++it) {
      std::cout << "TR;";
      auto td = it->child("td").child("a");
      for (pugi::xml_attribute_iterator ait = td.attributes_begin(); ait != td.attributes_end(); ++ait) {
          std::cout << " " << ait->name() << " <=> " << ait->value();
      }
      std::cout << " TD " << td << std::endl;
    }

    std::vector<std::string> ret;
    return ret;
}

std::vector<std::string>
parseManifest(std::string const& manifest_xml)
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(manifest_xml.c_str());

    if (result) {
        std::cout << "XML parsed without errors\n";
    } else {
        std::cout << "Error description: " << result.description() << "\n";
    }

    std::cout << "version name: " << doc.child("metadata").child("version").child_value() << std::endl;
    std::cout << "lastUpdated: " << doc.child("metadata").child("versioning").child("lastUpdated").child_value() << std::endl;

    std::vector<std::string> ret;
    return ret;
}

void
pull_one_url(std::string const& url, bool maven)
{
    std::unique_lock<std::mutex> lk(m);
    try {
        // That's all that is needed to do cleanup of used resources (RAII style).
        curlpp::Cleanup myCleanup;

        // Our request to be sent.
        curlpp::Easy myRequest;

        // Set the URL.
        myRequest.setOpt(new curlpp::options::Url(url));
        //myRequest.setOpt(new curlpp::options::FtpListOnly(true));

        // Send request and get a result.
        // By default the result goes to standard output.

        std::ostringstream os;
        curlpp::options::WriteStream ws(&os);
        myRequest.setOpt(ws);
        myRequest.perform();
        std::cout << "os: " << cleanhtml(os.str()) << std::endl;

        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_string(cleanhtml(os.str()).c_str());

        if (result) {
            std::cout << "XML parsed without errors\n";
        } else {
            std::cout << "Error description: " << result.description() << "\n";
        }

        //simple_walker walker;
        //doc.traverse(walker);

        if (maven) {
            std::cout << "version name: " << doc.child("metadata").child("version").child_value() << std::endl;
            std::cout << "lastUpdated: " << doc.child("metadata").child("versioning").child("lastUpdated").child_value() << std::endl;
        } else {
            std::cout << "html.body.h1: " << doc.child("html").child("body").child("h1").child_value() << std::endl;
            auto table = doc.child("html").child("body").child("table");
            // tag::code[]
            for (auto it = table.begin(); it != table.end(); ++it) {
              std::cout << "TR;";
              auto td = it->child("td").child("a");
              for (pugi::xml_attribute_iterator ait = td.attributes_begin(); ait != td.attributes_end(); ++ait) {
                  std::cout << " " << ait->name() << " <=> " << ait->value();
              }
              std::cout << " TD " << td << std::endl;
            }
            // end::code[]
        }
    } catch(curlpp::RuntimeError & e) {
        std::cout << e.what() << std::endl;
    }

    catch(curlpp::LogicError & e) {
        std::cout << e.what() << std::endl;
    }

    is_ready = true;
    cv.notify_one();
}

int main(int, char **)
{
    std::string url1 = "https://oss.sonatype.org/content/repositories/snapshots/edu/berkeley/cs/chisel3_2.11/";
    std::string url2 = "https://oss.sonatype.org/content/repositories/snapshots/edu/berkeley/cs/chisel3_2.11/3.0-BETA-SNAPSHOT/maven-metadata.xml";
    std::string page;
    //getWebPage(url1, page);
    std::thread t1(getWebPage, url1, std::ref(page));
    std::unique_lock<std::mutex> lk(m);
    while (!is_ready) {
        cv.wait(lk);
    }
    t1.join();
    dirList(page);

    return 0;
}

