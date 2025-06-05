/**
 * @file    imuerrorplotwidget.h
 * @brief   Real-time IMU error visualization widget using Qt Charts
 *
 * @details Provides dual chart visualization for comparing IMU sensor differences:
 *          - Accelerometer error (X, Y, Z axes)
 *          - Gyroscope error (X, Y, Z axes)
 *          - Configurable sample history
 *          - Automatic axis scaling
 *
 * @author  Piotr Siembab
 * @date    18.04.2025
 * @version 1.2
 */

#ifndef IMUERRORPLOTWIDGET_H
#define IMUERRORPLOTWIDGET_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <QElapsedTimer>

/**
 * @class ImuErrorPlotWidget
 * @brief Dual-chart widget for visualizing IMU sensor differences
 *
 * @details Features include:
 *          - Separate charts for accelerometer and gyroscope errors
 *          - 200-sample rolling history
 *          - Color-coded axes (X=red, Y=green, Z=blue)
 *          - Dynamic Y-axis scaling
 *          - Millisecond-accurate timing
 *
 * Typical use cases:
 * - IMU calibration verification
 * - Sensor fusion algorithm debugging
 * - Vibration analysis
 * - Hardware synchronization monitoring
 */
class ImuErrorPlotWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the error plot widget
     * @param parent Parent widget (default: nullptr)
     *
     * @details Initializes:
     *          - Dual chart layout (accelerometer/gyroscope)
     *          - Color-coded data series
     *          - Axis configurations
     *          - Sample history buffers
     */
    explicit ImuErrorPlotWidget(QWidget *parent = nullptr);

    /**
     * @brief Adds new error sample to both charts
     * @param dax X-axis acceleration difference (m/s²)
     * @param day Y-axis acceleration difference (m/s²)
     * @param daz Z-axis acceleration difference (m/s²)
     * @param dgx X-axis gyro difference (rad/s)
     * @param dgy Y-axis gyro difference (rad/s)
     * @param dgz Z-axis gyro difference (rad/s)
     *
     * @details Handles:
     *          - Sample buffer rotation
     *          - Automatic axis scaling
     *          - Chart updates
     *          - Time tracking
     *
     * @note Sample values should be in consistent SI units
     * @see maxSamples
     */
    void addErrorSample(float dax, float day, float daz, float dgx, float dgy, float dgz);

    /**
     * @brief Updates all user-visible strings in the UI to reflect the current language.
     *
     * This function should be called whenever the application language changes,
     * to re-apply translations to all text elements such as labels, titles, and tooltips.
     * It ensures the interface dynamically updates without restarting the application.
     */
    void retranslateUi();

private:
    int sampleIndex;                  ///< Circular buffer index (0-maxSamples)
    const int maxSamples = 200;       ///< Maximum stored samples per axis

    // Accelerometer error series
    QLineSeries *accelX;              ///< X-axis acceleration error (typically red)
    QLineSeries *accelY;              ///< Y-axis acceleration error (typically green)
    QLineSeries *accelZ;              ///< Z-axis acceleration error (typically blue)

    // Gyroscope error series
    QLineSeries *gyroX;               ///< X-axis angular velocity error
    QLineSeries *gyroY;               ///< Y-axis angular velocity error
    QLineSeries *gyroZ;               ///< Z-axis angular velocity error

    // Chart display components
    QChartView *accelChartView;       ///< Container for accelerometer chart
    QChartView *gyroChartView;        ///< Container for gyroscope chart

    // Chart objects
    QChart *accelChart;               ///< Accelerometer error visualization
    QChart *gyroChart;                ///< Gyroscope error visualization

    // Chart axes
    QValueAxis *accelAxisX;           ///< Accelerometer time/index axis
    QValueAxis *accelAxisY;           ///< Accelerometer value axis (auto-scaled)
    QValueAxis *gyroAxisX;            ///< Gyroscope time/index axis
    QValueAxis *gyroAxisY;            ///< Gyroscope value axis (auto-scaled)

    QElapsedTimer timer;              ///< High-resolution timer for sample timing

    /**
     * @brief Initializes a chart with default settings
     * @param chart Pointer to chart object
     * @param title Chart display title
     * @param axisX Pointer to X-axis
     * @param axisY Pointer to Y-axis
     *
     * @details Configures:
     *          - Chart title and legend
     *          - Axis labels and ranges
     *          - Animation options
     *          - Background styling
     */
    void setupChart(QChart* chart, const QString& title, QValueAxis* axisX, QValueAxis* axisY);

    /**
     * @brief Initializes a data series with default settings
     * @param series Pointer to line series
     * @param name Series display name
     * @param color Series line color
     *
     * @details Configures:
     *          - Line style and width
     *          - Color and opacity
     *          - Point visualization
     */
    void setupSeries(QLineSeries* series, const QString& name, const QColor& color);

    /**
     * @brief Updates chart axis ranges based on current data
     * @param axisY Y-axis to update
     * @param series List of series to consider
     *
     * @details Automatically scales Y-axis to:
     *          - Show all visible data points
     *          - Maintain reasonable margins
     *          - Keep zero centered when appropriate
     */
    void updateYAxisRange(QValueAxis* axisY, const QList<QLineSeries*>& series);
};

#endif // IMUERRORPLOTWIDGET_H
