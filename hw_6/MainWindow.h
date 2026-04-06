#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QTabWidget>

#include "math/MonomialIdeal.h"
#include "io/InputData.h"
#include "ui/IdealGrid.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void loadInputFile();
    void loadData(InputData&& data);

    QLabel* fileLabel_ = nullptr;
    QLabel* infoLabel_ = nullptr;
    QLabel* statusLabel_ = nullptr;

    IdealGrid* idealGrid_ = nullptr;
    IdealGrid* outsideGrid_ = nullptr;
    QTabWidget* tabs_ = nullptr;

    MonomialIdeal ideal_;
};