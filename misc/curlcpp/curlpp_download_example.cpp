/**
* \file
* The most simple example.
* install: `yaourt -S aur/libcurlpp`
* [curlpp examples](https://github.com/jpbarrette/curlpp/tree/master/examples)
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


using namespace curlpp::options;

bool is_ready(false);
std::mutex m;
std::condition_variable cv;

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
        myRequest.setOpt<Url>("https://oss.sonatype.org/content/repositories/snapshots/edu/berkeley/cs/chisel3_2.11/");

        // Send request and get a result.
        // By default the result goes to standard output.
        myRequest.perform();
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
    std::thread t(pull_one_url, Url);
    std::unique_lock<std::mutex> lk(m);
    while (!is_ready) {
        cv.wait(lk);
    }
    t.join();

    return 0;
}
