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

// Aktualizacja fizyki pilki
void PlatformViewer::updateBallPhysics() {
    const float deltaTime = 0.016f;   // ~60Hz
    float gravityMag = m_gravityInput->text().toFloat();
    const float friction = 0.98f;     // Tarcie
    const float ballRadius = 0.5f;
    const float platformTop = 0.25f;

    // Obliczenie rotacji platformy i normalnej
    QQuaternion platformRot = m_platformTransform->rotation();
    QVector3D up = platformRot.rotatedVector(QVector3D(0, 1, 0));
    QVector3D normal = up.normalized();

    // Wektor grawitacji
    QVector3D gravity(0, -gravityMag, 0);

    // Projekcja grawitacji na platforme
    QVector3D gravityOnPlane = gravity - QVector3D::dotProduct(gravity, normal) * normal;

    // Aktualizacja predkosci pilki
    m_ballVelocity += gravityOnPlane * deltaTime;

    // Aktualizacja pozycji
    QVector3D pos = m_ballTransform->translation();
    normal = platformRot.rotatedVector(QVector3D(0, 1, 0)).normalized();

    // Obliczenie wysokosci nad platforma
    QVector3D platformPoint(0, 0.25f, 0);
    float d = QVector3D::dotProduct(normal, platformPoint);
    float ballY = (d - normal.x() * pos.x() - normal.z() * pos.z()) / normal.y();

    // Sprawdzenie czy pilka jest na platformie
    const float edgeMargin = ballRadius * (2.0f / 3.0f);

    bool onPlatform =
        pos.x() >= -3.0f + edgeMargin && pos.x() <= 3.0f - edgeMargin &&
        pos.z() >= -2.0f + edgeMargin && pos.z() <= 2.0f - edgeMargin;

    if (onPlatform && pos.y() - ballRadius <= ballY) {
        // Ustawienie pilki na platformie
        pos.setY(ballY + ballRadius);
        m_ballVelocity.setY(0);
    } else {
        // Swobodny spadek
        m_ballVelocity += gravity * deltaTime;
    }

    // Zastosowanie tarcia
    m_ballVelocity.setX(m_ballVelocity.x() * friction);
    m_ballVelocity.setZ(m_ballVelocity.z() * friction);

    // Aktualizacja pozycji
    pos += m_ballVelocity * deltaTime;

    // Ustawienie nowej pozycji
    m_ballTransform->setTranslation(pos);
}

// Resetowanie pozycji pilki
void PlatformViewer::resetBall() {
    m_ballVelocity = QVector3D(0, 0, 0);
    m_ballTransform->setTranslation(QVector3D(0, 2.0f, 0));
}

// Konstruktor PlatformViewer
PlatformViewer::PlatformViewer(QWidget *parent)
    : QWidget(parent),
    m_view(new Qt3DExtras::Qt3DWindow()),
    m_container(QWidget::createWindowContainer(m_view, this))
{
    // Ustawienie rozmiaru kontenera
    m_container->setMinimumSize(300, 250);
    m_container->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    m_container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Glowny layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(0);
    layout->addWidget(m_container);
    layout->addSpacing(8);

    // Panel kontrolny
    QHBoxLayout *controls = new QHBoxLayout();
    layout->addLayout(controls);

    // Pole do wprowadzania grawitacji
    gravityLabel = new QLabel(tr("Gravity:"));
    m_gravityInput = new QLineEdit("9.8");
    m_gravityInput->setFixedWidth(50);
    controls->addWidget(gravityLabel);
    controls->addSpacing(15);
    controls->addWidget(m_gravityInput);

    controls->addStretch();

    // Przycisk resetujacy pilke
    resetButton = new QPushButton(tr("Reset Ball"));
    resetButton->setFixedWidth(150);
    controls->addWidget(resetButton);
    connect(resetButton, &QPushButton::clicked, this, &PlatformViewer::resetBall);

    // Glowna encja sceny
    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity();

    // Encja platformy
    Qt3DCore::QEntity *platformEntity = new Qt3DCore::QEntity(rootEntity);
    m_platformTransform = new Qt3DCore::QTransform();
    platformEntity->addComponent(m_platformTransform);

    // Ksztalt platformy (prostopadloscian)
    Qt3DExtras::QCuboidMesh *platformMesh = new Qt3DExtras::QCuboidMesh();
    platformMesh->setXExtent(5.0f);
    platformMesh->setYExtent(0.5f);
    platformMesh->setZExtent(3.0f);

    // Material platformy (zielony)
    Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial();
    material->setDiffuse(QColor(QRgb(0x00b140)));
    material->setShininess(100.0f);
    material->setSpecular(QColor(QRgb(0xFFFFFF)));

    // Oswietlenie
    Qt3DCore::QEntity *lightEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DRender::QPointLight *light = new Qt3DRender::QPointLight();
    light->setColor(Qt::white);
    light->setIntensity(1.5f);

    // Pozycja swiatla
    Qt3DCore::QTransform *lightTransform = new Qt3DCore::QTransform();
    lightTransform->setTranslation(QVector3D(0, 10, 0));
    lightEntity->addComponent(lightTransform);
    lightEntity->addComponent(light);

    // Dodanie komponentow platformy
    platformEntity->addComponent(platformMesh);
    platformEntity->addComponent(material);

    // Encja pilki
    m_ballEntity = new Qt3DCore::QEntity(rootEntity);
    m_ballTransform = new Qt3DCore::QTransform();
    m_ballEntity->addComponent(m_ballTransform);

    // Ksztalt pilki (kula)
    Qt3DExtras::QSphereMesh *ballMesh = new Qt3DExtras::QSphereMesh();
    ballMesh->setRadius(0.5f);
    m_ballEntity->addComponent(ballMesh);

    // Material pilki (czerwony)
    Qt3DExtras::QPhongMaterial *ballMaterial = new Qt3DExtras::QPhongMaterial();
    ballMaterial->setDiffuse(Qt::red);
    m_ballEntity->addComponent(ballMaterial);

    // Pozycja poczatkowa pilki
    m_ballTransform->setTranslation(QVector3D(0, 2.0f, 0));

    // Predkosc poczatkowa
    m_ballVelocity = QVector3D(0, 0, 0);

    // Timer do aktualizacji fizyki
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &PlatformViewer::updateBallPhysics);
    m_updateTimer->start(16);

    // Konfiguracja kamery
    m_view->camera()->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    m_view->camera()->setPosition(QVector3D(5, 3, 5));
    m_view->camera()->setViewCenter(QVector3D(0, 0, 0));

    // Kontroler kamery
    Qt3DExtras::QOrbitCameraController *camController =
        new Qt3DExtras::QOrbitCameraController(rootEntity);
    camController->setCamera(m_view->camera());

    // Ustawienie glownej encji
    m_view->setRootEntity(rootEntity);
}

// Aktualizacja orientacji platformy na podstawie danych z IMU
void PlatformViewer::updatePlatformOrientation(int ax, int ay, int az) {
    // Normalizacja danych z akcelerometru
    float normX = qBound(-1.0f, ax / 17000.0f, 1.0f);
    float normY = qBound(-1.0f, ay / 17000.0f, 1.0f);

    // Konwersja na katy
    float roll  = normX * 90.0f;
    float pitch = normY * 90.0f;

    // Ustawienie rotacji platformy
    m_platformTransform->setRotation(QQuaternion::fromEulerAngles(pitch, 0, roll));
}


void PlatformViewer::retranslateUi() {
    gravityLabel->setText(tr("Gravity:"));
    resetButton->setText(tr("Reset Ball"));
}
