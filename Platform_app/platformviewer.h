#ifndef PLATFORMVIEWER_H
#define PLATFORMVIEWER_H

#include <QWidget>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DCore/QEntity>
#include <QPainter>
#include <Qt3DCore/QTransform>
#include <QLineEdit>

class PlatformViewer : public QWidget
{
    Q_OBJECT
public:
    explicit PlatformViewer(QWidget *parent = nullptr);
    QSize sizeHint() const override { return QSize(500, 400); }
    void updatePlatformOrientation(int ax, int ay, int az); // Called when new IMU data arrives
private:
    Qt3DExtras::Qt3DWindow *m_view;
    QWidget *m_container;
    Qt3DCore::QTransform *m_platformTransform; // Add this to store the platform's transform

    Qt3DCore::QEntity *m_ballEntity;
    QVector3D m_ballVelocity;
    Qt3DCore::QTransform *m_ballTransform;
    QLineEdit *m_gravityInput;
    QTimer *m_updateTimer;

private slots:
    void updateBallPhysics();
    void resetBall();

};

#endif // PLATFORMVIEWER_H
