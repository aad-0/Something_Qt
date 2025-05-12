#ifndef CSVACCELBUFFER_H
#define CSVACCELBUFFER_H

#include <QObject>
#include <QTimer>
#include <QVector3D>
#include <QList>
#include <QFile>
#include <QTextStream>
#include "acceldatabuffer.h"

class CsvAccelBuffer : public QObject {
    Q_OBJECT
public:
    explicit CsvAccelBuffer(QObject *parent = nullptr);
    ~CsvAccelBuffer();

public slots:
    void onAccelDataReceived(const QVector3D& data);
    void onAccelBatchReceived(const QList<AccelDataPoint>& batch);
    void startLogging(const QString& filePath);
    void stopLogging();

signals:
    void errorOccurred(const QString& error);
    void writeCompleted(int samplesWritten);

private:
    void writeData();

private:
    AccelDataBuffer _buffer;
    QFile _file;
    QTextStream _stream;
    QTimer* _writeTimer;
    bool _isLogging;
};

#endif // CSVACCELBUFFER_H 