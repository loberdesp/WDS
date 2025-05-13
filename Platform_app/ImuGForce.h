#ifndef IMUGFORCE_H
#define IMUGFORCE_H

#pragma once

#include <QWidget>

/**
 * @class ImuGForceWidget
 * @brief A custom widget to visualize 2D G-forces from an IMU sensor.
 *
 * This widget displays a circular graph with concentric rings representing different G-force levels.
 * A dot inside the circle indicates the current acceleration vector (ax, ay) in the X-Y plane.
 * Useful for visualizing lateral and longitudinal forces, like in motorsport or robotics.
 */
class ImuGForceWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructs the widget for displaying G-force.
     * @param parent Optional parent widget.
     */
    explicit ImuGForceWidget(QWidget *parent = nullptr);

    /**
     * @brief Sets the current acceleration values to be visualized.
     * @param ax Acceleration along the X-axis, in G's.
     * @param ay Acceleration along the Y-axis, in G's.
     */
    void setAcceleration(float ax, float ay);

    /**
     * @brief Returns the recommended minimum size of the widget.
     * @return QSize object representing the minimum size.
     */
    QSize minimumSizeHint() const override;

    /**
     * @brief Returns the preferred size of the widget.
     * @return QSize object representing the preferred size.
     */
    QSize sizeHint() const override;

protected:
    /**
     * @brief Paints the widget including the concentric G-rings and acceleration dot.
     * @param event Qt paint event (unused).
     */
    void paintEvent(QPaintEvent *event) override;

private:
    float accX;  ///< Current acceleration in X direction (G's)
    float accY;  ///< Current acceleration in Y direction (G's)

    float maxG;  ///< Maximum G-force visualized by the outer ring (default: 2G, 3G, etc.)
};

#endif // IMUGFORCE_H
