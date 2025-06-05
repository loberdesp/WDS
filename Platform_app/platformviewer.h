/**
 * @file    platformviewer.h
 * @brief   3D Stewart platform visualization with IMU integration and physics simulation
 *
 * @details Provides real-time 3D visualization of:
 *          - Stewart platform orientation from IMU data
 *          - Virtual ball physics simulation
 *          - Configurable gravity effects
 *          - Interactive reset functionality
 *
 * @author  Piotr Siembab
 * @date    18.04.2025
 * @version 1.4
 */

#ifndef PLATFORMVIEWER_H
#define PLATFORMVIEWER_H

#include <QWidget>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DCore/QEntity>
#include <QPainter>
#include <Qt3DCore/QTransform>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

/**
 * @class PlatformViewer
 * @brief 3D visualization widget for Stewart platform with physics simulation
 *
 * @details Combines:
 *          - Qt3D-based rendering
 *          - IMU orientation visualization
 *          - Real-time physics simulation
 *          - User-configurable parameters
 *
 * The widget displays:
 * - Stewart platform model responding to IMU data
 * - Virtual ball affected by platform tilt and gravity
 * - Gravity control interface
 * - Automatic physics updates
 */
class PlatformViewer : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the 3D visualization widget
     * @param parent Parent widget (default: nullptr)
     *
     * @details Initializes:
     *          - 3D rendering environment
     *          - Platform and ball models
     *          - Physics simulation timer
     *          - Gravity control interface
     */
    explicit PlatformViewer(QWidget *parent = nullptr);

    /**
     * @brief Provides recommended widget size
     * @return QSize(500, 400) for optimal 3D viewing
     */
    QSize sizeHint() const override { return QSize(500, 400); }

    /**
     * @brief Updates platform orientation from IMU data
     * @param ax X-axis rotation (pitch) in raw IMU units
     * @param ay Y-axis rotation (roll) in raw IMU units
     * @param az Z-axis rotation (yaw) in raw IMU units
     *
     * @details Converts raw IMU values to 3D transform:
     *          - Applies platform rotation
     *          - Maintains physical constraints
     *          - Triggers visual update
     */
    void updatePlatformOrientation(int ax, int ay, int az);


    /**
     * @brief Updates all user-visible strings in the UI to reflect the current language.
     *
     * This function should be called whenever the application language changes,
     * to re-apply translations to all text elements such as labels, titles, and tooltips.
     * It ensures the interface dynamically updates without restarting the application.
     */
    void retranslateUi();

private:
    Qt3DExtras::Qt3DWindow *m_view;           ///< Main 3D rendering window
    QWidget *m_container;                     ///< Container for embedding Qt3D in QWidget
    Qt3DCore::QTransform *m_platformTransform;///< Platform's 3D transformation

    Qt3DCore::QEntity *m_ballEntity;          ///< Virtual ball entity
    QVector3D m_ballVelocity;                 ///< Current ball velocity (m/s)
    Qt3DCore::QTransform *m_ballTransform;    ///< Ball's 3D transformation

    QLineEdit *m_gravityInput;                ///< Gravity magnitude input field
    QTimer *m_updateTimer;                    ///< Physics update timer (60Hz)

    /**
 * @brief Label displaying gravity or related measurement information.
 */
    QLabel *gravityLabel;

    /**
 * @brief Button to reset the current view or settings.
 */
    QPushButton *resetButton;

    /**
     * @brief Initializes the 3D scene components
     *
     * @details Creates:
     *          - Platform model with realistic dimensions
     *          - Virtual ball with physics properties
     *          - Lighting and camera setup
     */
    void initialize3DScene();

    /**
     * @brief Creates the platform control interface
     *
     * @details Builds:
     *          - Gravity input field
     *          - Reset button
     *          - Status indicators
     */
    void createControlPanel();

private slots:
    /**
     * @brief Updates ball physics simulation
     *
     * @details Calculates each frame:
     *          - Gravity effects based on platform tilt
     *          - Collision detection with platform
     *          - Velocity and position updates
     *          - Energy loss on impact
     */
    void updateBallPhysics();

    /**
     * @brief Resets the ball to initial position
     *
     * @details:
     *          - Places ball at platform center
     *          - Zeroes velocity
     *          - Clears any accumulated error
     */
    void resetBall();
};

#endif // PLATFORMVIEWER_H
