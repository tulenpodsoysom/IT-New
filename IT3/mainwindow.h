#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QMessageBox>
#include <random>
#include "filterprogram.h"
#include "Complex.h"
#include "mygraph.h"



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    std::random_device rd;
    std::mt19937 mt;

    MyGraph graphSignal;
    MyGraph graphWindow;

    MyGraph graphFiltered1;
    MyGraph graphFiltered2;
    MyGraph graphFiltered3;
    MyGraph graphFiltered4;
    MyGraph graphFiltered5;

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event);
//    void DrawGraph(graphParams graph);
//    QLine* GetGraphLines(QRect outerrect, QRectF worldRect,std::vector<QPointF> values);
//    double ClosestMantissa(std::vector<double> values);
};
#endif // MAINWINDOW_H
