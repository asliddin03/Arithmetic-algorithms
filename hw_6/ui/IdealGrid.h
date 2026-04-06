#pragma once

#include <QWidget>
#include "../math/MonomialIdeal.h"

class IdealGrid : public QWidget {
    Q_OBJECT
public:
    enum class Mode {
        Ideal,
        Outside
    };

    explicit IdealGrid(QWidget *parent = nullptr);

    void setIdeal(const MonomialIdeal* ideal) noexcept;
    void setRange(int maxX, int maxY) noexcept;
    void setMode(Mode mode) noexcept;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    bool needFillCell(int x, int y) const noexcept;

    const MonomialIdeal* ideal_ = nullptr;
    int maxX_ = 15;
    int maxY_ = 15;
    Mode mode_ = Mode::Ideal;
};