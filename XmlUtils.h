#ifndef XMLUTILS_H
#define XMLUTILS_H

#include <QtXml>

namespace XML
{
    struct AppConfig
    {
        int winPosX = 100;
        int winPosY = 100;
        int calcPrecCount = -1;
    };

    QDomDocument getDoc(const QString& fileName);
    QDomElement getElem(const QDomElement& root, const QString& name);
    AppConfig getAppConfig();
    bool storeAppConfig(const AppConfig& conf);
}



#endif // XMLUTILS_H
