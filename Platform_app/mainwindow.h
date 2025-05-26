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
 * @brief Main window for managing connections and controlling the 3D platform and IMU display.
 *
 * This class serves as the user interface for connecting to the IMU hardware via serial ports,
 * displaying the platform orientation, and visualizing IMU data such as gravity force and sensor readings.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the main window and initializes the necessary components.
     * @param parent The parent widget, or nullptr if none.
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destructor for cleaning up resources.
     */
    ~MainWindow();

private slots:
    /**
     * @brief Refreshes the list of available serial ports.
     *
     * Called when the "Refresh" button is clicked to update the available COM port options for connection.
     */
    void refreshPorts();

    /**
     * @brief Toggles the connection state to the IMU hardware.
     *
     * Called when the "Connect" button is clicked. Establishes or closes the connection to the serial port.
     */
    void toggleConnection();

    /**
     * @brief Reads data from the serial port.
     *
     * Continuously reads incoming data from the connected serial port and processes it as necessary.
     */
    void readSerialData();

    /**
     * @brief Calculates the CRC-8 checksum for data verification.
     * @param data A list of QByteArrays representing the data packets to be checked.
     * @return The calculated CRC-8 value.
     *
     * This method is used to ensure data integrity when reading from the IMU over serial communication.
     */
    uint8_t calculateCrc8(const QList<QByteArray>& data);

private:
    /**
     * @brief Updates the connection status displayed in the UI.
     * @param connected True if the connection is established, false if disconnected.
     *
     * This function updates the status label to reflect whether the application is connected to the IMU hardware.
     */
    void updateConnectionStatus(bool connected);

    QSerialPort *serial;           ///< Serial port object used for communication with the IMU.
    QPushButton *refreshButton;    ///< Button to refresh available serial ports.
    QPushButton *connectButton;    ///< Button to toggle the connection state.
    QComboBox *portComboBox;       ///< ComboBox displaying available serial ports.
    QLabel *statusLabel;           ///< Label displaying the current connection status.

    PlatformViewer *platformViewer; ///< 3D platform viewer for visualizing the platform's orientation.
    IMUDisplay *imuDisplay;         ///< Widget for displaying real-time IMU sensor readings.
    ImuGForceWidget *gForceWidget;  ///< Widget for visualizing the gravity forces acting on the platform.
    HexagonBars *hexagonBars;      ///< Visual component displaying hexagonal bars to represent the data.

    ImuErrorPlotWidget *errorPlotWidget;
    struct ImuData {
        float ax, ay, az;
        float gx, gy, gz;
        bool valid = false;
    };
    ImuData imu1, imu2;

signals:
    /**
     * @brief Signal emitted when data is processed from the serial port.
     * @param intValue The integer value processed from the incoming data.
     * @param floatValue The float value processed from the incoming data.
     *
     * This signal allows other components to react to the processed data, such as updating the platform's state.
     */
    void dataProcessed(int intValue, float floatValue);
};

#endif // MAINWINDOW_H
