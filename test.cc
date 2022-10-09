#include <iostream>
#include <fstream>

int main() 
{
    std::ofstream out("data/output_html/raw.txt", std::ios::binary | std::ios::out);
    if (!out.is_open()) {
        std::cout << "fail" << std::endl;
    }
    return 0;
}