#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    struct graphParams
    {
        QRect  outerrect;
        QRectF worldRect;

        std::vector<std::vector<QPointF>> points;
        QLine* lines;
        int n;

        QString title;
        QString captionX;
        QString captionY;
        graphParams() {}
    };

    std::vector<QPointF> signalPoints;
    std::vector<QPointF> predictionPoints;
    std::vector<QPointF> errorPoints;
    std::vector<QPointF> convolutionPoints;
    std::vector<QPointF> thresholdPoints;

    double ymax_2;

    int N;
    float fd;

    float f1;
    float f2;
    float f3;

    int N1, N2;

    void DrawGraph(graphParams graph);
    double ClosestMantissa(std::vector<double> values);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event);
};
#endif // MAINWINDOW_H
