/**
* \file
* The most simple example.
* install: `yaourt -S aur/libcurlpp`
* [curlpp examples](https://github.com/jpbarrette/curlpp/tree/master/examples)
* [curlpp download wnd write callback example](https://github.com/datacratic/curlpp/blob/master/examples/example11.cpp)
* [Link errors with curlpp](http://stackoverflow.com/questions/33420631/link-errors-with-curlpp)
*/

#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

// curlpp specific headers
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

// pugixml
#include <pugixml.hpp>


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
pull_one_url(std::string const& url)
{
    std::unique_lock<std::mutex> lk(m);
    try {
        // That's all that is needed to do cleanup of used resources (RAII style).
        curlpp::Cleanup myCleanup;

        // Our request to be sent.
        curlpp::Easy myRequest;

        // Set the URL.
        //myRequest.setOpt<Url>(url);
        myRequest.setOpt(new curlpp::options::Url(url));
	std::list<std::string> HeaderData; 
      	HeaderData.push_back("Content-Type: application/octet-stream"); 
        myRequest.setOpt(new cURLpp::Options::HttpHeader(HeaderData));

        // Send request and get a result.
        // By default the result goes to standard output.
        myRequest.perform();

	std::string xml;
        for (auto const& h : HeaderData) {
            xml += h;
            //std::cout << h << "\n";
        }

	std::cout << xml.c_str();
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_string(xml.c_str());
	
        if (result) {
            std::cout << "XML parsed without errors\n";
	} else {
            std::cout << "Error description: " << result.description() << "\n";
            std::cout << "Error offset: " << result.offset << " (error at [..." << (xml.c_str() + result.offset) << "]\n\n";
	}

        simple_walker walker;
        doc.traverse(walker);
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
    std::string Url = "https://oss.sonatype.org/content/repositories/snapshots/edu/berkeley/cs/chisel3_2.11/";
    //std::string Url = "https://oss.sonatype.org/content/repositories/snapshots/edu/berkeley/cs/chisel3_2.11/3.0-BETA-SNAPSHOT/maven-metadata.xml";
    std::thread t(pull_one_url, Url);
    std::unique_lock<std::mutex> lk(m);
    while (!is_ready) {
        cv.wait(lk);
    }
    t.join();

    return 0;
}
