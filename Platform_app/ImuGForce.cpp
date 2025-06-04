#include "ImuGForce.h"
#include <QPainter>
#include <QtMath>
#include <QElapsedTimer>

// Globalny timer do pomiaru czasu od uruchomienia programu
static QElapsedTimer globalTimer;

// Konstruktor widgetu wyswietlajacego G-force
ImuGForceWidget::ImuGForceWidget(QWidget *parent)
    : QWidget(parent), accX(0.0f), accY(0.0f), maxG(3.0f), lastUpdateTime(0.0) {
    if (!globalTimer.isValid())
        globalTimer.start();  // Start timera, jesli jeszcze nie zostal uruchomiony

    // Ustawienie polityki rozmiaru (automatyczne dopasowanie do dostepnej przestrzeni)
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

// Funkcja pomocnicza - zwraca czas w sekundach od uruchomienia programu
qreal ImuGForceWidget::getElapsedSeconds() {
    return globalTimer.elapsed() / 1000.0;
}

// Ustawienie nowego przyspieszenia i aktualizacja wykresu
void ImuGForceWidget::setAcceleration(float ax, float ay) {
    accX = ax;
    accY = ay;

    qreal now = getElapsedSeconds();  // Aktualny czas
    lastUpdateTime = now;             // Zapamietanie czasu ostatniej aktualizacji

    trace.append({ ax, ay, now });    // Dodanie punktu do listy sladow (trace)

    // Usuniecie punktow starszych niz 2 sekundy
    while (!trace.isEmpty() && (now - trace.first().timestamp > 2.0)) {
        trace.removeFirst();
    }

    update();  // Wymuszenie przerysowania widgetu
}

// Minimalny rozmiar widgetu
QSize ImuGForceWidget::minimumSizeHint() const {
    return QSize(175, 175);
}

// Preferowany rozmiar widgetu
QSize ImuGForceWidget::sizeHint() const {
    return QSize(300, 300);
}

// Funkcja rysujaca zawartosc widgetu
void ImuGForceWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);  // Wlaczenie wygladzania linii

    int w = width();
    int h = height();
    int size = qMin(w, h);  // Ustal najmniejszy wymiar jako podstawe
    QPointF center(w / 2.0, h / 2.0);  // Srodek widgetu
    float radius = size / 2.0 * 0.9;   // Promien okregu G-force (90% polowy rozmiaru)

    painter.translate(center);  // Przesuniecie ukladu wspolrzednych do srodka

    // Rysowanie okregow G (kazdy odpowiada 1g)
    QPen ringPen(Qt::gray, 1, Qt::DashLine);
    painter.setPen(ringPen);
    painter.setBrush(Qt::NoBrush);

    for (int i = 1; i <= static_cast<int>(maxG); ++i) {
        float r = radius * (i / maxG);  // Oblicz promien dla danego poziomu G
        painter.drawEllipse(QPointF(0, 0), r, r);
    }

    // Rysowanie osi X i Y
    painter.setPen(QPen(Qt::lightGray, 1));
    painter.drawLine(QPointF(-radius, 0), QPointF(radius, 0));  // Oś X
    painter.drawLine(QPointF(0, -radius), QPointF(0, radius));  // Oś Y

    // Rysowanie sladu G z ostatnich 2 sekund (blednosc zalezy od wieku punktu)
    qreal now = lastUpdateTime;
    if (trace.size() >= 2) {
        for (int i = 1; i < trace.size(); ++i) {
            const TracePoint &prev = trace[i - 1];
            const TracePoint &curr = trace[i];

            qreal age = now - curr.timestamp;
            if (age > 2.0) continue;  // Pomijamy stare punkty

            qreal alpha = 1.0 - (age / 2.0);  // Przezroczystosc w zaleznosci od wieku
            QColor traceColor = Qt::blue;
            traceColor.setAlphaF(alpha);

            // Przeskalowanie wspolrzednych G do rozmiaru wykresu
            float x1 = prev.ax / maxG * radius;
            float y1 = -prev.ay / maxG * radius;
            float x2 = curr.ax / maxG * radius;
            float y2 = -curr.ay / maxG * radius;

            QPen tracePen(traceColor, 1.0);  // Cienka linia
            painter.setPen(tracePen);
            painter.drawLine(QPointF(x1, y1), QPointF(x2, y2));
        }
    }

    // Rysowanie srodka (punkt 0G)
    painter.setBrush(Qt::white);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QPointF(0, 0), 3, 3);

    // Rysowanie aktualnego punktu G (czerwony, zolty lub rozowy w zaleznosci od wielkosci G)
    float gx = accX / maxG * radius;
    float gy = -accY / maxG * radius;

    QColor dotColor = Qt::red;
    float gMag = std::hypot(accX, accY);  // Obliczenie modul G
    if (gMag > 1.5) dotColor = Qt::yellow;
    if (gMag > 2.5) dotColor = Qt::magenta;

    painter.setBrush(dotColor);
    painter.drawEllipse(QPointF(gx, gy), 10, 10);  // Punkt aktualnego przyspieszenia
}
