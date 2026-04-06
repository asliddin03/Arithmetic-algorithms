#pragma once

#include <QString>
#include "InputData.h"

class FileReader {
public:
    static bool readFromFile(const QString& fileName, InputData& data, QString& errorText);
};