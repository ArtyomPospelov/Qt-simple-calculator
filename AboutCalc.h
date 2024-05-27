#ifndef ABOUTCALC_H
#define ABOUTCALC_H

#include <qdialog.h>

namespace Ui
{
    class AboutCalc;
}

//"About box".
class AboutCalc : public QDialog
{
public:
    AboutCalc(QWidget* wgt);
    ~AboutCalc();

private:
    Ui::AboutCalc *m_Ui = nullptr;
};

#endif // ABOUTCALC_H
