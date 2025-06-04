/**
 * @file    imugforce.h
 * @brief   2D G-force visualization widget for IMU sensor data
 *
 * @details Visualizes real-time acceleration vectors with:
 *          - Circular G-force meter display
 *          - Fading position history trail
 *          - Configurable maximum G-force range
 *          - Responsive design with size hints
 *
 * @author  Piotr Siembab
 * @date    18.04.2025
 * @version 1.3
 */

#ifndef IMUGFORCE_H
#define IMUGFORCE_H

#include <QWidget>
#include <QList>

/**
 * @class ImuGForceWidget
 * @brief Circular G-force visualization widget with history trail
 *
 * @details Features include:
 *          - Real-time 2D acceleration vector display
 *          - Configurable maximum G-force range
 *          - 2-second fading position history
 *          - Color-coded G-force levels
 *          - Automatic scaling to widget size
 *
 * The visualization consists of:
 * - Concentric circles representing G-force levels
 * - Current position marker (colored dot)
 * - Fading trail of previous positions
 * - Axis indicators (X/Y directions)
 */
class ImuGForceWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructs a G-force visualization widget
     * @param parent Parent widget (default: nullptr)
     *
     * @details Initializes:
     *          - Default max G-force (2G)
     *          - Empty position history
     *          - Default color scheme
     */
    explicit ImuGForceWidget(QWidget *parent = nullptr);

    /**
     * @brief Updates the displayed acceleration vector
     * @param ax X-axis acceleration (in G-forces)
     * @param ay Y-axis acceleration (in G-forces)
     *
     * @details Records the new position to the history trail
     *          and triggers a widget repaint. Values are
     *          automatically clamped to ±maxG range.
     *
     * @note 1G ≈ 9.81 m/s² (standard gravity)
     */
    void setAcceleration(float ax, float ay);

    /**
     * @brief Provides the recommended minimum widget size
     * @return Minimum size in logical pixels
     *
     * @details Ensures all visual elements remain usable.
     *          Default: 150x150 pixels.
     */
    QSize minimumSizeHint() const override;

    /**
     * @brief Provides the ideal widget size
     * @return Preferred size in logical pixels
     *
     * @details Default: 200x200 pixels for optimal visibility.
     */
    QSize sizeHint() const override;

protected:
    /**
     * @brief Handles all widget rendering
     * @param event Qt paint event object
     *
     * @details Draws:
     *          - Background grid and axes
     *          - Concentric G-force circles
     *          - Current position marker
     *          - Fading history trail
     *          - Legend and scale indicators
     */
    void paintEvent(QPaintEvent *event) override;

private:
    float accX;         ///< Current X acceleration (G-forces)
    float accY;         ///< Current Y acceleration (G-forces)
    float maxG;         ///< Maximum displayable G-force (default: 2G)

    /**
     * @struct TracePoint
     * @brief Stores historical acceleration data with timestamps
     */
    struct TracePoint {
        float ax;       ///< Historical X acceleration value
        float ay;       ///< Historical Y acceleration value
        qreal timestamp;///< Time in seconds since reference
    };

    QList<TracePoint> trace;    ///< Buffer of recent positions (2-second history)
    qreal lastUpdateTime;       ///< Timestamp of last update

    /**
     * @brief Gets monotonic time in seconds
     * @return Current time in seconds
     *
     * @details Used for calculating position trail ages.
     *          Platform-independent high-resolution timer.
     */
    static qreal getElapsedSeconds();

    /**
     * @brief Draws the G-force meter background
     * @param painter Reference to active QPainter
     *
     * @details Renders:
     *          - Concentric G-force circles
     *          - Axis lines and labels
     *          - Background gradient
     */
    void drawMeterBackground(QPainter &painter);

    /**
     * @brief Draws the position history trail
     * @param painter Reference to active QPainter
     *
     * @details Renders fading trail with:
     *          - Age-based opacity (2-second fade)
     *          - Color-coded by recency
     *          - Smooth bezier interpolation
     */
    void drawHistoryTrail(QPainter &painter);

    /**
     * @brief Draws the current position indicator
     * @param painter Reference to active QPainter
     *
     * @details Renders:
     *          - Position-appropriate color
     *          - Direction indicator
     *          - Value tooltip (optional)
     */
    void drawCurrentPosition(QPainter &painter);
};

#endif // IMUGFORCE_H
