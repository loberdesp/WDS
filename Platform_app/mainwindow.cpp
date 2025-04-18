#include "mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QMessageBox>
#include <QDebug>
#include <QByteArray>
#include <QApplication>  // Add this at the top with other includes
#include <QtGlobal>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    serial(new QSerialPort(this)),
    platformViewer(new PlatformViewer())
{
    // Create central widget
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // 1. Control panel
    QWidget *controlPanel = new QWidget();
    QHBoxLayout *controlLayout = new QHBoxLayout(controlPanel);

    // Create controls
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



    // Add to control layout
    controlLayout->addWidget(refreshButton);
    controlLayout->addWidget(portComboBox);
    controlLayout->addWidget(connectButton);
    controlLayout->addWidget(statusLabel);
    controlLayout->addStretch();

    // 2. Bordered 3D view
    QFrame *modelFrame = new QFrame();
    modelFrame->setFrameStyle(QFrame::Box | QFrame::Raised);
    modelFrame->setLineWidth(2);
    modelFrame->setFixedSize(520, 420);

    QVBoxLayout *frameLayout = new QVBoxLayout(modelFrame);
    frameLayout->addWidget(platformViewer);

    //imu data display
    imuDisplay = new IMUDisplay();

    // 3. Add to main layout
    mainLayout->addWidget(controlPanel);
    mainLayout->addWidget(modelFrame, 0, Qt::AlignHCenter);
    mainLayout->addStretch();
    mainLayout->addWidget(imuDisplay);

    // Final setup
    setCentralWidget(centralWidget);
    resize(800, 700);

    // Initial refresh
    refreshPorts();
    updateConnectionStatus(false);

    // Connections
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

    // Process complete lines (ending with \r\n or \n)
    while (true) {
        const int lineEnd = buffer.indexOf('\n');
        if (lineEnd == -1) break;  // No complete line yet

        // Extract line and remove from buffer
        QByteArray line = buffer.left(lineEnd).trimmed();
        buffer.remove(0, lineEnd + 1);

        if (line.isEmpty()) continue;

        // Parse IMU data with CRC verification
        if (line.startsWith("IMU:") && line.contains('*')) {
            const int crcPos = line.lastIndexOf('*');
            const QByteArray dataPart = line.mid(4, crcPos - 4);  // Skip "IMU:"
            const QByteArray crcPart = line.mid(crcPos + 1);

            // Verify CRC
            bool crcOk;
            const uint8_t receivedCrc = crcPart.toUInt(&crcOk, 16);
            if (!crcOk || crcPart.length() != 2) {
                qWarning() << "Invalid CRC format:" << line;
                continue;
            }

            // Parse data values
            const QList<QByteArray> values = dataPart.split(',');
            if (values.size() != 6) {
                qWarning() << "Invalid data field count:" << line;
                continue;
            }

            const uint8_t calculatedCrc = calculateCrc8(values);
            if (receivedCrc != calculatedCrc) {
                qWarning() << "CRC mismatch. Received:" << receivedCrc
                           << "Calculated:" << calculatedCrc;
                continue;
            }

            bool conversionOk[6];
            const int16_t fax = values[0].toInt(&conversionOk[0]);
            const int16_t fay = values[1].toInt(&conversionOk[1]);
            const int16_t faz = values[2].toInt(&conversionOk[2]);
            const int16_t fgx = values[3].toInt(&conversionOk[3]);
            const int16_t fgy = values[4].toInt(&conversionOk[4]);
            const int16_t fgz = values[5].toInt(&conversionOk[5]);

            if (!conversionOk[0] || !conversionOk[1] || !conversionOk[2] ||
                !conversionOk[3] || !conversionOk[4] || !conversionOk[5]) {
                qWarning() << "Invalid IMU data conversion:" << line;
                continue;
            }

            // Update UI thread-safely
            QMetaObject::invokeMethod(this, [this, fax, fay, faz, fgx, fgy, fgz]() {
                imuDisplay->updateValues(fax, fay, faz, fgx, fgy, fgz);
                platformViewer->updatePlatformOrientation(fax, fay, faz);
            }, Qt::QueuedConnection);

        } else {
            qDebug() << "Received non-IMU data:" << line;
        }
    }
}

uint8_t MainWindow::calculateCrc8(const QList<QByteArray>& data) {
    uint8_t crc = 0xFF;  // Initial value
    const uint8_t poly = 0x31;  // Polynomial 0x31 (CRC-8-Dallas/Maxim)

    // Convert each int16 value to little-endian bytes
    for (const QByteArray& valStr : data) {
        bool ok;
        int16_t value = valStr.toInt(&ok);
        if (!ok) {
            qWarning() << "Invalid int16 value:" << valStr;
            return 0;
        }

        // Get little-endian bytes
        uint8_t lsb = static_cast<uint8_t>(value & 0xFF);
        uint8_t msb = static_cast<uint8_t>((value >> 8) & 0xFF);

        // Process both bytes
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
