#pragma once

#include <vector>
#include "../math/Monomial2D.h"

struct InputData {
    std::vector<Monomial2D> generators;
    int maxX = 15;
    int maxY = 15;
};