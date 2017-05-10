/********************************************************************************
** Form generated from reading UI file 'qtopengl.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTOPENGL_H
#define UI_QTOPENGL_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_qtopenglClass
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QLabel *label;

    void setupUi(QMainWindow *qtopenglClass)
    {
        if (qtopenglClass->objectName().isEmpty())
            qtopenglClass->setObjectName(QStringLiteral("qtopenglClass"));
        qtopenglClass->resize(600, 400);
        centralWidget = new QWidget(qtopenglClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(0);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout->addWidget(label);

        qtopenglClass->setCentralWidget(centralWidget);

        retranslateUi(qtopenglClass);

        QMetaObject::connectSlotsByName(qtopenglClass);
    } // setupUi

    void retranslateUi(QMainWindow *qtopenglClass)
    {
        qtopenglClass->setWindowTitle(QApplication::translate("qtopenglClass", "qtopengl", Q_NULLPTR));
        label->setText(QApplication::translate("qtopenglClass", "TextLabel", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class qtopenglClass: public Ui_qtopenglClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTOPENGL_H
