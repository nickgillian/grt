/********************************************************************************
** Form generated from reading UI file 'timeseriesgraph.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TIMESERIESGRAPH_H
#define UI_TIMESERIESGRAPH_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_TimeseriesGraph
{
public:
    QCustomPlot *graph;
    QPushButton *snapShotButton;

    void setupUi(QWidget *TimeseriesGraph)
    {
        if (TimeseriesGraph->objectName().isEmpty())
            TimeseriesGraph->setObjectName(QString::fromUtf8("TimeseriesGraph"));
        TimeseriesGraph->resize(400, 196);
        TimeseriesGraph->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255)"));
        graph = new QCustomPlot(TimeseriesGraph);
        graph->setObjectName(QString::fromUtf8("graph"));
        graph->setGeometry(QRect(10, 10, 381, 181));
        snapShotButton = new QPushButton(TimeseriesGraph);
        snapShotButton->setObjectName(QString::fromUtf8("snapShotButton"));
        snapShotButton->setGeometry(QRect(380, 0, 22, 22));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/snapShot.png"), QSize(), QIcon::Normal, QIcon::Off);
        snapShotButton->setIcon(icon);

        retranslateUi(TimeseriesGraph);

        QMetaObject::connectSlotsByName(TimeseriesGraph);
    } // setupUi

    void retranslateUi(QWidget *TimeseriesGraph)
    {
        TimeseriesGraph->setWindowTitle(QApplication::translate("TimeseriesGraph", "Form", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        snapShotButton->setToolTip(QApplication::translate("TimeseriesGraph", "take a snapShot", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        snapShotButton->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class TimeseriesGraph: public Ui_TimeseriesGraph {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TIMESERIESGRAPH_H
