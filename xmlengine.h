#ifndef XMLENGINE_H
#define XMLENGINE_H

#include <QObject>
#include <QDomDocument>
#include <QMap>

typedef QMap<QString, QString> StringMap;

class XMLEngine : public QObject
{
    Q_OBJECT
public:
    explicit XMLEngine(QObject *parent = 0);
    static StringMap parseXml(QString xml);
    static StringMap parseXml(QDomElement elem);
    static QString buildXml(StringMap map);
    
signals:
 
public slots:

private:

};

#endif // XMLENGINE_H
