#include "ImuErrorPlotWidget.h"
#include <QVBoxLayout>

/**
 * @brief Constructs the ImuErrorPlotWidget and initializes the chart components.
 *
 * Sets up two charts for visualizing the difference in accelerometer and gyroscope values between two IMU devices.
 * Initializes line series, axes, and chart views. Lays them out horizontally.
 *
 * @param parent The parent widget, or nullptr if none.
 */
ImuErrorPlotWidget::ImuErrorPlotWidget(QWidget *parent)
    : QWidget(parent), sampleIndex(0)
{
    timer.start();

    // Initialize line series for acceleration errors
    accelX = new QLineSeries(); accelX->setName("ΔAccel X");
    accelY = new QLineSeries(); accelY->setName("ΔAccel Y");
    accelZ = new QLineSeries(); accelZ->setName("ΔAccel Z");

    // Initialize line series for gyroscope errors
    gyroX = new QLineSeries(); gyroX->setName("ΔGyro X");
    gyroY = new QLineSeries(); gyroY->setName("ΔGyro Y");
    gyroZ = new QLineSeries(); gyroZ->setName("ΔGyro Z");

    // === Accelerometer Chart Setup ===
    accelChart = new QChart();
    accelChart->addSeries(accelX);
    accelChart->addSeries(accelY);
    accelChart->addSeries(accelZ);
    accelChart->createDefaultAxes();
    accelChart->setTitle("Accelerometer Error");

    // Configure X axis
    if (auto xAxis = qobject_cast<QValueAxis*>(accelChart->axisX())) {
        accelAxisX = xAxis;
        accelAxisX->setLabelFormat("%d");
        accelAxisX->setLabelsVisible(false);
        accelAxisX->setTickCount(0);
        accelAxisX->setRange(0, 1);
    }

    // Configure Y axis
    if (auto yAxis = qobject_cast<QValueAxis*>(accelChart->axisY())) {
        accelAxisY = yAxis;
        accelAxisY->setLabelFormat("%.2f");
        accelAxisY->setTickCount(7);
        accelAxisY->setRange(-6.0, 6.0);
    }

    // Chart view for accelerometer chart
    accelChartView = new QChartView(accelChart);
    accelChartView->setRenderHint(QPainter::Antialiasing);
    accelChartView->setMinimumHeight(300);
    accelChartView->setMinimumWidth(300);
    accelChart->setMargins(QMargins(5, 5, 5, 5));

    // === Gyroscope Chart Setup ===
    gyroChart = new QChart();
    gyroChart->addSeries(gyroX);
    gyroChart->addSeries(gyroY);
    gyroChart->addSeries(gyroZ);
    gyroChart->createDefaultAxes();
    gyroChart->setTitle("Gyroscope Error");

    // Configure X axis
    if (auto xAxis = qobject_cast<QValueAxis*>(gyroChart->axisX())) {
        gyroAxisX = xAxis;
        gyroAxisX->setLabelFormat("%d");
        gyroAxisX->setLabelsVisible(false);
        gyroAxisX->setTickCount(0);
        gyroAxisX->setRange(0, 1);
    }

    // Configure Y axis
    if (auto yAxis = qobject_cast<QValueAxis*>(gyroChart->axisY())) {
        gyroAxisY = yAxis;
        gyroAxisY->setLabelFormat("%.2f");
        gyroAxisY->setTickCount(7);
        gyroAxisY->setRange(-5.0, 5.0);
    }

    // Chart view for gyroscope chart
    gyroChartView = new QChartView(gyroChart);
    gyroChartView->setRenderHint(QPainter::Antialiasing);
    gyroChartView->setMinimumHeight(300);
    gyroChartView->setMinimumWidth(300);
    gyroChart->setMargins(QMargins(5, 5, 5, 5));

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Layout setup
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(accelChartView);
    layout->addWidget(gyroChartView);
    setLayout(layout);
}

/**
 * @brief Adds a new sample of IMU error data to the charts.
 *
 * Updates the line series with new error values and prunes old values to maintain a moving time window.
 * Dynamically rescales the Y axes based on the range of current visible data.
 *
 * @param dax Delta acceleration on X axis.
 * @param day Delta acceleration on Y axis.
 * @param daz Delta acceleration on Z axis.
 * @param dgx Delta gyroscope on X axis.
 * @param dgy Delta gyroscope on Y axis.
 * @param dgz Delta gyroscope on Z axis.
 */
void ImuErrorPlotWidget::addErrorSample(float dax, float day, float daz, float dgx, float dgy, float dgz)
{
    qreal timeSec = timer.elapsed() / 1000.0;

    // Append new data points to each series
    accelX->append(timeSec, dax);
    accelY->append(timeSec, day);
    accelZ->append(timeSec, daz);

    gyroX->append(timeSec, dgx);
    gyroY->append(timeSec, dgy);
    gyroZ->append(timeSec, dgz);

    // Define visible window width in seconds
    constexpr qreal windowWidth = 4.0;

    qreal start = qMax(0.0, timeSec - windowWidth);
    qreal end = timeSec;

    // Lambda to remove old points from series
    auto pruneOldPoints = [start](QLineSeries* series) {
        while (!series->pointsVector().isEmpty() && series->pointsVector().first().x() < start) {
            series->remove(0);
        }
    };

    for (QLineSeries* s : {accelX, accelY, accelZ, gyroX, gyroY, gyroZ}) {
        pruneOldPoints(s);
    }

    // Update X axis ranges
    accelAxisX->setRange(start, end);
    gyroAxisX->setRange(start, end);

    // Lambda to update Y axis range based on current data
    auto updateYAxis = [](QValueAxis* axis, const std::vector<QLineSeries*>& seriesList) {
        qreal minY = std::numeric_limits<qreal>::max();
        qreal maxY = std::numeric_limits<qreal>::lowest();
        for (auto* series : seriesList) {
            for (const QPointF& point : series->pointsVector()) {
                minY = std::min(minY, point.y());
                maxY = std::max(maxY, point.y());
            }
        }

        // Prevent flat line from collapsing Y range
        if (minY == maxY) {
            minY -= 1;
            maxY += 1;
        } else {
            qreal margin = (maxY - minY) * 0.1;
            minY -= margin;
            maxY += margin;
        }

        axis->setRange(minY, maxY);
    };

    // Update Y axes based on the current visible data
    // updateYAxis(accelAxisY, {accelX, accelY, accelZ});
    // updateYAxis(gyroAxisY, {gyroX, gyroY, gyroZ});
}
