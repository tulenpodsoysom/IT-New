#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <cmath>

#include <qpainter.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

     N = 512;
     fd = 512;

     f1 = 20;
     f2 = 50;
     f3 = 10;

     N1 = N/3, N2 = 2*N/3;

     ui->editF1->setText(QString::number(f1));
     ui->editF2->setText(QString::number(f2));
     ui->editF3->setText(QString::number(f3));

     ui->editFd->setText(QString::number(fd));

     ui->editN->setText(QString::number(N));

     ui->editN1->setText(QString::number(N1));
     ui->editN2->setText(QString::number(N2));

     signalPoints.resize(N);
     predictionPoints.resize(N);
     errorPoints.resize(N);
     convolutionPoints.resize(N);
     thresholdPoints.resize(2);
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::DrawGraph(graphParams graph)
{
    auto& outerrect = graph.outerrect;
    auto& worldRect = graph.worldRect;
    auto& lines     = graph.lines;
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


double MainWindow::ClosestMantissa(std::vector<double> values)
{
    double fmax = *(std::max_element(values.begin(),values.end()));
    double fmin = *(std::min_element(values.begin(),values.end()));
    double fextr = (fmax > -fmin)? fmax:-fmin;

    return pow(10,floor(log10(fextr))) * ceil(pow(10,log10(fextr)-floor(log10(fextr))));
}



void MainWindow::on_pushButton_clicked()
{
    f1 = ui->editF1->text().toDouble();
    f2 = ui->editF2->text().toDouble();
    f3 = ui->editF3->text().toDouble();

    fd = ui->editFd->text().toDouble();

    N  = ui->editN->text().toDouble();

    N1 = ui->editN1->text().toDouble();
    N2 = ui->editN2->text().toDouble();

    signalPoints.resize(N);
    predictionPoints.resize(N);
    errorPoints.resize(N);
    convolutionPoints.resize(N);

    static const double PI = 3.14159265358979323;
    double phase = 0; double f = 0;
    for (int var = 0; var < N; ++var) {
        signalPoints[var].setY(sin(phase));
        signalPoints[var].setX(var/fd);

        if      (var < N1) f = f1;
        else if (var < N2) f = f2;
        else               f = f3;

        phase += 2*PI*f/fd;
    }

    auto (Predict) = [this] (double x_1,double x_2)
    {
        return 2.0*cos(2*PI*f2/fd)*x_1 - x_2;
    };

    for (int var = 2; var < N; ++var) {
        predictionPoints[var].setY(Predict(signalPoints[var-1].y(),signalPoints[var-2].y()));
        predictionPoints[var].setX(var/fd);
    }


    for (int var = 2; var < N; ++var) {
        double s = signalPoints[var].y();
        double p = predictionPoints[var].y();
        errorPoints[var].setY((s-p)*(s-p));
        errorPoints[var].setX(var/fd);
    }



    std::vector<double> errorY(N);
    for (int var = 0; var < N; ++var) {
        errorY[var] = errorPoints[var].y();
    }

    double windowWidthReal = ui->editWidth->text().toDouble();
    int    windowWidthN    = windowWidthReal*fd;

    std::vector<double> convolution(N);
    for (int var = windowWidthN/2; var < N - windowWidthN/2; ++var) {
        for (int var2 = -windowWidthN/2; var2 < windowWidthN/2; ++var2) {
            convolution[var] += errorPoints[var + var2].y();
        }   convolution[var] /= windowWidthN;
    }

    for (int var = 0; var < N; ++var) {
        convolutionPoints[var].setX(var/fd);
        convolutionPoints[var].setY(convolution[var]);
    }


    double a = *std::max_element(errorY.begin(),errorY.end());
           a = ceil(pow(10,log10(a)-floor(log10(a)))) * pow(10,floor(log10(a)));
    ymax_2 = a;
           a = *std::max_element(convolution.begin(),convolution.end());
           a = ceil(pow(10,log10(a)-floor(log10(a)))) * pow(10,floor(log10(a)));
    ymax_2 = (a > ymax_2)? a:ymax_2;

    double threshold = ui->editErrorthreshold->text().toDouble();

    thresholdPoints[0].setX(0);
    thresholdPoints[0].setY(threshold);
    thresholdPoints[1].setX(N/fd);
    thresholdPoints[1].setY(threshold);

    int N1Calc = 0,N2Calc = 0;

    int i = windowWidthN;
    while (convolution[i] > threshold) {
        N1Calc = i; i++;
    }
    while (convolution[i] < threshold) {
        N2Calc = i; i++;
    }

    ui->editN1Calc->setText(QString::number(N1Calc));
    ui->editN2Calc->setText(QString::number(N2Calc));
    update();
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    graphParams graph1;

    graph1.worldRect = QRectF(QPointF(0,1),QPointF(N/fd,-1));
    graph1.outerrect = ui->frame->geometry();
    graph1.points = {signalPoints};
    graph1.captionX = "t, c";
    graph1.captionY = "X";
    graph1.title = "Входной сигнал";
    graph1.lines = NULL;
    graph1.n = N;



    graphParams graph2;

    graph2.worldRect = QRectF(QPointF(0,ymax_2),QPointF(N/fd,0));
    graph2.outerrect = ui->frame_2->geometry();
    graph2.points = {errorPoints,convolutionPoints,thresholdPoints};
    graph2.captionX = "t, c";
    graph2.captionY = "Error";
    graph2.title = "Ошибка предсказания";
    graph2.lines = NULL;
    graph2.n = N;

    DrawGraph(graph1);
    DrawGraph(graph2);

}

