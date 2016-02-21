#include "timeseriesgraph.h"
#include "ui_timeseriesgraph.h"

using namespace GRT;

double TimeseriesGraph::maximumGraphRefreshFramerate = 0.2;
bool TimeseriesGraph::setMaximumGraphRefreshRate(const double framerate){
    maximumGraphRefreshFramerate = framerate;
    return true;
}

TimeseriesGraph::TimeseriesGraph(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TimeseriesGraph)
{
    ui->setupUi(this);
    initialized = false;
    lockRanges = false;
    numDimensions = 0;
    graphWidth = 0;
    plotTimestamp = 0;
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
    const double bigNumber = grt_numeric_limits<double>::max();
    ui->yMin->setDecimals( 4 );
    ui->yMax->setDecimals( 4 );
    ui->yMin->setRange( - bigNumber, bigNumber );
    ui->yMax->setRange( - bigNumber, bigNumber );
}
TimeseriesGraph::~TimeseriesGraph()
{
    delete ui;
}
bool TimeseriesGraph::init(const unsigned int numDimensions,const unsigned int graphWidth){
    initialized = true;
    this->numDimensions = numDimensions;
    this->graphWidth = graphWidth;
    plotTimestamp = 0;
    data.resize( graphWidth, GRT::VectorDouble(numDimensions,0) );
    x.resize( graphWidth );
    y.resize(numDimensions, QVector<double>(graphWidth) );
    if( !lockRanges ){
        minRange = 99e+99;
        maxRange = -minRange;
    }
    return true;
}
bool TimeseriesGraph::draw(){
    if( !initialized ) return false;
    //If the plot is hidden then there is no point in updating the graph
    if( this->isHidden() ){
        if( !lockRanges ){
            //Reset the min and max values
            minRange = 99e+99;
            maxRange = -minRange;
        }
        return true;
    }
    //Limit the rate at which the graph is replotted
    double timestamp = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
    if( timestamp - plotTimestamp < maximumGraphRefreshFramerate ){
        return true;
    }
    plotTimestamp = timestamp;
    QCustomPlot *plot = ui->graph;
    //Clear any previous graphs
    plot->clearGraphs();
    //Get the data to plot
    double lastMinRange = minRange;
    double lastMaxRange = maxRange;
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
    if( lastMinRange != minRange ) ui->yMin->setValue( minRange );
    if( lastMaxRange != maxRange ) ui->yMax->setValue( maxRange );
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

bool TimeseriesGraph::update( const VectorDouble &sample ){

    if( !initialized ) return false;
    //Add the new sample to the buffer
    data.push_back( sample );
    draw();
    return true;
}

bool TimeseriesGraph::setYAxisRanges(const double minRange,const double maxRange,const bool lockRanges){
    this->minRange = minRange;
    this->maxRange = maxRange;
    this->lockRanges = lockRanges;
    ui->yMin->setValue( minRange );
    ui->yMax->setValue( maxRange );
    return true;
}

//resize graph on window resize
void TimeseriesGraph::resizeEvent (QResizeEvent *event){
    int width = ui->graph->parentWidget()->width();
    int height = ui->graph->parentWidget()->height();
    ui->graph->setGeometry(5,5,width-10,height-50);
    ui->optionsGroupbox->setGeometry(5,height-55,width-10,50);
}

void TimeseriesGraph::on_yMin_valueChanged(double arg1)
{
    this->minRange = arg1;
}

void TimeseriesGraph::on_yMax_valueChanged(double arg1)
{
    this->maxRange = arg1;
}

void TimeseriesGraph::on_lockAxis_clicked(bool checked)
{
    this->lockRanges = checked;
}
