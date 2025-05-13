#include "ImuGForce.h"
#include <QPainter>
#include <QtMath>

ImuGForceWidget::ImuGForceWidget(QWidget *parent)
    : QWidget(parent), accX(0.0f), accY(0.0f), maxG(3.0f) {}

void ImuGForceWidget::setAcceleration(float ax, float ay) {
    accX = ax;
    accY = ay;
    update();
}

QSize ImuGForceWidget::minimumSizeHint() const {
    return QSize(100, 100);
}

QSize ImuGForceWidget::sizeHint() const {
    return QSize(200, 200);
}

void ImuGForceWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int w = width();
    int h = height();
    int size = qMin(w, h);
    QPointF center(w / 2.0, h / 2.0);

    float radius = size / 2.0 * 0.9; // 90% of half-size for padding

    painter.translate(center);

    // Draw concentric G rings (1G, 2G, 3G)
    QPen ringPen(Qt::gray);
    ringPen.setStyle(Qt::DashLine);
    painter.setPen(ringPen);
    painter.setBrush(Qt::NoBrush);

    int ringCount = static_cast<int>(maxG);
    for (int i = 1; i <= ringCount; ++i) {
        float r = radius * (i / maxG);
        painter.drawEllipse(QPointF(0, 0), r, r);
    }

    // Draw axes
    painter.setPen(QPen(Qt::lightGray, 1));
    painter.drawLine(QPointF(-radius, 0), QPointF(radius, 0));
    painter.drawLine(QPointF(0, -radius), QPointF(0, radius));

    // Draw G-point
    float gx = accX / maxG * radius;
    float gy = -accY / maxG * radius; // Invert Y for screen coordinates

    QColor dotColor = Qt::red;
    if (std::hypot(accX, accY) > 1.5)
        dotColor = Qt::yellow;
    if (std::hypot(accX, accY) > 2.5)
        dotColor = Qt::magenta;

    painter.setPen(Qt::NoPen);
    painter.setBrush(dotColor);
    painter.drawEllipse(QPointF(gx, gy), 6, 6);

    // Optional center point
    painter.setBrush(Qt::white);
    painter.drawEllipse(QPointF(0, 0), 3, 3);
}
