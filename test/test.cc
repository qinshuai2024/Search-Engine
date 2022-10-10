#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>

int main() 
{
    // std::ofstream out("data/output_html/raw.txt", std::ios::binary | std::ios::out);
    // if (!out.is_open()) {
    //     std::cout << "fail" << std::endl;
    // }
    std::string str = "abcd\3ef\3\3\3\3\3gh";
    std::vector<std::string> sub;
    boost::split(sub, str, boost::is_any_of("\3"), boost::token_compress_on);
    for (auto e : sub) {
        std::cout << e << std::endl;
    }

    return 0;
}