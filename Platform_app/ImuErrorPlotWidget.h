#ifndef IMUERRORPLOTWIDGET_H
#define IMUERRORPLOTWIDGET_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <QElapsedTimer>

    class ImuErrorPlotWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ImuErrorPlotWidget(QWidget *parent = nullptr);

    void addErrorSample(float dax, float day, float daz, float dgx, float dgy, float dgz);

private:
private:
    int sampleIndex;
    const int maxSamples = 200;

    QLineSeries *accelX, *accelY, *accelZ;
    QLineSeries *gyroX, *gyroY, *gyroZ;

    QChartView *accelChartView;
    QChartView *gyroChartView;

    QChart *accelChart;
    QChart *gyroChart;

    QValueAxis *accelAxisX;
    QValueAxis *accelAxisY;
    QValueAxis *gyroAxisX;
    QValueAxis *gyroAxisY;
    QElapsedTimer timer;

};

#endif // IMUERRORPLOTWIDGET_H
