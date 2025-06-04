/**
 * @file    platformviewer.cpp
 * @brief   Implementation of 3D platform visualization
 * @author  Piotr Siembab
 * @date    18.04.2025
 */

#include "platformviewer.h"
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QOrbitCameraController>
#include <QVBoxLayout>
#include <Qt3DRender/QCamera>
#include <QPointLight>
#include <Qt3DExtras/QSphereMesh>
#include <QTimer>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

void PlatformViewer::updateBallPhysics() {
    const float deltaTime = 0.016f;   // ~60Hz
    float gravityMag = m_gravityInput->text().toFloat();
    const float friction = 0.98f;     // Simple friction
    const float ballRadius = 0.5f;
    const float platformTop = 0.25f;

    // Step 1: Get current rotation and surface normal
    QQuaternion platformRot = m_platformTransform->rotation();
    QVector3D up = platformRot.rotatedVector(QVector3D(0, 1, 0));  // platform's up
    QVector3D normal = up.normalized();

    // Step 2: Gravity direction (world space)
    QVector3D gravity(0, -gravityMag, 0);

    // Step 3: Project gravity onto platform surface
    QVector3D gravityOnPlane = gravity - QVector3D::dotProduct(gravity, normal) * normal;

    // Step 4: Update ball velocity from projected gravity
    m_ballVelocity += gravityOnPlane * deltaTime;

    // Step 5: Apply gravity for falling
    QVector3D pos = m_ballTransform->translation();
    // Step 1: Get platform rotation and up vector
    QQuaternion rot = m_platformTransform->rotation();
    normal = rot.rotatedVector(QVector3D(0, 1, 0)).normalized();

    // Step 2: Platform is at origin and flat in local space
    // So use a point on platform's center and project the ball's XZ onto that plane
    QVector3D ballXZ(pos.x(), 0, pos.z()); // X/Z fixed, solve for Y
    QVector3D platformPoint(0, 0.25f, 0);  // center-top of platform

    // Step 3: Compute vertical distance from platform plane
    float d = QVector3D::dotProduct(normal, platformPoint);
    float ballY = (d - normal.x() * pos.x() - normal.z() * pos.z()) / normal.y();

    // Step 4: If ball is above, let it fall. Otherwise, snap to platform
    bool onPlatform =
        pos.x() >= -2.5f + ballRadius && pos.x() <= 2.5f - ballRadius &&
        pos.z() >= -1.5f + ballRadius && pos.z() <= 1.5f - ballRadius;

    if (onPlatform && pos.y() - ballRadius <= ballY) {
        // Snap to platform
        pos.setY(ballY + ballRadius);
        m_ballVelocity.setY(0);
    } else {
        // Let it fall
        m_ballVelocity += gravity * deltaTime;
    }

    // Step 6: Apply friction to slow down over time
    m_ballVelocity.setX(m_ballVelocity.x() * friction);
    m_ballVelocity.setZ(m_ballVelocity.z() * friction);

    // Step 7: Integrate position
    pos += m_ballVelocity * deltaTime;

    // // Optional: Bound within platform
    // pos.setX(qBound(-2.5f + ballRadius, pos.x(), 2.5f - ballRadius));
    // pos.setZ(qBound(-1.5f + ballRadius, pos.z(), 1.5f - ballRadius));

    // Step 8: Apply new position
    m_ballTransform->setTranslation(pos);
}


/**
 * @brief Constructs and initializes the 3D platform viewer
 * @param parent Parent widget (optional)
 *
 * Creates a 3D scene containing:
 * - Green platform (5×0.5×3 units)
 * - Directional lighting
 * - Orbit camera controller
 * - Fixed size container (500×400px)
 */

void PlatformViewer::resetBall() {
    m_ballVelocity = QVector3D(0, 0, 0);
    m_ballTransform->setTranslation(QVector3D(0, 2.0f, 0));  // Above platform
}

PlatformViewer::PlatformViewer(QWidget *parent)
    : QWidget(parent),
    m_view(new Qt3DExtras::Qt3DWindow()),
    m_container(QWidget::createWindowContainer(m_view, this))
{

    // Set fixed widget size
    m_container->setMinimumSize(300, 250);
    m_container->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    m_container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Main layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);  // Adds 8px padding on all sides
    layout->setSpacing(0);
    layout->addWidget(m_container);
    layout->addSpacing(8);  // <-- Adds 8px padding below the 3D view

    QHBoxLayout *controls = new QHBoxLayout();
    layout->addLayout(controls);

    // Gravity input
    QLabel *gravityLabel = new QLabel("Gravity:");
    m_gravityInput = new QLineEdit("9.8");
    m_gravityInput->setFixedWidth(50);
    controls->addWidget(gravityLabel);
    controls->addSpacing(15);  // <-- Add 10px space here
    controls->addWidget(m_gravityInput);

    controls->addStretch();

    // Reset button
    QPushButton *resetButton = new QPushButton("Reset Ball");
    resetButton->setFixedWidth(150);
    controls->addWidget(resetButton);
    connect(resetButton, &QPushButton::clicked, this, &PlatformViewer::resetBall);

    // Scene root entity
    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity();

    // Platform entity
    Qt3DCore::QEntity *platformEntity = new Qt3DCore::QEntity(rootEntity);
    m_platformTransform = new Qt3DCore::QTransform();
    platformEntity->addComponent(m_platformTransform);

    // Platform geometry (cuboid)
    Qt3DExtras::QCuboidMesh *platformMesh = new Qt3DExtras::QCuboidMesh();
    platformMesh->setXExtent(5.0f);  // Length
    platformMesh->setYExtent(0.5f);  // Height
    platformMesh->setZExtent(3.0f);  // Width

    // Platform material (green with specular highlights)
    Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial();
    material->setDiffuse(QColor(QRgb(0x00b140)));  // Green color
    material->setShininess(100.0f);                // Surface glossiness
    material->setSpecular(QColor(QRgb(0xFFFFFF))); // White highlights

    // Lighting
    Qt3DCore::QEntity *lightEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DRender::QPointLight *light = new Qt3DRender::QPointLight();
    light->setColor(Qt::white);
    light->setIntensity(1.5f);  // Brightness multiplier

    // Light positioning
    Qt3DCore::QTransform *lightTransform = new Qt3DCore::QTransform();
    lightTransform->setTranslation(QVector3D(0, 10, 0));  // Above platform
    lightEntity->addComponent(lightTransform);
    lightEntity->addComponent(light);

    // Assemble platform components
    platformEntity->addComponent(platformMesh);
    platformEntity->addComponent(material);

    // Ball entity
    m_ballEntity = new Qt3DCore::QEntity(rootEntity);
    m_ballTransform = new Qt3DCore::QTransform();
    m_ballEntity->addComponent(m_ballTransform);

    // Ball mesh (sphere)
    Qt3DExtras::QSphereMesh *ballMesh = new Qt3DExtras::QSphereMesh();
    ballMesh->setRadius(0.5f);  // Small ball
    m_ballEntity->addComponent(ballMesh);

    // Ball material
    Qt3DExtras::QPhongMaterial *ballMaterial = new Qt3DExtras::QPhongMaterial();
    ballMaterial->setDiffuse(Qt::red);
    m_ballEntity->addComponent(ballMaterial);

    // Initial position above the platform
    m_ballTransform->setTranslation(QVector3D(0, 2.0f, 0));  // 2 units above

    // Physics: start with no velocity
    m_ballVelocity = QVector3D(0, 0, 0);

    // Timer for physics update
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &PlatformViewer::updateBallPhysics);
    m_updateTimer->start(16); // ~60 FPS

    // Camera configuration
    m_view->camera()->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    m_view->camera()->setPosition(QVector3D(5, 3, 5));  // Diagonal view
    m_view->camera()->setViewCenter(QVector3D(0, 0, 0));  // Focus on origin

    // Camera controls
    Qt3DExtras::QOrbitCameraController *camController =
        new Qt3DExtras::QOrbitCameraController(rootEntity);
    camController->setCamera(m_view->camera());

    // Finalize scene
    m_view->setRootEntity(rootEntity);
}

/**
 * @brief Updates platform orientation based on IMU accelerometer data
 * @param ax X-axis accelerometer value (raw)
 * @param ay Y-axis accelerometer value (raw)
 * @param az Z-axis accelerometer value (raw)
 *
 * Converts raw accelerometer values to:
 * 1. Normalized tilt values (-1.0 to 1.0)
 * 2. Euler angles (pitch and roll)
 * 3. Platform rotation quaternion
 *
 * @note Uses 17000 as normalization factor (1g)
 * @note Applies 25° scaling factor for visible tilt
 */
void PlatformViewer::updatePlatformOrientation(int ax, int ay, int az) {
    // Normalize and clamp accelerometer readings
    float normX = qBound(-1.0f, ax / 17000.0f, 1.0f);  // Roll axis
    float normY = qBound(-1.0f, ay / 17000.0f, 1.0f);  // Pitch axis

    // Convert to angles (25° maximum tilt)
    float roll  = normX * 90.0f;
    float pitch = normY * 90.0f;

    // Apply rotation to platform
    m_platformTransform->setRotation(QQuaternion::fromEulerAngles(pitch, 0, roll));
}
