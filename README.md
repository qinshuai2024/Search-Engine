# 一、项目相关背景

- boost官网没有站内搜索，用起来比较麻烦。

- 模仿百度等搜索引擎，利用boost提供的文档，构建一个搜索boost库的搜索引擎。

![image-20220926093454188](https://s2.loli.net/2022/09/26/eBQOlgSHEij6uY5.png)



![image-20220926093741337](https://s2.loli.net/2022/09/26/NOvyg6ClopxLAKr.png)

# 二、项目基本原理

![image-20220926093105547](https://s2.loli.net/2022/09/26/OxcyE5jvTILCPZ4.png)

# 三、项目技术栈和环境

技术栈：

- 后端： `C/C++ C++11、 STL、 准标准库Boost、Jsoncpp、cppjieba、cpp-httplib`
- 前端：`html、css、js、jQuery、Ajax`

项目环境：

- `Centos 7、g++/Makefile、VS Code`

# 四、正排、倒排索引

## 4.1 正排索引

- 正排索引：就是从文档ID找到文档内容(文档内的关键字)，是**一对一**的关系。

举例文档：

> 文档1 ： 南京哪里可以做核酸
>
> 文档2 ： 核酸检测地



| 文档ID | 文档内容           |
| :----- | ------------------ |
| 1      | 南京哪里可以做核酸 |
|2| 核酸检测地 |





## 4.2 倒排索引

- 倒排索引：就是根据某些关键字，找到对应的文档ID，是**一对多**的关系。

在构建倒排索引之前，需要对文档进行分词，确定文档有哪些关键词。

> 停止词：了，的，吗，a，the，一般我们在分词的时候可以不考虑

例如：

> 文档1 ： 南京/哪里/可以/做/做核酸

> 文档2 ： 核酸/核酸检测/检测

| 关键字     | 文档ID |
| ---------- | ------ |
| 南京       | 1      |
| 核酸       | 1，2   |
| 核酸检测地 | 2      |
| 哪里       | 1      |
| 检测       | 2      |

模拟一次搜索的过程：

> 用户输入"核酸" -> 根据倒排索引 -> 获得文档ID（1,2）-> 根据正排索引 -> 得到完整的文档内容 
>
> -> 构建文档摘要 -> 返回摘要构成的网页 -> 用户点击 -> 进入对应网页

# 五、对HTML文件清洗并形成摘要



首先要下载`boost`库提供的官方文档，里面有`HTML`文件。

> boost 官网： https://www.boost.org/
> 只需要`boost_1_80_0/doc/html`目录下的`html`文件，用它来进行建立索引



## 5.1 清洗目标和过程

去标签就是把文档内容，从`HTML`网页中提取出来，为后面形成摘要，分词做准备。


去标签的代码放在 `parser.cc`中，其中

- 原始`HTML`文件放在`./data/input_html`。
- 清洗过后的内容放在 `./data/ouput_html/raw.txt`里面。

对于每个`HTML`文件，经过去标签后需要获得对于的`title, content, url`，所以需要创建一个结构体用来表示。

```c++
typedef struct DocInfo{
    std::string title;   // 文档标题
    std::string content; // 文档内容
    std::string url;     // 官网路径
}DocInfo_t;
```

将一个`HTML`文件清理出来需要经过下面三个步骤

1. 找到每个`HTML`文件的路径。
2. 对`HTML`去标签，填充结构体。
3. 将得到的数据，按照约定分隔符写入文件。

由这三步可以完成解析的基本框架。

对于C++中的传参，我们习惯于用下面三种方式加以限制，方便参数的传入传出。

```c++
const & : 作为输入参数，不可修改
& : 输入输出参数
* : 输出参数
```

## 5.2 使用boost库的文件操作

由于C++对文件操作的支持不好，所以使用boost库的文件操作来完成对目录中HTML文件的枚举。

- 安装boost开发库 `sudo yum install -y boost-devel`
- 打开文件`vim /usr/include/boost/version.hpp`，查看安装的boost库的版本

![image-20220928190735734](https://s2.loli.net/2022/09/28/BlztsHTd2ZcWmSg.png)

这样就可以上官网查询指定版本的文档了。[boost库文件相关文档](https://www.boost.org/doc/libs/1_53_0/libs/filesystem/doc/reference.html)

需要使用的主要对象和函数：

1. 首先要使用一个对象`path`来保存目录的路径。
2. 全局函数`bool exists(const path& p);`可以用来判断路径是否存在。
3. 使用`recursive_directory_iterator`目录递归迭代器，可以递归的遍历`path`对象目录下的文件或者目录。

使用boost库注意事项：

- 使用boost库文件类型需要包含头文件`boost/filesystem.hpp`。
- 编译需要链接动态库，一个系统库一个文件库，命令加上`-lboost-system -lboost-filesystem` 让编译器找到对应库名。

## 5.3 HTML文件信息的提取

1. 遍历所有文件路径，并读取文件，将一个文件所有内容放在一个字符串里面

2. 解析`HTML`有效内容

   - 提取`title`

     > 主要思路：目的是标签`<title>`  和 `</title>`之间的内容，那么使用`find`找到标题的首尾位置即可。

   - 提取`content`

     > 主要思路：目的是提取所有标签中的内容，这里需要创建是一个状态机，具有两个状态`LABLE`和`CONTEN`，表示当前处于标签中还是文本内容中，由于`html`中 `< `和 `>`是特殊字符，分别使用`&lt;`和`&gt;`表示，所以标签之间不会出现`<`和`>`，可以用来区分状态。

   - 生成`url`

     ```C++
     官网URL样例：       https://www.boost.org/doc/libs/1_80_0/doc/html/accumulators.html
     下载的文档中url样例：boost_1_80_0/doc/html/accumulators.html
     我们拷贝到我们项目中的样例：data/input_html/accumulators.html 
     url_head = "https://www.boost.org/doc/libs/1_80_0/doc/html";
     url_tail = [data/input_html](删除) /accumulators.html -> url_tail = /accumulators.html
     url = url_head + url_tail ; 相当于形成了一个官网链接
     ```


3. 将解析出来的内容写入文件中
   加入数组即可。

## 5.4  将解析内容写入文件中

因为后面需要使用到文档内容，所以存放去标签的文档内容的时候需要使用约定的分隔符，将不同文档，同一个文档的不同部分分隔开。

- 对于同一个文档的标题和内容用 `\3` （3在`ASCII`表中对应文本结束，是不可以显示字符）分隔。
- 对于不同文档之间用 `\n` 分隔，方便后面对单个文档的读取。
- 不仅要把文档标题、内容去标签后存入文件，还有将`HTML`文档在`boost`官网的`URL`存入，方便后面设置超链接直接跳转。

> 形如：`title\3content\3url \n title\3content\3url \n title\3content\3url \n ...`

# 六、建立索引

## 6.1 建立正排索引

正排索引是文档ID对应文档内容，是一对一的关系，对应的数据结构可以使用数组，下标充当ID，存放的内容就是文档内容。

对于文档内容可以使用如下结构体描述
```c++
// 正排 通过文档ID找到文件内容
struct DocInfo
{
    std::string title;
    std::string content;
    std::string url;
    uint64_t doc_id;
};
```

```c++
std::vector<DocInfo> forward_index; // 正排数组，下标即文档ID
```

重要接口：

```c++
// 根据doc_id找到文档内容 --- 正排
DofInfo* GetForwardIndex(int64_t id);
// 对每个html内容（一行）建立正排索引
DocInfo* BuildForwardIndex(const std::string& line);
```

需要读取之前使用特定分隔符分隔的数据
使用`getline`可以读取每一行，也就是一个`HTML`的清洗内容。
使用`boost`库中的`split`函数可以按照指定分隔符，分隔字符串。

```c++
// 库中split函数的定义
template< typename SequenceSequenceT, typename RangeT, typename PredicateT >
inline SequenceSequenceT& split(
    SequenceSequenceT& Result,
    RangeT& Input,
    PredicateT Pred,
    token_compress_mode_type eCompress=token_compress_off )
```

参数解释：

- `Result`是切割生成的子串
- `Input`是要切割的字符串
- `Pred`是切割的分隔符，需要使用boost给定的函数指定分隔符

```c++
以多个字符中的任意一个分割（is_any_of）: boost::is_any_of("-*")
```

- `eCompress`用来设置相邻分隔符是否合并
  例如对字符串 `abcd\3ef\3\3\3\3\3gh` 用`\3`分隔

```c++
std::string str = "abcd\3ef\3\3\3\3\3gh";
std::vector<std::string> sub;
boost::split(sub, str, boost::is_any_of("\3")); // 默认
boost::split(sub, str, boost::is_any_of("\3"), boost::token_compress_on); // 合并多个连续的分隔符
```

​	值为`token_compress_off`（默认）：`[abcd, ef, , , , , gh] `
​	值为`token_compress_on`：`[abcd, ef, gh]`

- 返回值是`Result`的引用





## 6.2 建立倒排索引

倒排索引是一个关键字对应多个文档ID，是一对多的关系，为了提高效率，可以使用哈希表来存放这种对应关系。

对于关键字对应的每一个文档可以使用如下结构描述
```c++
// 倒排基本元素 通过关键字找到文档ID
struct InvertedElem
{
    std::string word; // 对应关键字
    uint64_t doc_id;  // 对应文档ID
    int weight;       // 关键字在文档中的权值
    InvertedElem(int w = 0)
        :weight(w)
    {}
};
```

多个这个的结构体可以使用数组来保存，构成倒排拉链

```c++
// 通过一个关键字可以找到多个文档ID，多个文档形成倒排拉链
typedef std::vector<InvertedElem> InvertedList; // 重定义类型，增强可读性
// 倒排索引，文档关键字对应倒排拉链，使用hash表建立一对多的映射关系
std::unordered_map<std::string, InvertedList> inverted_index; 
```

重要接口：

```C++
// 根据关键字得到倒排拉链 --- 倒排
InvertedList* GetInvertedIndex(const std::string& word);
// 根据doc中的内容，分词统计，插入hash表，建立每个关键词的倒排索引
bool BuildInvertedIndex(const DocInfo& doc);
```

**建立倒排步骤：**

1. 对`title/content`分词，使用`jieba`分词。
   我想上清华大学：我 / 想 / 上 / 清华 / 华大 / 大学 / 清华大学

2. 词频统计

   标题中的词和内容中的词的重要程度是不一样的，所以需要分别统计标题词和内容词的数量。
   ```c++
   struct word_cnt {
       int title_cnt;
       int content_cnt;
   }
   // 为每个分词和对应出现次数建立映射关系
   std::unordered_map<std::string, word_cnt> word_hash;
   ```

3. 将分词以及对应的`InvertedElem`插入倒排索引



## 6.3 jieba库的安装和分词

[下载`jcppieba`库地址](https://github.com/yanyiwu/cppjieba)

`cppjieba`是提供头文件的，而本次使用的搜索分词，只需要使用里面的函数`CutForSearch`，只需要拷贝三个文件即可使用：

> ` cppjieba/include/cppjieba/ 里面的头文件`
>
> ``cppjieba/dict/ 里面的分割词库`
>
> `cppjieba/deps/limonp/ `目录下的一些文件

具体使用可以参考`cppjieba/test`里面的示例代码，如`demo.cpp`。

本次只需只用分割词库和函数`CutForSearch`。
```c++
// 定义字典的路径，根据实际路径填写
const char* const DICT_PATH = "./dict/jieba.dict.utf8";
const char* const HMM_PATH = "./dict/hmm_model.utf8";
const char* const USER_DICT_PATH = "./dict/user.dict.utf8";
const char* const IDF_PATH = "./dict/idf.utf8";
const char* const STOP_WORD_PATH = "./dict/stop_words.utf8";
// 使用路径初始一个jieba对象
cppjieba::Jieba jieba(DICT_PATH,
        HMM_PATH,
        USER_DICT_PATH,
        IDF_PATH,
        STOP_WORD_PATH);
```

```c++
void CutForSearch(const string& sentence, vector<string>& words, bool hmm = true) const {
	query_seg_.Cut(sentence, words, hmm);
}
```

- `sentence`是要分割的字符串
- `words`是分割后的词语组成的数组
- `HMM`是一种分词方法，默认使用即可

## 6.4 索引模块的单例模式

因为搜索的内容是固定的，所以只用建立一次索引，而且全局使用的都是同一个，所以需要将索引模块构建为单例模式。

单例实现常见方式：
>饿汉方式：首先创建一个对象，用到的时候直接使用
>
>懒汉模式：创建一个对象的指针，用到的时候再去创建对象，并给指针赋值

懒汉方式的特点：延时加载

用懒汉方式实现单例需要注意的几个点：

- 定义一个静态的本类的指针对象
- 构造函数必须私有，并且实现，拷贝构造和赋值构造需要删除
- 编写一个静态的函数，获取实例，但存在线程安全问题，所以需要加锁保护
- 每次使用之前都要获取锁，是比较麻烦的，由于只需要创建一次对象，所以可以使用两重判断指针是否为空。若一开始就不是空，就不用竞争锁了；若是空指针，再竞争锁，然后判断是否为空，为空就创建对象，否则释放锁退出。

```c++
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
```

## 6.5 消除停用词版本

停用词：出现非常普遍，但是不具有实际意义的词，比如中文的`'么'、'的'、'但'`，英语的`'the'、'is'、'at'、'which'、'on'`。

适当地减少`Stop Words`出现的频率，可以有效地帮助我们提高关键词密度，而在网页`Title`中避免出现`Stop Words`往往能够让我们优化的关键词更突出。

`jieba`分词里面有停用词词库，但由于只有一份且通用，所以将`JiebaUtil`设计为单例模式，避免每次使用都要将停用词库从磁盘读入内存。

算法核心：

1. 将停用词库读入内存，用哈希表存储。
2. 遍历分词后的每一个词，在哈希表中查找，如果是停用词，就将其从分词数组中删除。

但是总体来看这非常影响效率，遍历数组，删除数组元素，效率都是很低的。

# 七、搜索服务

## 7.1 搜索服务的基本框架 

1. 需要根据源文档构建索引
2. 根据搜索关键字，通过倒排，找到对应文档ID
3. 根据ID通过正排索引，找到文档信息，并构成一个`json`对象
4. 所有的文档信息构成所有`json`组合成一个大`json`对象，然后序列化成字符串返回

## 7.2 `jsoncpp`的安装和使用

安装命令：`sudo yum install -y jsoncpp-devel`

`json`是一种键值对的存储关系，这里只会使用到`json`的序列化和反序列化，使用很简单。
需要使用头文件：`jsoncpp/json/json.h`
需要链接动态库：`jsoncpp`

使用`json`进行序列化：

```c++
// 创建一个json对象
Json::Value root;
// 存入信息
root["abcd"] = "dcba";
root["1234"] = "4321";

Json::FastWriter writer; // 还有一个 Json::StyledWriter 只是不同
// 序列化，返回一个字符串
std::string json_str = writer.write(root);
std::cout << json_str << std::endl;

[sqin@VM-16-8-centos test]$ g++ test.cc -std=c++11 -ljsoncpp
[sqin@VM-16-8-centos test]$ ./a.out 
{"1234":"4321","abcd":"dcba"}
```

使用`json`进行反序列化：

```c++

std::string json_str = R"({"1234":"4321","abcd":"dcba"})";
Json::Value root;
Json::Reader reader;
// 解析一个json字符串，并放在root里面
reader.parse(json_str, root);
// 可以直接使用[]取出信息
std::cout << "abcd: " <<  root["abcd"] << " 1234: " << root["1234"] << std::endl;

[sqin@VM-16-8-centos test]$ g++ test.cc -std=c++11 -ljsoncpp
[sqin@VM-16-8-centos test]$ ./a.out 
abcd: "dcba" 1234: "4321"
```

C++新语法，不转义显示原始字符串：`R"(要显示的字符串)"`
如果要显示的字符串中有要输出括号： `R"+*(要显示的字符串)+*"`

```c++
std::string json_str = R"({"1234":"4321","abcd":"dcba"})";
std::cout << json_str << std::endl;
std::string str = R"+*(abc()bd())+*";
std::cout << str << std::endl;

[sqin@VM-16-8-centos test]$ g++ test.cc -std=c++11
[sqin@VM-16-8-centos test]$ ./a.out 
{"1234":"4321","abcd":"dcba"}
abc()bd()
```

## 7.3 搜索服务流程

主要函数：

```c++
// query为用户输入的关键字
// json_str是返回用户的json字符串，用于网络传输
void Search(const std::string& query, std::string *json_str);
```

1. 先将用户输入的字符串进行分词。

2. 根据分词查找倒排索引。

   > 注意：可能存在多个分词对应同一片文章，所以查出来的文档ID可能会重复，需要去重，但是权值是要相加的。
   >
   > 引入一个结构体，里面包括文档ID对应的所有分词。
   >
   > ```C++
   > // 倒排找到对应文档ID，同时存储ID和对应的多个关键字（搜索字符串拆分而来）
   > struct InvertedWords
   > {
   >     uint64_t doc_id;
   >     int weight;
   >     std::vector<std::string> words;
   >     InvertedWords(): id(0), weight(0) {}
   > };
   > ```
   >
   > 为了去重，需要建立一个文档ID和该结构体的对应关系，使得一个文档只保存一次。
   >
   > ```C++
   > // 搜索字符串分割之后对应总的文档数据
   > std::vector<InvertedWords> inverted_list_all; 
   > // 文档ID和结构体的映射
   > std::unordered_map<uint64_t, InvertedWords> tokens_map;
   > ```

3. 根据相关性(`weight`)对总的文档排序
   
4. 形成`json`字符串返回

   ![image-20221010192727114](https://s2.loli.net/2022/10/10/Mir4IfCHzgNBLKX.png)
   实现同一般搜索，会显示搜索关键字周围的部分描述，最后还有省略号。

## 7.4 测试

```c++
#include <iostream>
#include "searcher.hpp"

const std::string src = "data/output_html/raw.txt";

int main()
{
    ns_searcher::Searcher searcher;
    searcher.InitSearcher(src);
    
    std::string query;
    std::string json_str;
    while (true) {
        std::cout << "Please Enter You Search Query# ";
        std::cin >> query;
        searcher.Search(query, &json_str);
        std::cout << json_str << std::endl;
    }

    return 0;
}
```

![image-20221010203902081](https://s2.loli.net/2022/10/10/24XsqH31vYBwgfL.png)

# 八、网络服务

使用`cpp-httplib`库进行网络传输，但这个库需要使用较高版本的`gcc`

## 8.1 升级`gcc`

- `gcc -v` ：查看`gcc`版本，建议升级到7以上。

- `sudo yum install centos-release-scl scl-utils-build` ：安装`scl(Software Collections)`，是`CentOS/RHEL Linux`平台的软件多版本共存解决方案，可以用来安装新版本的`gcc`。

- `sudo yum install -y devtoolset-7-gcc devtoolset-7-gcc-c++` ：安装新版本的`gcc`，其中数字7表示安装`gcc`的版本为7，也可以安装现在最新的12

-  `ls /opt/rh/` ：查看安装哪些版本的`gcc`
  ```c++
  [sqin@VM-16-8-centos search_engine]$ ls /opt/rh/
  devtoolset-7  devtoolset-9
  ```

- `scl enable devtoolset-7 bash` ：启用对应版本的`gcc`，但是只在本次回话有效。

- `~/.bash_profile` ：将命令写入该文件即可
  ![image-20221010205512067](https://s2.loli.net/2022/10/10/48muV5xayqcIz7R.png)



## 8.2 安装 和使用`cpp-httplib`

[压缩包下载地址，版本0.7.15](https://github.com/yhirose/cpp-httplib/releases/tag/v0.7.15)，版本越高，对`gcc`的版本要求越高。

简单使用：

```c++
#include <iostream>
#include "cpp-httplib-0.7.15/httplib.h"

const std::string root_path = "./root";

int main()
{
    httplib::Server server; // 启动一个http服务
    server.set_base_dir(root_path.c_str()); // 设置网页根目录
    // httplib::Server & Get(const char *pattern, httplib::Server::Handler handler)
    // pattern : 资源路径    handler : 回调函数，访问的时候执行
    server.Get("/s", [](const httplib::Request& req, httplib::Response& res){
        // void set_content(const std::string &s, const char *content_type)
        // 设置响应的内容
        // s : 内容， content_type ：内容的类型和编码
        res.set_content("hello world", "text/plain; charset=utf-8");
    });
    // bool listen(const char *host, int port, int socket_flags = 0)
    // 启动服务器，监听连接
    server.listen("0.0.0.0", 8081);
    return 0;
}
```

[HTTP content-type](https://www.runoob.com/http/http-content-type.html)

因为这个库里面使用了`pthread`库，所以需要链接动态库`pthread`。

## 8.3 设计网络服务

同样是在回调方法里面，启动搜索服务，返回将响应内容设置为处理好的`json`串，返回给网络即可。

![image-20221010221955320](https://s2.loli.net/2022/10/10/oQiKnBNIAfO15lV.png)

# 九、前端

获取后端的`json`串，需要使用`JQuery`的`ajax`。然后使用给页面添加标签和内容即可。

模仿百度搜索界面

![image-20221011090851572](https://s2.loli.net/2022/10/11/limUn3b5zpjaGYH.png)

![image-20221011233319509](https://s2.loli.net/2022/10/11/izmj3n6FC8MGeAI.png)

最终效果：

![image-20221012102001532](https://s2.loli.net/2022/10/12/k4xmvsfzau7WrGh.png)












