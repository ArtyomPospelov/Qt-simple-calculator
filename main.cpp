#include "Calculator.h"
#include <QApplication>
#include <QFile>
#include "XmlUtils.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //app init:
    a.setWindowIcon(QIcon(":/Calcul/icon.png"));

    QFile file(":/Calcul/styles.qss");
    file.open(QFile::ReadOnly);
    a.setStyleSheet(file.readAll());
    file.close();

    XML::AppConfig conf = XML::getAppConfig();

    //main win:
    Calculator calc;
    calc.Init(conf.calcPrecCount);
    calc.move(conf.winPosX, conf.winPosY);
    calc.show();

    if (!QObject::connect(&calc, &QObject::destroyed, [&calc] {
        XML::AppConfig conf;
        const QRect geom = calc.frameGeometry();
        conf.winPosX = geom.x();
        conf.winPosY = geom.y();
        conf.calcPrecCount = calc.GetPrecision();
        if (!XML::storeAppConfig(conf))
        {
            Q_ASSERT(0);
        }
    }))
    {
        Q_ASSERT(0);
    }

    return a.exec();
}
