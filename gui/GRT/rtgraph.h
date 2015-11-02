#ifndef RTGRAPH_H
#define RTGRAPH_H

#include <QWidget>
#include <GRT/GRT.h>

namespace Ui {
class RtGraph;
}

class RtGraph : public QWidget
{
    Q_OBJECT
    
public:
    explicit RtGraph(QWidget *parent = 0);
    virtual ~RtGraph();

    virtual bool init(const unsigned int numDimensions,const unsigned int graphWidth = 0);
    virtual bool update(const GRT::VectorDouble &sample );
    virtual bool setYAxisRanges(const double minRange,const double maxRange,const bool lockRanges=false);
    virtual void resizeEvent (QResizeEvent *event);
    
protected:
    bool initialized;
    bool lockRanges;
    unsigned int numDimensions;
    unsigned int graphWidth;
    double minRange;
    double maxRange;
    vector< Qt::GlobalColor > colors;
};

#endif // RTGRAPH_H
