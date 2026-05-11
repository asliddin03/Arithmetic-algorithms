#include "mainwindow.h"
#include "powersum.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QMessageBox>
#include <QString>
#include <QtCharts/QChart>
#include <QPainter>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    buildInterface();
}

void MainWindow::buildInterface() {
    QWidget* centralWidget = new QWidget(this);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    QHBoxLayout* inputLayout = new QHBoxLayout();

    QLabel* nLabel = new QLabel("Введите n:");
    nInput = new QLineEdit();
    nInput->setPlaceholderText("Например: 100");

    QLabel* pLabel = new QLabel("Введите p:");
    pInput = new QLineEdit();
    pInput->setPlaceholderText("Например: 3");

    calculateButton = new QPushButton("Вычислить");

    inputLayout->addWidget(nLabel);
    inputLayout->addWidget(nInput);
    inputLayout->addWidget(pLabel);
    inputLayout->addWidget(pInput);
    inputLayout->addWidget(calculateButton);

    resultText = new QTextEdit();
    resultText->setReadOnly(true);

    chartByNView = new QChartView();
    chartByNView->setMinimumHeight(300);

    chartByPView = new QChartView();
    chartByPView->setMinimumHeight(300);

    mainLayout->addLayout(inputLayout);
    mainLayout->addWidget(resultText);
    mainLayout->addWidget(chartByNView);
    mainLayout->addWidget(chartByPView);

    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    connect(calculateButton, &QPushButton::clicked,
            this, &MainWindow::calculate);

    setWindowTitle("Вычисление суммы степеней");
    resize(1000, 900);
}

bool MainWindow::readInput(int& n, int& p) {
    bool nOk = false;
    bool pOk = false;

    n = nInput->text().toInt(&nOk);
    p = pInput->text().toInt(&pOk);

    if (!nOk || !pOk) {
        QMessageBox::warning(
            this,
            "Ошибка ввода",
            "n и p должны быть натуральными числами."
        );
        return false;
    }

    if (n <= 0 || p <= 0) {
        QMessageBox::warning(
            this,
            "Ошибка ввода",
            "n и p должны быть больше нуля."
        );
        return false;
    }

    return true;
}

void MainWindow::calculate() {
    int n;
    int p;

    if (!readInput(n, p)) {
        return;
    }

    Result direct = sumDirect(n, p);
    Result asymptotic = sumAsymptotic(n, p);
    Result exact = sumExactFormula(n, p);

    long double asymptoticError = direct.value - asymptotic.value;
    if (asymptoticError < 0) {
        asymptoticError = -asymptoticError;
    }

    long double exactError = direct.value - exact.value;
    if (exactError < 0) {
        exactError = -exactError;
    }

    QString text;

    text += "S(n, p) = 1^p + 2^p + ... + n^p\n\n";

    text += "n = " + QString::number(n) + "\n";
    text += "p = " + QString::number(p) + "\n\n";

    text += "1) Непосредственно по определению:\n";
    text += "Значение: " + QString::number(static_cast<double>(direct.value), 'g', 16) + "\n";
    text += "Количество операций: " + QString::number(direct.operations) + "\n\n";

    text += "2) С помощью асимптотики Эйлера:\n";
    text += "Значение: " + QString::number(static_cast<double>(asymptotic.value), 'g', 16) + "\n";
    text += "Количество операций: " + QString::number(asymptotic.operations) + "\n";
    text += "Погрешность: " + QString::number(static_cast<double>(asymptoticError), 'g', 16) + "\n\n";

    text += "3) С помощью точной формулы Бернулли:\n";
    text += "Значение: " + QString::number(static_cast<double>(exact.value), 'g', 16) + "\n";
    text += "Количество операций: " + QString::number(exact.operations) + "\n";
    text += "Погрешность: " + QString::number(static_cast<double>(exactError), 'g', 16) + "\n\n";

    text += "Вывод:\n";
    text += "Асимптотическая формула работает быстрее всего, но даёт приближённый результат.\n";
    text += "Точная формула Бернулли даёт более точный результат, но зависит от p.\n";
    text += "Непосредственное вычисление простое, но при большом n работает медленнее.\n";

    resultText->setText(text);

    buildChartByN(p, n);
    buildChartByP(n, p);
}

void MainWindow::buildChartByN(int fixedP, int maxN) {
    QLineSeries* directSeries = new QLineSeries();
    QLineSeries* asymptoticSeries = new QLineSeries();
    QLineSeries* exactSeries = new QLineSeries();

    directSeries->setName("По определению");
    asymptoticSeries->setName("Асимптотика Эйлера");
    exactSeries->setName("Точная формула");

    int step = maxN / 50;
    if (step < 1) {
        step = 1;
    }

    for (int n = 1; n <= maxN; n += step) {
        Result direct = sumDirect(n, fixedP);
        Result asymptotic = sumAsymptotic(n, fixedP);
        Result exact = sumExactFormula(n, fixedP);

        directSeries->append(n, direct.operations);
        asymptoticSeries->append(n, asymptotic.operations);
        exactSeries->append(n, exact.operations);
    }

    QChart* chart = new QChart();
    chart->addSeries(directSeries);
    chart->addSeries(asymptoticSeries);
    chart->addSeries(exactSeries);

    chart->setTitle("Зависимость числа операций от n при фиксированном p");

    chart->createDefaultAxes();

    chartByNView->setChart(chart);
    chartByNView->setRenderHint(QPainter::Antialiasing);
}

void MainWindow::buildChartByP(int fixedN, int maxP) {
    QLineSeries* directSeries = new QLineSeries();
    QLineSeries* asymptoticSeries = new QLineSeries();
    QLineSeries* exactSeries = new QLineSeries();

    directSeries->setName("По определению");
    asymptoticSeries->setName("Асимптотика Эйлера");
    exactSeries->setName("Точная формула");

    for (int p = 1; p <= maxP; p++) {
        Result direct = sumDirect(fixedN, p);
        Result asymptotic = sumAsymptotic(fixedN, p);
        Result exact = sumExactFormula(fixedN, p);

        directSeries->append(p, direct.operations);
        asymptoticSeries->append(p, asymptotic.operations);
        exactSeries->append(p, exact.operations);
    }

    QChart* chart = new QChart();
    chart->addSeries(directSeries);
    chart->addSeries(asymptoticSeries);
    chart->addSeries(exactSeries);

    chart->setTitle("Зависимость числа операций от p при фиксированном n");

    chart->createDefaultAxes();

    chartByPView->setChart(chart);
    chartByPView->setRenderHint(QPainter::Antialiasing);
}