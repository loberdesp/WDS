#include "hexagon.h"
#include <QPainter>
#include <QtMath>

HexagonBars::HexagonBars(QWidget *parent)
    : QWidget(parent), barValues(6, 0.5f) {
    calculateHexagon();
}

void HexagonBars::setBarValue(int index, float value) {
    if (index < 0 || index >= 6) return;
    barValues[index] = qBound(0.0f, value, 1.0f);
    update();
}

float HexagonBars::getBarValue(int index) const {
    return (index >= 0 && index < 6) ? barValues[index] : 0.0f;
}

void HexagonBars::calculateHexagon() {
    hexagonPoints.clear();
    float radius = 70; // You can scale this based on widget size
    float angleDeg = 0;
    for (int i = 0; i < 6; ++i) {
        float angleRad = qDegreesToRadians(angleDeg);
        float x = width() / 2 + radius * qCos(angleRad);
        float y = height() / 2 + radius * qSin(angleRad);
        hexagonPoints.append(QPointF(x, y));
        angleDeg += 60;
    }
}

void HexagonBars::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    calculateHexagon();
    drawBars(painter);
    drawHexagon(painter);

}

void HexagonBars::drawHexagon(QPainter &painter) {
    QPolygonF polygon(hexagonPoints);

    QPen pen(Qt::white);        // Set color
    pen.setWidth(3);            // Thickness in pixels (increase as needed)
    pen.setJoinStyle(Qt::MiterJoin); // Optional: cleaner corners

    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.drawPolygon(polygon);
}

void HexagonBars::drawHeightIndicators(QPainter &painter, const QVector<float>& heights) {
    for (int i = 0; i < 6; ++i) {
        QPointF outerPoint = hexagonPoints[i];

        // Simulate the raised section effect (e.g., draw a shadow or glow)
        float height = heights[i];
        if (height > 0) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(0, 0, 0, 40));  // Semi-transparent shadow effect
            QRectF shadowRect(outerPoint.x() - 5, outerPoint.y() - 5, 10, height);
            painter.drawEllipse(shadowRect);  // Shadow for raised section

            // Optionally, draw a glow or a raised visual effect here
        }
    }
}

void HexagonBars::drawBars(QPainter &painter) {
    float barLength = 50;  // Length from outer hex point toward center
    float barWidth = 10;
    QVector<QPointF> barTips;

    // Simulate "height" changes for expanded bars
    QVector<float> barHeights;  // To store height values for each bar
    QPointF center = QPointF(width() / 2, height() / 2);

    for (int i = 0; i < 6; ++i) {
        float value = barValues[i];
        QPointF outerPoint = hexagonPoints[i];

        // Direction vector from outer point to center
        QPointF direction = center - outerPoint;
        direction = direction / std::hypot(direction.x(), direction.y());  // Normalize

        // Compute tip based on value
        QPointF tip = outerPoint + direction * (barLength * value);
        barTips.append(tip);

        // Simulate a "height effect" by adjusting the z-axis or platform height
        float height = value * 10;  // For example, height increases as value increases
        barHeights.append(height);

        // Draw bar rectangle along direction vector
        painter.save();

        // Translate to outer point
        painter.translate(outerPoint);

        // Compute rotation angle
        float angleRad = std::atan2(direction.y(), direction.x());
        float angleDeg = qRadiansToDegrees(angleRad);
        painter.rotate(angleDeg);

        // Set the color based on the value (Green -> Yellow -> Red gradient)
        QColor barColor = getBarColor(value);
        painter.setBrush(barColor);

        QPen borderPen(Qt::white);
        borderPen.setWidth(2);
        painter.setPen(borderPen);
        QRectF barRect(0, -barWidth / 2, barLength * value, barWidth);
        painter.drawRect(barRect);

        painter.restore();
    }

    // Draw connecting inner hexagon
    painter.setPen(QPen(Qt::magenta, 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawPolygon(QPolygonF(barTips));

    // Optionally, you could simulate raising the platform section by drawing
    // additional effects like shadows or adjusting the height of the platform part
    drawHeightIndicators(painter, barHeights);
}

// Function to return the color based on the bar value
QColor HexagonBars::getBarColor(float value) {
    // Map the value from 0.0 to 1.0 to a gradient from green to yellow to red
    if (value <= 0.5f) {
        // Green to Yellow (value 0 to 0.5)
        int green = 255;  // Full green
        int red = static_cast<int>(510 * value); // Red starts increasing from 0 to 255
        return QColor(red, green, 0);  // Return the color as RGB
    } else {
        // Yellow to Red (value 0.5 to 1.0)
        int red = 255; // Red increases from 0 to 255
        int green = static_cast<int>(255 - 510 * (value - 0.5f)); // Green decreases from 255 to 0
        return QColor(red, green, 0);  // Return the color as RGB
    }
}
