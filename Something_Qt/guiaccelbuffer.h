#ifndef GUIACCELBUFFER_H
#define GUIACCELBUFFER_H

#include <QObject>
#include <QTimer>
#include <QVector3D>
#include <QList>
#include <QPointF>
#include "acceldatabuffer.h"

class GuiAccelBuffer : public QObject
{
    Q_OBJECT
public:
    explicit GuiAccelBuffer(QObject *parent = nullptr);
    ~GuiAccelBuffer();

public slots:
    void onAccelDataReceived(const QVector3D& data);
    void onAccelBatchReceived(const QList<AccelDataPoint>& batch);
    void requestPlotData();

signals:
    void dataReady(const QList<QPointF>& x, const QList<QPointF>& y, const QList<QPointF>& z);

private:
    AccelDataBuffer _buffer;
    QTimer* _plotTimer;
};

#endif // GUIACCELBUFFER_H 