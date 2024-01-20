#include "xmldocument.h"

XmlDocument::XmlDocument()
{

}

XmlDocument::~XmlDocument()
{
    foreach(XmlNode* p, nodes) {
        delete p;
    }
}

XmlElement XmlDocument::createProcessingInstruction(const QString& name, const QString& value)
{
    XmlNode* nod = new XmlNode(XmlNode::TYPE_NORMAL, name);
    if(nod)
    {
        XmlNode* pro = new XmlNode(XmlNode::TYPE_PROCESS, value);
        nod->addChild(pro);
    }
    return XmlElement(nod);
}

XmlElement XmlDocument::createElement(const QString& name)
{
    return XmlElement(new XmlNode(XmlNode::TYPE_NORMAL, name));
}

XmlElement XmlDocument::createTextNode(const QString& text)
{
    return XmlElement(new XmlNode(XmlNode::TYPE_TEXT, text));
}

void XmlDocument::appendChild(const XmlElement& child)
{
    nodes.append(child.node);
}

void XmlDocument::saveIndent(QTextStream& stream, int level, const QString& indent)
{
    for(int i=0; i<level; ++i)
    {
        stream << indent;
    }
}

void XmlDocument::saveNode(QTextStream& stream, XmlNode* node, int level, const QString& indent)
{
    if(XmlNode::TYPE_NORMAL != node->type)
    {
        return;
    }
    saveIndent(stream, level, indent);
    stream << "<" << node->name;
    int num = node->attributes.size();
    for(int i=0; i<num; ++i)
    {
        XmlAttr& attr = node->attributes[i];
        stream << " " << attr.getName() << "=\"" << attr.getValue() << "\"";
    }
    if(node->children.size() > 0)
    {
        XmlNode* first = node->children[0];
        if(XmlNode::TYPE_TEXT == first->type)
        {
            stream << ">" << first->name << "</" << node->name << ">\n";
        }
        else
        {
            stream << ">\n";
            foreach(XmlNode* child, node->children)
            {
                saveNode(stream, child, level+1, indent);
            }
            saveIndent(stream, level, indent);
            stream << "</" << node->name << ">\n";
        }
    }
    else
    {
        stream << "/>\n";
    }
}

bool XmlDocument::save(QTextStream& stream, int indent, int flag)
{
    Q_UNUSED(flag);
    QString sp;
    for(int i=0; i<indent; ++i)
    {
        sp += " ";
    }
    foreach(XmlNode* nod, nodes)
    {
        if(nod->children.size() > 0 && XmlNode::TYPE_PROCESS == nod->children[0]->type)
        {
            stream << "<?" << nod->name << " " << nod->children[0]->name << "?>\n";
        }
        else
        {
            saveNode(stream, nod, 0, sp);
        }
    }
    stream.flush();
    return true;
}

QString XmlDocument::toString(int indent)
{
    QString text;
    text.reserve(16*1024);
    QTextStream stream(&text, QIODevice::WriteOnly);
    save(stream, indent, 0);
    text.squeeze();
    return text;
}

