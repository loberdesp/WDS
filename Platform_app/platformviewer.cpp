#include "platformviewer.h"
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QOrbitCameraController>
#include <QVBoxLayout>
#include <Qt3DRender/QCamera>  // Add this line
#include <QPointLight>


PlatformViewer::PlatformViewer(QWidget *parent)
    : QWidget(parent),
    m_view(new Qt3DExtras::Qt3DWindow()),
    m_container(QWidget::createWindowContainer(m_view, this))
{
    // Set size
    m_container->setMinimumSize(500, 400);
    m_container->setMaximumSize(500, 400);

    // Layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_container);
    layout->setContentsMargins(0, 0, 0, 0);

    // Root entity
    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity();

    // Platform entity
    Qt3DCore::QEntity *platformEntity = new Qt3DCore::QEntity(rootEntity);

    m_platformTransform = new Qt3DCore::QTransform();
    platformEntity->addComponent(m_platformTransform); // Add this line

    // Platform mesh
    Qt3DExtras::QCuboidMesh *platformMesh = new Qt3DExtras::QCuboidMesh();
    platformMesh->setXExtent(5.0f);
    platformMesh->setYExtent(0.5f);
    platformMesh->setZExtent(3.0f);

    // Platform material
    Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial();
    material->setDiffuse(QColor(QRgb(0x00b140)));
    material->setShininess(100.0f);  // Increase shininess (0-1000)
    material->setSpecular(QColor(QRgb(0xFFFFFF)));  // Bright white specular highlights

    // Create ambient light
    Qt3DCore::QEntity *lightEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DRender::QPointLight *light = new Qt3DRender::QPointLight();
    light->setColor(Qt::white);
    light->setIntensity(1.5f);  // Increase beyond 1.0 for brighter light


    // Position the light
    Qt3DCore::QTransform *lightTransform = new Qt3DCore::QTransform();
    lightTransform->setTranslation(QVector3D(0, 10, 0));  // Above the platform
    lightEntity->addComponent(lightTransform);
    lightEntity->addComponent(light);

    // Add components
    platformEntity->addComponent(platformMesh);
    platformEntity->addComponent(material);

    // Camera setup
    m_view->camera()->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    m_view->camera()->setPosition(QVector3D(10, 5, 10));
    m_view->camera()->setViewCenter(QVector3D(0, 0, 0));

    // Camera controller
    Qt3DExtras::QOrbitCameraController *camController =
        new Qt3DExtras::QOrbitCameraController(rootEntity);
    camController->setCamera(m_view->camera());

    // Set root entity
    m_view->setRootEntity(rootEntity);
}


void PlatformViewer::updatePlatformOrientation(int ax, int ay, int az) {
    // Normalize accelerometer readings (assuming max 17000)
    float normX = (ax / 17000.0f); // Tilt along X-axis (roll)
    float normY = (ay / 17000.0f); // Tilt along Y-axis (pitch)

    // Clamp values to [-1, 1] to avoid excessive rotation
    normX = qBound(-1.0f, normX, 1.0f);
    normY = qBound(-1.0f, normY, 1.0f);

    // Convert to angles (in degrees) and apply rotation
    float roll  = -normX * 25.0f;  // Scale factor for visual effect
    float pitch = normY * 25.0f;

    // Create a quaternion for the rotation
    QQuaternion rotation = QQuaternion::fromEulerAngles(pitch, 0, roll);
    m_platformTransform->setRotation(rotation);
}
