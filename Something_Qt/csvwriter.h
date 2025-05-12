#ifndef CSVWRITER_H
#define CSVWRITER_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QTextStream>
#include "comm.h" // For ComDefImu_TypeDef

class CsvWriter : public QObject
{
    Q_OBJECT
public:
    // Constructor takes an optional file path, defaults to "imu_log.csv"
    explicit CsvWriter(const QString& filePath = "imu_log.csv", QObject *parent = nullptr);
    ~CsvWriter();

public slots:
    // Slot to receive IMU data and write it to the file
    void writeImuData(ComDefImu_TypeDef data);

private:
    QFile _mFile;         // File handle for the CSV
    QTextStream _mTextStream; // Stream for writing text to the file
    QString _mFilePath;   // Stores the file path
    bool _mIsFileOpen; // Flag to track if file is successfully opened
};

#endif // CSVWRITER_H 