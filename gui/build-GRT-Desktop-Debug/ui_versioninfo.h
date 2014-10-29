/********************************************************************************
** Form generated from reading UI file 'versioninfo.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VERSIONINFO_H
#define UI_VERSIONINFO_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>

QT_BEGIN_NAMESPACE

class Ui_VersionInfo
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *label_1;
    QLabel *versionLabel;
    QLabel *label_3;
    QLabel *revisionLabel;
    QLabel *versionLabel_2;
    QLabel *label_5;
    QLabel *developersLabel;

    void setupUi(QDialog *VersionInfo)
    {
        if (VersionInfo->objectName().isEmpty())
            VersionInfo->setObjectName(QString::fromUtf8("VersionInfo"));
        VersionInfo->resize(400, 300);
        buttonBox = new QDialogButtonBox(VersionInfo);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(30, 240, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        label_1 = new QLabel(VersionInfo);
        label_1->setObjectName(QString::fromUtf8("label_1"));
        label_1->setGeometry(QRect(20, 60, 121, 41));
        versionLabel = new QLabel(VersionInfo);
        versionLabel->setObjectName(QString::fromUtf8("versionLabel"));
        versionLabel->setGeometry(QRect(90, 60, 101, 41));
        label_3 = new QLabel(VersionInfo);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(20, 100, 171, 41));
        revisionLabel = new QLabel(VersionInfo);
        revisionLabel->setObjectName(QString::fromUtf8("revisionLabel"));
        revisionLabel->setGeometry(QRect(190, 100, 171, 41));
        versionLabel_2 = new QLabel(VersionInfo);
        versionLabel_2->setObjectName(QString::fromUtf8("versionLabel_2"));
        versionLabel_2->setGeometry(QRect(10, 10, 381, 41));
        label_5 = new QLabel(VersionInfo);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(20, 140, 91, 41));
        developersLabel = new QLabel(VersionInfo);
        developersLabel->setObjectName(QString::fromUtf8("developersLabel"));
        developersLabel->setGeometry(QRect(120, 140, 171, 41));

        retranslateUi(VersionInfo);
        QObject::connect(buttonBox, SIGNAL(accepted()), VersionInfo, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), VersionInfo, SLOT(reject()));

        QMetaObject::connectSlotsByName(VersionInfo);
    } // setupUi

    void retranslateUi(QDialog *VersionInfo)
    {
        VersionInfo->setWindowTitle(QApplication::translate("VersionInfo", "Dialog", 0, QApplication::UnicodeUTF8));
        label_1->setText(QApplication::translate("VersionInfo", "<html><head/><body><p><span style=\" font-size:14pt;\">Version:</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        versionLabel->setText(QApplication::translate("VersionInfo", "<html><head/><body><p><span style=\" font-size:14pt;\">VERSION</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("VersionInfo", "<html><head/><body><p><span style=\" font-size:14pt;\">Based on GRT revision: </span></p></body></html>", 0, QApplication::UnicodeUTF8));
        revisionLabel->setText(QApplication::translate("VersionInfo", "<html><head/><body><p><span style=\" font-size:14pt;\">GRT REVISION</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        versionLabel_2->setText(QApplication::translate("VersionInfo", "<html><head/><body><p align=\"center\"><span style=\" font-size:18pt;\">Gesture Recognition Toolkit</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("VersionInfo", "<html><head/><body><p><span style=\" font-size:14pt;\">Developers:</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        developersLabel->setText(QApplication::translate("VersionInfo", "<html><head/><body><p><span style=\" font-size:14pt;\">DEVELOPERS</span></p></body></html>", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class VersionInfo: public Ui_VersionInfo {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VERSIONINFO_H
