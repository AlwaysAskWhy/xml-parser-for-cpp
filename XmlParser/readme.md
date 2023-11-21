## XmlParser

### 简介

XmlParser是一个面向C++的xml文件解析工具，实现了在C++中对xml文件的增删查改操作。该工具包以面向对象的方式实现对xml文件数据的封装，为数据构建DOM树，并提供了简洁的访问接口。

### 快速使用



#### 引入工具

文件目录:

```shell
└─XmlParser
        XmlParser.cpp
        XmlParser.h
```

文件仅包含两个文件，使用时仅需要将两个源文件添加到项目中，并引用头文件即可。

```c++
#include "XmlParser/XmlParser.h"
```

使用g++进行编译时，需要将XmlParser.cpp添加到编译参数中。如以下文件中，test.cpp引用了XmlParser工具包，编译test.cpp时，具体命令为：

```shell
g++ -g test.cpp XmlParser/XmlParser.cpp -o test.exe
```



#### 创建xml对象

1. 无参构造

```c++
XmlParser xml();
```

> 创建空白的xml对象，可在此基础上添加数据，编辑并导出新的xml文件。

2. 有参构造

```c++
XmlParser xml("test.xml");
```

> 构造时传入要打开的文件路径，由于编译环境的不一致可能会导致相对路径的方式存在异常，传入路径path时，通常建议使用__绝对路径__。
>
> 通过有参的方法构建的xml对象会直接读取目标文件并构建DOM树。



如何获取文件路径？这里分享一种在C++中获取当前文件路径的方法：

```c++
#include <direct.h>
int main(){
    char buf[128];
    getcwd(buf, 128);
    cout << buf;		//D:\Desktop\C++_WORKSPACE
}
```

3. 使用  <u>_xml.loadXml(path)_</u> 加载文件


4. <u>_xml.path()_</u> 获取对象路径

```c++
XmlParser xml("test.xml");
cout << xml.path(); 	//test.xml
xml.path() = "test2.xml";
cout << xml.path();		//test2.xml
```

> 使用path方法修改对象的路径并不会重新读取目标文件，需要手动调用 <u>_loadXml_</u> 函数加载文件。

```c++
XmlParser xml();
xml.path() = "test2.xml";
xml.loadXml();
```



#### 获取文件声明

```xml
<?xml version="1.0" encoding="UTF-8"?>
<?xml-stylesheet href="file.xsl" type="text/xsl"?>
```

1. 获取第一行声明 <u>_xml.declaration(key)_</u>;

> 获取以<?xml开头的主要文件声明，只需要传入一个参数，该参数为对应属性的键名。

```C++
XmlParser xml("test.xml");
string version = xml.declaration("version");	//1.0
string encoding = xml.declaration("encoding");	//UTF-8
```

2. 获取其他声明

> 获取其他声明时，需要传入前缀部分的声明类型

```c++
XmlParser xml("test.xml");
string version = xml.declaration("xml-stylesheet", "href");	
//file.xsl
```

3. 上述两种接口均支持__读写__

```c++
xml.declaration("version") = "2.0";
```



#### 数据管理

> XmlParser 中使用 <u>_XmlParser::iterator_</u> 对象表示某一个DOM节点，通过iterator 对象可以访问该节点的信息，以及该节点的子元素的信息。

##### 获取元素信息

> 示例参考如下xml文件

```xml
<note type="test">
	<name>C/C++ Runner: Debug Session</name>
	<request>launch</request>
	<setupCommands>
		<description>Enable pretty-printing for gdb</description>
		<text>-enable-pretty-printing</text>
		<text>-enable-pretty-printing</text>
		<ignoreFailures>true</ignoreFailures>
	</setupCommands>
</note>
```

1. 获取文档根节点 <u>_xml.root()_</u> 

```c++
XmlParser::iterator note = xml.root();	//获取根节点note
```

2. 节点标签名 <u>_iterator->tagName_</u>

   > 支持__读写__

```c++
cout << note->tagName;	//note
```

3. 节点内文本  <u>_iterator->innerText_</u>

   > 支持__读写__

```xml
<root>This is root</root>
```

```c++
cout << root->innerText;	//This is root
```

4. 节点属性 <u>_iterator->attribute (key)_</u> 

   > 支持__读写__，若不存在该属性会直接添加

```xml
<root type="test">This is root</root>
```

```c++
string attr = root->attribute("type");
cout << attr;	//test
```

5. 获取子节点 <u>_iterator->childElement (nth)_</u>  or <u>_iterator->childElement (tagName, nth = 0)_</u>  or <u>_iterator []_</u>

   > 通过文档前后序号或者子元素标签名获取子节点，返回子节点的iterator对象

```xml
<note type="test">
	<name>C/C++ Runner: Debug Session</name>
	<setupCommands>
		<description>Enable pretty-printing for gdb</description>
		<text>the first text</text>
         <text>the second text</text>
	</setupCommands>
</note>
```

```c++
XmlParser::iterator note = xml.root();
//1.通过序号获取
//获取note的第一个子节点
XmlParser::iterator firstChild = note->childElement(0);
cout << firstChild->tagName; 	//name
//也可使用[]运算符获取
firstChild = note[0];
    
//2.通过标签名获取
XmlParser::iterator setupCommands = note->childElement("setupCommands");
//获取标签名为setupCommands的子元素，默认获取第一个匹配的元素
//若需获取非首个匹配元素，需要指定序号，如：
XmlParser::iterator text1 = setupCommands->childElement("text", 0);
XmlParser::iterator text2 = setupCommands->childElement("text", 1);
cout << text1->innerText << '\n' << text2->innerText;
//the first text
//the second text
//也可通过[]运算符获取，只能获取第一个匹配的子元素
XmlParser::iterator text = setupCommands["text"];
//若不存在匹配的子元素，则返回xml.end();
XmlParser::iterator aa = setupCommands->childElement("aa");
cout << (aa == xml.end());	//1
```

6. 获取所有子元素的标签名 <u>_iterator->keys()_</u>

   > __只读__访问，返回vector<string>对象，包含该节点下的所有子节点的标签名

```xml
<note type="test">
	<name>C/C++ Runner: Debug Session</name>
	<request>launch</request>
	<setupCommands>
		<description>Enable pretty-printing for gdb</description>
		<text>-enable-pretty-printing</text>
		<ignoreFailures>true</ignoreFailures>
	</setupCommands>
</note>
```

```c++
XmlParser::iterator note = xml.root();
vector<string> keys = note->keys();
//["name", "request", "setupCommands"]
```



##### 添加节点

> 调用 <u>_iterator->appendChild (tagName, innerText = "")_</u>  为调用者创建子元素，并返回新建元素的iterator对象

```xml
<note>
    <name>c++</name>
</note>
```

```c++
XmlParser::iterator note = xml.root();
note->appendChild("request", "launch");
xml.save();
```

> 保存得到如下文件

```xml
<note>
    <name>c++</name>
    <request>launch</request>
</note>
```

> 调用函数会返回新结点的iterator对象

```c++
XmlParser::iterator note = xml.root();
XmlParser::iterator setupCommands = note->appendChild("setupCommands");
setupCommands->appendChild("ignoreFailures", "true");
xml.save();
```

> 再次保存

```xml
<note>
    <name>c++</name>
    <request>launch</request>
    <setupCommands>
        <ignoreFailures>true</ignoreFailures>
    </setupCommands>
</note>
```



##### 删除节点

> 通过 <u>_iterator->eraseChild (tagName, nth = 0)_</u> 删除调用者的第nth + 1个标签名为tagName的子元素

> 通过 <u>_iterator->eraseChild (nth)_</u> 删除调用者的第nth + 1个子元素

```xml
<note>
    <name>c++</name>
    <request>launch</request>
    <setupCommands>
        <ignoreFailures>true</ignoreFailures>
    </setupCommands>
</note>
```

```c++
XmlParser::iterator note = xml.root();
note->eraseChild(0);
note->eraseChild("setupCommands");
xml.save();
```

> 保存得到

```xml
<note>
    <request>launch</request>
</note>
```



#### 文件保存

```c++
xml.save();
//保存到xml.path()路径下的文件

xml.saveAs("test2.xml");
//另存为test2.xml
//等价于：
xml.path() = "test2.path";
xml.save();
```








