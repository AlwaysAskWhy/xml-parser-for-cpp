#ifndef XMLPARSER
#define XMLPARSER

#include <map>
#include <vector>
#include <stack>
#include <string>
#include <fstream>
#include <algorithm>
#include <queue>

class XmlParser
{
public:
    class iterator;
private:
    class element
    {
    public:
        std::string tagName;
        std::string innerText;
        std::string& attribute(std::string key);
        XmlParser::iterator childElement(int nth);
        XmlParser::iterator childElement(std::string tagName, unsigned nth = 0);
        XmlParser::iterator appendChild(std::string tagName, std::string innerText = "");
        void eraseChild(std::string tagName, unsigned nth = 0);
        void eraseChild(unsigned nth);
        std::vector<std::string> keys();
    private:
        std::map<std::string, std::string> attributes;
        std::vector<element*> childElements;
        friend class XmlParser;
    };
//<u>__</u>
    std::string _path;
    element* rootElement;
    std::map<std::string, std::map<std::string, std::string>> declarations;

public:
    XmlParser();
    XmlParser(std::string path);
    ~XmlParser();
    void loadXml();
    void loadXml(std::string path);
    void clear();
    void save();
    void saveAs(std::string path);

    std::string& declaration(std::string type, std::string key);
    std::string& declaration(std::string key);
    std::string& path();
    

    class iterator
    {
    private:
        element* p;
        friend class XmlParser;
        iterator(element* p);
    public:
        ~iterator();
        element* operator->();
        iterator operator[](int nth);
        iterator operator[](std::string key);
        bool operator==(iterator& other);
        bool operator!=(iterator& other);
    };
    
    iterator root();
    iterator end();
    
private:
    bool isEnd(char a, char left = '\"');
    void skipSpace(std::string & s, int & pos);
    void _save(std::ofstream& ofs, element* root, int layer = 0);
    static void _clear(element* &root);

};




#endif