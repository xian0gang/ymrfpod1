/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 5.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *masterport_lineEdit;
    QLabel *label_2;
    QLineEdit *testport_lineEdit;
    QLabel *label_5;
    QLineEdit *uart_lineEdit;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_3;
    QLineEdit *ip_lineEdit;
    QLabel *label_4;
    QLineEdit *port_lineEdit;
    QTextBrowser *textBrowser;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QStringLiteral("Widget"));
        Widget->resize(379, 449);
        gridLayout = new QGridLayout(Widget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(Widget);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        masterport_lineEdit = new QLineEdit(Widget);
        masterport_lineEdit->setObjectName(QStringLiteral("masterport_lineEdit"));

        horizontalLayout->addWidget(masterport_lineEdit);

        label_2 = new QLabel(Widget);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout->addWidget(label_2);

        testport_lineEdit = new QLineEdit(Widget);
        testport_lineEdit->setObjectName(QStringLiteral("testport_lineEdit"));

        horizontalLayout->addWidget(testport_lineEdit);

        label_5 = new QLabel(Widget);
        label_5->setObjectName(QStringLiteral("label_5"));

        horizontalLayout->addWidget(label_5);

        uart_lineEdit = new QLineEdit(Widget);
        uart_lineEdit->setObjectName(QStringLiteral("uart_lineEdit"));
        uart_lineEdit->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(uart_lineEdit);


        gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_3 = new QLabel(Widget);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout_3->addWidget(label_3);

        ip_lineEdit = new QLineEdit(Widget);
        ip_lineEdit->setObjectName(QStringLiteral("ip_lineEdit"));

        horizontalLayout_3->addWidget(ip_lineEdit);

        label_4 = new QLabel(Widget);
        label_4->setObjectName(QStringLiteral("label_4"));

        horizontalLayout_3->addWidget(label_4);

        port_lineEdit = new QLineEdit(Widget);
        port_lineEdit->setObjectName(QStringLiteral("port_lineEdit"));

        horizontalLayout_3->addWidget(port_lineEdit);


        gridLayout->addLayout(horizontalLayout_3, 3, 0, 1, 1);

        textBrowser = new QTextBrowser(Widget);
        textBrowser->setObjectName(QStringLiteral("textBrowser"));

        gridLayout->addWidget(textBrowser, 4, 0, 1, 1);


        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QApplication::translate("Widget", "Widget", 0));
        label->setText(QApplication::translate("Widget", "\344\270\273\346\216\247\346\234\215\345\212\241\347\253\257\345\217\243:", 0));
        label_2->setText(QApplication::translate("Widget", "\346\265\213\350\257\225\347\253\257\345\217\243:", 0));
        label_5->setText(QApplication::translate("Widget", "\344\270\262\345\217\243\347\253\257\345\217\243\345\217\267\357\274\232", 0));
        label_3->setText(QApplication::translate("Widget", "\347\203\255\345\203\217\344\273\2522 IP:", 0));
        label_4->setText(QApplication::translate("Widget", "\347\203\255\345\203\217\344\273\2522 PORT\357\274\232", 0));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
