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
 * @brief Widget for visualizing IMU error data over time.
 *
 * This widget displays real-time differences in acceleration and gyroscope readings
 * between two IMU devices using line charts. It maintains a history of error samples
 * and plots them for comparison and diagnostics.
 */
class ImuErrorPlotWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructor for ImuErrorPlotWidget.
     * @param parent Optional parent widget.
     *
     * Initializes the plotting components for both accelerometer and gyroscope error data.
     */
    explicit ImuErrorPlotWidget(QWidget *parent = nullptr);

    /**
     * @brief Adds a new sample of IMU error data to the plot.
     * @param dax Difference in acceleration along the X axis.
     * @param day Difference in acceleration along the Y axis.
     * @param daz Difference in acceleration along the Z axis.
     * @param dgx Difference in angular velocity (gyroscope) along the X axis.
     * @param dgy Difference in angular velocity (gyroscope) along the Y axis.
     * @param dgz Difference in angular velocity (gyroscope) along the Z axis.
     *
     * This method appends a new set of error measurements to the series and updates the chart display.
     * When the maximum number of samples is reached, the oldest samples are discarded.
     */
    void addErrorSample(float dax, float day, float daz, float dgx, float dgy, float dgz);

private:
    int sampleIndex;                  ///< Current index used for plotting new samples.
    const int maxSamples = 200;       ///< Maximum number of samples to display on the plot.

    // Accelerometer error series
    QLineSeries *accelX;              ///< Line series for X-axis acceleration error.
    QLineSeries *accelY;              ///< Line series for Y-axis acceleration error.
    QLineSeries *accelZ;              ///< Line series for Z-axis acceleration error.

    // Gyroscope error series
    QLineSeries *gyroX;               ///< Line series for X-axis gyroscope error.
    QLineSeries *gyroY;               ///< Line series for Y-axis gyroscope error.
    QLineSeries *gyroZ;               ///< Line series for Z-axis gyroscope error.

    QChartView *accelChartView;       ///< View for displaying the accelerometer error chart.
    QChartView *gyroChartView;        ///< View for displaying the gyroscope error chart.

    QChart *accelChart;               ///< Chart object for accelerometer errors.
    QChart *gyroChart;                ///< Chart object for gyroscope errors.

    QValueAxis *accelAxisX;           ///< X-axis for accelerometer error chart (typically sample/time index).
    QValueAxis *accelAxisY;           ///< Y-axis for accelerometer error chart (value scale).
    QValueAxis *gyroAxisX;            ///< X-axis for gyroscope error chart.
    QValueAxis *gyroAxisY;            ///< Y-axis for gyroscope error chart.

    QElapsedTimer timer;              ///< Timer to optionally track time since the widget was initialized (not currently used).
};

#endif // IMUERRORPLOTWIDGET_H
