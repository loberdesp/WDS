#include "hexagon.h"
#include <QPainter>
#include <QtMath>

// Konstruktor widgetu szesciokata z paskami
HexagonBars::HexagonBars(QWidget *parent)
    : QWidget(parent), barValues(6, 0.5f) {  // Inicjalizacja 6 pasków z wartoscia 0.5
    calculateHexagon();  // Wylicz wspolrzedne szesciokata
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

// Ustawienie wartosci paska w danym indeksie (0-5)
void HexagonBars::setBarValue(int index, float value) {
    if (index < 0 || index >= 6) return;  // Sprawdzenie zakresu
    barValues[index] = qBound(0.0f, value, 1.0f);  // Ograniczenie wartosci do [0,1]
    update();  // Przerysuj widget
}

// Pobranie wartosci paska
float HexagonBars::getBarValue(int index) const {
    return (index >= 0 && index < 6) ? barValues[index] : 0.0f;
}

// Oblicz wspolrzedne 6 wierzcholkow szesciokata
void HexagonBars::calculateHexagon() {
    hexagonPoints.clear();

    float size = qMin(width(), height());
    float radius = size * 0.4f;  // Promien to 40% rozmiaru
    float angleDeg = -90;        // Startowy kat skierowany do gory

    for (int i = 0; i < 6; ++i) {
        float angleRad = qDegreesToRadians(angleDeg);
        float x = width() / 2 + radius * qCos(angleRad);
        float y = height() / 2 + radius * qSin(angleRad);
        hexagonPoints.append(QPointF(x, y));  // Dodaj wierzcholek
        angleDeg += 60;  // Kolejny kat (co 60 stopni)
    }
}

// Minimalny rozmiar widgetu
QSize HexagonBars::minimumSizeHint() const {
    return QSize(200, 200);
}

// Preferowany rozmiar widgetu
QSize HexagonBars::sizeHint() const {
    return QSize(300, 300);
}

// Glowna funkcja rysujaca widget
void HexagonBars::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    calculateHexagon();      // Odswiez wspolrzedne szesciokata
    drawBars(painter);       // Rysuj paski
    drawHexagon(painter);    // Rysuj kontur szesciokata
}

// Rysuj kontur szesciokata
void HexagonBars::drawHexagon(QPainter &painter) {
    QPolygonF polygon(hexagonPoints);

    QPen pen(Qt::white);          // Kolor bialy
    pen.setWidth(3);              // Grubosc linii
    pen.setJoinStyle(Qt::MiterJoin);  // Styl laczenia krawedzi

    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.drawPolygon(polygon);
}

// Rysuj wskazniki wysokosci (np. cienie lub efekty podniesienia)
void HexagonBars::drawHeightIndicators(QPainter &painter, const QVector<float>& heights) {
    for (int i = 0; i < 6; ++i) {
        QPointF outerPoint = hexagonPoints[i];
        float height = heights[i];

        if (height > 0) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(0, 0, 0, 40));  // Pólprzezroczysty cien
            QRectF shadowRect(outerPoint.x() - 5, outerPoint.y() - 5, 10, height);
            painter.drawEllipse(shadowRect);  // Cien elipsy jako efekt podniesienia
        }
    }
}

// Rysuj paski wychodzace ze szesciokata
void HexagonBars::drawBars(QPainter &painter) {
    float barLength = qMin(width(), height()) * 0.3f;  // Dlugosc paskow = 30% rozmiaru
    float barWidth = 10;  // Szerokosc paska
    QVector<QPointF> barTips;  // Koncowe punkty paskow

    QVector<float> barHeights;  // Wysokosci do efektu podniesienia
    QPointF center = QPointF(width() / 2, height() / 2);  // Srodek widgetu

    for (int i = 0; i < 6; ++i) {
        float value = barValues[i];
        QPointF outerPoint = hexagonPoints[i];

        // Kierunek od punktu zewnetrznego do srodka
        QPointF direction = center - outerPoint;
        direction = direction / std::hypot(direction.x(), direction.y());  // Normalizacja

        // Oblicz koniec paska na podstawie wartosci
        QPointF tip = outerPoint + direction * (barLength * value);
        barTips.append(tip);

        float height = value * 10;  // "Wysokosc" efektu 3D
        barHeights.append(height);

        // Rysowanie paska
        painter.save();
        painter.translate(outerPoint);  // Przesun punkt odniesienia

        float angleRad = std::atan2(direction.y(), direction.x());
        float angleDeg = qRadiansToDegrees(angleRad);
        painter.rotate(angleDeg);  // Obrót do kierunku paska

        QColor barColor = getBarColor(value);  // Kolor w zaleznosci od wartosci
        painter.setBrush(barColor);

        QPen borderPen(Qt::white);  // Obramowanie biale
        borderPen.setWidth(2);
        painter.setPen(borderPen);

        QRectF barRect(0, -barWidth / 2, barLength * value, barWidth);
        painter.drawRect(barRect);  // Rysuj prostokat paska

        painter.restore();  // Przywroc poprzedni stan painter'a
    }

    // Rysuj szesciokat laczacy konce paskow
    painter.setPen(QPen(Qt::magenta, 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawPolygon(QPolygonF(barTips));

    // Rysuj efekty wysokosci (np. cienie)
    drawHeightIndicators(painter, barHeights);
}

// Funkcja zwraca kolor paska w zaleznosci od wartosci
QColor HexagonBars::getBarColor(float value) {
    // Gradient: zielony -> zolty -> czerwony
    if (value <= 0.5f) {
        int green = 255;
        int red = static_cast<int>(510 * value);  // Czerwien rosnie do 255
        return QColor(red, green, 0);
    } else {
        int red = 255;
        int green = static_cast<int>(255 - 510 * (value - 0.5f));  // Zielony maleje do 0
        return QColor(red, green, 0);
    }
}
