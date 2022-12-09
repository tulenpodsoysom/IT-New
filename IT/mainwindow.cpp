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

    fd = 128.0;
    N = 128;

    dt = 1.0/fd;

    t0 = 0.0;
    tmax = 1.0;

    ymin = -3.0;
    ymax =  3.0;

    //lines = new QLine[N];
    //linesSpectre = new QLine[N];
    //lines_2 = new QLine[N];

    lines = NULL;
    linesSpectre = NULL;
    lines_2 = NULL;



}

MainWindow::~MainWindow()
{
    delete ui;
    delete[] lines;
}


void MainWindow::on_pushButton_clicked()
{

    fd = ui->editFd->text().toFloat();
    N = ui->editN->text().toInt();
    if (abs(round(log2(N)) - log2(N)) > DBL_EPSILON) {QMessageBox(QMessageBox::Critical,"Ошибка","N не является степенью двойки!").exec(); return;}

    dt = 1./fd;

    t0 = 0.0;
    tmax = N * dt;


    QRectF worldRect_1 (QPointF(t0,ymax),QPointF(tmax,ymin));
    QRectF worldRect_2 (QPointF(0,ymax_2),QPoint(fd,0));
    QRectF worldRect_3 (QPointF(t0,ymax_3),QPointF(tmax,ymin_3));

    std::vector<Complex>    complexSignal   (N);
    std::vector<Complex>    complexSpectre  (N);
    std::vector<double>     amplSpectre     (N);
    std::vector<Complex>    complexSpectreNoiseReduced(N);
    std::vector<double>     amplSpectreNoiseReduced(N);

    std::vector<Complex> timeSignal2complex(N);
    std::vector<double>  timeSignal2real(N);

    auto values = getFuncValues();
    auto trueSignal = values;

    // Наложение гауссового шума
    if (ui->buttonNoise->isChecked())
    {
        std::vector<double> noiseValues(N);


        std::mt19937 mt(rd());
        for (int var = 0; var < N; ++var)
        {
            std::normal_distribution<double> dist(0,1);
            noiseValues[var] = dist(mt);
        }

        double E1 = 0;
        double E2 = 0;
        for (int var = 0; var < N; ++var) {
            E1 += values[var]*values[var];
            E2 += noiseValues[var]*noiseValues[var];
        }

        double d = ui->editD->text().toDouble();
        double a = E1/E2 * d;

        for (int var = 0; var <N; ++var) {
            values[var] += sqrt(a)*noiseValues[var];
        }
    }


    for (int var = 0; var < N; ++var)
    {
        complexSignal[var] = Complex(values[var],0.0);
    }

    complexSpectre = fourier(complexSignal,-1);
    for (int var = 0; var < N; ++var) {
        Complex& z = complexSpectre[var];
        amplSpectre[var] = sqrt(z.re*z.re+z.im*z.im);
    }



    double a = ui->editA->text().toDouble();
    double Esumm = 0;
    for (int var = 0; var < N; ++var) {
        Esumm += amplSpectre[var]*amplSpectre[var];
    }
    double Esignal = 0;
    int iSignalThreshold = 0;

    if (abs(a) >= __DBL_EPSILON__)
    for (int var = 0; var < N/2; ++var) {
        Esignal += amplSpectre[var]*amplSpectre[var];
        Esignal += amplSpectre[N-1-var]*amplSpectre[N-1-var];
        iSignalThreshold = var + 1;
        if (Esignal > a * Esumm) {break;}
    }


    amplSpectreNoiseReduced = amplSpectre;
    complexSpectreNoiseReduced = complexSpectre;

    if ((iSignalThreshold < N/2 - 1))
    for (int var = iSignalThreshold; var < N-iSignalThreshold; ++var) {
        amplSpectreNoiseReduced[var] = 0;
        complexSpectreNoiseReduced[var] = Complex();
    }


    timeSignal2complex = fourier(complexSpectreNoiseReduced,1);

    for (int var = 0; var < N; ++var) {
        auto& ts = timeSignal2complex[var];
        timeSignal2real[var] = ts.re;
    }

    double signalError = 0;
    for (int var = 0; var < N; ++var) {
        signalError += (timeSignal2real[var] - trueSignal[var])*(timeSignal2real[var] - trueSignal[var]);
    }   signalError /= N;

    ui->labelError->setText("Ошибка: " + QString::number(signalError,'g',6));



    // Задаем значения для графиков
    graphValues_1.resize(N);
    graphValues_2_1.resize(N);
    graphValues_2_2.resize(N);
    graphValues_3_1.resize(N);
    graphValues_3_2.resize(N);


    ymax = ClosestMantissa(values);
    ymin = -ymax;

    ymax_2 = ClosestMantissa(amplSpectre);
    ymin_2 = 0.0;

    ymax_3 = ClosestMantissa(timeSignal2real);
    ymin_3 = -ymax_3;

    for (int var = 0; var < N; ++var) {
        graphValues_1[var].setX  (worldRect_1.left() + var*worldRect_1.width()/values.size());
        graphValues_1[var].setY  (values[var]);

        graphValues_2_1[var].setX(worldRect_2.left() + var*worldRect_2.width()/values.size());
        graphValues_2_1[var].setY(amplSpectre[var]);

        graphValues_2_2[var].setX(worldRect_2.left() + var*worldRect_2.width()/values.size());
        graphValues_2_2[var].setY(amplSpectreNoiseReduced[var]);

        graphValues_3_1[var].setX(worldRect_3.left() + var*worldRect_3.width()/values.size());
        graphValues_3_1[var].setY(timeSignal2real[var]);

        graphValues_3_2[var].setX(worldRect_3.left() + var*worldRect_3.width()/values.size());
        graphValues_3_2[var].setY(trueSignal[var]);
    }


    update();
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QRectF worldRect_1  (QPointF(t0,ymax),QPointF(tmax,ymin));
    QRectF worldRect_2  (QPointF(0,ymax_2),QPoint(fd,0));
    QRectF worldRect_3  (QPointF(t0,ymax),QPointF(tmax,ymin));

    QRect outerrect_1 = ui->frame->geometry();
    QRect outerrect_2 = ui->frame_2->geometry();
    QRect outerrect_3 = ui->frame_3->geometry();

    graphParams graph1;
    graph1.outerrect = outerrect_1;
    graph1.worldRect = worldRect_1;
    graph1.points = {graphValues_1};
//    graph1.lines = lines;
    graph1.n = N-1;
    graph1.title = "Исходный сигнал";
    graph1.captionX = "t, с";
    graph1.captionY = "X";

    graphParams graph2;
    graph2.outerrect = outerrect_2;
    graph2.worldRect = worldRect_2;
    graph2.points = {graphValues_2_1,graphValues_2_2};
//    graph2.lines = linesSpectre;
    graph2.n = N-1;
    graph2.title = "Фурье образ сигнала";
    graph2.captionX = "f, Гц";
    graph2.captionY = "X";

    graphParams graph3;
    graph3.outerrect = outerrect_3;
    graph3.worldRect = worldRect_3;
    graph3.points = {graphValues_3_1,graphValues_3_2};
//    graph3.lines = lines_2;
    graph3.n = N-1;
    graph3.title = "Отфильтрованный сигнал";
    graph3.captionX = "t, с";
    graph3.captionY = "X";


    DrawGraph(graph1);
    DrawGraph(graph2);
    DrawGraph(graph3);

}

std::vector<double> MainWindow::getFuncValues()
{
    float A1 = ui->editA1->text().toFloat();
    float A2 = ui->editA2->text().toFloat();
    float A3 = ui->editA3->text().toFloat();

    float f1 = ui->editf1->text().toFloat();
    float f2 = ui->editf2->text().toFloat();
    float f3 = ui->editf3->text().toFloat();

    float PHI1 = ui->editPhi1->text().toFloat();
    float PHI2 = ui->editPhi2->text().toFloat();
    float PHI3 = ui->editPhi3->text().toFloat();

    fd = ui->editFd->text().toFloat();
    N = ui->editN->text().toInt();

    dt = 1./fd;

    static double PI = 3.14159265358979323;
    auto sin1 = [&A1,&f1,&PHI1] (double t) -> double {return A1*sin(2*PI*f1*t + PHI1);};
    auto sin2 = [&A2,&f2,&PHI2] (double t) -> double {return A2*sin(2*PI*f2*t + PHI2);};
    auto sin3 = [&A3,&f3,&PHI3] (double t) -> double {return A3*sin(2*PI*f3*t + PHI3);};

    std::vector<double> values;

    for (int var = 0; var < N; ++var) {
        values.push_back(sin1(var/fd) + sin2(var/fd)+ sin3(var/fd));
    }

    return values;
}



void MainWindow::DrawGraph(graphParams graph)
{
    auto& outerrect = graph.outerrect;
    auto& worldRect = graph.worldRect;
//    auto& lines     = graph.lines;
    auto& points    = graph.points;
    auto& n         = graph.n;
    auto& title     = graph.title;
    auto& captionX  = graph.captionX;
    auto& captionY  = graph.captionY;

    QRect rect = outerrect; rect.adjust(25,15,-25,-15);

    double y_max = worldRect.top();

    int nGridX = 10;
    int nGridY = 2*round(pow(10,log10(y_max)-floor(log10(y_max))))* ((ceil(pow(10,log10(y_max)-floor(log10(y_max))))==1)? 5:1 );
    //ymax = [0,1,2,3,4,5,6,7,8,9] * 10^n!!!




    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(outerrect,QBrush(Qt::white, Qt::SolidPattern));
    painter.fillRect(rect     ,QBrush(Qt::white, Qt::SolidPattern));

//     Отрисовка сетки
    painter.setPen(QPen(Qt::gray,1,Qt::PenStyle::DashLine));
    for (int var = 1; var < nGridX; ++var)
    {
        float varF = var;

        int left = rect.left();
        int right = rect.right();
        int top = rect.top();
        int bottom = rect.bottom();

        int width = rect.width();

        int screenX = left + varF / nGridX * width;
        painter.drawLine(screenX,top,screenX,bottom);
    }
    for (int var = 1; var < nGridY; ++var)
    {
        //if (var == nGridY/2) continue;

        float varF = var;

        int left = rect.left();
        int right = rect.right();
        int top = rect.top();
        int bottom = rect.bottom();

        int height = rect.height();

        int screenY = top + varF / nGridY * height;
        painter.drawLine(left,screenY,right,screenY);
    }

//     Отрисовка графика
    QPen pen(Qt::red); pen.setWidth(2);
    painter.setPen(pen);
    if (lines != NULL) painter.drawLines(lines,n);

    if (!points.empty())
        for (int var = 0; var < points.size(); ++var) {
            switch (var) {
            case 0: pen.setColor(Qt::red);      break;
            case 1: pen.setColor(Qt::blue);     break;
            case 2: pen.setColor(Qt::green);    break;
            default:
                break;
            }
            painter.setPen(pen);

            if (!points[var].empty())
            for (int i = 0; i < points[var].size()-1; i++)
            {
                QPointF p1, p2;
                p1.setX(rect.left() + (points[var][i].x() - worldRect.left()) * rect.width() / worldRect.width());
                p1.setY(rect.top()  + (points[var][i].y() - worldRect.top() ) * rect.height()/ worldRect.height());

                p2.setX(rect.left() + (points[var][i+1].x() - worldRect.left())* rect.width() / worldRect.width());
                p2.setY(rect.top()  + (points[var][i+1].y() - worldRect.top()) * rect.height()/ worldRect.height());

//                if (i == n) {painter.setPen(pen2); i++; continue;}
                painter.drawLine(p1, p2);
            }
        }




//     Отрисовка главных осей

    painter.setPen(QPen(Qt::black));

    int x0 = rect.left() + worldRect.left()*rect.width()/worldRect.width();
    int y0 = rect.bottom() - worldRect.bottom()*rect.height()/worldRect.height();
    painter.drawLine(QPointF(x0,rect.top()),QPointF(x0,rect.bottom()));
    painter.drawLine(QPointF(rect.left(),y0),QPointF(rect.right(),y0));


//      Подписи к сетке
    painter.setFont(QFont("Arial",7));
    for (int var = 1; var < nGridX; ++var)
    {
        float varF = var;

        int left = rect.left();
        int right = rect.right();
        int top = rect.top();
        int bottom = rect.bottom();

        int width = rect.width();

        int screenX  = left + varF / nGridX * width;
        float worldX = varF/nGridX * worldRect.width() + worldRect.left();

        QRect textRect(QPoint(screenX -10,y0),QPoint(screenX + 10,y0+10));
        painter.drawText(textRect,Qt::AlignCenter,QString::number(worldX,'g',3));
    }
    for (int var = 1; var < nGridY; ++var)
    {
        float varF = var;

        int left = rect.left();
        int right = rect.right();
        int top = rect.top();
        int bottom = rect.bottom();

        int height = rect.height();

        int screenY = top + varF / nGridY * height;
        float worldY = varF/nGridY * worldRect.height() + worldRect.top();

        QRect textRect(QPoint(outerrect.left(),screenY-5),QPoint(rect.left(),screenY+5));
        painter.drawText(textRect,Qt::AlignCenter,QString::number(worldY,'g',3));
    }


//      Подписи к осям
    painter.setFont(QFont("Arial",8));
    painter.setPen(QPen(Qt::black));
    painter.drawText(QRect(QPoint(x0,rect.top()),QSize(30,30)), captionY);
    painter.drawText(QRect(QPoint(rect.right()-30,y0),QSize(30,30)),captionX);

//    Название графика
    painter.setFont(QFont("Arial",10,-1,true));
    QTextOption textoption;
    textoption.setAlignment(Qt::AlignCenter);
    painter.drawText(QRect(outerrect.left(),outerrect.top(),outerrect.width(),15),title,textoption);

//    Края графика
    painter.drawLine(outerrect.topLeft(),outerrect.topRight());
    painter.drawLine(outerrect.bottomLeft(),outerrect.bottomRight());
    painter.drawLine(outerrect.topLeft(),outerrect.bottomLeft());
    painter.drawLine(outerrect.topRight(),outerrect.bottomRight());
}

QLine *MainWindow::GetGraphLines(QRect outerrect, QRectF worldRect, std::vector<QPointF> values)
{
    QRect rect = outerrect; rect.adjust(25,15,-25,-15);
    int n = values.size();


    QLine* graphlines = new QLine[n-1];
    for (int var = 0; var < n-1; ++var) {

        double x1 = rect.left() + (-worldRect.left() + values[var].x()) * rect.width() / worldRect.width();
        double x2 = rect.left() + (-worldRect.left() + values[var+1].x()) * rect.width() / worldRect.width();

        double y1 = rect.top() + (-worldRect.top() + values[var].y()) * rect.height()/ worldRect.height();
        double y2 = rect.top() + (-worldRect.top() + values[var+1].y()) * rect.height()/ worldRect.height();

        graphlines[var] = QLine(x1,y1,x2,y2);
    }
    return graphlines;
}

double MainWindow::ClosestMantissa(std::vector<double> values)
{
    double fmax = *(std::max_element(values.begin(),values.end()));
    double fmin = *(std::min_element(values.begin(),values.end()));
    double fextr = (fmax > -fmin)? fmax:-fmin;

    return pow(10,floor(log10(fextr))) * ceil(pow(10,log10(fextr)-floor(log10(fextr))));
}

