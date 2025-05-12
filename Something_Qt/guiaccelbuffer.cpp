#include "guiaccelbuffer.h"
#include <QDebug>

GuiAccelBuffer::GuiAccelBuffer(QObject *parent)
    : QObject(parent)
    , _buffer(256) // 256 samples for display
    , _plotTimer(new QTimer(this))
{
    _plotTimer->setInterval(100); // Update plot every 100ms
    connect(_plotTimer, &QTimer::timeout, this, &GuiAccelBuffer::requestPlotData);
    _plotTimer->start();
}

GuiAccelBuffer::~GuiAccelBuffer()
{
    _plotTimer->stop();
}

void GuiAccelBuffer::onAccelDataReceived(const QVector3D& data)
{
    _buffer.append(AccelDataPoint(0, 0, data)); // Use default values for tick and encoder
}

void GuiAccelBuffer::onAccelBatchReceived(const QList<AccelDataPoint>& batch)
{
    for (const auto& point : batch) {
        _buffer.append(point);
    }
}

void GuiAccelBuffer::requestPlotData()
{
    QList<AccelDataPoint> data = _buffer.getAll();
    QList<QPointF> xData, yData, zData;
    
    for (int i = 0; i < data.size(); ++i) {
        const auto& point = data[i];
        xData.append(QPointF(i, point.data.x()));
        yData.append(QPointF(i, point.data.y()));
        zData.append(QPointF(i, point.data.z()));
    }
    
    emit dataReady(xData, yData, zData);
} 