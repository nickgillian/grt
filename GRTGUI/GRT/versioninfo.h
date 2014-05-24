#ifndef VERSIONINFO_H
#define VERSIONINFO_H

#include <QDialog>

namespace Ui {
class VersionInfo;
}

class VersionInfo : public QDialog
{
    Q_OBJECT
    
public:
    explicit VersionInfo(QWidget *parent = 0);
    ~VersionInfo();

    bool setVersionText( const QString &versionText );
    bool setRevisionText( const QString &revisionText );
    bool setDevelopersText( const QString &developersText );
    
private:
    Ui::VersionInfo *ui;
};

#endif // VERSIONINFO_H
