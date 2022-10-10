#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <mutex>
#include <boost/algorithm/string.hpp>

#include "cppjieba/Jieba.hpp"
#include "log.hpp"

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
        static void CutString(const std::string& input, std::vector<std::string>* res, const std::string& exp) {
            // 不合并分隔符
            boost::split(*res, input, boost::is_any_of(exp), boost::token_compress_on);
        }
    };


    // 定义字典的路径
    const char* const DICT_PATH = "./cppjieba/dict/jieba.dict.utf8";
    const char* const HMM_PATH = "./cppjieba/dict/hmm_model.utf8";
    const char* const USER_DICT_PATH = "./cppjieba/dict/user.dict.utf8";
    const char* const IDF_PATH = "./cppjieba/dict/idf.utf8";
    const char* const STOP_WORD_PATH = "./cppjieba/dict/stop_words.utf8";

    // // 消除停用词版本
    // // 由于停用词库只有一份，所以设计为单例模式，避免每次使用都要创建
    // class JiebaUtil 
    // {
    // private:
    //     cppjieba::Jieba jieba;
    //     // 单例
    //     static JiebaUtil* instance;
    //     static std::mutex mtx;
    //     // 停用词哈希表
    //     std::unordered_set<std::string> stop_words;

    //     JiebaUtil() :jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH)
    //     {}
    //     JiebaUtil(const JiebaUtil&) = delete;
    //     JiebaUtil& operator=(const JiebaUtil&) = delete; 

    //     void CutForSearchHelper(const std::string &sentence, std::vector<std::string> *words) {
    //         jieba.CutForSearch(sentence, *words);
    //         for (auto iter = words->begin(); iter != words->end(); ) {
    //             if (stop_words.find(*iter) != stop_words.end()) {
    //                 iter = words->erase(iter); // 更新迭代器，否则会产生迭代器失效的问题
    //             } else {
    //                 iter ++ ;
    //             }
    //         }
    //     }

    // public:
    //     static JiebaUtil* GetInstance() {
    //         if (nullptr == instance) {
    //             mtx.lock();
    //             if (nullptr == instance) {
    //                 instance = new JiebaUtil();
    //                 instance->InitJiebaUtil();
    //             }
    //             mtx.unlock();
    //         }
    //         return instance;
    //     }
    //     // 读入停用词词库，使用哈希表存储
    //     void InitJiebaUtil() {
    //         std::ifstream in(STOP_WORD_PATH);
    //         if (!in.is_open()) {
    //             LOG(ERROR, "stop word open fail");
    //             return;
    //         }

    //         // 因为停用词是按行存储的，所以可以按行读取
    //         std::string line;
    //         while (std::getline(in, line)) {
    //             stop_words.insert(line);
    //         }
            
    //         in.close();
    //     }

    //     // 接口不变，添加一个辅助接口，用来去掉停用词
    //     static void JiebaCutString(const std::string &sentence, std::vector<std::string> *words) {
    //         GetInstance()->CutForSearchHelper(sentence, words);
    //     }
    // };
    // JiebaUtil* JiebaUtil::instance = nullptr;
    // std::mutex JiebaUtil::mtx;


    // 没有消除停用词版本
    class JiebaUtil
    {
    private:
        static cppjieba::Jieba jieba;
    public:
        static void JiebaCutString(const std::string &sentence, std::vector<std::string> *words) {
            jieba.CutForSearch(sentence, *words);
        }
    };
    // 静态成员，类外初始化
    cppjieba::Jieba JiebaUtil::jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH);
}