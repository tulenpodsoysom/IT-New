#include "mygraph.h"
#include <cmath>

void MyGraph::Draw(QPainter& painter)
{
    if (!isShown) return;
    QRect rect = outerrect; rect.adjust(25,15,-25,-15);

    double y_max = worldRect.top();

    int nGridX = 10;
    int nGridY = 2*round(pow(10,log10(y_max)-floor(log10(y_max))))* ((ceil(pow(10,log10(y_max)-floor(log10(y_max))))==1)? 5:1 );
    //ymax = [0,1,2,3,4,5,6,7,8,9] * 10^n!!!




//    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

        painter.fillRect(outerrect,QBrush(backgroundColor, Qt::SolidPattern));
//    painter.fillRect(outerrect,QBrush(Qt::white, Qt::SolidPattern));
//    painter.fillRect(rect     ,QBrush(Qt::white, Qt::SolidPattern));

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
//    if (lines != NULL) painter.drawLines(lines,n);

    if (!points.empty())
        for (int var = 0; var < points.size(); ++var) {
            switch (var) {
                case 0: pen.setColor(Qt::red);      break;
                case 1: pen.setColor(Qt::blue);     break;
                case 2: pen.setColor(Qt::green);    break;
                case 3: pen.setColor(Qt::yellow);    break;
                case 4: pen.setColor(Qt::cyan);    break;

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
    painter.setFont(QFont("Arial", 8));
    painter.setPen(QPen(Qt::black));
    painter.drawText(QRect(QPoint(x0,rect.top()),QSize(30,30)), captionY);
    painter.drawText(QRect(QPoint(rect.right()-30,y0),QSize(30,30)),captionX);

//    Название графика
    painter.setFont(QFont("Arial", 10, -1, true));
    QTextOption textoption;
    textoption.setAlignment(Qt::AlignCenter);
    painter.drawText(QRect(outerrect.left(),outerrect.top(),outerrect.width(),15),title,textoption);

//    Края графика
    painter.drawLine(outerrect.topLeft(),outerrect.topRight());
    painter.drawLine(outerrect.bottomLeft(),outerrect.bottomRight());
    painter.drawLine(outerrect.topLeft(),outerrect.bottomLeft());
    painter.drawLine(outerrect.topRight(),outerrect.bottomRight());
}

double MyGraph::ClosestMantissa(std::vector<double> values)
{
    double fmax = *(std::max_element(values.begin(),values.end()));
    double fmin = *(std::min_element(values.begin(),values.end()));
    double fextr = (fmax > -fmin)? fmax:-fmin;

    return pow(10,floor(log10(fextr))) * ceil(pow(10,log10(fextr)-floor(log10(fextr))));
}

std::vector<QPointF> MyGraph::DoubleToPoints(std::vector<double> values,QRectF worldRect)
{
    std::vector<QPointF> graphPoints(values.size());
    for (size_t var = 0; var < values.size(); ++var) {
        graphPoints[var].setX  (worldRect.left() + var*worldRect.width()/values.size());
        graphPoints[var].setY  (values[var]);
    }

    return graphPoints;
}

MyGraph::MyGraph()
{

}
