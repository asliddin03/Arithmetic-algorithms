#include "IdealGrid.h"
#include <QPainter>
#include <algorithm>

IdealGrid::IdealGrid(QWidget *parent) : QWidget(parent) {
    setMinimumSize(500, 500);
}

void IdealGrid::setIdeal(const MonomialIdeal* ideal) noexcept {
    ideal_ = ideal;
    update();
}

void IdealGrid::setRange(int maxX, int maxY) noexcept {
    maxX_ = std::max(0, maxX);
    maxY_ = std::max(0, maxY);
    update();
}

void IdealGrid::setMode(Mode mode) noexcept {
    mode_ = mode;
    update();
}

bool IdealGrid::needFillCell(int x, int y) const noexcept {
    if (ideal_ == nullptr || ideal_->empty()) {
        return false;
    }

    if (mode_ == Mode::Ideal) {
        return ideal_->hasMonomial(x, y);
    }

    return ideal_->isOutsideIdeal(x, y);
}

void IdealGrid::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(rect(), Qt::white);

    const int left = 60;
    const int right = 30;
    const int top = 30;
    const int bottom = 50;

    QRect area(left, top, width() - left - right, height() - top - bottom);

    if (area.width() <= 0 || area.height() <= 0) {
        return;
    }

    const int cols = maxX_ + 1;
    const int rows = maxY_ + 1;

    if (cols <= 0 || rows <= 0) {
        return;
    }

    const double cellW = static_cast<double>(area.width()) / cols;
    const double cellH = static_cast<double>(area.height()) / rows;

    painter.setPen(Qt::black);
    if (mode_ == Mode::Ideal) {
        painter.drawText(10, 20, "Points in ideal");
    } else {
        painter.drawText(10, 20, "Points outside ideal");
    }

    const QColor fillColor =
        (mode_ == Mode::Ideal) ? QColor(120, 180, 255) : QColor(180, 255, 180);

    for (int x = 0; x <= maxX_; ++x) {
        const double px = area.left() + x * cellW;

        for (int y = 0; y <= maxY_; ++y) {
            if (!needFillCell(x, y)) {
                continue;
            }

            const double py = area.bottom() - (y + 1) * cellH;
            QRectF cell(px, py, cellW, cellH);
            painter.fillRect(cell.adjusted(1, 1, -1, -1), fillColor);
        }
    }

    painter.setPen(QPen(Qt::lightGray, 1));
    for (int i = 0; i <= cols; ++i) {
        const double x = area.left() + i * cellW;
        painter.drawLine(QPointF(x, area.top()), QPointF(x, area.bottom()));
    }

    for (int j = 0; j <= rows; ++j) {
        const double y = area.bottom() - j * cellH;
        painter.drawLine(QPointF(area.left(), y), QPointF(area.right(), y));
    }

    painter.setPen(QPen(Qt::black, 2));
    painter.drawLine(area.bottomLeft(), area.bottomRight());
    painter.drawLine(area.bottomLeft(), area.topLeft());

    painter.setPen(Qt::black);
    painter.drawText(area.right() + 8, area.bottom() + 5, "m");
    painter.drawText(area.left() - 15, area.top() - 5, "n");

    for (int x = 0; x <= maxX_; ++x) {
        const double px = area.left() + (x + 0.5) * cellW;
        painter.drawText(QPointF(px - 5, area.bottom() + 20), QString::number(x));
    }

    for (int y = 0; y <= maxY_; ++y) {
        const double py = area.bottom() - (y + 0.5) * cellH;
        painter.drawText(QPointF(area.left() - 25, py + 5), QString::number(y));
    }

    if (ideal_ == nullptr) {
        return;
    }

    const auto& gens = ideal_->minGenerators();

    painter.setBrush(Qt::red);
    painter.setPen(QPen(Qt::darkRed, 2));

    const double radius = std::min(cellW, cellH) * 0.18;

    for (const auto& g : gens) {
        if (g.xDeg > maxX_ || g.yDeg > maxY_) {
            continue;
        }

        const double cx = area.left() + (g.xDeg + 0.5) * cellW;
        const double cy = area.bottom() - (g.yDeg + 0.5) * cellH;

        painter.drawEllipse(QPointF(cx, cy), radius, radius);
        painter.drawText(QPointF(cx + 6, cy - 6),
                         QString("(%1,%2)").arg(g.xDeg).arg(g.yDeg));
    }
}