#include "roadvisualizer.h"

#include <QVBoxLayout>
#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QSurfaceDataProxy>
#include <QtDataVisualization/QSurface3DSeries>
#include <QtDataVisualization/QCustom3DItem>


RoadVisualizer::RoadVisualizer(QWidget *parent)
    : QWidget(parent),
    m_surface(new Q3DSurface()),
    m_container(QWidget::createWindowContainer(m_surface, this)),
    m_proxy(new QSurfaceDataProxy()),
    m_series(new QSurface3DSeries(m_proxy)),
    m_arrow(nullptr)
{
    // Set up layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_container);
    layout->setContentsMargins(0, 0, 0, 0);

    // Add surface and series
    m_surface->addSeries(m_series);

    // Setup axis ranges
    m_surface->axisX()->setTitle("Forward (Y accel)");
    m_surface->axisX()->setRange(-50, 50);

    m_surface->axisY()->setTitle("Left-Right (X accel)");
    m_surface->axisY()->setRange(-50, 50);

    m_surface->axisZ()->setTitle("Up-Down (Z accel)");
    m_surface->axisZ()->setRange(-50, 50);

    initSurface();
    initArrow();
}

RoadVisualizer::~RoadVisualizer()
{
    delete m_surface; // deletes container, series, proxy
}

void RoadVisualizer::initSurface()
{
    QSurfaceDataArray *array = new QSurfaceDataArray;
    array->reserve(20);

    for (int x = -10; x <= 10; ++x)
    {
        QSurfaceDataRow *newRow = new QSurfaceDataRow(21);

        for (int z = -10; z <= 10; ++z)
        {
            (*newRow)[z + 10].setPosition(QVector3D(x * 2.0f, 0.0f, z * 2.0f));
        }

        array->append(newRow);
    }

    m_proxy->resetArray(array);
}

void RoadVisualizer::initArrow()
{
    m_arrow = new QCustom3DItem();
    m_arrow->setScaling(QVector3D(2.0f, 2.0f, 2.0f)); // Size of the arrow
    m_arrow->setPosition(QVector3D(0.0f, 0.0f, 0.0f)); // Start at center
    m_arrow->setMeshFile(":/arrow.obj"); // Use a resource or a simple sphere temporarily
    m_arrow->setTextureFile(""); // Optional
    m_surface->addCustomItem(m_arrow);
}
#include <QtMath>

void RoadVisualizer::updateSurface(float accelX, float accelY, float accelZ)
{
    if (!m_surface)
        return;

    // 1. Normalize or scale acceleration input
    const float nanoToG = 1e9f;
    accelX *= nanoToG;
    accelY *= nanoToG;
    accelZ *= nanoToG;

    // 2. Calculate Roll and Pitch from accelerometer
    float rollRad = qAtan2(accelY, accelZ);
    float pitchRad = qAtan2(-accelX, qSqrt(accelY * accelY + accelZ * accelZ));

    // 3. Convert to degrees
    float rollDeg = rollRad * (180.0f / M_PI);
    float pitchDeg = pitchRad * (180.0f / M_PI);

    // 4. Build Rotation
    QQuaternion rotation = QQuaternion::fromEulerAngles(
        pitchDeg, // Rotation around X-axis (tilt forward/backward)
        0.0f,     // No yaw rotation (around vertical Z axis)
        -rollDeg  // Rotation around Z-axis (tilt left/right)
        );

    // 5. Apply rotation to surface
    m_surface->setRotation(rotation);
}



// void RoadVisualizer::updateSurface(float accelX, float accelY, float accelZ)
// {
//     if (!m_arrow)
//         return;
//
//     const float scaleX = 10.0f; // Left/Right scale
//     const float scaleY = 10.0f; // Forward/Backward scale
//     const float scaleZ = 10.0f; // Up/Down scale
//
//     QVector3D newPos(
//         accelY * scaleY,  // Forward/backward (Y accel mapped to X axis)
//         accelX * scaleX,  // Sideways tilt (X accel mapped to Y axis)
//         accelZ * scaleZ   // Height (Z accel mapped to Z axis)
//         );
//
//     m_arrow->setPosition(newPos);
// }




// #include "roadvisualizer.h"
//
// #include <QWidget>
// #include <QtDataVisualization/Q3DSurface>
// #include <QtDataVisualization/QSurfaceDataProxy>
// #include <QtDataVisualization/QSurface3DSeries>
// #include <QtDataVisualization>
// #include <QVBoxLayout>
//
// RoadVisualizer::RoadVisualizer(QWidget *parent)
//     : QWidget(parent),
//     m_surface(new Q3DSurface()),
//     m_container(QWidget::createWindowContainer(m_surface, this)),
//     m_proxy(new QSurfaceDataProxy()),
//     m_series(new QSurface3DSeries(m_proxy)),
//     m_forwardStep(0)
// {
//     QVBoxLayout *layout = new QVBoxLayout(this);
//     layout->addWidget(m_container);
//     layout->setContentsMargins(0, 0, 0, 0);
//
//     m_surface->addSeries(m_series);
//
//     // Setup axis ranges (adjust these to your needs)
//     m_surface->axisX()->setTitle("Forward");
//     m_surface->axisX()->setRange(-100000, -100000);
//
//     m_surface->axisY()->setTitle("Left-Right");
//     m_surface->axisY()->setRange(-100000, -100000);
//
//     m_surface->axisZ()->setTitle("Height");
//     m_surface->axisZ()->setRange(-100000, -100000);
//
//     initSurface();
// }
//
// RoadVisualizer::~RoadVisualizer()
// {
//     delete m_surface;  // Automatically deletes proxy and series
// }
//
// void RoadVisualizer::initSurface()
// {
//     QSurfaceDataArray *array = new QSurfaceDataArray;
//     array->reserve(50); // 50 points
//
//     for (int x = 0; x < 50; ++x)
//     {
//         QSurfaceDataRow *newRow = new QSurfaceDataRow(1); // Single column "road" strip
//         (*newRow)[0].setPosition(QVector3D(x, 0.0f, 0.0f));
//         array->append(newRow);
//     }
//     m_proxy->resetArray(array);
// }
//
// // void RoadVisualizer::updateSurface(float accelX, float accelY, float accelZ)
// // {
// //     QSurfaceDataArray const * array = m_proxy->array();
// //
// //     if (array->isEmpty())
// //         return;
// //
// //     // Shift all points
// //     for (int i = 0; i < array->size() - 1; ++i)
// //     {
// //         QSurfaceDataRow *currentRow = (*array)[i];
// //         QSurfaceDataRow *nextRow = (*array)[i+1];
// //
// //         (*currentRow)[0].setPosition((*nextRow)[0].position());
// //     }
// //
// //     // Add new incoming point at the end
// //     QSurfaceDataRow *lastRow = (*array)[array->size() - 1];
// //     (*lastRow)[0].setPosition(QVector3D(m_forwardStep++, accelY, accelZ));
// //
// //     // Update the graph
// //     m_proxy->resetArray(array);
// // }
//
// void RoadVisualizer::updateSurface(float accelX, float accelY, float accelZ)
// {
//     const QSurfaceDataArray *oldArray = m_proxy->array();
//     if (!oldArray || oldArray->isEmpty())
//         return;
//
//     QSurfaceDataArray *newArray = new QSurfaceDataArray;
//     newArray->reserve(oldArray->size());
//
//     for (int i = 0; i < oldArray->size() - 1; ++i)
//     {
//         QSurfaceDataRow *row = new QSurfaceDataRow(1);
//         const QSurfaceDataItem &item = (*(*oldArray)[i+1])[0];
//         (*row)[0].setPosition(item.position());
//         newArray->append(row);
//     }
//
//     QSurfaceDataRow *newLastRow = new QSurfaceDataRow(1);
//     (*newLastRow)[0].setPosition(QVector3D(m_forwardStep++, accelY*100, accelZ*100));
//     // (*newLastRow)[0].setPosition(QVector3D(accelX, accelY, accelZ));
//     newArray->append(newLastRow);
//
//     m_proxy->resetArray(newArray);
// }
//
//
