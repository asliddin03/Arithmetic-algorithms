#include "MonomialIdeal.h"
#include <algorithm>

namespace {
    bool lessMonomial(const Monomial2D& a, const Monomial2D& b) noexcept {
        if (a.xDeg != b.xDeg) {
            return a.xDeg < b.xDeg;
        }
        return a.yDeg < b.yDeg;
    }

    bool sameMonomial(const Monomial2D& a, const Monomial2D& b) noexcept {
        return a.xDeg == b.xDeg && a.yDeg == b.yDeg;
    }
}

MonomialIdeal::MonomialIdeal(std::vector<Monomial2D> generators) : generators_(std::move(generators)) {
    normalize();
}

void MonomialIdeal::setGenerators(std::vector<Monomial2D> generators) {
    generators_ = std::move(generators);
    normalize();
}

bool MonomialIdeal::empty() const noexcept {
    return generators_.empty();
}

bool MonomialIdeal::hasMonomial(int xDeg, int yDeg) const noexcept {
    for (const auto& g : generators_) {
        if (g.xDeg > xDeg) {
            break;
        }
        if (g.yDeg <= yDeg) {
            return true;
        }
    }
    return false;
}

bool MonomialIdeal::isOutsideIdeal(int xDeg, int yDeg) const noexcept {
    return !hasMonomial(xDeg, yDeg);
}

const std::vector<Monomial2D>& MonomialIdeal::minGenerators() const noexcept {
    return generators_;
}

void MonomialIdeal::normalize() {
    if (generators_.empty()) {
        return;
    }

    std::sort(generators_.begin(), generators_.end(), lessMonomial);

    generators_.erase(
        std::unique(generators_.begin(), generators_.end(), sameMonomial),
        generators_.end()
    );

    std::vector<Monomial2D> minList;
    minList.reserve(generators_.size());

    int bestY = -1;

    for (const auto& g : generators_) {
        if (bestY != -1 && g.yDeg >= bestY) {
            continue;
        }

        minList.push_back(g);
        bestY = g.yDeg;
    }

    generators_ = std::move(minList);
}