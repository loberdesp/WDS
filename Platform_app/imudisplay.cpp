#include "imudisplay.h"
#include <QFont>
#include <QFrame>
#include <QGridLayout>
#include <QLabel>

// Konstruktor klasy IMUDisplay
IMUDisplay::IMUDisplay(bool id, QWidget *parent) : QWidget(parent) {
    QGridLayout *layout = new QGridLayout(this);
    layout->setSpacing(5);
    layout->setContentsMargins(10, 10, 10, 10);

    Title = tr("IMU Data ");
    id_str = id ? "2" : "1";
    header = new QLabel(Title + id_str);
    QFont headerFont = header->font();
    headerFont.setBold(true);
    headerFont.setPointSize(12);
    header->setFont(headerFont);
    layout->addWidget(header, 0, 0, 1, 3, Qt::AlignCenter);

    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    layout->addWidget(line, 1, 0, 1, 3);

    setupAxisDisplay(layout, tr("Accel X"), 2);
    setupAxisDisplay(layout, tr("Accel Y"), 3);
    setupAxisDisplay(layout, tr("Accel Z"), 4);

    line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    layout->addWidget(line, 5, 0, 1, 3);

    setupAxisDisplay(layout, tr("Gyro X"), 6);
    setupAxisDisplay(layout, tr("Gyro Y"), 7);
    setupAxisDisplay(layout, tr("Gyro Z"), 8);

    this->setStyleSheet(
        "IMUDisplay {"
        "   background-color: #404040;"
        "}"
        "QLabel {"
        "   color: white;"
        "}"
        "QLabel[accessibleName='valueLabel'] {"
        "   background-color: #353535;"
        "   border: 1px solid #555555;"
        "   border-radius: 4px;"
        "   padding: 4px;"
        "   min-width: 50px;"
        "   font-family: 'Courier New', monospace;"
        "   color: black;"
        "}"
        );
}

QLabel *IMUDisplay::createValueLabel() {
    QLabel *label = new QLabel("0.00");
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    label->setMinimumWidth(50);
    label->setFrameStyle(QFrame::Box | QFrame::Plain);
    label->setStyleSheet("QLabel { background-color: grey; padding: 3px; border: 1px solid black }");
    return label;
}

void IMUDisplay::setupAxisDisplay(QGridLayout *layout, const QString &name, int row) {
    AxisDisplay *display = nullptr;
    bool isAccel = name.startsWith(tr("Accel"));

    if (isAccel) {
        if (name.endsWith("X")) display = &m_accelX;
        else if (name.endsWith("Y")) display = &m_accelY;
        else display = &m_accelZ;
    } else {
        if (name.endsWith("X")) display = &m_gyroX;
        else if (name.endsWith("Y")) display = &m_gyroY;
        else display = &m_gyroZ;
    }

    display->nameLabel = new QLabel(name);
    display->valueLabel = createValueLabel();
    display->valueLabel->setText("0.00");

    display->nameLabel->setAlignment(Qt::AlignCenter);
    display->nameLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    layout->addWidget(display->nameLabel, row, 0);
    layout->addWidget(display->valueLabel, row, 1);

    QLabel *unit = new QLabel(name.startsWith(tr("Accel")) ? "m/s²" : "°/s");
    layout->addWidget(unit, row, 2);
}

void IMUDisplay::updateValues(float ax, float ay, float az, float gx, float gy, float gz) {
    m_accelX.valueLabel->setText(QString::number(ax, 'f', 2));
    m_accelY.valueLabel->setText(QString::number(ay, 'f', 2));
    m_accelZ.valueLabel->setText(QString::number(az, 'f', 2));
    m_gyroX.valueLabel->setText(QString::number(gx, 'f', 2));
    m_gyroY.valueLabel->setText(QString::number(gy, 'f', 2));
    m_gyroZ.valueLabel->setText(QString::number(gz, 'f', 2));

    this->update();
}

void IMUDisplay::retranslateUi() {
    Title = tr("IMU Data ");
    header->setText(Title + id_str);

    m_accelX.nameLabel->setText(tr("Accel X"));
    m_accelY.nameLabel->setText(tr("Accel Y"));
    m_accelZ.nameLabel->setText(tr("Accel Z"));

    m_gyroX.nameLabel->setText(tr("Gyro X"));
    m_gyroY.nameLabel->setText(tr("Gyro Y"));
    m_gyroZ.nameLabel->setText(tr("Gyro Z"));
}
