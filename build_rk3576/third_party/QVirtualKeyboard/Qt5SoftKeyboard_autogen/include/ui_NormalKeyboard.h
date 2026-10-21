/********************************************************************************
** Form generated from reading UI file 'NormalKeyboard.ui'
**
** Created by: Qt User Interface Compiler version 5.15.11
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NORMALKEYBOARD_H
#define UI_NORMALKEYBOARD_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_NormalKeyboard
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *KeyboardRow1;
    QPushButton *Q;
    QPushButton *W;
    QPushButton *E;
    QPushButton *R;
    QPushButton *T;
    QPushButton *Y;
    QPushButton *U;
    QPushButton *I;
    QPushButton *O;
    QPushButton *P;
    QPushButton *backspace;
    QHBoxLayout *KeyboardRow2;
    QWidget *widget;
    QPushButton *A;
    QPushButton *S;
    QPushButton *D;
    QPushButton *F;
    QPushButton *G;
    QPushButton *H;
    QPushButton *J;
    QPushButton *K;
    QPushButton *L;
    QPushButton *enter;
    QHBoxLayout *KeyboardRow3;
    QPushButton *shiftl;
    QPushButton *Z;
    QPushButton *X;
    QPushButton *C;
    QPushButton *V;
    QPushButton *B;
    QPushButton *N;
    QPushButton *M;
    QPushButton *at;
    QPushButton *period;
    QPushButton *shiftr;
    QHBoxLayout *KeyboardRow4;
    QPushButton *symbol;
    QPushButton *language;
    QPushButton *space;
    QPushButton *apostrophe;
    QPushButton *minus;
    QPushButton *hide;
    QButtonGroup *groupLetter;

    void setupUi(QWidget *NormalKeyboard)
    {
        if (NormalKeyboard->objectName().isEmpty())
            NormalKeyboard->setObjectName(QString::fromUtf8("NormalKeyboard"));
        NormalKeyboard->resize(982, 315);
        NormalKeyboard->setStyleSheet(QString::fromUtf8(""));
        verticalLayout = new QVBoxLayout(NormalKeyboard);
        verticalLayout->setSpacing(10);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        KeyboardRow1 = new QHBoxLayout();
        KeyboardRow1->setObjectName(QString::fromUtf8("KeyboardRow1"));
        Q = new QPushButton(NormalKeyboard);
        groupLetter = new QButtonGroup(NormalKeyboard);
        groupLetter->setObjectName(QString::fromUtf8("groupLetter"));
        groupLetter->addButton(Q);
        Q->setObjectName(QString::fromUtf8("Q"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Q->sizePolicy().hasHeightForWidth());
        Q->setSizePolicy(sizePolicy);
        Q->setSizeIncrement(QSize(4, 3));
        Q->setBaseSize(QSize(80, 60));

        KeyboardRow1->addWidget(Q);

        W = new QPushButton(NormalKeyboard);
        groupLetter->addButton(W);
        W->setObjectName(QString::fromUtf8("W"));
        sizePolicy.setHeightForWidth(W->sizePolicy().hasHeightForWidth());
        W->setSizePolicy(sizePolicy);

        KeyboardRow1->addWidget(W);

        E = new QPushButton(NormalKeyboard);
        groupLetter->addButton(E);
        E->setObjectName(QString::fromUtf8("E"));
        sizePolicy.setHeightForWidth(E->sizePolicy().hasHeightForWidth());
        E->setSizePolicy(sizePolicy);

        KeyboardRow1->addWidget(E);

        R = new QPushButton(NormalKeyboard);
        groupLetter->addButton(R);
        R->setObjectName(QString::fromUtf8("R"));
        sizePolicy.setHeightForWidth(R->sizePolicy().hasHeightForWidth());
        R->setSizePolicy(sizePolicy);

        KeyboardRow1->addWidget(R);

        T = new QPushButton(NormalKeyboard);
        groupLetter->addButton(T);
        T->setObjectName(QString::fromUtf8("T"));
        sizePolicy.setHeightForWidth(T->sizePolicy().hasHeightForWidth());
        T->setSizePolicy(sizePolicy);

        KeyboardRow1->addWidget(T);

        Y = new QPushButton(NormalKeyboard);
        groupLetter->addButton(Y);
        Y->setObjectName(QString::fromUtf8("Y"));
        sizePolicy.setHeightForWidth(Y->sizePolicy().hasHeightForWidth());
        Y->setSizePolicy(sizePolicy);

        KeyboardRow1->addWidget(Y);

        U = new QPushButton(NormalKeyboard);
        groupLetter->addButton(U);
        U->setObjectName(QString::fromUtf8("U"));
        sizePolicy.setHeightForWidth(U->sizePolicy().hasHeightForWidth());
        U->setSizePolicy(sizePolicy);

        KeyboardRow1->addWidget(U);

        I = new QPushButton(NormalKeyboard);
        groupLetter->addButton(I);
        I->setObjectName(QString::fromUtf8("I"));
        sizePolicy.setHeightForWidth(I->sizePolicy().hasHeightForWidth());
        I->setSizePolicy(sizePolicy);

        KeyboardRow1->addWidget(I);

        O = new QPushButton(NormalKeyboard);
        groupLetter->addButton(O);
        O->setObjectName(QString::fromUtf8("O"));
        sizePolicy.setHeightForWidth(O->sizePolicy().hasHeightForWidth());
        O->setSizePolicy(sizePolicy);

        KeyboardRow1->addWidget(O);

        P = new QPushButton(NormalKeyboard);
        groupLetter->addButton(P);
        P->setObjectName(QString::fromUtf8("P"));
        sizePolicy.setHeightForWidth(P->sizePolicy().hasHeightForWidth());
        P->setSizePolicy(sizePolicy);

        KeyboardRow1->addWidget(P);

        backspace = new QPushButton(NormalKeyboard);
        backspace->setObjectName(QString::fromUtf8("backspace"));
        sizePolicy.setHeightForWidth(backspace->sizePolicy().hasHeightForWidth());
        backspace->setSizePolicy(sizePolicy);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/backspace.png"), QSize(), QIcon::Normal, QIcon::Off);
        backspace->setIcon(icon);
        backspace->setIconSize(QSize(48, 48));
        backspace->setAutoRepeat(true);

        KeyboardRow1->addWidget(backspace);


        verticalLayout->addLayout(KeyboardRow1);

        KeyboardRow2 = new QHBoxLayout();
        KeyboardRow2->setObjectName(QString::fromUtf8("KeyboardRow2"));
        widget = new QWidget(NormalKeyboard);
        widget->setObjectName(QString::fromUtf8("widget"));
        sizePolicy.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy);

        KeyboardRow2->addWidget(widget);

        A = new QPushButton(NormalKeyboard);
        groupLetter->addButton(A);
        A->setObjectName(QString::fromUtf8("A"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(3);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(A->sizePolicy().hasHeightForWidth());
        A->setSizePolicy(sizePolicy1);

        KeyboardRow2->addWidget(A);

        S = new QPushButton(NormalKeyboard);
        groupLetter->addButton(S);
        S->setObjectName(QString::fromUtf8("S"));
        QSizePolicy sizePolicy2(QSizePolicy::Ignored, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(3);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(S->sizePolicy().hasHeightForWidth());
        S->setSizePolicy(sizePolicy2);

        KeyboardRow2->addWidget(S);

        D = new QPushButton(NormalKeyboard);
        groupLetter->addButton(D);
        D->setObjectName(QString::fromUtf8("D"));
        sizePolicy1.setHeightForWidth(D->sizePolicy().hasHeightForWidth());
        D->setSizePolicy(sizePolicy1);

        KeyboardRow2->addWidget(D);

        F = new QPushButton(NormalKeyboard);
        groupLetter->addButton(F);
        F->setObjectName(QString::fromUtf8("F"));
        sizePolicy1.setHeightForWidth(F->sizePolicy().hasHeightForWidth());
        F->setSizePolicy(sizePolicy1);

        KeyboardRow2->addWidget(F);

        G = new QPushButton(NormalKeyboard);
        groupLetter->addButton(G);
        G->setObjectName(QString::fromUtf8("G"));
        sizePolicy1.setHeightForWidth(G->sizePolicy().hasHeightForWidth());
        G->setSizePolicy(sizePolicy1);

        KeyboardRow2->addWidget(G);

        H = new QPushButton(NormalKeyboard);
        groupLetter->addButton(H);
        H->setObjectName(QString::fromUtf8("H"));
        sizePolicy1.setHeightForWidth(H->sizePolicy().hasHeightForWidth());
        H->setSizePolicy(sizePolicy1);

        KeyboardRow2->addWidget(H);

        J = new QPushButton(NormalKeyboard);
        groupLetter->addButton(J);
        J->setObjectName(QString::fromUtf8("J"));
        sizePolicy1.setHeightForWidth(J->sizePolicy().hasHeightForWidth());
        J->setSizePolicy(sizePolicy1);

        KeyboardRow2->addWidget(J);

        K = new QPushButton(NormalKeyboard);
        groupLetter->addButton(K);
        K->setObjectName(QString::fromUtf8("K"));
        sizePolicy1.setHeightForWidth(K->sizePolicy().hasHeightForWidth());
        K->setSizePolicy(sizePolicy1);

        KeyboardRow2->addWidget(K);

        L = new QPushButton(NormalKeyboard);
        groupLetter->addButton(L);
        L->setObjectName(QString::fromUtf8("L"));
        sizePolicy1.setHeightForWidth(L->sizePolicy().hasHeightForWidth());
        L->setSizePolicy(sizePolicy1);

        KeyboardRow2->addWidget(L);

        enter = new QPushButton(NormalKeyboard);
        enter->setObjectName(QString::fromUtf8("enter"));
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(5);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(enter->sizePolicy().hasHeightForWidth());
        enter->setSizePolicy(sizePolicy3);
        enter->setSizeIncrement(QSize(5, 0));
        enter->setBaseSize(QSize(80, 80));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/images/enter.png"), QSize(), QIcon::Normal, QIcon::Off);
        enter->setIcon(icon1);
        enter->setIconSize(QSize(48, 48));

        KeyboardRow2->addWidget(enter);


        verticalLayout->addLayout(KeyboardRow2);

        KeyboardRow3 = new QHBoxLayout();
        KeyboardRow3->setObjectName(QString::fromUtf8("KeyboardRow3"));
        shiftl = new QPushButton(NormalKeyboard);
        shiftl->setObjectName(QString::fromUtf8("shiftl"));
        QSizePolicy sizePolicy4(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(shiftl->sizePolicy().hasHeightForWidth());
        shiftl->setSizePolicy(sizePolicy4);
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/images/shift.png"), QSize(), QIcon::Normal, QIcon::Off);
        shiftl->setIcon(icon2);
        shiftl->setIconSize(QSize(48, 48));

        KeyboardRow3->addWidget(shiftl);

        Z = new QPushButton(NormalKeyboard);
        groupLetter->addButton(Z);
        Z->setObjectName(QString::fromUtf8("Z"));
        sizePolicy4.setHeightForWidth(Z->sizePolicy().hasHeightForWidth());
        Z->setSizePolicy(sizePolicy4);

        KeyboardRow3->addWidget(Z);

        X = new QPushButton(NormalKeyboard);
        groupLetter->addButton(X);
        X->setObjectName(QString::fromUtf8("X"));
        sizePolicy4.setHeightForWidth(X->sizePolicy().hasHeightForWidth());
        X->setSizePolicy(sizePolicy4);

        KeyboardRow3->addWidget(X);

        C = new QPushButton(NormalKeyboard);
        groupLetter->addButton(C);
        C->setObjectName(QString::fromUtf8("C"));
        sizePolicy4.setHeightForWidth(C->sizePolicy().hasHeightForWidth());
        C->setSizePolicy(sizePolicy4);

        KeyboardRow3->addWidget(C);

        V = new QPushButton(NormalKeyboard);
        groupLetter->addButton(V);
        V->setObjectName(QString::fromUtf8("V"));
        sizePolicy4.setHeightForWidth(V->sizePolicy().hasHeightForWidth());
        V->setSizePolicy(sizePolicy4);

        KeyboardRow3->addWidget(V);

        B = new QPushButton(NormalKeyboard);
        groupLetter->addButton(B);
        B->setObjectName(QString::fromUtf8("B"));
        sizePolicy4.setHeightForWidth(B->sizePolicy().hasHeightForWidth());
        B->setSizePolicy(sizePolicy4);

        KeyboardRow3->addWidget(B);

        N = new QPushButton(NormalKeyboard);
        groupLetter->addButton(N);
        N->setObjectName(QString::fromUtf8("N"));
        sizePolicy4.setHeightForWidth(N->sizePolicy().hasHeightForWidth());
        N->setSizePolicy(sizePolicy4);

        KeyboardRow3->addWidget(N);

        M = new QPushButton(NormalKeyboard);
        groupLetter->addButton(M);
        M->setObjectName(QString::fromUtf8("M"));
        sizePolicy4.setHeightForWidth(M->sizePolicy().hasHeightForWidth());
        M->setSizePolicy(sizePolicy4);

        KeyboardRow3->addWidget(M);

        at = new QPushButton(NormalKeyboard);
        at->setObjectName(QString::fromUtf8("at"));
        sizePolicy4.setHeightForWidth(at->sizePolicy().hasHeightForWidth());
        at->setSizePolicy(sizePolicy4);

        KeyboardRow3->addWidget(at);

        period = new QPushButton(NormalKeyboard);
        period->setObjectName(QString::fromUtf8("period"));
        sizePolicy4.setHeightForWidth(period->sizePolicy().hasHeightForWidth());
        period->setSizePolicy(sizePolicy4);

        KeyboardRow3->addWidget(period);

        shiftr = new QPushButton(NormalKeyboard);
        shiftr->setObjectName(QString::fromUtf8("shiftr"));
        sizePolicy4.setHeightForWidth(shiftr->sizePolicy().hasHeightForWidth());
        shiftr->setSizePolicy(sizePolicy4);
        shiftr->setIcon(icon2);
        shiftr->setIconSize(QSize(48, 48));
        shiftr->setCheckable(true);
        shiftr->setChecked(true);

        KeyboardRow3->addWidget(shiftr);


        verticalLayout->addLayout(KeyboardRow3);

        KeyboardRow4 = new QHBoxLayout();
        KeyboardRow4->setObjectName(QString::fromUtf8("KeyboardRow4"));
        symbol = new QPushButton(NormalKeyboard);
        symbol->setObjectName(QString::fromUtf8("symbol"));
        QSizePolicy sizePolicy5(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy5.setHorizontalStretch(4);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(symbol->sizePolicy().hasHeightForWidth());
        symbol->setSizePolicy(sizePolicy5);

        KeyboardRow4->addWidget(symbol);

        language = new QPushButton(NormalKeyboard);
        language->setObjectName(QString::fromUtf8("language"));
        sizePolicy1.setHeightForWidth(language->sizePolicy().hasHeightForWidth());
        language->setSizePolicy(sizePolicy1);
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/images/globe.png"), QSize(), QIcon::Normal, QIcon::Off);
        language->setIcon(icon3);
        language->setIconSize(QSize(48, 48));

        KeyboardRow4->addWidget(language);

        space = new QPushButton(NormalKeyboard);
        space->setObjectName(QString::fromUtf8("space"));
        QSizePolicy sizePolicy6(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy6.setHorizontalStretch(17);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(space->sizePolicy().hasHeightForWidth());
        space->setSizePolicy(sizePolicy6);

        KeyboardRow4->addWidget(space);

        apostrophe = new QPushButton(NormalKeyboard);
        apostrophe->setObjectName(QString::fromUtf8("apostrophe"));
        sizePolicy1.setHeightForWidth(apostrophe->sizePolicy().hasHeightForWidth());
        apostrophe->setSizePolicy(sizePolicy1);

        KeyboardRow4->addWidget(apostrophe);

        minus = new QPushButton(NormalKeyboard);
        minus->setObjectName(QString::fromUtf8("minus"));
        sizePolicy1.setHeightForWidth(minus->sizePolicy().hasHeightForWidth());
        minus->setSizePolicy(sizePolicy1);

        KeyboardRow4->addWidget(minus);

        hide = new QPushButton(NormalKeyboard);
        hide->setObjectName(QString::fromUtf8("hide"));
        sizePolicy5.setHeightForWidth(hide->sizePolicy().hasHeightForWidth());
        hide->setSizePolicy(sizePolicy5);
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/images/hidekeyboard.png"), QSize(), QIcon::Normal, QIcon::Off);
        hide->setIcon(icon4);
        hide->setIconSize(QSize(48, 48));

        KeyboardRow4->addWidget(hide);


        verticalLayout->addLayout(KeyboardRow4);


        retranslateUi(NormalKeyboard);

        QMetaObject::connectSlotsByName(NormalKeyboard);
    } // setupUi

    void retranslateUi(QWidget *NormalKeyboard)
    {
        NormalKeyboard->setWindowTitle(QCoreApplication::translate("NormalKeyboard", "Form", nullptr));
        Q->setText(QCoreApplication::translate("NormalKeyboard", "Q", nullptr));
        W->setText(QCoreApplication::translate("NormalKeyboard", "W", nullptr));
        E->setText(QCoreApplication::translate("NormalKeyboard", "E", nullptr));
        R->setText(QCoreApplication::translate("NormalKeyboard", "R", nullptr));
        T->setText(QCoreApplication::translate("NormalKeyboard", "T", nullptr));
        Y->setText(QCoreApplication::translate("NormalKeyboard", "Y", nullptr));
        U->setText(QCoreApplication::translate("NormalKeyboard", "U", nullptr));
        I->setText(QCoreApplication::translate("NormalKeyboard", "I", nullptr));
        O->setText(QCoreApplication::translate("NormalKeyboard", "O", nullptr));
        P->setText(QCoreApplication::translate("NormalKeyboard", "P", nullptr));
        backspace->setText(QString());
        A->setText(QCoreApplication::translate("NormalKeyboard", "A", nullptr));
        S->setText(QCoreApplication::translate("NormalKeyboard", "S", nullptr));
        D->setText(QCoreApplication::translate("NormalKeyboard", "D", nullptr));
        F->setText(QCoreApplication::translate("NormalKeyboard", "F", nullptr));
        G->setText(QCoreApplication::translate("NormalKeyboard", "G", nullptr));
        H->setText(QCoreApplication::translate("NormalKeyboard", "H", nullptr));
        J->setText(QCoreApplication::translate("NormalKeyboard", "J", nullptr));
        K->setText(QCoreApplication::translate("NormalKeyboard", "K", nullptr));
        L->setText(QCoreApplication::translate("NormalKeyboard", "L", nullptr));
        enter->setText(QString());
        shiftl->setText(QString());
        Z->setText(QCoreApplication::translate("NormalKeyboard", "Z", nullptr));
        X->setText(QCoreApplication::translate("NormalKeyboard", "X", nullptr));
        C->setText(QCoreApplication::translate("NormalKeyboard", "C", nullptr));
        V->setText(QCoreApplication::translate("NormalKeyboard", "V", nullptr));
        B->setText(QCoreApplication::translate("NormalKeyboard", "B", nullptr));
        N->setText(QCoreApplication::translate("NormalKeyboard", "N", nullptr));
        M->setText(QCoreApplication::translate("NormalKeyboard", "M", nullptr));
        at->setText(QCoreApplication::translate("NormalKeyboard", "@", nullptr));
        period->setText(QCoreApplication::translate("NormalKeyboard", ".", nullptr));
        shiftr->setText(QString());
        symbol->setText(QCoreApplication::translate("NormalKeyboard", ".?123", nullptr));
        language->setText(QString());
        space->setText(QCoreApplication::translate("NormalKeyboard", "English", nullptr));
        apostrophe->setText(QCoreApplication::translate("NormalKeyboard", "'", nullptr));
        minus->setText(QCoreApplication::translate("NormalKeyboard", "-", nullptr));
        hide->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class NormalKeyboard: public Ui_NormalKeyboard {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NORMALKEYBOARD_H
