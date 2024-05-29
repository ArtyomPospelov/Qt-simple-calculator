#include "XmlUtils.h"
#include <QFile>
#include <QApplication>

namespace XML
{
    const QString CONFIG_FILE_NAME = "config.xml";

    QString combineAbsoluteAppPath(const QString& fileName)
    {
        QString appDir = QApplication::applicationDirPath();
        if (QDir::separator() == '\\')
            appDir = appDir.replace('/', '\\');
        else
            appDir = appDir.replace('\\', '/');

        if (appDir.back() != QDir::separator())
            appDir.append(QDir::separator());

        return appDir + fileName;
    }

    QDomDocument getDoc(const QString& fileName)
    {
        QDomDocument doc;

        QFile file(combineAbsoluteAppPath(fileName));
        if (file.open(QFile::ReadOnly))
            doc.setContent(file.readAll());
        file.close();

        return doc;
    }

    QDomElement getElem(const QDomElement& root, const QString& name)
    {
        if (root.isNull())
            return QDomElement();

        QDomNodeList childs = root.childNodes();
        for (int i = 0, c = childs.count(); i < c; ++i)
        {
            QDomElement elem = childs.at(i).toElement();
            if (elem.isNull())
                continue;

            QString tagName = elem.tagName();
            if (tagName == name)
                return elem;

            QDomElement inElemChild = getElem(elem, name);
            if (inElemChild.isNull())
                continue;
        }

        return QDomElement();
    }

    AppConfig getAppConfig()
    {
        AppConfig conf;

        QDomDocument doc = getDoc(CONFIG_FILE_NAME);
        QDomElement winPosElem = getElem(doc.documentElement(), "winPos");

        bool posXok = false, posYok = false, precOk = false;

        const int posX = getElem(winPosElem, "x").text().toInt(&posXok);
        const int posY = getElem(winPosElem, "y").text().toInt(&posYok);
        const int precCount = getElem(doc.documentElement(), "calcPrec").text().toInt(&precOk);

        if (posXok)
            conf.winPosX = posX;
        if (posYok)
            conf.winPosY = posY;
        if (precOk)
            conf.calcPrecCount = precCount;

        return conf;
    }

    bool storeAppConfig(const AppConfig& conf)
    {
        QDomDocument doc;
        QDomElement root = doc.createElement("config");

        QDomElement calcPrec = doc.createElement("calcPrec");
        QDomText calcPrecText = doc.createTextNode(QString::number(conf.calcPrecCount));
        calcPrec.appendChild(calcPrecText);

        QDomElement calcPos = doc.createElement("winPos");
        QDomElement calcPosX = doc.createElement("x");
        QDomElement calcPosY = doc.createElement("y");
        QDomText calcPosXText = doc.createTextNode(QString::number(conf.winPosX));
        QDomText calcPosYText = doc.createTextNode(QString::number(conf.winPosY));

        calcPosX.appendChild(calcPosXText);
        calcPosY.appendChild(calcPosYText);
        calcPos.appendChild(calcPosX);
        calcPos.appendChild(calcPosY);

        root.appendChild(calcPos);
        root.appendChild(calcPrec);
        doc.appendChild(root);

        QFile file(combineAbsoluteAppPath(CONFIG_FILE_NAME));
        if (!file.open(QFile::WriteOnly | QFile::Text))
            return false;

        file.write(doc.toString().toUtf8());
        file.close();

        return true;
    }
}
