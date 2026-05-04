#pragma once

#include <QMainWindow>

class QSpinBox;
class QTextEdit;
class QChartView;
class QLineSeries;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QSpinBox *rSpinBox;
    QSpinBox *digitsSpinBox;
    QTextEdit *resultText;

    QChartView *termsChartView;
    QChartView *operationsChartView;

    QLineSeries *termsExpSeries;
    QLineSeries *termsPiSeries;

    QLineSeries *operationsExpSeries;
    QLineSeries *operationsPiSeries;

    QChartView* createChart(
        const QString &title,
        const QString &xTitle,
        const QString &yTitle,
        QLineSeries *expSeries,
        QLineSeries *piSeries
    );

    void calculate();
    void buildGraphs(int r);
};