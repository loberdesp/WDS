#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include "platformviewer.h"
#include "imudisplay.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void refreshPorts();
    void toggleConnection();
    void readSerialData();
    uint8_t calculateCrc8(const QList<QByteArray>& data);

private:
    void updateConnectionStatus(bool connected);

    QSerialPort *serial;
    QPushButton *refreshButton;
    QPushButton *connectButton;
    QComboBox *portComboBox;
    QLabel *statusLabel;
    PlatformViewer *platformViewer;

    IMUDisplay *imuDisplay;  // Replace your old QLabels with this


signals:
    void dataProcessed(int intValue, float floatValue);
};

#endif // MAINWINDOW_H
