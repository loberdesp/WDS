/**
 * @file    mainwindow.cpp
 * @brief   Implementation of main application window
 * @author  Piotr Siembab
 * @date    18.04.2025
 */

#include "mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QMessageBox>
#include <QDebug>
#include <QByteArray>
#include <QApplication>
#include <QtGlobal>
#include <QTimer>
#include <cmath>

/**
 * @brief Constructs the main application window
 * @param parent Parent widget (optional)
 *
 * Initializes:
 * - Serial port interface
 * - 3D visualization widget
 * - IMU data display
 * - Control panel with port selection
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    serial(new QSerialPort(this)),
    platformViewer(new PlatformViewer())
{
    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // === LEFT SIDE: Control Panel + Platform Viewer ===
    QVBoxLayout *leftLayout = new QVBoxLayout();

    // 1. Control panel
    QWidget *controlPanel = new QWidget();
    QHBoxLayout *controlLayout = new QHBoxLayout(controlPanel);

    refreshButton = new QPushButton("Ports ▼");
    refreshButton->setFixedWidth(80);
    refreshButton->setToolTip("Refresh available COM ports");

    portComboBox = new QComboBox();
    portComboBox->setMinimumWidth(150);

    connectButton = new QPushButton("Connect");
    connectButton->setFixedWidth(100);
    connectButton->setToolTip("Connect to selected COM port");

    statusLabel = new QLabel("Status: Disconnected");
    statusLabel->setStyleSheet("QLabel { color: red; font-weight: bold; }");

    controlLayout->addWidget(refreshButton);
    controlLayout->addWidget(portComboBox);
    controlLayout->addWidget(connectButton);
    controlLayout->addWidget(statusLabel);
    controlLayout->addStretch();

    leftLayout->addWidget(controlPanel, 0);

    // 2. Platform viewer inside a frame
    QFrame *modelFrame = new QFrame();
    modelFrame->setFrameStyle(QFrame::Box | QFrame::Raised);
    modelFrame->setLineWidth(2);
    modelFrame->setMinimumSize(300, 240);  // Przykładowe minimum
    modelFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout *frameLayout = new QVBoxLayout(modelFrame);
    frameLayout->addWidget(platformViewer);

    leftLayout->addWidget(modelFrame, 2);

    errorPlotWidget = new ImuErrorPlotWidget();
    leftLayout->addWidget(errorPlotWidget, 2);

    leftLayout->addStretch();

    // === RIGHT SIDE: HexagonBars + IMU Display ===
    QVBoxLayout *rightLayout = new QVBoxLayout();

    hexagonBars = new HexagonBars();
    hexagonBars->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // QTimer* timer = new QTimer(this);
    // int timeStep = 0;

    // connect(timer, &QTimer::timeout, this, [=]() mutable {
    //     for (int i = 0; i < 6; ++i) {
    //         float value = 0.5f + 0.4f * std::sin((timeStep + i * 20) * 0.05);  // Smooth cycling
    //         hexagonBars->setBarValue(i, value);
    //     }
    //     timeStep++;
    // });
    // timer->start(30);  // Approx ~33 FPS

    gForceWidget = new ImuGForceWidget();
    gForceWidget->setAcceleration(0, 0);

    // Create one QFrame to hold both IMU displays
    QFrame* imuFrame = new QFrame;
    imuFrame->setFrameShape(QFrame::Box);
    imuFrame->setLineWidth(1);
    imuFrame->setMidLineWidth(0);
    imuFrame->setContentsMargins(2, 2, 2, 2);  // Outer margins around the frame
    // Apply a darker border using a style sheet
    imuFrame->setStyleSheet(
        "QFrame { "
        "  border: 1px solid #444444; "   // Dark gray border
        "  border-radius: 4px; "          // Optional: rounded corners
        "}"
        );

    // Create a layout inside the frame
    QVBoxLayout* imuLayout = new QVBoxLayout(imuFrame);
    imuLayout->setContentsMargins(4, 4, 4, 4); // Inner padding inside the frame
    imuLayout->setSpacing(4);                 // Space between imu1 and imu2

    // Create the IMU displays
    imu1Display = new IMUDisplay(0);
    imu2Display = new IMUDisplay(1);


    // Add displays to layout inside the frame
    imuLayout->addWidget(imu1Display);
    imuLayout->addWidget(imu2Display);

    // Add the framed widget to your main layout
    QHBoxLayout* sensorLayout = new QHBoxLayout();
    sensorLayout->addWidget(hexagonBars);
    sensorLayout->addWidget(gForceWidget);

    rightLayout->addWidget(imuFrame);  // ← Now both are inside one frame

    rightLayout->addLayout(sensorLayout);

    rightLayout->addStretch();


    // Combine into main layout
    mainLayout->addLayout(leftLayout, 3);
    mainLayout->addLayout(rightLayout, 2);

    setCentralWidget(centralWidget);
    resize(800, 700);

    refreshPorts();
    updateConnectionStatus(false);

    connect(refreshButton, &QPushButton::clicked, this, &MainWindow::refreshPorts);
    connect(connectButton, &QPushButton::clicked, this, &MainWindow::toggleConnection);
    connect(serial, &QSerialPort::readyRead, this, &MainWindow::readSerialData, Qt::QueuedConnection);
    connect(this, &MainWindow::dataProcessed, this, [this](int i, float f) {
        qDebug() << "Data received - Int:" << i << "Float:" << f;
    });
}

void MainWindow::readSerialData() {
    static QByteArray buffer;
    buffer += serial->readAll();

    while (true) {
        const int lineEnd = buffer.indexOf('\n');
        if (lineEnd == -1) break;

        QByteArray line = buffer.left(lineEnd).trimmed();
        buffer.remove(0, lineEnd + 1);
        if (line.isEmpty()) continue;

        if (line.startsWith("IMU:") && line.contains('*')) {
            const int crcPos = line.lastIndexOf('*');
            const QByteArray dataPart = line.mid(4, crcPos - 4);
            const QByteArray crcPart = line.mid(crcPos + 1);

            bool crcOk;
            const uint8_t receivedCrc = crcPart.toUInt(&crcOk, 16);
            if (!crcOk || crcPart.length() != 2) {
                qWarning() << "Invalid CRC format:" << line;
                continue;
            }

            const QList<QByteArray> values = dataPart.split(',');
            if (values.size() != 7) {
                qWarning() << "Invalid data field count:" << line;
                continue;
            }

            const uint8_t calculatedCrc = calculateCrc8(values.mid(1, 6));
            if (receivedCrc != calculatedCrc) {
                qWarning() << "CRC mismatch. Received:" << receivedCrc
                           << "Calculated:" << calculatedCrc;
                continue;
            }

            bool conversionOk[7];
            const int imuId = values[0].toInt(&conversionOk[0]);
            const int16_t fax = values[1].toInt(&conversionOk[1]);
            const int16_t fay = values[2].toInt(&conversionOk[2]);
            const int16_t faz = values[3].toInt(&conversionOk[3]);
            const int16_t fgx = values[4].toInt(&conversionOk[4]);
            const int16_t fgy = values[5].toInt(&conversionOk[5]);
            const int16_t fgz = values[6].toInt(&conversionOk[6]);

            if (!std::all_of(std::begin(conversionOk), std::end(conversionOk), [](bool ok) { return ok; })) {
                qWarning() << "Invalid IMU data conversion:" << line;
                continue;
            }

            QMetaObject::invokeMethod(this, [=]() {
                if (imuId == 1) {

                    platformViewer->updatePlatformOrientation(fax, fay, faz);
                    imu1.ax = fax*0.000565;
                    imu1.ay = fay*0.000565;
                    imu1.az = faz*0.000565;
                    imu1.gx = fgx/65.5f*M_PI/180.0f;
                    imu1.gy = fgy/65.5f*M_PI/180.0f;
                    imu1.gz = fgz/65.5f*M_PI/180.0f;
                    if(!imu1.valid) imu1.valid=1;
                    imu1Display->updateValues(imu1.ax, imu1.ay, imu1.az, imu1.gx, imu1.gy, imu1.gz);

                    float gX = static_cast<float>(fax) / 16390.0f;
                    float gY = static_cast<float>(fay) / 16390.0f;

                    gForceWidget->setAcceleration(gX, gY);
                } else if (imuId == 2) {
                    imu2.ax = fax*0.000565;
                    imu2.ay = fay*0.000565;
                    imu2.az = faz*0.000565;
                    imu2.gx = fgx/65.5f*M_PI/180.0f;
                    imu2.gy = fgy/65.5f*M_PI/180.0f;
                    imu2.gz = fgz/65.5f*M_PI/180.0f;
                    imu2Display->updateValues(imu2.ax, imu2.ay, imu2.az, imu2.gx, imu2.gy, imu2.gz);
                    if(!imu2.valid) imu2.valid=1;

                } else {
                    qWarning() << "Unknown IMU ID:" << imuId;
                }

                // Jeśli dane obu IMU są gotowe:
                if (imu1.valid && imu2.valid) {

                    const float dax = imu1.ax - imu2.ax;
                    const float day = imu1.ay - imu2.ay;
                    const float daz = imu1.az - imu2.az;
                    const float dgx = imu1.gx - imu2.gx;
                    const float dgy = imu1.gy - imu2.gy;
                    const int dgz = imu1.gz - imu2.gz;

                    errorPlotWidget->addErrorSample(dax, day, daz, dgx, dgy, dgz);
                }
            }, Qt::QueuedConnection);
        } else if (line.startsWith("S:") && line.contains('*')) {
                const int crcPos = line.lastIndexOf('*');
                const QByteArray dataPart = line.mid(2, crcPos - 2);  // Skip "S:"
                const QByteArray crcPart = line.mid(crcPos + 1);

                bool crcOk;
                const uint8_t receivedCrc = crcPart.toUInt(&crcOk, 16);
                if (!crcOk || crcPart.length() != 2) {
                    qWarning() << "Invalid CRC format in servo line:" << line;
                    continue;
                }

                const QList<QByteArray> values = dataPart.split(',');
                if (values.size() != 6) {
                    qWarning() << "Invalid servo data field count:" << line;
                    continue;
                }

                uint8_t calculatedCrc = calculateCrc8(values);  // reuse your CRC-8 function
                if (receivedCrc != calculatedCrc) {
                    qWarning() << "Servo CRC mismatch. Received:" << receivedCrc
                               << "Calculated:" << calculatedCrc;
                    continue;
                }

                bool conversionOk[6];
                QVector<int> servoAngles(6);
                for (int i = 0; i < 6; ++i) {
                    servoAngles[i] = values[i].toInt(&conversionOk[i]);
                }

                if (!std::all_of(std::begin(conversionOk), std::end(conversionOk), [](bool ok) { return ok; })) {
                    qWarning() << "Invalid servo angle conversion:" << line;
                    continue;
                }

                // Safely dispatch update to GUI
                QMetaObject::invokeMethod(this, [=]() {
                    for(int i=0;i<6;i++) {
                        hexagonBars->setBarValue(i, (servoAngles[i]+90.0f)/180.0f);
                    }
                }, Qt::QueuedConnection);
            }

            else {
                qDebug() << "Received unrecognized data:" << line;
            }
    }
}

uint8_t MainWindow::calculateCrc8(const QList<QByteArray>& data) {
    uint8_t crc = 0xFF;
    const uint8_t poly = 0x31;

    for (const QByteArray& valStr : data) {
        bool ok;
        int16_t value = valStr.toInt(&ok);
        if (!ok) {
            qWarning() << "Invalid int16 value:" << valStr;
            return 0;
        }

        uint8_t lsb = static_cast<uint8_t>(value & 0xFF);
        uint8_t msb = static_cast<uint8_t>((value >> 8) & 0xFF);

        for (uint8_t byte : {lsb, msb}) {
            crc ^= byte;
            for (uint8_t j = 0; j < 8; j++) {
                crc = (crc & 0x80) ? ((crc << 1) ^ poly) : (crc << 1);
            }
        }
    }
    return crc;
}

void MainWindow::refreshPorts()
{
    portComboBox->clear();
    const auto ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &port : ports) {
        portComboBox->addItem(port.portName());
    }
}

void MainWindow::toggleConnection()
{
    if (serial->isOpen()) {
        serial->close();
        updateConnectionStatus(false);
        connectButton->setText("Connect");
        return;
    }

    QString portName = portComboBox->currentText();
    if (portName.isEmpty()) {
        QMessageBox::warning(this, "Error", "No port selected!");
        return;
    }

    serial->setPortName(portName);
    serial->setBaudRate(QSerialPort::Baud115200);

    if (serial->open(QIODevice::ReadWrite)) {
        updateConnectionStatus(true);
        connectButton->setText("Disconnect");
    } else {
        QMessageBox::critical(this, "Error", "Failed to open port: " + serial->errorString());
    }
}

void MainWindow::updateConnectionStatus(bool connected)
{
    if (connected) {
        statusLabel->setText("✓ Connected to " + portComboBox->currentText());
        statusLabel->setStyleSheet("QLabel { color: green; font-weight: bold; }");
    } else {
        statusLabel->setText("✗ Disconnected");
        statusLabel->setStyleSheet("QLabel { color: red; font-weight: bold; }");
    }
}

MainWindow::~MainWindow()
{
    if (serial->isOpen()) {
        serial->close();
    }
}
