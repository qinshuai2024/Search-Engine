#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <boost/algorithm/string.hpp>

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
    class StringUtil
    {
    public:
        static bool CutString(const std::string& target, std::vector<std::string>* out, const std::string& exp) {
        
        }
    };
}