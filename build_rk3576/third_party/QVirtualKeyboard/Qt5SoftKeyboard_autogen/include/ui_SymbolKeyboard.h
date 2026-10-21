/********************************************************************************
** Form generated from reading UI file 'SymbolKeyboard.ui'
**
** Created by: Qt User Interface Compiler version 5.15.11
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SYMBOLKEYBOARD_H
#define UI_SYMBOLKEYBOARD_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SymbolKeyboard
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *btn1;
    QPushButton *btn2;
    QPushButton *btn3;
    QPushButton *btn4;
    QPushButton *btn5;
    QPushButton *btn6;
    QPushButton *btn7;
    QPushButton *btn8;
    QPushButton *btn9;
    QPushButton *btn10;
    QPushButton *backspace;
    QHBoxLayout *horizontalLayout_2;
    QWidget *widget;
    QPushButton *btn11;
    QPushButton *btn12;
    QPushButton *btn13;
    QPushButton *btn14;
    QPushButton *btn15;
    QPushButton *btn16;
    QPushButton *btn17;
    QPushButton *btn18;
    QPushButton *btn19;
    QPushButton *enter;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *pagel;
    QPushButton *btn20;
    QPushButton *btn21;
    QPushButton *btn22;
    QPushButton *btn23;
    QPushButton *btn24;
    QPushButton *btn25;
    QPushButton *btn26;
    QPushButton *btn27;
    QPushButton *btn28;
    QPushButton *pager;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *symbol;
    QPushButton *language;
    QPushButton *space;
    QPushButton *btn29;
    QPushButton *btn30;
    QPushButton *hide;
    QButtonGroup *buttonGroup;

    void setupUi(QWidget *SymbolKeyboard)
    {
        if (SymbolKeyboard->objectName().isEmpty())
            SymbolKeyboard->setObjectName(QString::fromUtf8("SymbolKeyboard"));
        SymbolKeyboard->resize(982, 315);
        SymbolKeyboard->setStyleSheet(QString::fromUtf8(""));
        verticalLayout = new QVBoxLayout(SymbolKeyboard);
        verticalLayout->setSpacing(10);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        btn1 = new QPushButton(SymbolKeyboard);
        buttonGroup = new QButtonGroup(SymbolKeyboard);
        buttonGroup->setObjectName(QString::fromUtf8("buttonGroup"));
        buttonGroup->addButton(btn1);
        btn1->setObjectName(QString::fromUtf8("btn1"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(btn1->sizePolicy().hasHeightForWidth());
        btn1->setSizePolicy(sizePolicy);
        btn1->setSizeIncrement(QSize(4, 3));
        btn1->setBaseSize(QSize(80, 60));

        horizontalLayout->addWidget(btn1);

        btn2 = new QPushButton(SymbolKeyboard);
        buttonGroup->addButton(btn2);
        btn2->setObjectName(QString::fromUtf8("btn2"));
        sizePolicy.setHeightForWidth(btn2->sizePolicy().hasHeightForWidth());
        btn2->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(btn2);

        btn3 = new QPushButton(SymbolKeyboard);
        buttonGroup->addButton(btn3);
        btn3->setObjectName(QString::fromUtf8("btn3"));
        sizePolicy.setHeightForWidth(btn3->sizePolicy().hasHeightForWidth());
        btn3->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(btn3);

        btn4 = new QPushButton(SymbolKeyboard);
        buttonGroup->addButton(btn4);
        btn4->setObjectName(QString::fromUtf8("btn4"));
        sizePolicy.setHeightForWidth(btn4->sizePolicy().hasHeightForWidth());
        btn4->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(btn4);

        btn5 = new QPushButton(SymbolKeyboard);
        buttonGroup->addButton(btn5);
        btn5->setObjectName(QString::fromUtf8("btn5"));
        sizePolicy.setHeightForWidth(btn5->sizePolicy().hasHeightForWidth());
        btn5->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(btn5);

        btn6 = new QPushButton(SymbolKeyboard);
        buttonGroup->addButton(btn6);
        btn6->setObjectName(QString::fromUtf8("btn6"));
        sizePolicy.setHeightForWidth(btn6->sizePolicy().hasHeightForWidth());
        btn6->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(btn6);

        btn7 = new QPushButton(SymbolKeyboard);
        buttonGroup->addButton(btn7);
        btn7->setObjectName(QString::fromUtf8("btn7"));
        sizePolicy.setHeightForWidth(btn7->sizePolicy().hasHeightForWidth());
        btn7->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(btn7);

        btn8 = new QPushButton(SymbolKeyboard);
        buttonGroup->addButton(btn8);
        btn8->setObjectName(QString::fromUtf8("btn8"));
        sizePolicy.setHeightForWidth(btn8->sizePolicy().hasHeightForWidth());
        btn8->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(btn8);

        btn9 = new QPushButton(SymbolKeyboard);
        buttonGroup->addButton(btn9);
        btn9->setObjectName(QString::fromUtf8("btn9"));
        sizePolicy.setHeightForWidth(btn9->sizePolicy().hasHeightForWidth());
        btn9->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(btn9);

        btn10 = new QPushButton(SymbolKeyboard);
        buttonGroup->addButton(btn10);
        btn10->setObjectName(QString::fromUtf8("btn10"));
        sizePolicy.setHeightForWidth(btn10->sizePolicy().hasHeightForWidth());
        btn10->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(btn10);

        backspace = new QPushButton(SymbolKeyboard);
        backspace->setObjectName(QString::fromUtf8("backspace"));
        sizePolicy.setHeightForWidth(backspace->sizePolicy().hasHeightForWidth());
        backspace->setSizePolicy(sizePolicy);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/backspace.png"), QSize(), QIcon::Normal, QIcon::Off);
        backspace->setIcon(icon);
        backspace->setIconSize(QSize(48, 48));

        horizontalLayout->addWidget(backspace);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        widget = new QWidget(SymbolKeyboard);
        widget->setObjectName(QString::fromUtf8("widget"));
        sizePolicy.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy);

        horizontalLayout_2->addWidget(widget);

        btn11 = new QPushButton(SymbolKeyboard);
        buttonGroup->addButton(btn11);
        btn11->setObjectName(QString::fromUtf8("btn11"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(3);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(btn11->sizePolicy().hasHeightForWidth());
        btn11->setSizePolicy(sizePolicy1);

        horizontalLayout_2->addWidget(btn11);

        btn12 = new QPushButton(SymbolKeyboard);
        buttonGroup->addButton(btn12);
        btn12->setObjectName(QString::fromUtf8("btn12"));
        QSizePolicy sizePolicy2(QSizePolicy::Ignored, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(3);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(btn12->sizePolicy().hasHeightForWidth());
        btn12->setSizePolicy(sizePolicy2);

        horizontalLayout_2->addWidget(btn12);

        btn13 = new QPushButton(SymbolKeyboard);
        buttonGroup->addButton(btn13);
        btn13->setObjectName(QString::fromUtf8("btn13"));
        sizePolicy1.setHeightForWidth(btn13->sizePolicy().hasHeightForWidth());
        btn13->setSizePolicy(sizePolicy1);

        horizontalLayout_2->addWidget(btn13);

        btn14 = new QPushButton(SymbolKeyboard);
        buttonGroup->addButton(btn14);
        btn14->setObjectName(QString::fromUtf8("btn14"));
        sizePolicy1.setHeightForWidth(btn14->sizePolicy().hasHeightForWidth());
        btn14->setSizePolicy(sizePolicy1);

        horizontalLayout_2->addWidget(btn14);

        btn15 = new QPushButton(SymbolKeyboard);
        buttonGroup->addButton(btn15);
        btn15->setObjectName(QString::fromUtf8("btn15"));
        sizePolicy1.setHeightForWidth(btn15->sizePolicy().hasHeightForWidth());
        btn15->setSizePolicy(sizePolicy1);

        horizontalLayout_2->addWidget(btn15);

        btn16 = new QPushButton(SymbolKeyboard);
        buttonGroup->addButton(btn16);
        btn16->setObjectName(QString::fromUtf8("btn16"));
        sizePolicy1.setHeightForWidth(btn16->sizePolicy().hasHeightForWidth());
        btn16->setSizePolicy(sizePolicy1);

        horizontalLayout_2->addWidget(btn16);

        btn17 = new QPushButton(SymbolKeyboard);
        buttonGroup->addButton(btn17);
        btn17->setObjectName(QString::fromUtf8("btn17"));
        sizePolicy1.setHeightForWidth(btn17->sizePolicy().hasHeightForWidth());
        btn17->setSizePolicy(sizePolicy1);

        horizontalLayout_2->addWidget(btn17);

        btn18 = new QPushButton(SymbolKeyboard);
        buttonGroup->addButton(btn18);
        btn18->setObjectName(QString::fromUtf8("btn18"));
        sizePolicy1.setHeightForWidth(btn18->sizePolicy().hasHeightForWidth());
        btn18->setSizePolicy(sizePolicy1);

        horizontalLayout_2->addWidget(btn18);

        btn19 = new QPushButton(SymbolKeyboard);
        buttonGroup->addButton(btn19);
        btn19->setObjectName(QString::fromUtf8("btn19"));
        sizePolicy1.setHeightForWidth(btn19->sizePolicy().hasHeightForWidth());
        btn19->setSizePolicy(sizePolicy1);

        horizontalLayout_2->addWidget(btn19);

        enter = new QPushButton(SymbolKeyboard);
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

        horizontalLayout_2->addWidget(enter);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        pagel = new QPushButton(SymbolKeyboard);
        pagel->setObjectName(QString::fromUtf8("pagel"));
        QSizePolicy sizePolicy4(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(pagel->sizePolicy().hasHeightForWidth());
        pagel->setSizePolicy(sizePolicy4);
        pagel->setIconSize(QSize(48, 48));

        horizontalLayout_3->addWidget(pagel);

        btn20 = new QPushButton(SymbolKeyboard);
        buttonGroup->addButton(btn20);
        btn20->setObjectName(QString::fromUtf8("btn20"));
        sizePolicy4.setHeightForWidth(btn20->sizePolicy().hasHeightForWidth());
        btn20->setSizePolicy(sizePolicy4);

        horizontalLayout_3->addWidget(btn20);

        btn21 = new QPushButton(SymbolKeyboard);
        buttonGroup->addButton(btn21);
        btn21->setObjectName(QString::fromUtf8("btn21"));
        sizePolicy4.setHeightForWidth(btn21->sizePolicy().hasHeightForWidth());
        btn21->setSizePolicy(sizePolicy4);

        horizontalLayout_3->addWidget(btn21);

        btn22 = new QPushButton(SymbolKeyboard);
        buttonGroup->addButton(btn22);
        btn22->setObjectName(QString::fromUtf8("btn22"));
        sizePolicy4.setHeightForWidth(btn22->sizePolicy().hasHeightForWidth());
        btn22->setSizePolicy(sizePolicy4);

        horizontalLayout_3->addWidget(btn22);

        btn23 = new QPushButton(SymbolKeyboard);
        buttonGroup->addButton(btn23);
        btn23->setObjectName(QString::fromUtf8("btn23"));
        sizePolicy4.setHeightForWidth(btn23->sizePolicy().hasHeightForWidth());
        btn23->setSizePolicy(sizePolicy4);

        horizontalLayout_3->addWidget(btn23);

        btn24 = new QPushButton(SymbolKeyboard);
        buttonGroup->addButton(btn24);
        btn24->setObjectName(QString::fromUtf8("btn24"));
        sizePolicy4.setHeightForWidth(btn24->sizePolicy().hasHeightForWidth());
        btn24->setSizePolicy(sizePolicy4);

        horizontalLayout_3->addWidget(btn24);

        btn25 = new QPushButton(SymbolKeyboard);
        buttonGroup->addButton(btn25);
        btn25->setObjectName(QString::fromUtf8("btn25"));
        sizePolicy4.setHeightForWidth(btn25->sizePolicy().hasHeightForWidth());
        btn25->setSizePolicy(sizePolicy4);

        horizontalLayout_3->addWidget(btn25);

        btn26 = new QPushButton(SymbolKeyboard);
        buttonGroup->addButton(btn26);
        btn26->setObjectName(QString::fromUtf8("btn26"));
        sizePolicy4.setHeightForWidth(btn26->sizePolicy().hasHeightForWidth());
        btn26->setSizePolicy(sizePolicy4);

        horizontalLayout_3->addWidget(btn26);

        btn27 = new QPushButton(SymbolKeyboard);
        buttonGroup->addButton(btn27);
        btn27->setObjectName(QString::fromUtf8("btn27"));
        sizePolicy4.setHeightForWidth(btn27->sizePolicy().hasHeightForWidth());
        btn27->setSizePolicy(sizePolicy4);

        horizontalLayout_3->addWidget(btn27);

        btn28 = new QPushButton(SymbolKeyboard);
        buttonGroup->addButton(btn28);
        btn28->setObjectName(QString::fromUtf8("btn28"));
        sizePolicy4.setHeightForWidth(btn28->sizePolicy().hasHeightForWidth());
        btn28->setSizePolicy(sizePolicy4);

        horizontalLayout_3->addWidget(btn28);

        pager = new QPushButton(SymbolKeyboard);
        pager->setObjectName(QString::fromUtf8("pager"));
        sizePolicy4.setHeightForWidth(pager->sizePolicy().hasHeightForWidth());
        pager->setSizePolicy(sizePolicy4);
        pager->setIconSize(QSize(48, 48));
        pager->setCheckable(true);
        pager->setChecked(true);

        horizontalLayout_3->addWidget(pager);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        symbol = new QPushButton(SymbolKeyboard);
        symbol->setObjectName(QString::fromUtf8("symbol"));
        QSizePolicy sizePolicy5(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy5.setHorizontalStretch(4);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(symbol->sizePolicy().hasHeightForWidth());
        symbol->setSizePolicy(sizePolicy5);

        horizontalLayout_4->addWidget(symbol);

        language = new QPushButton(SymbolKeyboard);
        language->setObjectName(QString::fromUtf8("language"));
        sizePolicy1.setHeightForWidth(language->sizePolicy().hasHeightForWidth());
        language->setSizePolicy(sizePolicy1);
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/images/globe.png"), QSize(), QIcon::Normal, QIcon::Off);
        language->setIcon(icon2);
        language->setIconSize(QSize(48, 48));

        horizontalLayout_4->addWidget(language);

        space = new QPushButton(SymbolKeyboard);
        space->setObjectName(QString::fromUtf8("space"));
        QSizePolicy sizePolicy6(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy6.setHorizontalStretch(17);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(space->sizePolicy().hasHeightForWidth());
        space->setSizePolicy(sizePolicy6);

        horizontalLayout_4->addWidget(space);

        btn29 = new QPushButton(SymbolKeyboard);
        buttonGroup->addButton(btn29);
        btn29->setObjectName(QString::fromUtf8("btn29"));
        sizePolicy1.setHeightForWidth(btn29->sizePolicy().hasHeightForWidth());
        btn29->setSizePolicy(sizePolicy1);

        horizontalLayout_4->addWidget(btn29);

        btn30 = new QPushButton(SymbolKeyboard);
        buttonGroup->addButton(btn30);
        btn30->setObjectName(QString::fromUtf8("btn30"));
        sizePolicy1.setHeightForWidth(btn30->sizePolicy().hasHeightForWidth());
        btn30->setSizePolicy(sizePolicy1);

        horizontalLayout_4->addWidget(btn30);

        hide = new QPushButton(SymbolKeyboard);
        hide->setObjectName(QString::fromUtf8("hide"));
        sizePolicy5.setHeightForWidth(hide->sizePolicy().hasHeightForWidth());
        hide->setSizePolicy(sizePolicy5);
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/images/hidekeyboard.png"), QSize(), QIcon::Normal, QIcon::Off);
        hide->setIcon(icon3);
        hide->setIconSize(QSize(48, 48));

        horizontalLayout_4->addWidget(hide);


        verticalLayout->addLayout(horizontalLayout_4);


        retranslateUi(SymbolKeyboard);

        QMetaObject::connectSlotsByName(SymbolKeyboard);
    } // setupUi

    void retranslateUi(QWidget *SymbolKeyboard)
    {
        SymbolKeyboard->setWindowTitle(QCoreApplication::translate("SymbolKeyboard", "Form", nullptr));
        btn1->setText(QCoreApplication::translate("SymbolKeyboard", "1", nullptr));
        btn2->setText(QCoreApplication::translate("SymbolKeyboard", "2", nullptr));
        btn3->setText(QCoreApplication::translate("SymbolKeyboard", "3", nullptr));
        btn4->setText(QCoreApplication::translate("SymbolKeyboard", "4", nullptr));
        btn5->setText(QCoreApplication::translate("SymbolKeyboard", "5", nullptr));
        btn6->setText(QCoreApplication::translate("SymbolKeyboard", "6", nullptr));
        btn7->setText(QCoreApplication::translate("SymbolKeyboard", "7", nullptr));
        btn8->setText(QCoreApplication::translate("SymbolKeyboard", "8", nullptr));
        btn9->setText(QCoreApplication::translate("SymbolKeyboard", "9", nullptr));
        btn10->setText(QCoreApplication::translate("SymbolKeyboard", "0", nullptr));
        backspace->setText(QString());
        btn11->setText(QCoreApplication::translate("SymbolKeyboard", "@", nullptr));
        btn12->setText(QCoreApplication::translate("SymbolKeyboard", "#", nullptr));
        btn13->setText(QCoreApplication::translate("SymbolKeyboard", "%", nullptr));
        btn14->setText(QCoreApplication::translate("SymbolKeyboard", "&&", nullptr));
        btn15->setText(QCoreApplication::translate("SymbolKeyboard", "*", nullptr));
        btn16->setText(QCoreApplication::translate("SymbolKeyboard", "-", nullptr));
        btn17->setText(QCoreApplication::translate("SymbolKeyboard", "+", nullptr));
        btn18->setText(QCoreApplication::translate("SymbolKeyboard", "(", nullptr));
        btn19->setText(QCoreApplication::translate("SymbolKeyboard", ")", nullptr));
        enter->setText(QString());
        pagel->setText(QCoreApplication::translate("SymbolKeyboard", "1/2", nullptr));
        btn20->setText(QCoreApplication::translate("SymbolKeyboard", "!", nullptr));
        btn21->setText(QCoreApplication::translate("SymbolKeyboard", "\"", nullptr));
        btn22->setText(QCoreApplication::translate("SymbolKeyboard", "<", nullptr));
        btn23->setText(QCoreApplication::translate("SymbolKeyboard", ">", nullptr));
        btn24->setText(QCoreApplication::translate("SymbolKeyboard", "'", nullptr));
        btn25->setText(QCoreApplication::translate("SymbolKeyboard", ":", nullptr));
        btn26->setText(QCoreApplication::translate("SymbolKeyboard", ";", nullptr));
        btn27->setText(QCoreApplication::translate("SymbolKeyboard", "/", nullptr));
        btn28->setText(QCoreApplication::translate("SymbolKeyboard", "?", nullptr));
        pager->setText(QCoreApplication::translate("SymbolKeyboard", "1/2", nullptr));
        symbol->setText(QCoreApplication::translate("SymbolKeyboard", "ABC", nullptr));
        language->setText(QString());
        space->setText(QString());
        btn29->setText(QCoreApplication::translate("SymbolKeyboard", ".", nullptr));
        btn30->setText(QCoreApplication::translate("SymbolKeyboard", ":-)", nullptr));
        hide->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class SymbolKeyboard: public Ui_SymbolKeyboard {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SYMBOLKEYBOARD_H
