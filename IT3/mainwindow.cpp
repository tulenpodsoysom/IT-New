#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <cmath>
#include <float.h>

#include <algorithm>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    graphSignal.outerrect= ui->frame->geometry();
    graphWindow.outerrect= ui->frame_2->geometry();

    graphFiltered1.outerrect= ui->frame_3->geometry();
    graphFiltered2.outerrect= ui->frame_4->geometry();
    graphFiltered3.outerrect= ui->frame_5->geometry();
    graphFiltered4.outerrect= ui->frame_6->geometry();
    graphFiltered5.outerrect= ui->frame_7->geometry();

    mt.seed(rd());
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{

    FilterProgram::SineParams sin1,sin2,sin3;

    sin1.A = ui->editA1->text().toDouble();
    sin2.A = ui->editA2->text().toDouble();
    sin3.A = ui->editA3->text().toDouble();

    sin1.f = ui->editf1->text().toDouble();
    sin2.f = ui->editf2->text().toDouble();
    sin3.f = ui->editf3->text().toDouble();

    sin1.phi = ui->editPhi1->text().toDouble();
    sin2.phi = ui->editPhi2->text().toDouble();
    sin3.phi = ui->editPhi3->text().toDouble();

    double fd    =  ui->editFd->text().toDouble();
    int N        =  ui->editN->text().toInt();
    double d     =  ui->editD->text().toDouble();
    double width =  ui->editWidth->text().toDouble();

    if (abs(round(log2(N)) - log2(N)) > DBL_EPSILON) {QMessageBox(QMessageBox::Critical,"Ошибка","N не является степенью двойки!").exec(); return;}

    auto signal =           FilterProgram::CreatePolyHarmonicalSignal({sin1,sin2,sin3},N,fd);
    auto noisedSignal =     FilterProgram::AddNoiseSignal(signal,d,mt);

    std::vector<std::vector<double>> windowSignals(5);
    std::vector<std::vector<double>> filteredSignals(5);

    for (int var = 0; var < 5; ++var) {
        windowSignals[var] = FilterProgram::CreateWindowSignal(N,width,fd,static_cast<FilterProgram::WindowType>(var));
    }

    for (int var = 0; var < 5; ++var) {
        filteredSignals[var] = FilterProgram::Filter(signal,windowSignals[var]);
    }

    std::vector<double> energyNegotiation(5);

    for (int var = 0; var < N; ++var) {
        energyNegotiation[0] += (signal[var] - filteredSignals[0][var])*(signal[var] - filteredSignals[0][var])  ;
        energyNegotiation[1] += (signal[var] - filteredSignals[1][var])*(signal[var] - filteredSignals[1][var])  ;
        energyNegotiation[2] += (signal[var] - filteredSignals[2][var])*(signal[var] - filteredSignals[2][var])  ;
        energyNegotiation[3] += (signal[var] - filteredSignals[3][var])*(signal[var] - filteredSignals[3][var])  ;
        energyNegotiation[4] += (signal[var] - filteredSignals[4][var])*(signal[var] - filteredSignals[4][var])  ;
    }

    int bestFilter = std::distance(energyNegotiation.begin(), std::min_element(energyNegotiation.begin(),energyNegotiation.end()));


        // Задаем значения для графиков
    graphFiltered1.backgroundColor = Qt::white;
    graphFiltered2.backgroundColor = Qt::white;
    graphFiltered3.backgroundColor = Qt::white;
    graphFiltered4.backgroundColor = Qt::white;
    graphFiltered5.backgroundColor = Qt::white;
    switch (bestFilter)
    {
        case 0:graphFiltered1.backgroundColor = Qt::green; break;
        case 1:graphFiltered2.backgroundColor = Qt::green; break;
        case 2:graphFiltered3.backgroundColor = Qt::green; break;
        case 3:graphFiltered4.backgroundColor = Qt::green; break;
        case 4:graphFiltered5.backgroundColor = Qt::green; break;
    }

    std::vector<std::vector<QPointF>> windowPoints;
    for (auto& var : windowSignals) {
        windowPoints.push_back(MyGraph::DoubleToPoints(var,QRectF(QPointF(0,1),QPoint(N/fd, 0))));
    }

    double ymax = MyGraph::ClosestMantissa(noisedSignal);
    QRectF worldRectSignal(QPointF(0,ymax),QPoint(N/fd,-ymax));

    double ymax1 = MyGraph::ClosestMantissa(filteredSignals[0]);
    double ymax2 = MyGraph::ClosestMantissa(filteredSignals[1]);
    double ymax3 = MyGraph::ClosestMantissa(filteredSignals[2]);
    double ymax4 = MyGraph::ClosestMantissa(filteredSignals[3]);
    double ymax5 = MyGraph::ClosestMantissa(filteredSignals[4]);

    QRectF worldRectFilteredSignal1(QPointF(0,ymax1),QPoint(N/fd,-ymax1));
    QRectF worldRectFilteredSignal2(QPointF(0,ymax2),QPoint(N/fd,-ymax2));
    QRectF worldRectFilteredSignal3(QPointF(0,ymax3),QPoint(N/fd,-ymax3));
    QRectF worldRectFilteredSignal4(QPointF(0,ymax4),QPoint(N/fd,-ymax4));
    QRectF worldRectFilteredSignal5(QPointF(0,ymax5),QPoint(N/fd,-ymax5));

    graphSignal.worldRect   = worldRectSignal;
    graphSignal.points      = {MyGraph::DoubleToPoints(noisedSignal,graphSignal.worldRect)};
    graphSignal.outerrect   = ui->frame->geometry();
    graphSignal.title       = "Исходный сигнал";
    graphSignal.captionX    = "t, с";
    graphSignal.captionY    = "X, ед";

    graphWindow.worldRect   = QRectF(QPointF(0,1),QPoint(N/fd, 0));
    graphWindow.points      = windowPoints;
    graphWindow.outerrect   = ui->frame_2->geometry();
    graphWindow.title       = "Виды окон";
    graphWindow.captionX    = "t, с";
    graphWindow.captionY    = "X, ед";

    graphFiltered1.worldRect  = worldRectFilteredSignal1;
    graphFiltered1.points     = {MyGraph::DoubleToPoints(filteredSignals[0],worldRectSignal)};
    graphFiltered1.outerrect  = ui->frame_3->geometry();
    graphFiltered1.title      = "Фильтрация прямоугольным окном";
    graphFiltered1.captionX   = "t, с";
    graphFiltered1.captionY   = "X, ед";

    graphFiltered2.worldRect  = worldRectFilteredSignal2;
    graphFiltered2.points     = {MyGraph::DoubleToPoints(filteredSignals[1],worldRectSignal)};
    graphFiltered2.outerrect  = ui->frame_4->geometry();
    graphFiltered2.title      = "Фильтрация треугольным окном";
    graphFiltered2.captionX   = "t, с";
    graphFiltered2.captionY   = "X, ед";

    graphFiltered3.worldRect  = worldRectFilteredSignal3;
    graphFiltered3.points     = {MyGraph::DoubleToPoints(filteredSignals[2],worldRectSignal)};
    graphFiltered3.outerrect  = ui->frame_5->geometry();
    graphFiltered3.title      = "Фильтрация окном Ханна";
    graphFiltered3.captionX   = "t, с";
    graphFiltered3.captionY   = "X, ед";

    graphFiltered4.worldRect  = worldRectFilteredSignal4;
    graphFiltered4.points     = {MyGraph::DoubleToPoints(filteredSignals[3],worldRectSignal)};
    graphFiltered4.outerrect  = ui->frame_6->geometry();
    graphFiltered4.title      = "Фильтрация окном Хэмминга";
    graphFiltered4.captionX   = "t, с";
    graphFiltered4.captionY   = "X, ед";

    graphFiltered5.worldRect  = worldRectFilteredSignal5;
    graphFiltered5.points     = {MyGraph::DoubleToPoints(filteredSignals[4],worldRectSignal)};
    graphFiltered5.outerrect  = ui->frame_7->geometry();
    graphFiltered5.title      = "Фильтрация окном Гаусса";
    graphFiltered5.captionX   = "t, с";
    graphFiltered5.captionY   = "X, ед";

    update();
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    graphSignal.Draw(painter);
    graphWindow.Draw(painter);

    graphFiltered1.Draw(painter);
    graphFiltered2.Draw(painter);
    graphFiltered3.Draw(painter);
    graphFiltered4.Draw(painter);
    graphFiltered5.Draw(painter);

}



