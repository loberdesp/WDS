#include "ImuGForce.h"
#include <QPainter>
#include <QtMath>
#include <QElapsedTimer>

static QElapsedTimer globalTimer;

ImuGForceWidget::ImuGForceWidget(QWidget *parent)
    : QWidget(parent), accX(0.0f), accY(0.0f), maxG(3.0f), lastUpdateTime(0.0) {
    if (!globalTimer.isValid())
        globalTimer.start();
}

qreal ImuGForceWidget::getElapsedSeconds() {
    return globalTimer.elapsed() / 1000.0;
}

void ImuGForceWidget::setAcceleration(float ax, float ay) {
    accX = ax;
    accY = ay;

    qreal now = getElapsedSeconds();
    lastUpdateTime = now;

    trace.append({ ax, ay, now });

    // Remove old trace points (older than 2 seconds)
    while (!trace.isEmpty() && (now - trace.first().timestamp > 2.0)) {
        trace.removeFirst();
    }

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
    float radius = size / 2.0 * 0.9;

    painter.translate(center);

    // Draw G rings
    QPen ringPen(Qt::gray, 1, Qt::DashLine);
    painter.setPen(ringPen);
    painter.setBrush(Qt::NoBrush);

    for (int i = 1; i <= static_cast<int>(maxG); ++i) {
        float r = radius * (i / maxG);
        painter.drawEllipse(QPointF(0, 0), r, r);
    }

    // Draw axes
    painter.setPen(QPen(Qt::lightGray, 1));
    painter.drawLine(QPointF(-radius, 0), QPointF(radius, 0));
    painter.drawLine(QPointF(0, -radius), QPointF(0, radius));

    // Draw fading trace
    qreal now = lastUpdateTime;
    if (trace.size() >= 2) {
        for (int i = 1; i < trace.size(); ++i) {
            const TracePoint &prev = trace[i - 1];
            const TracePoint &curr = trace[i];

            qreal age = now - curr.timestamp;
            if (age > 2.0) continue;

            qreal alpha = 1.0 - (age / 2.0);
            QColor traceColor = Qt::blue;
            traceColor.setAlphaF(alpha);

            float x1 = prev.ax / maxG * radius;
            float y1 = -prev.ay / maxG * radius;
            float x2 = curr.ax / maxG * radius;
            float y2 = -curr.ay / maxG * radius;

            QPen tracePen(traceColor, 1.0); // Thin line
            painter.setPen(tracePen);
            painter.drawLine(QPointF(x1, y1), QPointF(x2, y2));
        }
    }

    // Draw origin
    painter.setBrush(Qt::white);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QPointF(0, 0), 3, 3);

    // Draw current G-point
    float gx = accX / maxG * radius;
    float gy = -accY / maxG * radius;

    QColor dotColor = Qt::red;
    float gMag = std::hypot(accX, accY);
    if (gMag > 1.5) dotColor = Qt::yellow;
    if (gMag > 2.5) dotColor = Qt::magenta;

    painter.setBrush(dotColor);
    painter.drawEllipse(QPointF(gx, gy), 10, 10);
}
