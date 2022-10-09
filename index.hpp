/*
    构建索引模块
*/
#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include "util.hpp"
#include "log.hpp"

namespace ns_index 
{
    struct DocInfo
    {
        std::string title;
        std::string content;
        std::string url;
        int64_t doc_id;
    };

    struct InvertedElem
    {
        std::string word;
        int64_t doc_id;
        int weight;
        InvertedElem(int w = 0) 
            :weight(w) 
        {}
    };

    typedef std::vector<InvertedElem> InvertedList;
    class Index 
    {
    private:
        // 正排索引，文档ID对应文档内容
        std::vector<DocInfo> forward_index; 
        // 倒排索引，文档关键字对应倒排拉链
        std::unordered_map<std::string, InvertedList> inverted_index; 
    public:
        Index()
        {}
        ~Index()
        {}
    public:
        // 构建正排和倒排索引
        bool BulidIndex(const std::string& src_path) {
            // src是存放清理过后的文档数据
            // 需要根据写入的分隔符，读出
            std::ifstream in(src_path, std::ios::in);
            if (!in.is_open()) {
                LOG(FATAL, src_path + " open failed");
                return false;
            }
            std::string line;
            while (std::getline(in, line)) {
                // 获取到一个文档，分别构建它的正排和倒排索引
                DocInfo* doc = BulidForwardIndex(line); 
                if (doc == nullptr) {
                    LOG(ERROR, "forward index bulid failed");
                    continue;
                }
                // 构建倒排索引
                if (!BulidInvertedIndex(*doc)) {
                    LOG(ERROR, "inverted index bulid failed");
                    continue;
                }
            }
            return true;
        }
        // 根据doc_id找到找到文档内容
        DocInfo* GetForwardIndex(int64_t id) {
            
        }
        // 更加关键字获得倒排拉链
        InvertedList* GetInvertedIndex(const std::string& word) {

        }
    private:
        // 构建正排索引
        DocInfo* BulidForwardIndex(const std::string& line) {

        }
        // 构建倒排索引
        bool BulidInvertedIndex(const DocInfo& doc) {

        }
    };
}