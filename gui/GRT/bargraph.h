#ifndef BARGRAPH_H
#define BARGRAPH_H

#include <QWidget>
#include <GRT/GRT.h>

namespace Ui {
class BarGraph;
}

class BarGraph : public QWidget
{
    Q_OBJECT

public:
    explicit BarGraph(QWidget *parent = 0);
    virtual ~BarGraph();
    bool init(const unsigned int numDimensions);
    bool update(const GRT::VectorFloat &sample );
    static bool setMaximumGraphRefreshRate(const double framerate);

private:
    bool initialized;
    unsigned int numDimensions;
    double maximumRefreshRate;
    double plotTimestamp;
    Ui::BarGraph *ui;
    QVector<double> keyData;
    QVector<double> valueData;
    QVector<double> tickVector;
    QVector<QString> tickLabels;
    static double maximumGraphRefreshFramerate;
};

#endif // BARGRAPH_H
