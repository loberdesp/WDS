// imudisplay.cpp
#include "imudisplay.h"
#include <QFont>
#include <QFrame>

IMUDisplay::IMUDisplay(bool id, QWidget *parent) : QWidget(parent) {
    QGridLayout *layout = new QGridLayout(this);
    layout->setSpacing(5);
    layout->setContentsMargins(10, 10, 10, 10);

    // Create header
    const QString Title = "IMU Data ";
    const QString id_str = id ? "2" : "1";
    QLabel *header = new QLabel(Title+id_str);
    QFont headerFont = header->font();
    headerFont.setBold(true);
    headerFont.setPointSize(12);
    header->setFont(headerFont);
    layout->addWidget(header, 0, 0, 1, 3, Qt::AlignCenter);

    // Add separator
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    layout->addWidget(line, 1, 0, 1, 3);

    // Setup accelerometer displays
    setupAxisDisplay(layout, "Accel X", 2);
    setupAxisDisplay(layout, "Accel Y", 3);
    setupAxisDisplay(layout, "Accel Z", 4);

    // Add separator
    line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    layout->addWidget(line, 5, 0, 1, 3);

    // Setup gyroscope displays
    setupAxisDisplay(layout, "Gyro X", 6);
    setupAxisDisplay(layout, "Gyro Y", 7);
    setupAxisDisplay(layout, "Gyro Z", 8);

    this->setStyleSheet(
        "IMUDisplay {"
        "   background-color: #404040;"  // Dark background for whole widget
        "}"
        "QLabel {"
        "   color: white;"  // All labels will have black text
        "}"
        "QLabel[accessibleName='valueLabel'] {"
        "   background-color: #353535;"
        "   border: 1px solid #555555;"
        "   border-radius: 4px;"
        "   padding: 4px;"
        "   min-width: 50px;"
        "   font-family: 'Courier New', monospace;"
        "   color: black;"  // Ensure value labels are black
        "}"
        );
}

QLabel *IMUDisplay::createValueLabel() {
    QLabel *label = new QLabel("0.00");
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    label->setMinimumWidth(50);  // Fixed width to prevent shifting
    label->setFrameStyle(QFrame::Box | QFrame::Plain);
    label->setStyleSheet("QLabel { background-color: grey; padding: 3px; border: 1px solid black }");
    return label;
}

void IMUDisplay::setupAxisDisplay(QGridLayout *layout, const QString &name, int row) {
    AxisDisplay *display = nullptr;  // Initialize pointer

    if (name.startsWith("Accel")) {
        if (name.endsWith("X")) display = &m_accelX;
        else if (name.endsWith("Y")) display = &m_accelY;
        else display = &m_accelZ;
    } else {
        if (name.endsWith("X")) display = &m_gyroX;
        else if (name.endsWith("Y")) display = &m_gyroY;
        else display = &m_gyroZ;
    }

    // Initialize the struct members
    display->nameLabel = new QLabel(name);
    display->valueLabel = createValueLabel();
    display->valueLabel->setText("0.00");  // Initialize with default value

    layout->addWidget(display->nameLabel, row, 0);
    layout->addWidget(display->valueLabel, row, 1);

    QLabel *unit = new QLabel(name.startsWith("Accel") ? "m/s²" : "°/s");
    layout->addWidget(unit, row, 2);
}

void IMUDisplay::updateValues(float ax, float ay, float az, float gx, float gy, float gz) {
    // qDebug() << "Updating values:" << ax << ay << az << gx << gy << gz;  // Add this line

    m_accelX.valueLabel->setText(QString::number(ax, 'f', 2));
    m_accelY.valueLabel->setText(QString::number(ay, 'f', 2));
    m_accelZ.valueLabel->setText(QString::number(az, 'f', 2));

    m_gyroX.valueLabel->setText(QString::number(gx, 'f', 2));
    m_gyroY.valueLabel->setText(QString::number(gy, 'f', 2));
    m_gyroZ.valueLabel->setText(QString::number(gz, 'f', 2));

    // Force immediate update
    this->update();
}
