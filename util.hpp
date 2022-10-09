#pragma once
#include <iostream>
#include <string>
#include <fstream>

namespace ns_util {
    class FileUtil 
    {
    public:
        static bool ReadFile(const std::string& file_path, std::string *out) {
            std::ifstream in(file_path, std::ios::in);
            if (!in.is_open()) {
                
                return false;
            }
            std::string line;
            while (std::getline(in, line)) {
                *out += line;
            }
            in.close();
            return true;
        }
    };
}