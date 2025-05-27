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
#include "hexagon.h"
#include "ImuGForce.h"
#include "ImuErrorPlotWidget.h"

/**
 * @class MainWindow
 * @brief Main application window for IMU visualization and platform control.
 *
 * This class provides the user interface and logic to manage serial communication
 * with IMU devices, display their orientation and sensor readings, and visualize
 * calculated errors and gravity forces in a Qt-based GUI.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the main window and initializes UI components and serial port handling.
     * @param parent Pointer to the parent QWidget, if any.
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destructor. Closes serial connection if open and cleans up resources.
     */
    ~MainWindow();

private slots:
    /**
     * @brief Refreshes the list of available COM ports in the dropdown.
     *
     * Triggered when the refresh button is clicked. Scans and updates available serial ports.
     */
    void refreshPorts();

    /**
     * @brief Connects or disconnects from the selected serial port.
     *
     * Triggered when the connect button is clicked. Opens or closes the serial port connection
     * and updates the connection status in the UI.
     */
    void toggleConnection();

    /**
     * @brief Handles incoming serial data.
     *
     * Continuously reads, verifies, and parses incoming IMU or servo data from the serial port.
     * Also dispatches updates to the UI components accordingly.
     */
    void readSerialData();

    /**
     * @brief Calculates CRC-8 checksum over a list of 16-bit values encoded in QByteArray.
     * @param data List of data fields to compute CRC over.
     * @return CRC-8 checksum value.
     *
     * Used for data integrity verification on incoming serial messages.
     */
    uint8_t calculateCrc8(const QList<QByteArray>& data);

private:
    /**
     * @brief Updates the label showing connection status.
     * @param connected True if serial port is open and connected, false otherwise.
     */
    void updateConnectionStatus(bool connected);

    QSerialPort *serial;              ///< Serial communication handler.
    QPushButton *refreshButton;       ///< Button to refresh the COM port list.
    QPushButton *connectButton;       ///< Button to initiate or end connection.
    QComboBox *portComboBox;          ///< Dropdown listing available COM ports.
    QLabel *statusLabel;              ///< Label displaying current connection state.

    PlatformViewer *platformViewer;   ///< Widget displaying 3D orientation of the IMU platform.
    IMUDisplay *imu1Display;          ///< Widget for showing IMU 1 sensor data.
    IMUDisplay *imu2Display;          ///< Widget for showing IMU 2 sensor data.
    ImuGForceWidget *gForceWidget;    ///< Widget visualizing gravity force from IMU readings.
    HexagonBars *hexagonBars;         ///< Widget showing servo angles as hexagonal bars.
    ImuErrorPlotWidget *errorPlotWidget; ///< Widget for plotting differences between IMUs.

    /**
     * @struct ImuData
     * @brief Structure for storing parsed IMU values.
     */
    struct ImuData {
        float ax, ay, az; ///< Linear acceleration (m/s² or g, depending on scaling).
        float gx, gy, gz; ///< Angular velocity (rad/s).
        bool valid = false; ///< True if current data is valid and can be used.
    };

    ImuData imu1; ///< Data for IMU 1.
    ImuData imu2; ///< Data for IMU 2.

signals:
    /**
     * @brief Emitted after processing data from serial input.
     * @param intValue Integer field parsed from the data stream.
     * @param floatValue Float field parsed from the data stream.
     *
     * Can be connected to other widgets or processing units that depend on updated IMU data.
     */
    void dataProcessed(int intValue, float floatValue);
};

#endif // MAINWINDOW_H
