#include "ImuErrorPlotWidget.h"
#include <QVBoxLayout>

ImuErrorPlotWidget::ImuErrorPlotWidget(QWidget *parent)
    : QWidget(parent), sampleIndex(0)
{
    timer.start();
    accelX = new QLineSeries(); accelX->setName("ΔAccel X");
    accelY = new QLineSeries(); accelY->setName("ΔAccel Y");
    accelZ = new QLineSeries(); accelZ->setName("ΔAccel Z");

    gyroX = new QLineSeries(); gyroX->setName("ΔGyro X");
    gyroY = new QLineSeries(); gyroY->setName("ΔGyro Y");
    gyroZ = new QLineSeries(); gyroZ->setName("ΔGyro Z");

    // === Accelerometer Chart ===
    accelChart = new QChart();
    accelChart->addSeries(accelX);
    accelChart->addSeries(accelY);
    accelChart->addSeries(accelZ);
    accelChart->createDefaultAxes();
    accelChart->setTitle("Accelerometer Error");

    // Retrieve and configure axes after createDefaultAxes()
    if (auto xAxis = qobject_cast<QValueAxis*>(accelChart->axisX())) {
        accelAxisX = xAxis;
        accelAxisX->setLabelFormat("%d");
        accelAxisX->setLabelsVisible(false);
        accelAxisX->setTickCount(0);
        accelAxisX->setRange(0, 1);
    }
    if (auto yAxis = qobject_cast<QValueAxis*>(accelChart->axisY())) {
        accelAxisY = yAxis;
        accelAxisY->setLabelFormat("%.2f");
        accelAxisX->setLabelsVisible(false);
        accelAxisY->setTickCount(0);
        accelAxisY->setRange(-1.0, 1.0);
    }

    accelChartView = new QChartView(accelChart);
    accelChartView->setRenderHint(QPainter::Antialiasing);
    accelChartView->setMinimumHeight(300);
    accelChartView->setMinimumWidth(300);
    accelChart->setMargins(QMargins(5, 5, 5, 5));

    // === Gyroscope Chart ===
    gyroChart = new QChart();
    gyroChart->addSeries(gyroX);
    gyroChart->addSeries(gyroY);
    gyroChart->addSeries(gyroZ);
    gyroChart->createDefaultAxes();
    gyroChart->setTitle("Gyroscope Error");

    // Retrieve and configure axes after createDefaultAxes()
    if (auto xAxis = qobject_cast<QValueAxis*>(gyroChart->axisX())) {
        gyroAxisX = xAxis;
        gyroAxisX->setLabelFormat("%d");
        gyroAxisX->setLabelsVisible(false);
        gyroAxisX->setTickCount(0);
        gyroAxisX->setRange(0, 1);
    }
    if (auto yAxis = qobject_cast<QValueAxis*>(gyroChart->axisY())) {
        gyroAxisY = yAxis;
        gyroAxisY->setLabelFormat("%.2f");
        gyroAxisY->setTickCount(7);
        gyroAxisY->setRange(-1.0, 1.0);
    }

    gyroChartView = new QChartView(gyroChart);
    gyroChartView->setRenderHint(QPainter::Antialiasing);
    gyroChartView->setMinimumHeight(300);
    gyroChartView->setMinimumWidth(300);
    gyroChart->setMargins(QMargins(5, 5, 5, 5));

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(accelChartView);
    layout->addWidget(gyroChartView);
    setLayout(layout);


}

void ImuErrorPlotWidget::addErrorSample(float dax, float day, float daz, float dgx, float dgy, float dgz)
{
    qreal timeSec = timer.elapsed() / 1000.0;

    accelX->append(timeSec, dax);
    accelY->append(timeSec, day);
    accelZ->append(timeSec, daz);

    gyroX->append(timeSec, dgx);
    gyroY->append(timeSec, dgy);
    gyroZ->append(timeSec, dgz);

    // Define max window width in seconds
    constexpr qreal windowWidth = 4.0;

    // Calculate start and end for X axis range
    qreal start = 0.0;
    qreal end = timeSec;

    if (timeSec > windowWidth) {
        start = timeSec - windowWidth;
        end = timeSec;
    }

    // Remove points older than start
    auto pruneOldPoints = [start](QLineSeries* series) {
        while (!series->pointsVector().isEmpty() && series->pointsVector().first().x() < start) {
            series->remove(0);
        }
    };

    for (QLineSeries* s : {accelX, accelY, accelZ, gyroX, gyroY, gyroZ}) {
        pruneOldPoints(s);
    }

    accelAxisX->setRange(start, end);
    gyroAxisX->setRange(start, end);


    auto updateYAxis = [](QValueAxis* axis, const std::vector<QLineSeries*>& seriesList) {
        qreal minY = std::numeric_limits<qreal>::max();
        qreal maxY = std::numeric_limits<qreal>::lowest();
        for (auto* series : seriesList) {
            for (const QPointF& point : series->pointsVector()) {
                minY = std::min(minY, point.y());
                maxY = std::max(maxY, point.y());
            }
        }
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

    updateYAxis(accelAxisY, {accelX, accelY, accelZ});
    updateYAxis(gyroAxisY, {gyroX, gyroY, gyroZ});
}
