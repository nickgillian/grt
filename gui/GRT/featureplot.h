#ifndef FEATUREPLOT_H
#define FEATUREPLOT_H

#include <QWidget>
#include "qcustomplot.h"
#include <GRT/GRT.h>

namespace Ui {
class FeaturePlot;
}

class FeaturePlot : public QWidget
{
    Q_OBJECT

public:
    explicit FeaturePlot(QWidget *parent = 0);
    ~FeaturePlot();

    bool init(const unsigned int axisIndexX,const unsigned int axisIndexY,const GRT::ClassificationData &data,const GRT::Vector< Qt::GlobalColor > &classColors);
    bool plot();

    void resizeEvent (QResizeEvent *event);

private slots:
    bool ctrlSShortcut();

private:
    Ui::FeaturePlot *ui;
    bool initialized;
    unsigned int axisIndexX;
    unsigned int axisIndexY;
    GRT::ClassificationData data;
    GRT::Vector< Qt::GlobalColor > classColors;

};

#endif // FEATUREPLOT_H
