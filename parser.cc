/**
 * @brief 用于数据清洗，将去标签之后每个HTML的标题、内容和对应官方的URL
 *        用约定的分隔符存入文本文件中，方便后面建立索引
 */


#include <iostream>
#include <vector>
#include <boost/filesystem.hpp>
#include <fstream>

#include "util.hpp"
#include "log.hpp"

const std::string input_src = "data/input_html";
const std::string output_src = "data/output_html/raw.txt";

typedef struct DocInfo {
    std::string title;
    std::string content;
    std::string url;
}DocInfo_t;


bool EnumFile(const std::string& input_src, std::vector<std::string> *file_list);
bool ParseFile(const std::vector<std::string>& file_list, std::vector<DocInfo_t>* results);
bool SaveFile(const std::vector<DocInfo_t>& results, const std::string& output_path);

int main() 
{
    // 1. 找到每个HTML文件的路径，保存在数组中
    std::vector<std::string> file_list;
    if (!EnumFile(input_src, &file_list)) {
        LOG(FATAL, "fail to enum file");
        return 1;
    }

    // 2. 将每个HTML文件去标签之后的信息存入结构体，所有的信息使用数组存储
    std::vector<DocInfo_t> results;
    if (!ParseFile(file_list, &results)) {
        LOG(FATAL, "fail to parse file");
        return 2;
    }
    
    // 3. 将所有解析出来的信息保存到指定文件
    if (!SaveFile(results, output_src)) {
        LOG(FATAL, "fail to save file");
        return 3;
    }
    return 0;
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
        //std::cout << "Debug " << file_list->back() << std::endl; // test
    }

    return true;
}

static bool GetTitle(const std::string& total_info, std::string *title) {
    // 目的：<title>要提取的内容</title>
    int begin = total_info.find("<title>"); // 找到标签的起始位置
    if (begin == std::string::npos) {
        return false;
    }
    begin += std::string("<title>").size(); // 指向标题的第一个字符

    int end = total_info.find("</title>"); // 结束位置
    if (end == std::string::npos) {
        return false;
    }

    if (begin > end) return false;
    *title = total_info.substr(begin, end - begin);
    return true;
} 
static bool GetContent(const std::string& total_info, std::string *content) {
    // 读取所有标签中的内容
    // 使用一个状态机，标签状态和内容状态
    enum State {
        LABLE,
        CONTENT
    };
    State s = LABLE;
    // 由于html中 < 和 >是特殊字符，分别使用&lt;和&gt表示，所以标签之间不会出现<和>
    for (auto e : total_info) {
        switch (e)
        {
        case '<':
            s = LABLE;
            break;
        case '>':
            s = CONTENT;
            break;
        case '\n':
            *content += ' '; // 忽略换行，因为后面需要使用换行来区分不同的html
        default:
            if (s == CONTENT) {
                *content += e;
            }
            break;
        }
    }
    return true;
} 
static bool GetURL(const std::string& file_path, std::string *url) {
    std::string url_head = "https://www.boost.org/doc/libs/1_80_0/doc/html";
    std::string url_tail = file_path.substr(input_src.size()); // 除去本地目录的文件名
    // 拼接形成URL
    *url = url_head + url_tail;
    return true;
} 

//for debug
static void ShowDoc( const DocInfo_t &doc)
{
    std::cout << "title: " << doc.title << std::endl;
    std::cout << "content: " << doc.content << std::endl;
    std::cout << "url: " << doc.url << std::endl;
}

bool ParseFile(const std::vector<std::string>& file_list, std::vector<DocInfo_t>* results) {
    // 1. 遍历所有文件路径，读取文件，将所有内容放在字符串里面
    for (const auto& file_path : file_list) {
        
        std::string total_info;
        DocInfo_t file_info;
        if (!ns_util::FileUtil::ReadFile(file_path, &total_info)) {
            continue;
        }
        // std::cout << "debug" << std::endl;
        // 2. 解析文件内容
        // 2.1 获取标题
        if (!GetTitle(total_info, &file_info.title)) {
            continue;
        }
        // 2.2 获取body中的内容
        if (!GetContent(total_info, &file_info.content)) {
            continue;
        }
        // 2.3 获取文档在boost官网中的URL
        if (!GetURL(file_path, &file_info.url)) {
            continue;
        }
        // debug
        // ShowDoc(file_info);
        // break;
        // 3. 返回数据
        results->push_back(std::move(file_info));// file_info只在for循环内有效，使用右值引用，提高拷贝效率
    }
    return true;
}


bool SaveFile(const std::vector<DocInfo_t>& results, const std::string& output_path) {
    // 用 \3 作为一个文档数据之间的分割符
    // 用 \n 作为文档之间的分隔符，这样读取的时候方便
#define EXP '\3'
    // 以二进制的方式写入
    std::ofstream out(output_path.c_str(), std::ios::binary | std::ios::out);
    if (!out.is_open()){
        LOG(FATAL, output_path + " open fail");
        return false;
    }
    for (auto& doc : results) {
        std::string out_str;
        out_str += doc.title;
        out_str += EXP;
        out_str += doc.content;
        out_str += EXP;
        out_str += doc.url;
        out_str += '\n';

        out.write(out_str.c_str(), out_str.size());
    }
    out.close();
    return true;
}