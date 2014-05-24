#include "timeseriesgraph.h"
#include "ui_timeseriesgraph.h"

TimeseriesGraph::TimeseriesGraph(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TimeseriesGraph)
{
    ui->setupUi(this);
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

TimeseriesGraph::~TimeseriesGraph()
{
    delete ui;
}

bool TimeseriesGraph::init(const unsigned int numDimensions,const unsigned int graphWidth){

    initialized = true;
    this->numDimensions = numDimensions;
    this->graphWidth = graphWidth;
    data.resize( graphWidth, GRT::VectorDouble(numDimensions,0) );

    if( !lockRanges ){
        minRange = 99e+99;
        maxRange = -minRange;
    }
    return true;
}

bool TimeseriesGraph::update(const GRT::VectorDouble &sample ){

    if( !initialized ) return false;

    //Add the new sample to the buffer
    data.push_back( sample );

    //If the plot is hidden then there is no point in updating the graph
    if( this->isHidden() ){
        if( !lockRanges ){
            //Reset the min and max values
            minRange = 99e+99;
            maxRange = -minRange;
        }
        return true;
    }

    QCustomPlot *plot = ui->graph;

    //Clear any previous graphs
    plot->clearGraphs();

    //Get the data to plot
    QVector<double> x( graphWidth );
    vector< QVector<double> > y(numDimensions, QVector<double>(graphWidth) );

    for (unsigned int i=0; i<graphWidth; i++)
    {
      x[i] = i;
      for(unsigned int j=0; j<numDimensions; j++){
          y[j][i] = data[i][j];
          if( !lockRanges ){
            if( data[i][j] < minRange ) minRange = data[i][j];
            else if( data[i][j] > maxRange ) maxRange = data[i][j];
          }
      }
    }

    //Create the graphs
    for(unsigned int j=0; j<numDimensions; j++){
        plot->addGraph();
        plot->graph(j)->setPen( QPen( colors[j%colors.size()] ));
        plot->graph(j)->setData(x, y[j]);
    }

    // give the axes some labels:
    plot->xAxis->setLabel("Time");
    plot->yAxis->setLabel("Values");

    // set axes ranges, so we see all data:
    plot->xAxis->setRange(0, graphWidth);
    plot->yAxis->setRange(minRange, maxRange);
    plot->replot();

    return true;
}

bool TimeseriesGraph::setYAxisRanges(const double minRange,const double maxRange,const bool lockRanges){
    this->minRange = minRange;
    this->maxRange = maxRange;
    this->lockRanges = lockRanges;
    return true;
}
