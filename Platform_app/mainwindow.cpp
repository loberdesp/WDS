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
#include <QPushButton>
#include <QDir>
#include <QApplication>


// Konstruktor glownego okna
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    serial(new QSerialPort(this)),                  // Inicjalizacja portu szeregowego
    platformViewer(new PlatformViewer())            // Widget do wizualizacji platformy 3D
{

    // Załaduj domyślny język
    translator.load("app_pl.qm", QDir::currentPath() + "/translations");
    qApp->installTranslator(&translator);

    // Glowny widget i layout
    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // Lewa kolumna z panelami
    QVBoxLayout *leftLayout = new QVBoxLayout();

    // Panel kontrolny
    QWidget *controlPanel = new QWidget();
    QHBoxLayout *controlLayout = new QHBoxLayout(controlPanel);

    languageButton = new QPushButton(tr("🇬🇧 EN"), this);
    connect(languageButton, &QPushButton::clicked, this, &MainWindow::switchLanguage);
    languageButton->setFixedWidth(50);

    // Przycisk odswiezania portow
    refreshButton = new QPushButton();
    refreshButton->setText(tr("Refresh Ports"));
    refreshButton->setFixedWidth(120);

    // Lista portow COM
    portComboBox = new QComboBox();
    portComboBox->setMinimumWidth(150);

    // Przycisk polaczenia
    connectButton = new QPushButton(tr("Connect"), this);
    connectButton->setFixedWidth(100);

    // Etykieta statusu
    statusLabel = new QLabel(tr("Status: Disconnected"));
    statusLabel->setStyleSheet("QLabel { color: red; font-weight: bold; }");

    // Dodanie elementow do panelu
    controlLayout->addWidget(languageButton);
    controlLayout->addWidget(refreshButton);
    controlLayout->addWidget(portComboBox);
    controlLayout->addWidget(connectButton);
    controlLayout->addWidget(statusLabel);
    controlLayout->addStretch();

    leftLayout->addWidget(controlPanel, 0);

    // Ramka z platforma 3D
    QFrame *modelFrame = new QFrame();
    modelFrame->setFrameStyle(QFrame::Box | QFrame::Raised);
    modelFrame->setLineWidth(2);
    modelFrame->setMinimumSize(300, 240);

    QVBoxLayout *frameLayout = new QVBoxLayout(modelFrame);
    frameLayout->addWidget(platformViewer);
    leftLayout->addWidget(modelFrame, 2);

    // Wykres bledow IMU
    errorPlotWidget = new ImuErrorPlotWidget();
    leftLayout->addWidget(errorPlotWidget, 2);

    leftLayout->addStretch();

    // Prawa kolumna z danymi
    QVBoxLayout *rightLayout = new QVBoxLayout();

    // Paski serw
    hexagonBars = new HexagonBars();

    // Wizualizacja przyspieszen
    gForceWidget = new ImuGForceWidget();
    gForceWidget->setAcceleration(0, 0);

    // Ramka z wyswietlaczami IMU
    QFrame* imuFrame = new QFrame;
    imuFrame->setFrameShape(QFrame::Box);
    imuFrame->setStyleSheet("QFrame { border: 1px solid #444444; border-radius: 4px; }");

    QVBoxLayout* imuLayout = new QVBoxLayout(imuFrame);
    imu1Display = new IMUDisplay(0);  // IMU nr 1
    imu2Display = new IMUDisplay(1);  // IMU nr 2
    imuLayout->addWidget(imu1Display);
    imuLayout->addWidget(imu2Display);

    // Layout czujnikow
    QHBoxLayout* sensorLayout = new QHBoxLayout();
    sensorLayout->addWidget(hexagonBars);
    sensorLayout->addWidget(gForceWidget);

    rightLayout->addWidget(imuFrame);
    rightLayout->addLayout(sensorLayout);
    rightLayout->addStretch();

    // Polaczenie kolumn
    mainLayout->addLayout(leftLayout, 3);
    mainLayout->addLayout(rightLayout, 2);

    setCentralWidget(centralWidget);
    resize(800, 700);

    // Inicjalizacja portow
    refreshPorts();
    updateConnectionStatus(false);

    // Polaczenia sygnalow
    connect(refreshButton, &QPushButton::clicked, this, &MainWindow::refreshPorts);
    connect(connectButton, &QPushButton::clicked, this, &MainWindow::toggleConnection);
    connect(serial, &QSerialPort::readyRead, this, &MainWindow::readSerialData, Qt::QueuedConnection);

retranslateUi();

}

// Odczyt danych z portu szeregowego
void MainWindow::readSerialData() {
    static QByteArray buffer;
    buffer += serial->readAll();

    // Przetwarzanie linii danych
    while (true) {
        const int lineEnd = buffer.indexOf('\n');
        if (lineEnd == -1) break;

        QByteArray line = buffer.left(lineEnd).trimmed();
        buffer.remove(0, lineEnd + 1);
        if (line.isEmpty()) continue;

        // Przetwarzanie danych IMU
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
                qWarning() << "CRC mismatch. Received:" << receivedCrc << "Calculated:" << calculatedCrc;
                continue;
            }

            // Konwersja danych
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

            // Aktualizacja GUI
            QMetaObject::invokeMethod(this, [=]() {
                if (imuId == 1) {
                    // Aktualizacja platformy
                    platformViewer->updatePlatformOrientation(fax, fay, faz);

                    // Skalowanie danych IMU1
                    imu1.ax = fax*0.000565;
                    imu1.ay = fay*0.000565;
                    imu1.az = faz*0.000565;
                    imu1.gx = fgx/65.5f*M_PI/180.0f;
                    imu1.gy = fgy/65.5f*M_PI/180.0f;
                    imu1.gz = fgz/65.5f*M_PI/180.0f;
                    if(!imu1.valid) imu1.valid=1;

                    // Aktualizacja wyswietlacza
                    imu1Display->updateValues(imu1.ax, imu1.ay, imu1.az, imu1.gx, imu1.gy, imu1.gz);

                    // Przeliczenie przyspieszen
                    float gX = static_cast<float>(fax) / 16390.0f;
                    float gY = static_cast<float>(fay) / 16390.0f;
                    gForceWidget->setAcceleration(gX, gY);
                } else if (imuId == 2) {
                    // Dane IMU2
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

                // Obliczanie roznic miedzy IMU
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
        }
        else if (line.startsWith("S:") && line.contains('*')) {
            // Przetwarzanie danych serw
            const int crcPos = line.lastIndexOf('*');
            const QByteArray dataPart = line.mid(2, crcPos - 2);
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

            uint8_t calculatedCrc = calculateCrc8(values);
            if (receivedCrc != calculatedCrc) {
                qWarning() << "Servo CRC mismatch. Received:" << receivedCrc << "Calculated:" << calculatedCrc;
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

            // Aktualizacja paskow serw
            QMetaObject::invokeMethod(this, [=]() {
                for(int i=0;i<6;i++) {
                    // hexagonBars->setBarValue(i, (servoAngles[i]+90.0f)/180.0f);
                    hexagonBars->updateServoAngles(servoAngles);
                }
            }, Qt::QueuedConnection);
        } else {
            qDebug() << "Received unrecognized data:" << line;
        }
    }
}

// Obliczanie sumy kontrolnej CRC8
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

        // Obliczenia CRC
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

// Odswiezanie listy portow COM
void MainWindow::refreshPorts()
{
    portComboBox->clear();
    const auto ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &port : ports) {
        portComboBox->addItem(port.portName());
    }
}

// Polaczenie/rozlaczenie z portem
void MainWindow::toggleConnection()
{
    if (serial->isOpen()) {
        serial->close();
        updateConnectionStatus(false);
        connectButton->setText(tr("Connect"));
        return;
    }

    QString portName = portComboBox->currentText();
    if (portName.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("No port selected!"));
        return;
    }

    serial->setPortName(portName);
    serial->setBaudRate(QSerialPort::Baud115200);

    if (serial->open(QIODevice::ReadWrite)) {
        updateConnectionStatus(true);
        connectButton->setText(tr("Disconnect"));
    } else {
        QMessageBox::critical(this, QObject::tr("Error"), tr("Failed to open port: ") + serial->errorString());
    }
}

// Aktualizacja statusu polaczenia
void MainWindow::updateConnectionStatus(bool connected)
{
    if (connected) {
        statusLabel->setText(tr("\u2713 Connected to ") + portComboBox->currentText());
        statusLabel->setStyleSheet("QLabel { color: green; font-weight: bold; }");
    } else {
        statusLabel->setText(tr("\u2717 Disconnected"));
        statusLabel->setStyleSheet("QLabel { color: red; font-weight: bold; }");
    }
}

// Destruktor
MainWindow::~MainWindow()
{
    if (serial->isOpen()) {
        serial->close();
    }
}

void MainWindow::switchLanguage() {
    qApp->removeTranslator(&translator);

    QString qmFile;
    QString qmPath = QCoreApplication::applicationDirPath() + "/../../translations";

    if (currentLanguage == "pl") {
        qmFile = "app_en.qm";
        currentLanguage = "en";
    } else {
        qmFile = "app_pl.qm";
        currentLanguage = "pl";
    }

    QString fullPath = qmPath + "/" + qmFile;


    translator.load(qmFile, qmPath);
    qApp->installTranslator(&translator);

    retranslateUi();
}

void MainWindow::retranslateUi() {

    languageButton->setText(tr("🇬🇧 EN"));
    refreshButton->setText(tr("Refresh Ports"));

    // Ten przycisk ma tekst zależny od stanu połączenia
    connectButton->setText(serial->isOpen() ? tr("Disconnect") : tr("Connect"));

    updateConnectionStatus(serial->isOpen());

    platformViewer->retranslateUi();
    imu1Display->retranslateUi();
    imu2Display->retranslateUi();
    errorPlotWidget->retranslateUi();
}
