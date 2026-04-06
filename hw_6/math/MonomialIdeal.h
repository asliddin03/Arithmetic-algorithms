#pragma once

#include <vector>
#include "Monomial2D.h"

class MonomialIdeal {
public:
    MonomialIdeal() = default;
    explicit MonomialIdeal(std::vector<Monomial2D> generators);

    void setGenerators(std::vector<Monomial2D> generators);

    bool empty() const noexcept;
    bool hasMonomial(int xDeg, int yDeg) const noexcept;
    bool isOutsideIdeal(int xDeg, int yDeg) const noexcept;

    const std::vector<Monomial2D>& minGenerators() const noexcept;

private:
    std::vector<Monomial2D> generators_;

    void normalize();
};