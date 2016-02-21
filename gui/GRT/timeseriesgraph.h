#ifndef TIMESERIESGRAPH_H
#define TIMESERIESGRAPH_H

#include <QWidget>
#include "qcustomplot.h"
#include <GRT/GRT.h>

namespace Ui {
class TimeseriesGraph;
}

class TimeseriesGraph : public QWidget
{
    Q_OBJECT

public:
    explicit TimeseriesGraph(QWidget *parent = 0);
    ~TimeseriesGraph();
    bool init(const unsigned int numDimensions,const unsigned int graphWidth);
    bool draw();
    bool update( const GRT::VectorDouble &sample );
    bool setYAxisRanges(const double minRange,const double maxRange,const bool lockRanges=false);
    void resizeEvent (QResizeEvent *event);
    static bool setMaximumGraphRefreshRate(const double framerate);

private slots:
    void on_yMin_valueChanged(double arg1);
    void on_yMax_valueChanged(double arg1);
    void on_lockAxis_clicked(bool checked);
private:
    bool initialized;
    bool lockRanges;
    unsigned int numDimensions;
    unsigned int graphWidth;
    double plotTimestamp;
    Ui::TimeseriesGraph *ui;
    GRT::CircularBuffer< GRT::VectorDouble > data;
    double minRange;
    double maxRange;
    QVector<double> x;
    GRT::Vector< QVector<double> > y;
    GRT::Vector< Qt::GlobalColor > colors;
    static double maximumGraphRefreshFramerate;
};

#endif // TIMESERIESGRAPH_H
