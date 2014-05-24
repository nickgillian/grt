#include "bargraph.h"
#include "ui_bargraph.h"

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
    this->numDimensions = numDimensions;
    return true;
}

bool BarGraph::update(const GRT::VectorDouble &sample ){

    if( !initialized ) return false;

    this->data = sample;

    //If the plot is hidden then there is no point in updating the graph
    if( this->isHidden() ){
        return true;
    }

    QCustomPlot *plot = ui->graph;
    QVector<double> keyData;
    QVector<double> valueData;
    QVector<double> tickVector;
    QVector<QString> tickLabels;
    const unsigned int K = (unsigned int)data.size();

    plot->clearPlottables();

    QCPBars *bar = new QCPBars(plot->xAxis,plot->yAxis);
    plot->addPlottable( bar );

    //Add the data to the graph
    for(unsigned int k=0; k<K; k++){
        keyData << k+1;
        valueData << data[k];
    }
    bar->setData(keyData, valueData);

    //Add the tick labels
    for(unsigned int k=0; k<K; k++){
        tickVector << double(k+1);
        tickLabels << QString::fromStdString( GRT::Util::intToString( k+1 ) );
    }
    plot->xAxis->setAutoTicks(false);
    plot->xAxis->setAutoTickLabels(false);
    plot->xAxis->setTickVector( tickVector );
    plot->xAxis->setTickVectorLabels( tickLabels );
    plot->xAxis->setLabel("Features");
    plot->yAxis->setLabel("Values");

    plot->rescaleAxes();
    plot->replot();

    return true;
}
