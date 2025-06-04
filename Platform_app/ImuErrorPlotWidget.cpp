#include "ImuErrorPlotWidget.h"
#include <QVBoxLayout>

/**
 * @brief Konstruktor klasy ImuErrorPlotWidget - inicjalizuje komponenty wykresu.
 *
 * Tworzy dwa wykresy do wizualizacji roznicy w odczytach akcelerometru i zyroskopu
 * pomiedzy dwoma urzadzeniami IMU. Inicjalizuje serie, osie oraz widoki wykresow.
 * Uklada je poziomo.
 *
 * @param parent Wskaznik na widget nadrzedny, albo nullptr.
 */
ImuErrorPlotWidget::ImuErrorPlotWidget(QWidget *parent)
    : QWidget(parent), sampleIndex(0)
{
    timer.start();

    // Inicjalizacja serii danych dla bledu akcelerometru
    accelX = new QLineSeries(); accelX->setName("ΔAccel X");
    accelY = new QLineSeries(); accelY->setName("ΔAccel Y");
    accelZ = new QLineSeries(); accelZ->setName("ΔAccel Z");

    // Inicjalizacja serii danych dla bledu zyroskopu
    gyroX = new QLineSeries(); gyroX->setName("ΔGyro X");
    gyroY = new QLineSeries(); gyroY->setName("ΔGyro Y");
    gyroZ = new QLineSeries(); gyroZ->setName("ΔGyro Z");

    // === Konfiguracja wykresu akcelerometru ===
    accelChart = new QChart();
    accelChart->addSeries(accelX);
    accelChart->addSeries(accelY);
    accelChart->addSeries(accelZ);
    accelChart->createDefaultAxes();
    accelChart->setTitle("Accelerometer Error");

    // Konfiguracja osi X
    if (auto xAxis = qobject_cast<QValueAxis*>(accelChart->axisX())) {
        accelAxisX = xAxis;
        accelAxisX->setLabelFormat("%d");
        accelAxisX->setLabelsVisible(false);
        accelAxisX->setTickCount(0);
        accelAxisX->setRange(0, 1);
    }

    // Konfiguracja osi Y
    if (auto yAxis = qobject_cast<QValueAxis*>(accelChart->axisY())) {
        accelAxisY = yAxis;
        accelAxisY->setLabelFormat("%.2f");
        accelAxisY->setTickCount(7);
        accelAxisY->setRange(-6.0, 6.0);
    }

    // Widok wykresu akcelerometru
    accelChartView = new QChartView(accelChart);
    accelChartView->setRenderHint(QPainter::Antialiasing);
    accelChartView->setMinimumHeight(300);
    accelChartView->setMinimumWidth(300);
    accelChart->setMargins(QMargins(5, 5, 5, 5));

    // === Konfiguracja wykresu zyroskopu ===
    gyroChart = new QChart();
    gyroChart->addSeries(gyroX);
    gyroChart->addSeries(gyroY);
    gyroChart->addSeries(gyroZ);
    gyroChart->createDefaultAxes();
    gyroChart->setTitle("Gyroscope Error");

    // Konfiguracja osi X
    if (auto xAxis = qobject_cast<QValueAxis*>(gyroChart->axisX())) {
        gyroAxisX = xAxis;
        gyroAxisX->setLabelFormat("%d");
        gyroAxisX->setLabelsVisible(false);
        gyroAxisX->setTickCount(0);
        gyroAxisX->setRange(0, 1);
    }

    // Konfiguracja osi Y
    if (auto yAxis = qobject_cast<QValueAxis*>(gyroChart->axisY())) {
        gyroAxisY = yAxis;
        gyroAxisY->setLabelFormat("%.2f");
        gyroAxisY->setTickCount(7);
        gyroAxisY->setRange(-5.0, 5.0);
    }

    // Widok wykresu zyroskopu
    gyroChartView = new QChartView(gyroChart);
    gyroChartView->setRenderHint(QPainter::Antialiasing);
    gyroChartView->setMinimumHeight(300);
    gyroChartView->setMinimumWidth(300);
    gyroChart->setMargins(QMargins(5, 5, 5, 5));

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Uklad poziomy
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(accelChartView);
    layout->addWidget(gyroChartView);
    setLayout(layout);
}

/**
 * @brief Dodaje nowa probe danych bledu IMU do wykresow.
 *
 * Uaktualnia serie danych o nowe wartosci bledu i usuwa stare punkty,
 * aby zachowac ruchome okno czasowe. Dynamicznie skaluje osie Y.
 *
 * @param dax Roznica przyspieszenia w osi X.
 * @param day Roznica przyspieszenia w osi Y.
 * @param daz Roznica przyspieszenia w osi Z.
 * @param dgx Roznica predkosci katowej w osi X.
 * @param dgy Roznica predkosci katowej w osi Y.
 * @param dgz Roznica predkosci katowej w osi Z.
 */
void ImuErrorPlotWidget::addErrorSample(float dax, float day, float daz, float dgx, float dgy, float dgz)
{
    qreal timeSec = timer.elapsed() / 1000.0;

    // Dodajemy nowe punkty danych do kazdej serii
    accelX->append(timeSec, dax);
    accelY->append(timeSec, day);
    accelZ->append(timeSec, daz);

    gyroX->append(timeSec, dgx);
    gyroY->append(timeSec, dgy);
    gyroZ->append(timeSec, dgz);

    // Szerokosc widocznego okna czasowego (w sekundach)
    constexpr qreal windowWidth = 4.0;

    qreal start = qMax(0.0, timeSec - windowWidth);
    qreal end = timeSec;

    // Lambda do usuwania starych punktow z serii
    auto pruneOldPoints = [start](QLineSeries* series) {
        while (!series->pointsVector().isEmpty() && series->pointsVector().first().x() < start) {
            series->remove(0);
        }
    };

    for (QLineSeries* s : {accelX, accelY, accelZ, gyroX, gyroY, gyroZ}) {
        pruneOldPoints(s);
    }

    // Aktualizacja zakresu osi X
    accelAxisX->setRange(start, end);
    gyroAxisX->setRange(start, end);
}
