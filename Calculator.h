#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <qwidget.h>
#include <qpushbutton.h>

class QLineEdit;
class QGridLayout;
class QHBoxLayout;
class QButtonGroup;

class Calculator : public QWidget
{
    Q_OBJECT

    enum Stat
    {
        S_FirstArg, //expecting first arg. on binary operator
        S_SecondArg, //expecting second arg. on binary operator
        S_Calculated //expression has been calculated, only binary operator or single operator continue evaluating expression. Calc button (=) repeat binary operator with join accumulator result.
    };

    enum BinOper
    {
        BO_None,
        BO_Div,
        BO_Mul,
        BO_Minus,
        BO_Plus
    };

    //Any "simple" button of calculator:
    class CalcBtn : public QPushButton
    {
    public:
        enum Type
        {
            T_About,
            T_CE,
            T_C,
            T_BACKSPACE,
            T_1DIVX,
            T_SQR,
            T_SQRT,
            T_DIV,
            T_7,
            T_8,
            T_9,
            T_MUL,
            T_4,
            T_5,
            T_6,
            T_MINUS,
            T_1,
            T_2,
            T_3,
            T_PLUS,
            T_SIGNCHANGE,
            T_0,
            T_COMMA,
            T_CALC,
            T_PRECAUTO,
            T_PRECZERO,
            T_PRECTWO,
            T_PRECTHREE,
            T_PRECMAX
        };

        enum Group
        {
            G_NUMBER,
            G_OPER,
            G_CLEAR,
            G_CALC,
            G_MORE,
            G_MODE
        };

    public:
        CalcBtn(QWidget* wgt, Type type)
            : QPushButton(wgt),
            m_Type(type)
        {
            switch (m_Type)
            {
            case T_About:
                m_Group = G_MORE;
                setText("?");
                break;
            case T_CE:
                m_Group = G_CLEAR;
                setText("CE");
                break;
            case T_C:
                m_Group = G_CLEAR;
                setText("C");
                break;
            case T_BACKSPACE:
                m_Group = G_CLEAR;
                setText("<--");
                break;
            case T_1DIVX:
                m_Group = G_CALC;
                setText("1/x");
                break;
            case T_SQR:
                m_Group = G_CALC;
                setText("x^2");
                break;
            case T_SQRT:
                m_Group = G_CALC;
                setText("2_/x");
                break;
            case T_DIV:
                m_Group = G_OPER;
                setText("/");
                break;
            case T_7:
                m_Group = G_NUMBER;
                setText("7");
                m_Number = 7;
                break;
            case T_8:
                m_Group = G_NUMBER;
                setText("8");
                m_Number = 8;
                break;
            case T_9:
                m_Group = G_NUMBER;
                setText("9");
                m_Number = 9;
                break;
            case T_MUL:
                m_Group = G_OPER;
                setText("*");
                break;
            case T_4:
                m_Group = G_NUMBER;
                setText("4");
                m_Number = 4;
                break;
            case T_5:
                m_Group = G_NUMBER;
                setText("5");
                m_Number = 5;
                break;
            case T_6:
                m_Group = G_NUMBER;
                setText("6");
                m_Number = 6;
                break;
            case T_MINUS:
                m_Group = G_OPER;
                setText("-");
                break;
            case T_1:
                m_Group = G_NUMBER;
                setText("1");
                m_Number = 1;
                break;
            case T_2:
                m_Group = G_NUMBER;
                setText("2");
                m_Number = 2;
                break;
            case T_3:
                m_Group = G_NUMBER;
                setText("3");
                m_Number = 3;
                break;
            case T_PLUS:
                m_Group = G_OPER;
                setText("+");
                break;
            case T_SIGNCHANGE:
                m_Group = G_OPER;
                setText("+/-");
                break;
            case T_0:
                m_Group = G_NUMBER;
                setText("0");
                m_Number = 0;
                break;
            case T_COMMA:
                m_Group = G_OPER;
                setText(",");
                break;
            case T_CALC:
                m_Group = G_CALC;
                setText("=");
                break;
            case T_PRECAUTO:
                m_Group = G_MODE;
                setText("A");
                break;
            case T_PRECZERO:
                m_Group = G_MODE;
                setText("0");
                break;
            case T_PRECTWO:
                m_Group = G_MODE;
                setText("2");
                break;
            case T_PRECTHREE:
                m_Group = G_MODE;
                setText("3");
                break;
            case T_PRECMAX:
                m_Group = G_MODE;
                setText("F");
                break;
            }

            setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

            switch (m_Group)
            {
            case G_NUMBER:
                setObjectName("Number");
                break;
            case G_OPER:
                setObjectName("Oper");
                break;
            case G_CALC:
                setObjectName("Calc");
                break;
            case G_CLEAR:
                setObjectName("Clear");
                break;
            case G_MORE:
                setObjectName("More");
                break;
            case G_MODE:
                setObjectName("Mode");
                break;
            }

        }

        Type GetType() const
        {
            return m_Type;
        }

        Group GetGroup() const
        {
            return m_Group;
        }

        int GetNumber() const
        {
            return m_Number;
        }

        QSize sizeHint() const override
        {
            return QSize(45, 45);
        }

    private:
        Type m_Type;
        Group m_Group;
        int m_Number = 0;
    };

    //Choose work mode of calculator:
    class CalcModeBtn : public CalcBtn
    {
    public:
        CalcModeBtn(QWidget* wgt, Type type)
            : CalcBtn(wgt, type)
        {
            setCheckable(true);
            setAutoExclusive(true);
        }

        QSize sizeHint() const override
        {
            return QSize(25, 25);
        }
    };

public:
    Calculator(QWidget* wgt = Q_NULLPTR);
    virtual ~Calculator() = default;

public:
    void Init(int precCount = -1);

    bool IsAutoPrecision() const;
    void SetPrecision(int precCount);
    int GetPrecision() const;

protected:
    void _initWindow();

    void _initDisplay();
    void _initButtons();
    void _initPrecisButtons();
    void _initConnections();

    void _resetCalc();
    void _newExprReset();
    void _resetPrecMode(int newPrecCount = -1);
    double _getArg() const;
    void _displayAccum();
    void _calc(CalcBtn::Type type);
    void _setBinOper(BinOper oper);

protected slots:
    void _onBtnClicked();

protected:
    QLineEdit* m_Display = nullptr;
    QGridLayout* m_Layout = nullptr;
    QHBoxLayout* m_LayBtnPrecis = nullptr;
    QButtonGroup* m_BGroupPrecis = nullptr;

    //registers:
    Stat m_Stat = S_FirstArg;
    BinOper m_BinOper = BO_None;
    double m_AArg = 0; //"accumulator".
    double m_BArg = 0;
    int m_Precision = 4; //precision = -1 for auto precision mode.

    //consts:
    const QString mc_Empty = "0";
    const QString mc_Comma = ",";
    const QString mc_NegSign = "-";
    const int mc_MaxPrecission = 7;
    const double mc_InvalidArg;
};

#endif // CALCULATOR_H
