#include "xmlengine.h"

XMLEngine::XMLEngine(QObject *parent) :
    QObject(parent)
{
}

StringMap XMLEngine::parseXml(QString xml)
{
    QDomDocument doc;
    doc.setContent(xml);
    QDomElement root = doc.documentElement();
    return parseXml(root);
}

StringMap XMLEngine::parseXml(QDomElement elem)
{
    StringMap map;
    QDomNodeList children = elem.childNodes();
    for (uint i=0; i<children.length(); i++) {
        QDomNode child = children.item(i);
        map.insert(child.nodeName(), child.toElement().text());
    }
    return map;
}

QString XMLEngine::buildXml(StringMap map)
{
    QDomDocument doc;
    QDomElement item = doc.createElement("item");
    doc.appendChild(item);
    foreach(QString key, map.keys()) {
        QDomElement tag = doc.createElement(key);
        item.appendChild(tag);
        QDomText text = doc.createTextNode(map.value(key));
        tag.appendChild(text);
    }
    return doc.toString(-1);
}
