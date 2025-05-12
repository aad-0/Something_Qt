#include "csvaccelbuffer.h"
#include <QDebug>
#include <QDateTime>

CsvAccelBuffer::CsvAccelBuffer(QObject *parent)
    : QObject(parent)
    , _buffer(256) // 256 samples for CSV buffer
    , _writeTimer(new QTimer(this))
    , _isLogging(false)
{
    _writeTimer->setInterval(1000); // Write every second
    connect(_writeTimer, &QTimer::timeout, this, &CsvAccelBuffer::writeData);
}

CsvAccelBuffer::~CsvAccelBuffer()
{
    stopLogging();
}

void CsvAccelBuffer::onAccelDataReceived(const QVector3D& data)
{
    if (_isLogging) {
        _buffer.append(AccelDataPoint(0, 0, data)); // Use default values for tick and encoder
    }
}

void CsvAccelBuffer::onAccelBatchReceived(const QList<AccelDataPoint>& batch)
{
    if (_isLogging) {
        for (const auto& point : batch) {
            _buffer.append(point);
        }
    }
}

void CsvAccelBuffer::startLogging(const QString& filePath)
{
    if (_isLogging) {
        stopLogging();
    }

    _file.setFileName(filePath);
    if (!_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "[CsvAccelBuffer] Failed to open file:" << filePath;
        return;
    }

    _stream.setDevice(&_file);
    // Write header
    _stream << "Timestamp,Tick,Encoder,X,Y,Z\n";
    _stream.flush();

    _isLogging = true;
    _writeTimer->start();
    qDebug() << "[CsvAccelBuffer] Started logging to:" << filePath;
}

void CsvAccelBuffer::stopLogging()
{
    if (!_isLogging) return;

    _writeTimer->stop();
    writeData(); // Write any remaining data
    _file.close();
    _isLogging = false;
    qDebug() << "[CsvAccelBuffer] Stopped logging";
}

void CsvAccelBuffer::writeData()
{
    if (!_isLogging) return;

    QList<AccelDataPoint> data = _buffer.getAll();
    if (data.isEmpty()) return;

    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    for (const auto& point : data) {
        _stream << timestamp << ","
                << point.tick << ","
                << point.encoder << ","
                << QString::number(point.data.x(), 'f', 6) << ","
                << QString::number(point.data.y(), 'f', 6) << ","
                << QString::number(point.data.z(), 'f', 6) << "\n";
    }
    _stream.flush();

    _buffer.clear();
    qDebug() << "[CsvAccelBuffer] Wrote" << data.size() << "samples";
} 