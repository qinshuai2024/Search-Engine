#include <iostream>
#include <regex>
#include "cpp-httplib-0.7.15/httplib.h"
#include "searcher.hpp"

const std::string src_path = "data/output_html/raw.txt";
const std::string root_path = "./wwwroot";

int main()
{
    ns_searcher::Searcher searcher;
    searcher.InitSearcher(src_path);

    httplib::Server server; // 启动一个http服务
    server.set_base_dir(root_path.c_str());
    server.Get("/s", [&searcher](const httplib::Request& req, httplib::Response& res){
        if (!req.has_param("word")) {
            res.set_content("必须要有搜索关键字!", "text/plain; charset=utf-8");
            return;
        }
        std::string word = req.get_param_value("word");
        LOG(NORMAL, "用户搜索的: " + word);
        std::string json_string;
        searcher.Search(word, &json_string);
        res.set_content(json_string, "application/json");
    });
    LOG(NORMAL, "服务器启动成功...");
    server.listen("0.0.0.0", 8081);
    return 0;
}
