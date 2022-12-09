#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QMessageBox>
#include <random>
#include "complex.h"



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    struct graphParams
    {
        QRect  outerrect;
        QRectF worldRect;

        std::vector<std::vector<QPointF>> points;
//        QLine* lines;
        int n;

        QString title;
        QString captionX;
        QString captionY;
        graphParams() {}
    };

    //graphParams graph1,graph2,graph3;

    float fd;
    int N;

    float dt;

    float t0;
    float tmax;

    double ymin;
    double ymax;

    double ymin_2;
    double ymax_2;

    double ymin_3;
    double ymax_3;

    std::random_device rd;

    QLine* lines = NULL;
    QLine* linesSpectre = NULL;
    QLine* lines_2 = NULL;

    std::vector<QPointF> graphValues_1,graphValues_2_1,graphValues_2_2,graphValues_3_1,graphValues_3_2;

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event);
    std::vector<double> getFuncValues();
    void DrawGraph(graphParams graph);
    QLine* GetGraphLines(QRect outerrect, QRectF worldRect,std::vector<QPointF> values);
    double ClosestMantissa(std::vector<double> values);
};
#endif // MAINWINDOW_H
