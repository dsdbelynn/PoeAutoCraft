/*
 * 用于代替QDomDocument类保存XML信息
 * 由于QDomDocument的属性每次保存时顺序是乱的，这里简单实现其替代的类
 */
#ifndef XMLDOCUMENT_H
#define XMLDOCUMENT_H
#include <QtCore>

class XmlAttr
{
public:
    friend class XmlDocument;
    XmlAttr() {}
    XmlAttr(const QString& key, const QString& val) : name(key), value(val) {}

    const QString& getName() const { return name; }
    const QString& getValue() const { return value; }

private:
    QString name;
    QString value;
};

class XmlNode
{
public:
    friend class XmlDocument;
    enum EType { TYPE_NORMAL, TYPE_TEXT, TYPE_PROCESS };

    XmlNode() : type(TYPE_NORMAL) {}
    XmlNode(EType typ, const QString& str) : type(typ), name(str) {}
    ~XmlNode() {
        foreach(XmlNode* p, children) {
            delete p;
        }
    }

    void addChild(XmlNode* p) { children.append(p); }
    void setAttribute(const QString& name, const QString& value) { attributes.append(XmlAttr(name, value)); }

private:
    EType type;
    QString name;
    QList<XmlAttr> attributes;
    QList<XmlNode*> children;
};

class XmlElement
{
public:
    friend class XmlDocument;
    XmlElement(XmlNode* p) : node(p) {}

    void appendChild(const XmlElement& child) { node->addChild(child.node); }
    void setAttribute(const QString& name, const QString& value) { node->setAttribute(name, value); }
    void setAttribute(const QString& name, int value) { node->setAttribute(name, QString::number(value)); }
    void setAttribute(const QString& name, uint value) { node->setAttribute(name, QString::number(value)); }
    void setAttributeHex(const QString& name, uint value) { node->setAttribute(name, QString("0x%1").arg(value, 0, 16)); }
    void setAttribute(const QString& name, float value) { node->setAttribute(name, QString::number(value, 'g', 7)); }
    void setAttribute(const QString& name, double value) { node->setAttribute(name, QString::number(value, 'g', 11)); }

private:
    XmlNode* node;
};

class XmlDocument
{
public:
    XmlDocument();
    ~XmlDocument();

    XmlElement createProcessingInstruction(const QString& name, const QString& value);
    XmlElement createElement(const QString& name);
    XmlElement createTextNode(const QString& text);

    void appendChild(const XmlElement& child);

    bool save(QTextStream& stream, int indent, int flag = 0);
    QString toString(int indent);

private:
    void saveIndent(QTextStream& stream, int level, const QString& indent);
    void saveNode(QTextStream& stream, XmlNode* node, int level, const QString& indent);

    QList<XmlNode*> nodes;
};

#endif // XMLDOCUMENT_H
