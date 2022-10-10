
#include <iostream>
#include "searcher.hpp"

const std::string src = "data/output_html/raw.txt";

int main()
{
    ns_searcher::Searcher searcher;
    searcher.InitSearcher(src);
    
    std::string query;
    std::string json_str;
    while (true) {
        std::cout << "Please Enter You Search Query# ";
        std::cin >> query;
        searcher.Search(query, &json_str);
        std::cout << json_str << std::endl;
    }

    return 0;
}