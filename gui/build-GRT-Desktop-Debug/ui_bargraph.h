/********************************************************************************
** Form generated from reading UI file 'bargraph.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BARGRAPH_H
#define UI_BARGRAPH_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_BarGraph
{
public:
    QCustomPlot *graph;

    void setupUi(QWidget *BarGraph)
    {
        if (BarGraph->objectName().isEmpty())
            BarGraph->setObjectName(QString::fromUtf8("BarGraph"));
        BarGraph->resize(400, 221);
        BarGraph->setStyleSheet(QString::fromUtf8("background-color: rgb(255,255,255);"));
        graph = new QCustomPlot(BarGraph);
        graph->setObjectName(QString::fromUtf8("graph"));
        graph->setGeometry(QRect(0, 10, 391, 201));

        retranslateUi(BarGraph);

        QMetaObject::connectSlotsByName(BarGraph);
    } // setupUi

    void retranslateUi(QWidget *BarGraph)
    {
        BarGraph->setWindowTitle(QApplication::translate("BarGraph", "Form", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class BarGraph: public Ui_BarGraph {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BARGRAPH_H
