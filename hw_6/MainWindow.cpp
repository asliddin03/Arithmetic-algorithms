#include "MainWindow.h"
#include "io/FileReader.h"

#include <QHBoxLayout>
#include <QGroupBox>
#include <QCoreApplication>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    auto* central = new QWidget(this);
    setCentralWidget(central);

    fileLabel_ = new QLabel("File: input.txt", this);
    infoLabel_ = new QLabel(this);
    statusLabel_ = new QLabel(this);

    idealGrid_ = new IdealGrid(this);
    idealGrid_->setMode(IdealGrid::Mode::Ideal);
    idealGrid_->setIdeal(&ideal_);

    outsideGrid_ = new IdealGrid(this);
    outsideGrid_->setMode(IdealGrid::Mode::Outside);
    outsideGrid_->setIdeal(&ideal_);

    tabs_ = new QTabWidget(this);
    tabs_->addTab(idealGrid_, "Ideal");
    tabs_->addTab(outsideGrid_, "Outside");

    auto* infoBox = new QGroupBox("Info", this);
    auto* infoLayout = new QVBoxLayout(infoBox);
    infoLayout->addWidget(fileLabel_);
    infoLayout->addWidget(infoLabel_);
    infoLayout->addWidget(statusLabel_);
    infoLayout->addStretch();

    auto* mainLayout = new QHBoxLayout(central);
    mainLayout->addWidget(infoBox, 0);
    mainLayout->addWidget(tabs_, 1);

    infoLabel_->setText(
        "Program reads data from input.txt\n"
        "Format:\n"
        "n\n"
        "a1 b1\n"
        "...\n"
        "an bn\n"
        "M N"
    );

    loadInputFile();
}

void MainWindow::loadInputFile() {
    const QString fileName = QCoreApplication::applicationDirPath() + "/input.txt";

    InputData data;
    QString errorText;

    if (!FileReader::readFromFile(fileName, data, errorText)) {
        statusLabel_->setText("<font color='red'>" + errorText + "</font>");
        return;
    }

    loadData(std::move(data));
}

void MainWindow::loadData(InputData&& data) {
    ideal_.setGenerators(std::move(data.generators));

    idealGrid_->setRange(data.maxX, data.maxY);
    outsideGrid_->setRange(data.maxX, data.maxY);

    statusLabel_->setText(
        QString("<font color='darkgreen'>"
                "Loaded. Generators: %1. Size: %2 x %3."
                "</font>")
            .arg(static_cast<int>(ideal_.minGenerators().size()))
            .arg(data.maxX)
            .arg(data.maxY)
    );
}