#include "XmlParser.h"

XmlParser::XmlParser()
{
    this->rootElement = nullptr;
}

XmlParser::XmlParser(std::string path)
{
    this->rootElement = nullptr;
    this->_path = path;
    loadXml(path);
}

XmlParser::~XmlParser()
{
    clear();
}

std::string &XmlParser::element::attribute(std::string key)
{
    return this->attributes[key];
}
XmlParser::iterator XmlParser::element::childElement(int nth)
{
    return iterator(childElements[nth]);
}

XmlParser::iterator XmlParser::element::childElement(std::string tagName, unsigned nth)
{
    for (std::vector<element *>::iterator it = this->childElements.begin(); it != this->childElements.end(); it++)
    {
        if ((*it)->tagName == tagName && (nth--) == 0)
        {
            return iterator(*it);
        }
    }
    return iterator(nullptr);
}

XmlParser::iterator XmlParser::element::appendChild(std::string tagName, std::string innerText)
{
    element *newelem = new element;
    newelem->tagName = tagName;
    newelem->innerText = innerText;
    this->childElements.push_back(newelem);
    return iterator(newelem);
}

void XmlParser::element::eraseChild(std::string tagName, unsigned nth)
{
    for (int i = 0; i < this->childElements.size(); i++)
    {
        if (childElements[i]->tagName == tagName && (nth--) == 0)
        {
            _clear(childElements[i]);
            this->childElements.erase(childElements.begin() + i);
        }
    }
    
}
void XmlParser::element::eraseChild(unsigned nth)
{
    _clear(this->childElements[nth]);
    this->childElements.erase(this->childElements.begin() + nth);
}

std::vector<std::string> XmlParser::element::keys()
{
    std::vector<std::string> arr;
    for (element *child : childElements)
    {
        arr.push_back(child->tagName);
    }
    return arr;
}
void XmlParser::loadXml()
{
    loadXml(_path);
}

void XmlParser::loadXml(std::string path)
{
    clear();
    this->_path = path;
    std::ifstream ifs;
    ifs.open(path, std::ios::in);
    ifs.seekg(0, std::ios::end);
    int length = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    char *buf = new char[length + 1];
    buf[length] = 0;
    ifs.read(buf, length);
    std::string bufstr = buf;
    delete[] buf;
    ifs.close();

    bool isProperties = true;
    int pos = 0, mark = 0;
    std::stack<element *> dom;
    while (isProperties)
    {
        skipSpace(bufstr, pos);
        if (bufstr.substr(pos, 5) != "<?xml")
        {
            isProperties = false;
            break;
        }

        pos += 2;
        mark = pos;
        while (!isEnd(bufstr[pos]) && pos < bufstr.size())
            pos++;
        std::string propertie = bufstr.substr(mark, pos - mark);
        skipSpace(bufstr, pos);

        while (bufstr.substr(pos, 2) != "?>" && pos < bufstr.size())
        {
            mark = pos;
            while (bufstr[pos] != '=' && pos < bufstr.size())
                pos++;
            std::string attr = bufstr.substr(mark, pos - mark);
            pos++;
            char valStartSymbol = bufstr[pos];
            pos++;
            mark = pos;
            while (!isEnd(bufstr[pos], valStartSymbol) && pos < bufstr.size())
                pos++;
            std::string value = bufstr.substr(mark, pos - mark);
            this->declarations[propertie][attr] = value;
            pos++;
            skipSpace(bufstr, pos);
        }
        pos += 2;
    }

    dom.push(new element);
    while (pos < length)
    {
        pos++;
        if (bufstr[pos] == '/')
        {
            while (pos < length && bufstr[pos] != '<')
            {
                pos++;
            }
            dom.pop();
        }
        else
        {
            mark = pos;
            while (pos < length && !isEnd(bufstr[pos]))
            {
                pos++;
            }
            std::string tagName = bufstr.substr(mark, pos - mark);
            element *newelem = new element;
            newelem->tagName = tagName;
            dom.top()->childElements.push_back(newelem);
            dom.push(newelem);
            skipSpace(bufstr, pos);
            while (bufstr[pos] != '>')
            {
                mark = pos;
                mark = pos;
                while (bufstr[pos] != '=' && pos < bufstr.size())
                    pos++;
                std::string attr = bufstr.substr(mark, pos - mark);
                pos++;
                char valStartSymbol = bufstr[pos];
                pos++;
                mark = pos;
                while (!isEnd(bufstr[pos], valStartSymbol) && pos < bufstr.size())
                    pos++;
                std::string value = bufstr.substr(mark, pos - mark);
                newelem->attributes[attr] = value;
                pos++;
                skipSpace(bufstr, pos);
            }
            skipSpace(bufstr, ++pos);
            if (bufstr[pos] == '<')
                continue;
            mark = pos;
            while (bufstr[pos] != '<')
            {
                pos++;
            }
            std::string innerText = bufstr.substr(mark, pos - mark);
            int i = innerText.size() - 1;
            while (innerText[i] == ' ' || innerText[i] == '\n' || innerText[i] == '\t')
                i--;
            newelem->innerText = innerText.substr(0, i + 1);
        }
    }
    this->rootElement = dom.top()->childElements[0];
    delete dom.top();
}

void XmlParser::clear()
{
    _clear(rootElement);
}

void XmlParser::_clear(element *&root)
{
    if (!root)
        return;
    std::queue<element *> buf;
    buf.push(root);
    root = nullptr;
    while (!buf.empty())
    {
        for (element *child : buf.front()->childElements)
        {
            buf.push(child);
        }
        delete buf.front();
        buf.pop();
    }
}

void XmlParser::save()
{
    saveAs(_path);
}

void XmlParser::saveAs(std::string path)
{
    if (path.empty())
        return;
    this->_path = path;
    std::ofstream ofs;
    ofs.open(path, std::ios::out);
    if (!declarations.empty())
    {
        std::map<std::string, std::map<std::string, std::string>>::iterator head = declarations.find("xml");
        if (head != declarations.end())
        {
            ofs << "<?" << head->first;
            std::map<std::string, std::string>::iterator version = head->second.find("version");
            if (version != head->second.end())
            {
                ofs << ' ' << version->first << '=' << '\"' << version->second << '\"';
            }
            for (std::map<std::string, std::string>::iterator attr = head->second.begin(); attr != head->second.end(); attr++)
            {
                if (attr->first == "version")
                    continue;
                ofs << ' ' << attr->first << '=' << '\"' << attr->second << '\"';
            }
            ofs << "?>\n";
        }
        for (std::map<std::string, std::map<std::string, std::string>>::iterator line = declarations.begin(); line != declarations.end(); line++)
        {
            if (line->first == "xml")
                continue;
            ofs << "<?" << line->first;
            for (std::map<std::string, std::string>::iterator attr = line->second.begin(); attr != line->second.end(); attr++)
            {
                ofs << ' ' << attr->first << '=' << '\"' << attr->second << '\"';
            }
            ofs << "?>\n";
        }
    }
    if (rootElement)
    {
        _save(ofs, rootElement);
    }
    ofs.close();
}

void XmlParser::_save(std::ofstream &ofs, element *root, int layer)
{
    ofs << "\n" << std::string(layer, '\t') << "<" << root->tagName;
    if (!(root->attributes.empty()))
    {
        for (std::map<std::string, std::string>::iterator attr = root->attributes.begin(); attr != root->attributes.end(); attr++)
        {
            ofs << ' ' << attr->first << "=\"" << attr->second << '\"';
        }
    }
    ofs << '>' << root->innerText;
    if (!(root->childElements.empty()))
    {
        for (element *child : root->childElements)
        {
            _save(ofs, child, layer + 1);
        }
        ofs << "\n"
            << std::string(layer, '\t') << "</" << root->tagName << '>';
        return;
    }
    ofs << "</" << root->tagName << '>';
}

std::string &XmlParser::declaration(std::string type, std::string key)
{
    return this->declarations[type][key];
}

std::string &XmlParser::declaration(std::string key)
{
    return this->declarations["xml"][key];
}

std::string &XmlParser::path()
{
    return this->_path;
}

bool XmlParser::isEnd(char a, char left)
{
    if (left == '\'')
        return a == '\'';
    return a == '>' || a == ' ' || a == '\"' || a == '\t' || a == '\n';
}

void XmlParser::skipSpace(std::string &s, int &pos)
{
    while (pos < s.size() && (s[pos] == ' ' || s[pos] == '\t' || s[pos] == '\n'))
        pos++;
}

XmlParser::iterator::iterator(element *p)
{
    this->p = p;
}

XmlParser::iterator::~iterator()
{
}

XmlParser::element *XmlParser::iterator::operator->()
{
    return this->p;
}

XmlParser::iterator XmlParser::iterator::operator[](std::string key)
{
    for (std::vector<element *>::iterator it = p->childElements.begin(); it != p->childElements.end(); it++)
    {
        if ((*it)->tagName == key)
        {
            return iterator(*it);
        }
    }
    return iterator(nullptr);
}
XmlParser::iterator XmlParser::iterator::operator[](int nth)
{
    return iterator(p->childElements[nth]);
}

bool XmlParser::iterator::operator==(iterator &other)
{
    return this->p == other.p;
}
bool XmlParser::iterator::operator!=(iterator &other)
{
    return this->p != other.p;
}

XmlParser::iterator XmlParser::root()
{
    if (!rootElement)
    {
        rootElement = new element;
    }
    return iterator(rootElement);
}

XmlParser::iterator XmlParser::end()
{
    return iterator(nullptr);
}