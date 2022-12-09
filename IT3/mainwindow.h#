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

        QLine* lines;
        int n;

        QString captionX;
        QString captionY;
        graphParams() {}
    };

    float fd;
    int N;

    float dt;

    float t0;
    float tmax;

    double ymin;
    double ymax;

    double ymin_2;
    double ymax_2;

    QLine* lines = NULL;
    QLine* linesSpectre = NULL;
    QLine* lines_2 = NULL;
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
