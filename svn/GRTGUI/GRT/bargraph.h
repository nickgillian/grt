#ifndef BARGRAPH_H
#define BARGRAPH_H

#include <QWidget>
#include "../../GRT/GRT.h"

namespace Ui {
class BarGraph;
}

class BarGraph : public QWidget
{
    Q_OBJECT
    
public:
    explicit BarGraph(QWidget *parent = 0);
    ~BarGraph();

    bool init(const unsigned int numDimensions);
    bool update(const GRT::VectorDouble &sample );
    
private:
    bool initialized;
    unsigned int numDimensions;
    Ui::BarGraph *ui;
    GRT::VectorDouble data;
};

#endif // BARGRAPH_H
