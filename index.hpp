/*
    构建索引模块
*/
#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>

#include "util.hpp"
#include "log.hpp"

namespace ns_index 
{
    struct DocInfo
    {
        std::string title;
        std::string content;
        std::string url;
        uint64_t doc_id;
    };

    struct InvertedElem
    {
        std::string word;
        uint64_t doc_id;
        int weight;
        InvertedElem(int w = 0) 
            :weight(w) 
        {}
    };
    // 倒排拉链
    typedef std::vector<InvertedElem> InvertedList;

    class Index 
    {

    private:
        // 单例
        static Index* instance;
        // 互斥锁
        static std::mutex mtx;

        // 构造函数需要私有，并且显式实现
        Index() {};
        // 拷贝构造、赋值构造直接删除，不可使用
        Index(const Index&) = delete;
        Index& operator=(const Index&) = delete;
    public:
        static Index* GetInstance() {
            // 使用双重判断减少竞争锁的次数
            if (nullptr == instance) {
                mtx.lock();
                if (nullptr == instance) {
                    instance = new Index();
                }
                mtx.unlock();
            }
            return instance;
        }
        ~Index()
        {}
    private:
        // 正排索引，文档ID对应文档内容
        std::vector<DocInfo> forward_index; 
        // 倒排索引，文档关键字对应倒排拉链
        std::unordered_map<std::string, InvertedList> inverted_index;
    public:
        // 构建正排和倒排索引
        bool BuildIndex(const std::string& src_path) {
            // src是存放清理过后的文档数据
            // 需要根据写入的分隔符，读出
            std::ifstream in(src_path, std::ios::in);
            if (!in.is_open()) {
                LOG(FATAL, src_path + " open failed");
                return false;
            }
            std::string line;
            // debug
            int count = 0;
            while (std::getline(in, line)) {
                // 获取到一个文档，分别构建它的正排和倒排索引
                DocInfo* doc = BuildForwardIndex(line); 
                if (doc == nullptr) {
                    LOG(ERROR, "forward index build failed");
                    continue;
                }
                // 构建倒排索引
                if (!BuildInvertedIndex(*doc)) {
                    LOG(ERROR, "inverted index build failed");
                    continue;
                }
                count ++ ;
                if (count % 50 == 0) {
                    LOG(NORMAL, "当前已经建立索引文档：" + std::to_string(count));
                }
                // if (count == 3000) break;
            }
            in.close();
            return true;
        }
        // 根据doc_id找到找到文档内容
        DocInfo* GetForwardIndex(uint64_t id) {
            if (id >= forward_index.size()) {
                LOG(DEBUG, "illegal id");
                return nullptr;
            }
            return &(forward_index[id]);
        }
        // 更加关键字获得倒排拉链
        InvertedList* GetInvertedIndex(const std::string& word) {
            auto iter = inverted_index.find(word);
            if (iter == inverted_index.end()) {
                LOG(DEBUG, "no inveted list");
                return nullptr;
            }
            // return &(inverted_index[word]);
            return &(iter->second);
        }
    private:
        // 构建正排索引
        DocInfo* BuildForwardIndex(const std::string& line) {
            // 1. 切分字符串
            std::vector<std::string> res;
            const std::string exp = "\3";
            ns_util::StringUtil::CutString(line, &res, exp);
            if (res.size() != 3) {
                LOG(ERROR, "cut string error");
                return nullptr;
            }
            // 2. 组合形成DocInfo
            DocInfo doc;
            doc.title = res[0];
            doc.content = res[1];
            doc.url = res[2];
            doc.doc_id = forward_index.size(); // 这是将要插入的位置
            // 3. 插入正排序列
            forward_index.push_back(std::move(doc));
            return &(forward_index.back()); // 返回刚插入的地址
        }
        // 构建倒排索引 关键字 -> 倒排拉链
        bool BuildInvertedIndex(const DocInfo& doc) {
            // 词频结构体
            struct word_cnt 
            {
                int title_cnt;
                int content_cnt;
                word_cnt() : title_cnt(0), content_cnt(0)
                {}
            };
            std::unordered_map<std::string, word_cnt> word_hash;
            // 1. 对title分词
            std::vector<std::string> words_title;
            ns_util::JiebaUtil::JiebaCutString(doc.title, &words_title);
            // title词频统计
            for (auto& s : words_title) {
                boost::to_lower(s); // 所有词转换为小写，也就是不区分大小写，搜索更加准确
                word_hash[s].title_cnt++;
            }

            // 2. 对content分词
            std::vector<std::string> words_content;
            ns_util::JiebaUtil::JiebaCutString(doc.content, &words_content);
            // content词频统计
            for (auto& s : words_content) {
                boost::to_lower(s);
                word_hash[s].content_cnt++;
            }

            // 3. 插入倒排索引
            // 需要处于不同位置的关键词添加不同权权重
            #define TITLE_WEIGHT 10
            #define CONTENT_WEIGHT 1

            for (auto& word_pair : word_hash) {
                InvertedElem invE;
                invE.word = word_pair.first;
                invE.doc_id = doc.doc_id;
                invE.weight = word_pair.second.title_cnt * TITLE_WEIGHT + word_pair.second.content_cnt * CONTENT_WEIGHT;
                inverted_index[word_pair.first].push_back(std::move(invE));
            }
            return true;
        }
    };
    // 类外初始静态成员
    Index* Index::instance = nullptr;
    std::mutex Index::mtx;
}
