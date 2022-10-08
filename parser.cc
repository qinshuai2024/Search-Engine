/**
 * @brief 用于数据清洗，将去标签之后每个HTML的标题、内容和对应官方的URL
 *        用约定的分隔符存入文本文件中，方便后面建立索引
 */


#include <iostream>
#include <vector>
#include <boost/filesystem.hpp>

const std::string input_src = "data/input_html";
const std::string output_src = "data/ouput_html/raw.txt";

typedef struct DocInfo {
    std::string title;
    std::string content;
    std::string url;
}DocInfo_t;


bool EnumFile(const std::string& input_src, std::vector<std::string> *file_list);
bool ParseFile(const std::vector<std::string>& file_list, std::vector<DocInfo_t>* results);
bool SaveFile(const std::vector<DocInfo_t>& results, const std::string& output_src);

int main() 
{
    // 1. 找到每个HTML文件的路径，保存在数组中
    std::vector<std::string> file_list;
    if (!EnumFile(input_src, &file_list)) {
        return 1;
    }

    // 2. 将每个HTML文件去标签之后的信息存入结构体，所有的信息使用数组存储
    std::vector<DocInfo_t> results;
    if (!ParseFile(file_list, &results)) {
        return 2;
    }

    // 3. 将所有解析出来的信息保存到指定文件
    if (!SaveFile(results, output_src)) {
        return 3;
    }
}

bool EnumFile(const std::string& input_src, std::vector<std::string> *file_list) {
    namespace fs = boost::filesystem;
    fs::path root_path(input_src); // 定义一个path对象，路径为input_src
    if (!fs::exists(root_path)) {
        return false;
    }

    // 递归遍历目录，定义一个空的迭代器，用来判断结尾
    fs::recursive_directory_iterator end;
    for (fs::recursive_directory_iterator iter(root_path); iter != end; iter ++ ) {
        if (!fs::is_regular_file(*iter)) {
            // 不是一个常规文件，不保存路径
            continue;
        }
        if (iter->path().extension() != ".html") {
            // 后缀不是html的文件不要
            continue;
        }
        // 放入数组中
        file_list->push_back(iter->path().string());
        std::cout << "Debug " << file_list->back() << std::endl; // test
    }

    return true;
}


bool ParseFile(const std::vector<std::string>& file_list, std::vector<DocInfo_t>* results){
    return true;
}


bool SaveFile(const std::vector<DocInfo_t>& results, const std::string& output_src){
    return true;
}