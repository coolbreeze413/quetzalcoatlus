// SPDX-License-Identifier: BSD-3-Clause

#ifndef LOGFILEERRORSEXTRACTOR_H
#define LOGFILEERRORSEXTRACTOR_H

#include <QObject>
#include <QString>

class LogFileErrorsExtractor : public QObject {
    Q_OBJECT
public:
    LogFileErrorsExtractor(const QString& filePath);
    ~LogFileErrorsExtractor();

    void doWork();

private:
    QString m_filePath;
};

#endif // #ifndef LOGFILEERRORSEXTRACTOR_H
