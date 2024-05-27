#include "AboutCalc.h"
#include "ui_AboutCalc.h"

AboutCalc::AboutCalc(QWidget* wgt)
   : QDialog(wgt),
     m_Ui(new Ui::AboutCalc)
{
    m_Ui->setupUi(this);
}

AboutCalc::~AboutCalc()
{
    delete m_Ui;
}
