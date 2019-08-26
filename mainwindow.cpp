#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QThread>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->customPlot, SIGNAL(mousePress(QMouseEvent* )), SLOT(clickedGraph(QMouseEvent*)));

    //Init errorX
    count = 0;

    // first set
    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle::ssCross);
    ui->customPlot->graph(0)->setLineStyle(QCPGraph::lsNone);

    //second set
    ui->customPlot->addGraph();
    ui->customPlot->graph(1)->setScatterStyle(QCPScatterStyle::ssDisc);
    ui->customPlot->graph(1)->setLineStyle(QCPGraph::lsNone);

    //line
    ui->customPlot->addGraph();
    ui->customPlot->graph(2)->setLineStyle(QCPGraph::lsLine);

    //setting range
    ui->customPlot->xAxis->setRange(-10, 10);
    ui->customPlot->yAxis->setRange(-10, 10);

    // error graph
    ui->errorPlot->addGraph();
    ui->errorPlot->xAxis->setRange(0,200);
    ui->errorPlot->yAxis->setRange(0,10);

    //initial factor and iterations
    ui->inputF->setText("0.5");
    ui->inputG->setText("100");

    //initial flag
    flag = false;

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::delay()
{
    QTime dieTime= QTime::currentTime().addSecs(1);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void MainWindow::addPoint(Point p)
{
    if(p.Class == 1){
        qv_x1.append(p.x);
        qv_y1.append(p.y);
    }else{
        qv_x2.append(p.x);
        qv_y2.append(p.y);
    }
    pointVector.append(p);

}

void MainWindow::clearData()
{
    ui->customPlot->graph(0)->data()->clear();
    ui->customPlot->graph(1)->data()->clear();
    ui->customPlot->graph(2)->data()->clear();

    ui->customPlot->replot();
    ui->customPlot->update();

    qv_x1.clear();
    qv_x2.clear();

    qv_y1.clear();
    qv_y2.clear();

    pointVector.clear();
    clearError();

    ui->finalizo->clear();

}

void MainWindow::clearError(){
    ui->errorPlot->graph(0)->data()->clear();
    ui->errorPlot->replot();
    ui->errorPlot->update();

    errorX.clear();
    errorY.clear();
    count = 0;
}

void MainWindow::plot()
{
    ui->customPlot->graph(0)->setData(qv_x1,qv_y1);
    ui->customPlot->graph(1)->setData(qv_x2,qv_y2);
    ui->customPlot->replot();
    ui->customPlot->update();
}

void MainWindow::drawLine(){
    QVector<double> x,y;
    double y1,y2;

    y1 = ((10 * percept.w1) + (percept.wb)) / percept.w2;
    y2 = ((-10 * percept.w1) + (percept.wb)) / percept.w2;

    x.append(-10.0);
    x.append(10);

    y.append(y1);
    y.append(y2);

    ui->customPlot->graph(2)->setData(x,y);
}


void MainWindow::clickedGraph(QMouseEvent *event)
{
    if(!flag)
    {
        QPoint point = event->pos();
        int Cl = -1;
        if(event->buttons() == Qt::RightButton)
        {
            Cl = 0;
        }else if(event->buttons() == Qt::LeftButton)
        {
            Cl = 1;
        }

        Point myPoint(ui->customPlot->xAxis->pixelToCoord(point.x()),ui->customPlot->yAxis->pixelToCoord(point.y()),Cl);
        addPoint(myPoint);
        plot();
    }
    else
    {
        QPoint point = event->pos();
        int Cl = -1;
        double ejeX = ui->customPlot->xAxis->pixelToCoord(point.x());
        double ejeY = ui->customPlot->yAxis->pixelToCoord(point.y());

        int test = percept.functionZ(ejeX,ejeY);

        if(test > 0.0)
        {
            Cl = 1;
        }
        else
        {
            Cl = 0;
        }
        Point myPoint(ejeX,ejeY,Cl);
        addPoint(myPoint);
        plot();
    }

}

void MainWindow::on_btnInit_clicked()
{
    percept.rand();
    drawLine();
    ui->w_0->setText("Wb " + QString::number(percept.wb));
    ui->w_1->setText("W1 " + QString::number(percept.w1));
    ui->w_2->setText("W2 " + QString::number(percept.w2));
    plot();
}

void MainWindow::plotError(){

    ui->errorPlot->graph(0)->setData(errorX,errorY);
    ui->errorPlot->replot();
    ui->errorPlot->update();
}

void MainWindow::on_btnCorrer_clicked()
{
    clearError();
    QString fString = ui->inputF->text();
    QString GS = ui->inputG->text();

    double factor = fString.toDouble();
    int G = GS.toInt();

    if(fString == "")
    {
        factor = 0.5;
        ui->inputF->setText("0.5");
    }
    if(GS == "")
    {
        G = 100;
        ui->inputG->setText("100");
    }


    ui->errorPlot->xAxis->setRange(0,G);

    int iteraciones;
    bool done = false;
    int error;
    int cont;
    for(int r = 0; r < G; r++)
    {
        done = true;
        cont = 0;

        for(int i=0; i < pointVector.size(); i++)
        {
            error = pointVector[i].Class - percept.functionZ(pointVector[i].x,pointVector[i].y);
            if(error != 0)
            {
                done = false;
                percept.update(pointVector[i],factor,error);
                cont += error * error;
            }
        }

        errorY.append(double(cont));
        errorX.append(count);
        count++;

        ui->w_0->setText("Wb " + QString::number(percept.wb));
        ui->w_1->setText("W1 " + QString::number(percept.w1));
        ui->w_2->setText("W2 " + QString::number(percept.w2));
        drawLine();
        plotError();
        plot();
        //QThread::sleep(1);

        if(done)
        {
            iteraciones = r;
            break;
        }

    }
    if(done)
    {
        QString respuesta = "Finalizo en " + QString::number(iteraciones) + " iteraciones";
        ui->finalizo->setText(respuesta);
    }
    else
    {
        ui->finalizo->setText("El perceptron No convergio :( ");
    }

}

void MainWindow::on_btnClean_clicked()
{
    clearData();
}

void MainWindow::on_btnVerificar_clicked()
{
    if(flag)
    {
        flag = false;
    }
    else
    {
        flag = true;
    }
}
