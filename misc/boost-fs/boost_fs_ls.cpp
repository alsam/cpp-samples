#include <iostream>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/range.hpp>

std::vector<std::string>
get_file_list(std::string const& path)
{
    std::vector<std::string> files;
    if (!path.empty()) {
        boost::filesystem::path dir = ".";
        boost::filesystem::recursive_directory_iterator it(dir), end;

        for (auto& entry : boost::make_iterator_range(it, end))
            if (boost::filesystem::is_regular(entry))
                files.push_back(entry.path().native());
    }
    return files;
}

int main(int argc, char** argv)
{
    if (argc > 1) {
        auto file_list = get_file_list(argv[1]);
        for (auto const& f : file_list) {
            std::cout << "- " << f << std::endl;
        }
    }
}
