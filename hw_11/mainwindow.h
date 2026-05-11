#pragma once

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void calculate();

private:
    QLineEdit* nInput;
    QLineEdit* pInput;
    QPushButton* calculateButton;
    QTextEdit* resultText;

    QChartView* chartByNView;
    QChartView* chartByPView;

    void buildInterface();
    void buildChartByN(int fixedP, int maxN);
    void buildChartByP(int fixedN, int maxP);

    bool readInput(int& n, int& p);
};
