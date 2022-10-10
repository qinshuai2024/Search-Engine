#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
// #include <boost/algorithm/string.hpp>
#include <jsoncpp/json/json.h>

int main() 
{
    // std::ofstream out("data/output_html/raw.txt", std::ios::binary | std::ios::out);
    // if (!out.is_open()) {
    //     std::cout << "fail" << std::endl;
    // }

    // std::string str = "abcd\3ef\3\3\3\3\3gh";
    // std::vector<std::string> sub;
    // boost::split(sub, str, boost::is_any_of("\3"), boost::token_compress_on);
    // for (auto e : sub) {
    //     std::cout << e << std::endl;
    // }

    // std::unordered_map<int, std::string> map;
    // std::cout << map[1] << std::endl;

    // jsoncpp的使用
    // 创建一个json对象
    // Json::Value root;
    // root["abcd"] = "dcba";
    // root["1234"] = "4321";

    // Json::FastWriter writer;
    // // 序列化，返回一个字符串
    // std::string json_str = writer.write(root);
    // std::cout << json_str << std::endl;

    // Json::Value read_root;
    // Json::Reader reader;
    // reader.parse(json_str, read_root);
    // std::cout << "abcd: " <<  root["abcd"] << " 1234: " << root["1234"] << std::endl;
    // std::string json_str = R"({"1234":"4321","abcd":"dcba"})";
    // Json::Value root;
    // Json::Reader reader;
    // // 解析一个json字符串，并放在root里面
    // reader.parse(json_str, root);
    // std::cout << "abcd: " <<  root["abcd"] << " 1234: " << root["1234"] << std::endl;
    std::string json_str = R"({"1234":"4321","abcd":"dcba"})";
    std::cout << json_str << std::endl;
    std::string str = R"+*(abc()bd())+*";
    std::cout << str << std::endl;

    return 0;
}