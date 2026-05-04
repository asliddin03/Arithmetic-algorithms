#include "mainwindow.h"
#include "math_utils.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QTextEdit>

#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

#include <QPainter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *central = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout();

    QHBoxLayout *inputLayout = new QHBoxLayout();

    QLabel *rLabel = new QLabel("r:");
    rSpinBox = new QSpinBox();
    rSpinBox->setRange(1, 50);
    rSpinBox->setValue(2);

    QLabel *digitsLabel = new QLabel("Точность:");
    digitsSpinBox = new QSpinBox();
    digitsSpinBox->setRange(5, 90);
    digitsSpinBox->setValue(30);

    QPushButton *button = new QPushButton("Вычислить и построить графики");

    inputLayout->addWidget(rLabel);
    inputLayout->addWidget(rSpinBox);
    inputLayout->addWidget(digitsLabel);
    inputLayout->addWidget(digitsSpinBox);
    inputLayout->addWidget(button);

    resultText = new QTextEdit();
    resultText->setReadOnly(true);

    termsExpSeries = new QLineSeries();
    termsPiSeries = new QLineSeries();

    operationsExpSeries = new QLineSeries();
    operationsPiSeries = new QLineSeries();

    termsExpSeries->setName("e^r");
    termsPiSeries->setName("π");

    operationsExpSeries->setName("e^r");
    operationsPiSeries->setName("π");

    termsChartView = createChart(
        "Количество членов ряда от точности",
        "Количество верных знаков",
        "Количество членов ряда",
        termsExpSeries,
        termsPiSeries
    );

    operationsChartView = createChart(
        "Количество арифметических операций от точности",
        "Количество верных знаков",
        "Количество операций",
        operationsExpSeries,
        operationsPiSeries
    );

    mainLayout->addLayout(inputLayout);
    mainLayout->addWidget(resultText);
    mainLayout->addWidget(termsChartView);
    mainLayout->addWidget(operationsChartView);

    central->setLayout(mainLayout);
    setCentralWidget(central);

    resize(1000, 800);
    setWindowTitle("Вычисление e^r и π с заданной точностью");

    connect(button, &QPushButton::clicked, this, &MainWindow::calculate);

    calculate();
}

QChartView* MainWindow::createChart(
    const QString &title,
    const QString &xTitle,
    const QString &yTitle,
    QLineSeries *expSeries,
    QLineSeries *piSeries
) {
    QChart *chart = new QChart();

    chart->addSeries(expSeries);
    chart->addSeries(piSeries);

    chart->setTitle(title);
    chart->legend()->setVisible(true);

    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText(xTitle);
    axisX->setLabelFormat("%d");

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText(yTitle);
    axisY->setLabelFormat("%d");

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    expSeries->attachAxis(axisX);
    expSeries->attachAxis(axisY);

    piSeries->attachAxis(axisX);
    piSeries->attachAxis(axisY);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    return chartView;
}

void MainWindow::calculate() {
    int r = rSpinBox->value();
    int digits = digitsSpinBox->value();

    CalculationResult expResult = computeExp(r, digits);
    CalculationResult piResult = computePi(digits);

    QString text;

    text += "Результаты вычислений\n\n";

    text += "e^" + QString::number(r) + " =\n";
    text += QString::fromStdString(expResult.value.str(digits + 2));
    text += "\n\n";

    text += "π =\n";
    text += QString::fromStdString(piResult.value.str(digits + 2));
    text += "\n\n";

    text += "Сложность вычисления e^" + QString::number(r) + ":\n";
    text += "Количество членов ряда: "
          + QString::number(expResult.terms) + "\n";
    text += "Количество арифметических операций: "
          + QString::number(expResult.operations) + "\n\n";

    text += "Сложность вычисления π:\n";
    text += "Количество членов ряда: "
          + QString::number(piResult.terms) + "\n";
    text += "Количество арифметических операций: "
          + QString::number(piResult.operations) + "\n";

    resultText->setText(text);

    buildGraphs(r);
}

void MainWindow::buildGraphs(int r) {
    termsExpSeries->clear();
    termsPiSeries->clear();

    operationsExpSeries->clear();
    operationsPiSeries->clear();

    int maxTerms = 0;
    long long maxOperations = 0;

    for (int digits = 5; digits <= 90; digits += 5) {
        CalculationResult expResult = computeExp(r, digits);
        CalculationResult piResult = computePi(digits);

        termsExpSeries->append(digits, expResult.terms);
        termsPiSeries->append(digits, piResult.terms);

        operationsExpSeries->append(digits, expResult.operations);
        operationsPiSeries->append(digits, piResult.operations);

        maxTerms = std::max(maxTerms, expResult.terms);
        maxTerms = std::max(maxTerms, piResult.terms);

        maxOperations = std::max(maxOperations, expResult.operations);
        maxOperations = std::max(maxOperations, piResult.operations);
    }

    QValueAxis *termsAxisX = qobject_cast<QValueAxis*>(
        termsChartView->chart()->axes(Qt::Horizontal).first()
    );

    QValueAxis *termsAxisY = qobject_cast<QValueAxis*>(
        termsChartView->chart()->axes(Qt::Vertical).first()
    );

    QValueAxis *operationsAxisX = qobject_cast<QValueAxis*>(
        operationsChartView->chart()->axes(Qt::Horizontal).first()
    );

    QValueAxis *operationsAxisY = qobject_cast<QValueAxis*>(
        operationsChartView->chart()->axes(Qt::Vertical).first()
    );

    if (termsAxisX && termsAxisY) {
        termsAxisX->setRange(5, 90);
        termsAxisX->setTickCount(10);

        termsAxisY->setRange(0, maxTerms + 5);
        termsAxisY->setTickCount(6);
    }

    if (operationsAxisX && operationsAxisY) {
        operationsAxisX->setRange(5, 90);
        operationsAxisX->setTickCount(10);

        operationsAxisY->setRange(0, maxOperations + 10);
        operationsAxisY->setTickCount(6);
    }
}
