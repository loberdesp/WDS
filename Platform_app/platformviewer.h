#ifndef PLATFORMVIEWER_H
#define PLATFORMVIEWER_H

#include <QWidget>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DCore/QEntity>
#include <QPainter>
#include <Qt3DCore/QTransform>
#include <QLineEdit>

/**
 * @class PlatformViewer
 * @brief A QWidget-based 3D visualizer for a Stewart platform with an IMU and virtual ball simulation.
 *
 * This widget provides a 3D visualization of a Stewart platform's orientation based on IMU data.
 * It also includes a simulation of a virtual ball affected by platform orientation and gravity.
 */
class PlatformViewer : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a new PlatformViewer object.
     * @param parent The parent widget, or nullptr if none.
     */
    explicit PlatformViewer(QWidget *parent = nullptr);

    /**
     * @brief Returns a recommended size for the widget.
     * @return Suggested QSize of 500x400.
     */
    QSize sizeHint() const override { return QSize(500, 400); }

    /**
     * @brief Updates the platform's orientation based on IMU data.
     * @param ax Rotation around the X-axis (pitch).
     * @param ay Rotation around the Y-axis (roll).
     * @param az Rotation around the Z-axis (yaw).
     *
     * This function updates the 3D transform of the platform model.
     */
    void updatePlatformOrientation(int ax, int ay, int az);

private:
    Qt3DExtras::Qt3DWindow *m_view;                 ///< 3D rendering window for the scene.
    QWidget *m_container;                           ///< Container widget for integrating Qt3DWindow into QWidget layout.
    Qt3DCore::QTransform *m_platformTransform;      ///< Transformation applied to the Stewart platform model.

    Qt3DCore::QEntity *m_ballEntity;                ///< Entity representing the virtual ball in the 3D scene.
    QVector3D m_ballVelocity;                       ///< Current velocity vector of the virtual ball.
    Qt3DCore::QTransform *m_ballTransform;          ///< Transformation applied to the virtual ball.

    QLineEdit *m_gravityInput;                      ///< UI element for user input to change gravity settings.
    QTimer *m_updateTimer;                          ///< Timer to update ball physics at regular intervals.

private slots:
    /**
     * @brief Updates the physics of the virtual ball.
     *
     * Called periodically by the update timer to simulate ball movement based on orientation and gravity.
     */
    void updateBallPhysics();

    /**
     * @brief Resets the virtual ball to its initial position and velocity.
     *
     * Useful for testing and reinitializing the simulation.
     */
    void resetBall();
};

#endif // PLATFORMVIEWER_H
