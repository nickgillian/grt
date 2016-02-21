#include "featureplot.h"
#include "ui_featureplot.h"

FeaturePlot::FeaturePlot(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FeaturePlot)
{
    ui->setupUi(this);
    setStyleSheet("background-color:white;");
    initialized = false;

    QShortcut *ctrlSShortcut = new QShortcut( QKeySequence( QString::fromStdString("Ctrl+S") ), this);
    QObject::connect(ctrlSShortcut, SIGNAL(activated()), this, SLOT(ctrlSShortcut()));
}

FeaturePlot::~FeaturePlot()
{
    delete ui;
}

bool FeaturePlot::init(const unsigned int axisIndexX,const unsigned int axisIndexY,const GRT::ClassificationData &data,const GRT::Vector< Qt::GlobalColor > &classColors){

    this->initialized = true;
    this->axisIndexX = axisIndexX;
    this->axisIndexY = axisIndexY;
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
    GRT::Vector< GRT::MinMax > ranges = data.getRanges();
    GRT::Vector< unsigned int > classLabels = data.getClassLabels();
    GRT::Vector< GRT::ClassTracker > classTracker = data.getClassTracker();
    double minRange = GRT::grt_numeric_limits<double>::max();
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
            plot->graph(k)->setScatterStyle( QCPScatterStyle(QCPScatterStyle::ssCross, 4) );

            unsigned int index = 0;
            for(unsigned int i=0; i<M; i++)
            {
                if( data[i].getClassLabel() == classTracker[k].classLabel ){
                    x[ index ] = data[i][ axisIndexX ];
                    y[ index ] = data[i][ axisIndexY ];
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

    //Add 20% to the min and max range
    minRange += minRange * 0.2;
    maxRange += maxRange * 0.2;

    plot->xAxis->setVisible( true );
    plot->xAxis->setTickLabels( true );
    plot->yAxis->setVisible( true );
    plot->yAxis->setTickLabels( true );
    plot->xAxis->setLabel( QString::fromStdString("X Axis Index: "  + GRT::Util::toString(axisIndexX)) );
    plot->yAxis->setLabel( QString::fromStdString("Y Axis Index: "  + GRT::Util::toString(axisIndexY)) );
    plot->xAxis->setRange(minRange, maxRange);
    plot->yAxis->setRange(minRange, maxRange);
    plot->replot();

    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    return true;
}

//resize graph on window resize
void FeaturePlot::resizeEvent (QResizeEvent *event){
    ui->plot->resize(ui->plot->parentWidget()->width()-20,ui->plot->parentWidget()->height()-20);
}

bool FeaturePlot::ctrlSShortcut(){

    if( !initialized ) return false;

    QCustomPlot *plot = ui->plot;

    QString filename = QFileDialog::getSaveFileName();

    if( filename == "" ){
        return false;
    }
    if( filename.endsWith(".png") ){
        return plot->savePng( filename, plot->width(), plot->height() );
    }
    if( filename.endsWith(".jpg") ){
        return plot->saveJpg( filename, plot->width(), plot->height() );
    }
    if( filename.endsWith(".jpeg") ){
        return plot->saveJpg( filename, plot->width(), plot->height() );
    }
    if( filename.endsWith(".pdf") ){
        return plot->savePdf( filename, plot->width(), plot->height() );
    }

    //If we get this far then save as a png
    return plot->savePng( filename, plot->width(), plot->height() );
}
