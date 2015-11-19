#include "rtgraph.h"

RtGraph::RtGraph(QWidget *parent) :
    QWidget(parent)
{
    initialized = false;
    lockRanges = false;
    numDimensions = 0;
    graphWidth = 0;
    colors.push_back( Qt::red );
    colors.push_back( Qt::green );
    colors.push_back( Qt::blue );
    colors.push_back( Qt::magenta );
    colors.push_back( Qt::yellow );
    colors.push_back( Qt::cyan );
    colors.push_back( Qt::gray );
    colors.push_back( Qt::darkRed );
    colors.push_back( Qt::darkGreen );
    colors.push_back( Qt::darkBlue );
    colors.push_back( Qt::darkMagenta );
    colors.push_back( Qt::darkYellow );
    colors.push_back( Qt::darkCyan );
    colors.push_back( Qt::darkGray );
}

RtGraph::~RtGraph()
{
}

bool RtGraph::init(const unsigned int numDimensions,const unsigned int graphWidth){
    return false;
}

bool RtGraph::update(const GRT::VectorDouble &sample ){
    return false;
}

bool RtGraph::setYAxisRanges(const double minRange,const double maxRange,const bool lockRanges){
    this->minRange = minRange;
    this->maxRange = maxRange;
    this->lockRanges = lockRanges;
    return true;
}

void RtGraph::resizeEvent (QResizeEvent *event){
    return;
}
