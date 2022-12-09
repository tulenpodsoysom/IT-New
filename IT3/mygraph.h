#ifndef MYGRAPH_H
#define MYGRAPH_H

#include <vector>
#include <QRect>
#include <QPointF>
#include <QString>
#include <QPainter>

class MyGraph
{
public:

    bool isShown = true;

    QRect  outerrect;
    QRectF worldRect;

    std::vector<std::vector<QPointF>> points;

    QString title;
    QString captionX;
    QString captionY;


    QColor backgroundColor = Qt::white;

    void Draw(QPainter& painter);
    static double ClosestMantissa(std::vector<double> values);

    static std::vector<QPointF> DoubleToPoints(std::vector<double>,QRectF worldRect);
    MyGraph();
};

#endif // MYGRAPH_H
