#include "bargraph.h"
#include "ui_bargraph.h"

using namespace GRT;

double BarGraph::maximumGraphRefreshFramerate = 0.2;
bool BarGraph::setMaximumGraphRefreshRate(const double framerate){
    maximumGraphRefreshFramerate = framerate;
    return true;
}

BarGraph::BarGraph(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BarGraph)
{
    ui->setupUi(this);
    initialized = false;
    numDimensions = 0;
}

BarGraph::~BarGraph()
{
    delete ui;
}

bool BarGraph::init(const unsigned int numDimensions){
    initialized = true;
    plotTimestamp = 0;
    this->numDimensions = numDimensions;
    keyData.clear();
    valueData.clear();
    tickVector.clear();
    tickLabels.clear();
    valueData.resize( numDimensions );
    for(unsigned int k=0; k<numDimensions; k++){
        keyData << k+1;
        tickVector << double(k+1);
        tickLabels << QString::fromStdString( GRT::Util::intToString( k+1 ) );
    }
    return true;
}

bool BarGraph::update(const GRT::VectorFloat &data ){
    if( !initialized ) return false;
    //If the plot is hidden then there is no point in updating the graph
    if( this->isHidden() ){
        return true;
    }
    if( data.size() != numDimensions ) return false;
    //Limit the rate at which the graph is replotted
    double timestamp = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
    if( timestamp - plotTimestamp < maximumGraphRefreshFramerate ){
        return true;
    }
    plotTimestamp = timestamp;
    QCustomPlot *plot = ui->graph;
    plot->clearPlottables();
    QCPBars *bar = new QCPBars(plot->xAxis,plot->yAxis);
    plot->addPlottable( bar );
    //Add the data to the graph
    for(unsigned int k=0; k<numDimensions; k++){
        valueData[k] = data[k];
    }
    bar->setData(keyData, valueData);
    //Add the tick labels
    plot->xAxis->setAutoTicks(true);
    plot->xAxis->setAutoTickLabels(true);
    plot->xAxis->setTickVector( tickVector );
    plot->xAxis->setTickVectorLabels( tickLabels );
    plot->xAxis->setLabel("Features");
    plot->yAxis->setLabel("Values");
    plot->rescaleAxes();
    plot->replot();
    return true;
}
