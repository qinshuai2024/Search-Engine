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

多个这个的结构体可以使用数组来保存

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












