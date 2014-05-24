#include "versioninfo.h"
#include "ui_versioninfo.h"

VersionInfo::VersionInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VersionInfo)
{
    ui->setupUi(this);
}

VersionInfo::~VersionInfo()
{
    delete ui;
}

bool VersionInfo::setVersionText( const QString &versionText ){
    ui->versionLabel->setText( versionText );
    return true;
}

bool VersionInfo::setRevisionText( const QString &revisionText ){
    ui->revisionLabel->setText( revisionText );
    return true;
}

bool VersionInfo::setDevelopersText( const QString &developersText ){
    ui->developersLabel->setText( developersText );
    return true;
}
