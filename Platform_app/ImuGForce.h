#ifndef IMUGFORCE_H
#define IMUGFORCE_H

#pragma once

#include <QWidget>
#include <QList>

/**
 * @class ImuGForceWidget
 * @brief A custom widget to visualize 2D G-forces from an IMU sensor.
 *
 * This widget displays a circular graph with concentric rings representing different G-force levels.
 * A dot inside the circle indicates the current acceleration vector (ax, ay) in the X-Y plane.
 * Also includes a 2-second fading trace of past acceleration positions.
 */
class ImuGForceWidget : public QWidget {
    Q_OBJECT

public:
    explicit ImuGForceWidget(QWidget *parent = nullptr);

    void setAcceleration(float ax, float ay);

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    float accX;
    float accY;
    float maxG;

    struct TracePoint {
        float ax;
        float ay;
        qreal timestamp; // seconds
    };

    QList<TracePoint> trace;
    qreal lastUpdateTime;

    static qreal getElapsedSeconds();
};

#endif // IMUGFORCE_H
