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

- 正排索引：就是从文档ID找到文档内容(文档内的关键字)

举例文档：

> 文档1 ： 南京哪里可以做核酸
>
> 文档2 ： 核酸检测地



| 文档ID | 文档内容           |
| :----- | ------------------ |
| 1      | 南京哪里可以做核酸 |
|2| 核酸检测地 |





## 4.2 倒排索引

- 倒排索引：就是根据某些关键字，找到对应的文档ID，是一对多的关系。

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

因为后面需要使用到文档内容，所以存放去标签的文档内容的时候需要使用约定的分隔符，将不同文档，同一个文档的不同部分分隔开。

- 对于同一个文档的标题和内容用 `\3` （3在`ASCII`表中对应文本结束，为不可以显示字符）分隔。
- 对于不同文档之间用 `\n` 分隔，方便后面对单个文档的读取。
- 不仅要把文档标题、内容去标签后存入文件，还有将`HTML`文档在`boost`官网的`URL`存入，方便后面设置超链接直接跳转。

> 形如：`title\3content\3url \n title\3content\3url \n title\3content\3url \n ...`

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
3. 将得到的数据，按照前面的约定分隔写入文件。

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

- 提取`title`



- 提取`content`



- 生成`url`





























