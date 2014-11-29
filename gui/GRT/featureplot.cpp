#include "featureplot.h"
#include "ui_featureplot.h"

FeaturePlot::FeaturePlot(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FeaturePlot)
{
    ui->setupUi(this);
    setStyleSheet("background-color:white;");
    initialized = false;
}

FeaturePlot::~FeaturePlot()
{
    delete ui;
}

bool FeaturePlot::init(const unsigned int axisIndexA,const unsigned int axisIndexB,const GRT::ClassificationData &data,const vector< Qt::GlobalColor > &classColors){

    this->initialized = true;
    this->axisIndexA = axisIndexA;
    this->axisIndexB = axisIndexB;
    this->data = data;
    this->classColors = classColors;

    plot();

    return true;
}

bool FeaturePlot::plot(){

    if( !initialized ) return false;

    QCustomPlot *plot = ui->plot;

    //Clear any previous graphs
    plot->clearGraphs();

    const unsigned int M = data.getNumSamples();
    const unsigned int K = data.getNumClasses();
    const unsigned int N = data.getNumDimensions();
    vector< GRT::MinMax > ranges = data.getRanges();
    vector< unsigned int > classLabels = data.getClassLabels();
    vector< GRT::ClassTracker > classTracker = data.getClassTracker();
    double minRange = numeric_limits<double>::max();
    double maxRange = -minRange;

    //Add a new graph for each class
    for(unsigned int k=0; k<K; k++){
        const unsigned int numSamplesInClass = classTracker[k].counter;

        if( numSamplesInClass > 0 ){
            QVector< double > x( numSamplesInClass );
            QVector< double > y( numSamplesInClass );
            plot->addGraph();
            plot->graph(k)->setPen( QPen( classColors[ k % classColors.size() ] ) );
            plot->graph(k)->setLineStyle( QCPGraph::lsNone );
            plot->graph(k)->setScatterStyle( QCPScatterStyle(QCPScatterStyle::ssPlus, 4) );

            unsigned int index = 0;
            for(unsigned int i=0; i<M; i++)
            {
                if( data[i].getClassLabel() == classTracker[k].classLabel ){
                    x[ index ] = data[i][ axisIndexA ];
                    y[ index ] = data[i][ axisIndexB ];
                    index++;

                    for(unsigned int j=0; j<N; j++){
                        if( data[i][j] > maxRange ){
                            maxRange = data[i][j];
                        }else if( data[i][j] < minRange ){
                            minRange = data[i][j];
                        }
                    }
                }
            }

            // pass data points to graphs:
            plot->graph( k )->setData(x, y);
        }
    }

    //Add 10% to the min and max range
    minRange += minRange * 0.1;
    maxRange += maxRange * 0.1;

    plot->xAxis->setVisible( true );
    plot->xAxis->setTickLabels( true );
    plot->yAxis->setVisible( true );
    plot->yAxis->setTickLabels( true );
    plot->xAxis->setLabel( QString::fromStdString("Axis Index: "  + GRT::Util::toString(axisIndexA)) );
    plot->yAxis->setLabel( QString::fromStdString("Axis Index: "  + GRT::Util::toString(axisIndexB)) );
    plot->xAxis->setRange(minRange, maxRange);
    plot->yAxis->setRange(minRange, maxRange);
    plot->replot();

    return true;
}

//resize graph on window resize
void FeaturePlot::resizeEvent (QResizeEvent *event){
    ui->plot->resize(ui->plot->parentWidget()->width()-20,ui->plot->parentWidget()->height()-20);
}
