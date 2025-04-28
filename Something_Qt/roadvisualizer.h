#ifndef ROADVISUALIZER_H
#define ROADVISUALIZER_H

#include <QWidget>
#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QSurfaceDataProxy>
#include <QtDataVisualization/QSurface3DSeries>
#include <QtDataVisualization/QCustom3DItem>
#include <QtDataVisualization>


class RoadVisualizer : public QWidget
{
    Q_OBJECT

public:
    explicit RoadVisualizer(QWidget *parent = nullptr);
    ~RoadVisualizer();

public slots:
    void updateSurface(float accelX, float accelY, float accelZ);

private:
    void initSurface();
    void initArrow();

private:
    Q3DSurface *m_surface;
    QWidget *m_container;
    QSurfaceDataProxy *m_proxy;
    QSurface3DSeries *m_series;
    QCustom3DItem *m_arrow;
};



// #include <QWidget>
// #include <QtDataVisualization/Q3DSurface>
// #include <QtDataVisualization/QSurfaceDataProxy>
// #include <QtDataVisualization/QSurface3DSeries>
// #include <QtDataVisualization/QCustom3DItem>
// #include <QtDataVisualization>
//
//
// class RoadVisualizer : public QWidget
// {
//     Q_OBJECT
//
// public:
//     explicit RoadVisualizer(QWidget *parent = nullptr);
//     ~RoadVisualizer();
//
// public slots:
//     void updateSurface(float accelX, float accelY, float accelZ);
//
// private:
//     void initSurface();
//     void initArrow();
//
// private:
//     QtDataVisualization::Q3DSurface *m_surface;
//     QWidget *m_container;
//     QtDataVisualization::QSurfaceDataProxy *m_proxy;
//     QtDataVisualization::QSurface3DSeries *m_series;
//     QtDataVisualization::QCustom3DItem *m_arrow;
// };




// #ifndef ROADVISUALIZER_HPP
// #define ROADVISUALIZER_HPP
//
// #include <QObject>
// #include <QWidget>
// #include <Q3DSurface>
// #include <QSurfaceDataProxy>
// #include <QSurface3DSeries>
//
//
// class RoadVisualizer : public QWidget
// {
//     Q_OBJECT
//
// public:
//     explicit RoadVisualizer(QWidget *parent = nullptr);
//     ~RoadVisualizer();
//
// public slots:
//     /**
//      * @brief updateSurface Updates surface with a new (x,y,z) point
//      * @param accelX Forward acceleration (time or distance)
//      * @param accelY Left/Right movement
//      * @param accelZ Up/Down movement (height)
//      */
//     void updateSurface(float accelX, float accelY, float accelZ);
//     void initSurface();
//
// private:
//     //void initSurface();
//
// private:
//     Q3DSurface *m_surface;
//     QWidget *m_container;
//     QSurfaceDataProxy *m_proxy;
//     QSurface3DSeries *m_series;
//     int m_forwardStep;
// };
//
// #endif // ROADVISUALIZER_HPP


#endif // ROADVISUALIZER_H
