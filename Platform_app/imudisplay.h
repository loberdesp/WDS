#ifndef IMUDISPLAY_H
#define IMUDISPLAY_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>

class IMUDisplay : public QWidget {
    Q_OBJECT
public:
    explicit IMUDisplay(QWidget *parent = nullptr);
    void updateValues(float ax, float ay, float az, float gx, float gy, float gz);

private:
    QLabel *createValueLabel();
    void setupAxisDisplay(QGridLayout *layout, const QString &name, int row);

    struct AxisDisplay {
        QLabel *nameLabel;
        QLabel *valueLabel;
    };

    AxisDisplay m_accelX, m_accelY, m_accelZ;
    AxisDisplay m_gyroX, m_gyroY, m_gyroZ;
};

#endif // IMUDISPLAY_H
