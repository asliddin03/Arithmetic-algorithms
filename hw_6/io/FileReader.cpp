#include "FileReader.h"
#include <QFile>
#include <QTextStream>

bool FileReader::readFromFile(const QString& fileName, InputData& data, QString& errorText) {
    data = InputData{};
    errorText.clear();

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        errorText = "Cannot open file: " + fileName;
        return false;
    }

    QTextStream in(&file);

    int count = 0;
    in >> count;
    if (in.status() != QTextStream::Ok || count <= 0) {
        errorText = "Bad first line";
        return false;
    }

    data.generators.clear();
    data.generators.reserve(count);

    for (int i = 0; i < count; ++i) {
        int a = 0;
        int b = 0;
        in >> a >> b;

        if (in.status() != QTextStream::Ok || a < 0 || b < 0) {
            errorText = QString("Bad generator line %1").arg(i + 1);
            return false;
        }

        data.generators.push_back({a, b});
    }

    in >> data.maxX >> data.maxY;
    if (in.status() != QTextStream::Ok || data.maxX < 0 || data.maxY < 0) {
        errorText = "Bad last line";
        return false;
    }

    for (const auto& g : data.generators) {
        if (g.xDeg > data.maxX) {
            data.maxX = g.xDeg;
        }
        if (g.yDeg > data.maxY) {
            data.maxY = g.yDeg;
        }
    }

    data.maxX += 2;
    data.maxY += 2;

    return true;
}