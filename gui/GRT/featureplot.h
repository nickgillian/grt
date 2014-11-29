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

    bool init(const unsigned int axisIndexA,const unsigned int axisIndexB,const GRT::ClassificationData &data,const vector< Qt::GlobalColor > &classColors);
    bool plot();

    void resizeEvent (QResizeEvent *event);

private:
    Ui::FeaturePlot *ui;
    bool initialized;
    unsigned int axisIndexA;
    unsigned int axisIndexB;
    GRT::ClassificationData data;
    vector< Qt::GlobalColor > classColors;

};

#endif // FEATUREPLOT_H
