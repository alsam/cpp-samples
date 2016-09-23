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
#include <regex>
#include <vector>
#include <queue> // for keeping directories list
#include <thread>
#include <mutex>
#include <condition_variable>

#include <boost/algorithm/string/predicate.hpp>

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

    } catch(curlpp::RuntimeError & e) {
        std::cout << e.what() << std::endl;
    }

    catch(curlpp::LogicError & e) {
        std::cout << e.what() << std::endl;
    }
}

void
dirList(std::string const& html, std::vector<std::string>& attrs)
{
    std::string cleaned_xml{cleanhtml(html)};
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(cleaned_xml.c_str());
    auto table = doc.child("html").child("body").child("table");
    for (auto it = table.begin(); it != table.end(); ++it) {
        auto td = it->child("td").child("a");
        for (pugi::xml_attribute_iterator ait = td.attributes_begin(); ait != td.attributes_end(); ++ait) {
            attrs.emplace_back(ait->value());
        }
    }
}

void
parseManifest(std::string const& manifest_xml, std::vector<std::string>& attrs)
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(manifest_xml.c_str());

    if (result) {
        std::cout << "XML parsed without errors\n";
    } else {
        std::cout << "Error description: " << result.description() << "\n";
    }

    attrs.push_back(doc.child("metadata").child("version").child_value());
    attrs.push_back(doc.child("metadata").child("versioning").child("lastUpdated").child_value());
}

// convenience methods
bool
isDirectory(std::string const& path)
{
    return boost::ends_with(path, "/");
}

// is it ".." or "."
bool
isSpecialDir(std::string const& path)
{
    return boost::ends_with(path, "../")
        || boost::ends_with(path, "./");
}

bool
traverseDirectories(std::string const& url, std::vector<std::string>& flist)
{
    std::queue<std::string> directories;
    directories.push(url);
    while (!directories.empty()) {
        std::string dir = directories.front(), page;
        directories.pop();
        getWebPage(dir, std::ref(page));
        std::vector<std::string> dirlist;
        dirList(page, dirlist);
        for (std::string const& d : dirlist) {
            if (!isSpecialDir(d)) { // ignore ``..'' and ``.''
                if (isDirectory(d)) { // add to queue
                    directories.push(d);
                } else {
                    flist.push_back(d);
                }
            }
        }
    }
}

bool
extractManifestAttrs(std::string const& url, std::vector<std::string>& attrs)
{
    std::unique_lock<std::mutex> lk(m);
    std::vector<std::string> flist;

    // get file list after recursive directories traversal
    traverseDirectories(url, flist);
    std::for_each(flist.begin(), flist.end(),
            [](auto const& f){ std::cout << "f: " << f << std::endl; });

    // find manifest file from the file list
    std::regex manifest_exp(url+"(.+?)(metadata\\.xml)");
    auto manifest_it = std::find_if(flist.begin(), flist.end(),
            [=](std::string const& f)->bool{ return std::regex_match(f,manifest_exp); });

    // load manifest file and parse it
    std::string manifest;
    getWebPage(*manifest_it, std::ref(manifest));
    parseManifest(manifest, attrs);

    is_ready = true;
    cv.notify_one();
}

int main(int, char **)
{
    std::string url1 = "https://oss.sonatype.org/content/repositories/snapshots/edu/berkeley/cs/chisel3_2.11/";
    std::vector<std::string> manifest_ver_attrs;
    std::thread t1(extractManifestAttrs, url1, std::ref(manifest_ver_attrs));
    std::unique_lock<std::mutex> lk(m);
    cv.wait(lk, []{ return is_ready;} );
    t1.join();

    for (auto const& attr : manifest_ver_attrs) {
        std::cout << attr << std::endl;
    }

    return 0;
}

