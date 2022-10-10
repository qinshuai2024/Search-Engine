// 提供搜索服务，包括建立索引，返回查找结果
#pragma once

#include <unordered_map>
#include <algorithm>
#include <map>
#include <jsoncpp/json/json.h>

#include "index.hpp"
#include "log.hpp"
#include "util.hpp"



namespace ns_searcher
{
    // 倒排找到对应文档ID，同时存储ID和对应的多个关键字（搜索字符串拆分而来）
    struct InvertedWords
    {
        uint64_t doc_id;
        int weight;
        std::vector<std::string> words;
        InvertedWords(): doc_id(0), weight(0) {}
    };

    class Searcher
    {
    private:
        ns_index::Index* index;
    public:
        Searcher()
        {}
        ~Searcher()
        {}
    public:
        // input是需要建立索引的文件路径
        void InitSearcher(const std::string& input) {
            // 1. 获取索引单例
            index = ns_index::Index::GetInstance();
            LOG(NORMAL, "get index instance successfully");
            // 2. 构建索引
            index->BuildIndex(input);
            LOG(NORMAL, "build successfully");
        }
        // query为用户输入的关键字
        // json_str是返回用户的json字符串，用于网络传输
        void Search(const std::string& query, std::string *json_str) {
            // 1. 对查询的字符串分词
            std::vector<std::string> words;
            ns_util::JiebaUtil::JiebaCutString(query, &words);

            // 2. 对每个词查找倒排拉链，并组合

            // 搜索字符串分割之后对应总的文档数据
            std::vector<InvertedWords> inverted_list_all; 

            // 文档ID和结构体的映射
            std::unordered_map<uint64_t, InvertedWords> tokens_map;
            for (auto& word : words) {
                boost::to_lower(word); // 因为建立索引也是全部转小写
                ns_index::InvertedList* inv_list = index->GetInvertedIndex(word);
                if (inv_list == nullptr) {
                    LOG(ERROR, "None");
                    return;
                }
                for (auto& inv_e : *inv_list) {
                    uint64_t id = inv_e.doc_id;
                    tokens_map[id].doc_id = inv_e.doc_id;
                    // 追加存在该文档中的关键字
                    tokens_map[id].words.push_back(word); 
                    // 多个关键字出现在一个文档，权重相加
                    tokens_map[id].weight += inv_e.weight;
                }
            }
            // 将搜索字符串对应的文档，插入总的数组中
            for (auto& item : tokens_map) {
                inverted_list_all.push_back(std::move(item.second));
            }
            // 3. 根据相关性(weight)对拉链排序
            std::sort(inverted_list_all.begin(), inverted_list_all.end(), \
            [](const InvertedWords& w1, const InvertedWords& w2){
                return w1.weight > w2.weight; // 降序
            });
            // 4. 形成json字符串（序列化）返回
            // 根据正排拿到文档内容

            Json::Value root;
            for (auto& item : inverted_list_all) {
                ns_index::DocInfo* pdoc = index->GetForwardIndex(item.doc_id); 
                if (pdoc == nullptr) {
                    continue;
                }
                // 序列化
                Json::Value elem;
                elem["title"] = pdoc->title;
                elem["content"] = GetDesc(item.words[0], pdoc->content);
                elem["url"] = pdoc->url;
                elem["id"] = (long long)pdoc->doc_id;
                elem["weight"] = item.weight;

                root.append(elem);  
            }
            Json::StyledWriter writer;
            *json_str = writer.write(root);
        }
    private:
        // 获取描述字段
        std::string GetDesc(const std::string& word, const std::string& content) {
            // 1. 找到该关键字首次出现的位置
            // int pos = content.find(word); 会有BUG，因为此时word是全小写，find是区分大小写的，找不全
            auto iter = std::search(content.begin(), content.end(), word.begin(), word.end(), \
            [](char a, char b){
                return std::tolower(a) == std::tolower(b);
            });
            if (content.end() == iter) {
                return "None"; // 查找失败
            }
            int pos = std::distance(content.begin(), iter);
            // 2. 截取一部分，用于展示
            const int prev_step = 50; // 向前截取的字节数
            const int next_step = 100; // 向后截取的字节数

            int begin, end;
            if ((begin = pos - prev_step) < 0) {
                begin = 0;
            } 
            if ((end = pos + next_step) >= content.size() ) {
                end = content.size() - 1;
            }
            // 3. 返回切分字符串
            std::string desc = content.substr(begin, end - begin);
            desc += "...";
            return desc;
        }
    };
}