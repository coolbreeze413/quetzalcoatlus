// SPDX-License-Identifier: BSD-3-Clause

#include "logfileerrorsextractor.h"

#include <fstream>
#include <regex>

#include <QDebug>
#include <QThread>

LogFileErrorsExtractor::LogFileErrorsExtractor(const QString& filePath)
    : m_filePath(filePath)
{}

LogFileErrorsExtractor::~LogFileErrorsExtractor()
{}

void LogFileErrorsExtractor::doWork()
{
    qDebug() << "LogFileErrorsExtractor::doWork in thread" << QThread::currentThreadId();

    // lambda for striping the line
    auto stripStringFn = [](const std::string& str) {
        size_t first = str.find_first_not_of(" \t\n\r"); // Find first non-whitespace character
        size_t last = str.find_last_not_of(" \t\n\r"); // Find last non-whitespace character

        if (first == std::string::npos || last == std::string::npos) {
            // The string only contains whitespace characters
            return std::string("");
        }

        return str.substr(first, last - first + 1); // Extract the stripped substring
    };

    // lambda to extract num of errors
    auto extractErrorsFn = [](const std::string& line) {
        int result = 0;
        static std::regex patternLine(R"regex(number of errors\s*:\s*(\d+))regex");
        std::smatch match;

        if (std::regex_search(line, match, patternLine)) {
            result = stoi(match[1].str());
        }
        return result;
    };

    std::ifstream file(m_filePath.toStdString());
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            line = stripStringFn(line);
            if (!line.empty()) {
                int errorsNum = extractErrorsFn(line);
                if (errorsNum > 0) {
                    qInfo() << QString("errorsNum=%1, from line=%2").arg(errorsNum).arg(line.c_str());
                }
            }
        }
        file.close();
    }
    else {
        qCritical() << "unable to open filePath" << m_filePath;
        return;
    }

    emit finished();
}
