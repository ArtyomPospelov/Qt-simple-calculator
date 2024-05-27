#include "Calculator.h"
#include "AboutCalc.h"

#include <qapplication.h>
#include <qgridlayout.h>
#include <qboxlayout.h>
#include <qlineedit.h>
#include <qbuttongroup.h>
#include <qlabel.h>
#include <qscreen.h>
#include <climits>
#include <cmath>

Calculator::Calculator(QWidget* wgt /*= Q_NULLPTR*/)
    : QWidget(wgt),
    mc_InvalidArg(std::numeric_limits<double>::max())
{
}

void Calculator::Init(int precCount /*= -1*/)
{
    _initWindow();
    _initDisplay();
    _initButtons();
    _initPrecisButtons();
    _initConnections();

    _resetCalc();
    _resetPrecMode(precCount);
}

bool Calculator::IsAutoPrecision() const
{
    return m_Precision < 0;
}

void Calculator::SetPrecision(int precCount)
{
    _resetPrecMode(precCount);
}

int Calculator::GetPrecision() const
{
    return m_Precision;
}

void Calculator::_initWindow()
{
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    setWindowFlag(Qt::WindowMinMaxButtonsHint, false);
    //setWindowFlag(Qt::FramelessWindowHint, true);
    setLayout(m_Layout = new QGridLayout(this));
    m_Layout->setSizeConstraint(QLayout::SetFixedSize);
    m_Layout->setContentsMargins(5, 5, 5, 5);
    m_Layout->setSpacing(5);
}

void Calculator::_initDisplay()
{
    m_Display = new QLineEdit(this);
    m_Display->setReadOnly(true);
    m_Display->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_Display->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    m_Display->setObjectName("Display");
    m_Layout->addWidget(m_Display, 0, 0, 1, 4);
}

void Calculator::_initButtons()
{
    auto add = [this](CalcBtn::Type type, int row, int col) {
        m_Layout->addWidget(new CalcBtn(this, type), row, col);
    };

    QList<CalcBtn::Type> btns = {
        CalcBtn::T_About, CalcBtn::T_CE, CalcBtn::T_C, CalcBtn::T_BACKSPACE,
        CalcBtn::T_1DIVX, CalcBtn::T_SQR, CalcBtn::T_SQRT, CalcBtn::T_DIV,
        CalcBtn::T_7, CalcBtn::T_8, CalcBtn::T_9, CalcBtn::T_MUL,
        CalcBtn::T_4, CalcBtn::T_5, CalcBtn::T_6, CalcBtn::T_MINUS,
        CalcBtn::T_1, CalcBtn::T_2, CalcBtn::T_3, CalcBtn::T_PLUS,
        CalcBtn::T_SIGNCHANGE, CalcBtn::T_0, CalcBtn::T_COMMA, CalcBtn::T_CALC
    };

    QListIterator<CalcBtn::Type> iter(btns);

    for (int row = 2; row <= 7; ++row)
        for (int col = 0; col <= 3; ++col)
            if (iter.hasNext())
                add(iter.next(), row, col);
}

void Calculator::_initPrecisButtons()
{
    auto add = [this](CalcBtn::Type type) {
        CalcModeBtn* btn = new CalcModeBtn(this, type);
        m_LayBtnPrecis->addWidget(btn);
        m_BGroupPrecis->addButton(btn);
    };

    m_BGroupPrecis = new QButtonGroup(this);
    m_LayBtnPrecis = new QHBoxLayout(this);
    m_Layout->addLayout(m_LayBtnPrecis, 1, 0, 1, 4);

    QLabel* lblPrec = new QLabel("Precision", this);
    lblPrec->setObjectName("lblPrec");
    m_LayBtnPrecis->addWidget(lblPrec);
    add(CalcBtn::T_PRECAUTO);
    add(CalcBtn::T_PRECZERO);
    add(CalcBtn::T_PRECTWO);
    add(CalcBtn::T_PRECTHREE);
    add(CalcBtn::T_PRECMAX);
}

void Calculator::_initConnections()
{
    for (CalcBtn* btn : findChildren<CalcBtn*>())
        connect(btn, &QAbstractButton::clicked, this, &Calculator::_onBtnClicked);
}

void Calculator::_resetCalc()
{
    m_Display->setText(mc_Empty);
    m_AArg = m_BArg = mc_InvalidArg;
    m_Stat = S_FirstArg;
    m_BinOper = BO_None;
}

void Calculator::_newExprReset()
{
    if (m_Stat == S_Calculated)
        _resetCalc();
}

void Calculator::_resetPrecMode(int newPrecCount /*= -1*/)
{
    auto clickByType = [this](CalcBtn::Type type)
    {
        for (CalcModeBtn* modeBtn : findChildren<CalcModeBtn*>())
            if (modeBtn->GetType() == type)
            {
                modeBtn->click();
                break;
            }
    };

    switch (newPrecCount)
    {
    case 0:
        clickByType(CalcBtn::T_PRECZERO);
        return;
    case 2:
        clickByType(CalcBtn::T_PRECTWO);
        return;
    case 3:
        clickByType(CalcBtn::T_PRECTHREE);
        return;
    }

    if (newPrecCount < 0)
        clickByType(CalcBtn::T_PRECAUTO);
    else if (newPrecCount >= mc_MaxPrecission)
        clickByType(CalcBtn::T_PRECMAX);
    else
        clickByType(CalcBtn::T_PRECMAX); //default dummy.
}

double Calculator::_getArg() const
{
    QString dispText = m_Display->text();
    dispText.replace(mc_Comma, ".");
    dispText.replace(mc_NegSign, "-");

    bool isOk = false;
    const double arg = dispText.toDouble(&isOk);
    if (!isOk)
        return mc_InvalidArg;
    return arg;
}

void Calculator::_displayAccum()
{
    if (m_AArg == mc_InvalidArg)
    {
        m_Display->setText(mc_Empty);
        return;
    }

    QString result = QString::number(m_AArg, 'f', IsAutoPrecision() ? mc_MaxPrecission : m_Precision);
    result.replace(".", mc_Comma);
    result.replace(",", mc_Comma);
    result.replace("-", mc_NegSign);

    if (IsAutoPrecision())
    {
        QStringList splitted = result.split(mc_Comma, Qt::SkipEmptyParts);
        if (splitted.size() == 2)
        {
            bool startCount = false;
            int precCount = 0;
            QString decPart = splitted[1];
            for (qsizetype i = decPart.length() - 1; i >= 0;)
            {
                if (!startCount && decPart.at(i) != '0')
                {
                    startCount = true;
                    continue;
                }

                if (startCount)
                    ++precCount;

                --i;
            }


            QString aarg = splitted[0];
            if (precCount > 0)
            {
                aarg.append(mc_Comma);
                aarg.append(splitted[1].left(precCount));
            }

            m_Display->setText(aarg);
            return;
        }
    }

    m_Display->setText(result);
}

void Calculator::_calc(CalcBtn::Type type)
{
    switch (type)
    {
    case CalcBtn::T_SQR:
    case CalcBtn::T_SQRT:
    case CalcBtn::T_1DIVX:
    {
        double arg = _getArg();
        if (arg)
        {
            switch (type)
            {
            case CalcBtn::T_SQR:
                arg = std::pow(arg, 2);
                break;
            case CalcBtn::T_SQRT:
                arg = std::sqrt(arg);
                break;
            case CalcBtn::T_1DIVX:
                arg = 1 / arg;
                break;
            }

            _resetCalc();
            m_AArg = arg;
            _displayAccum();
            m_Stat = S_Calculated;
        }
    }
    break;

    case CalcBtn::T_CALC:
    {
        if (m_Stat == S_FirstArg)
        {
            m_AArg = _getArg();
        }
        else if (m_Stat != S_Calculated)
        {
            m_BArg = _getArg();
            m_Stat = S_Calculated;
        }

        switch (m_BinOper)
        {
        case BO_Div:
            m_AArg /= m_BArg;
            break;
        case BO_Mul:
            m_AArg *= m_BArg;
            break;
        case BO_Plus:
            m_AArg += m_BArg;
            break;
        case BO_Minus:
            m_AArg -= m_BArg;
            break;
        }

        _displayAccum();
    }
    break;
    }
}

void Calculator::_setBinOper(BinOper oper)
{
    m_BinOper = oper;
    m_AArg = _getArg();
    m_BArg = mc_InvalidArg;
    m_Stat = S_SecondArg;
    m_Display->setText(mc_Empty);
}

void Calculator::_onBtnClicked()
{
    CalcBtn* btn = dynamic_cast<CalcBtn*>(sender());
    Q_ASSERT(btn);
    if (!btn)
        return;

    const CalcBtn::Group grp = btn->GetGroup();
    const CalcBtn::Type type = btn->GetType();

    if (grp == CalcBtn::G_MORE)
    {
        switch (type)
        {
        case CalcBtn::T_About:
        {
            AboutCalc *about = new AboutCalc(this);
            about->setAttribute(Qt::WA_DeleteOnClose, true);
            about->exec();
        }
        break;
        }
    }
    else if (grp == CalcBtn::G_NUMBER)
    {
        _newExprReset();

        QString dispText = m_Display->text();
        if (dispText == mc_Empty)
            dispText = "";
        m_Display->setText(dispText + QString::number(btn->GetNumber()));
    }
    else if (grp == CalcBtn::G_OPER)
    {
        switch (type)
        {
        case CalcBtn::T_COMMA:
        {
            _newExprReset();
            QString dispText = m_Display->text();
            if (!dispText.contains(mc_Comma))
                m_Display->setText(dispText + mc_Comma);
        }
        break;
        case CalcBtn::T_SIGNCHANGE:
        {
            _newExprReset();
            QString dispText = m_Display->text();
            if ((dispText != mc_Empty) && (dispText != (mc_Empty+mc_Comma)))
            {
                if (!dispText.contains(mc_NegSign))
                    dispText.push_front(mc_NegSign);
                else
                    dispText = dispText.right(dispText.count() - mc_NegSign.count());
                m_Display->setText(dispText);
            }
        }
        break;
        case CalcBtn::T_DIV:
            _setBinOper(BO_Div);
            break;
        case CalcBtn::T_MUL:
            _setBinOper(BO_Mul);
            break;
        case CalcBtn::T_MINUS:
            _setBinOper(BO_Minus);
            break;
        case CalcBtn::T_PLUS:
            _setBinOper(BO_Plus);
            break;
        }
    }
    else if (grp == CalcBtn::G_CLEAR)
    {
        switch (type)
        {
        case CalcBtn::T_CE:
        {
            _newExprReset();
            if (m_Stat == S_FirstArg)
                m_AArg = mc_InvalidArg;
            else if (m_Stat == S_SecondArg)
                m_BArg = mc_InvalidArg;
            m_Display->setText(mc_Empty);
        }
        break;
        case CalcBtn::T_C:
            return _resetCalc();
        case CalcBtn::T_BACKSPACE:
        {
            _newExprReset();
             QString dispText = m_Display->text();
            if (dispText != mc_Empty)
            {
                dispText = dispText.left(dispText.count() - 1);
                if ((dispText.right(1) == mc_Comma) || (dispText.right(1) == mc_NegSign))
                    dispText = dispText.left(dispText.count() - 1);
                if (dispText.isEmpty())
                    dispText = mc_Empty;
                m_Display->setText(dispText);
            }
        }
        break;
        }
    }
    else if (grp == CalcBtn::G_CALC)
    {
        switch (type)
        {
        case CalcBtn::T_SQR:
            _calc(CalcBtn::T_SQR);
            break;
        case CalcBtn::T_SQRT:
            _calc(CalcBtn::T_SQRT);
            break;
        case CalcBtn::T_1DIVX:
            _calc(CalcBtn::T_1DIVX);
            break;
        case CalcBtn::T_CALC:
            _calc(CalcBtn::T_CALC);
            break;
        }
    }
    else if (grp == CalcBtn::G_MODE)
    {
        switch (type)
        {
        case CalcBtn::T_PRECZERO:
            m_Precision = 0;
            _displayAccum();
            break;
        case CalcBtn::T_PRECTWO:
            m_Precision = 2;
            _displayAccum();
            break;
        case CalcBtn::T_PRECTHREE:
            m_Precision = 3;
            _displayAccum();
            break;
        case CalcBtn::T_PRECMAX:
            m_Precision = mc_MaxPrecission;
            _displayAccum();
            break;
        case CalcBtn::T_PRECAUTO:
            m_Precision = -1;
            _displayAccum();
            break;
        }
    }
}
