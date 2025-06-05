/**
 * @file    mainwindow.h
 * @brief   Main application window declaration for IMU visualization system
 *
 * @details Defines the MainWindow class which serves as the central GUI component
 *          for interacting with IMU sensors, displaying their data, and visualizing
 *          platform orientation and servo positions.
 *
 * @author  Piotr Siembab
 * @date    18.04.2025
 * @version 1.1
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QApplication>
#include <QTranslator>
#include <QDir>

#include "platformviewer.h"
#include "imudisplay.h"
#include "hexagon.h"
#include "ImuGForce.h"
#include "ImuErrorPlotWidget.h"

/**
 * @class MainWindow
 * @brief Central widget managing IMU data visualization and serial communication
 *
 * @details The MainWindow class provides:
 *          - Serial port management for IMU communication
 *          - Real-time 3D visualization of platform orientation
 *          - Dual IMU data comparison and error plotting
 *          - Servo position visualization
 *          - G-force vector display
 *
 * The UI is organized with control elements on the left and visualization
 * widgets on the right, following a logical data flow from input to display.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the main application window
     * @param parent Optional parent widget (default: nullptr)
     *
     * @details Initializes all UI components and sets up:
     *          - Serial port interface
     *          - 3D visualization widget
     *          - IMU data displays
     *          - Control panel elements
     *          - Signal-slot connections
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destructor ensuring proper resource cleanup
     *
     * @details Automatically:
     *          - Closes serial port connection if open
     *          - Releases all dynamically allocated resources
     *          - Maintains Qt object hierarchy
     */
    ~MainWindow() override;

private slots:
    /**
     * @brief Refreshes available serial ports list
     *
     * @details Scans system for available COM ports and updates the port selection
     *          combo box. Triggered both at startup and by user request.
     */
    void refreshPorts();

    /**
     * @brief Switches the application language at runtime
     *
     * @details Loads appropriate .qm translation files and triggers UI retranslation.
     *          Supports toggling between multiple languages dynamically.
     */
    void switchLanguage();

    /**
     * @brief Toggles serial port connection state
     *
     * @details Manages the full connection lifecycle:
     *          - Opens port at 115200 baud when connecting
     *          - Properly closes port when disconnecting
     *          - Updates UI elements to reflect connection state
     *          - Handles connection errors gracefully
     */
    void toggleConnection();

    /**
     * @brief Processes incoming serial data
     *
     * @details Continuously reads and parses data from the serial port,
     *          supporting two message formats:
     *          1. IMU data packets (format: "IMU:<id>,<ax>,<ay>,<az>,<gx>,<gy>,<gz>*<crc>")
     *          2. Servo data packets (format: "S:<a1>,<a2>,<a3>,<a4>,<a5>,<a6>*<crc>")
     *
     * Validates CRC checksums and updates appropriate visualization widgets.
     */
    void readSerialData();

    /**
     * @brief Computes CRC-8 checksum for data validation
     * @param data List of data fields to checksum
     * @return Computed CRC-8 value
     *
     * @details Uses polynomial 0x31 (x^8 + x^5 + x^4 + 1) and processes
     *          both LSB and MSB of each 16-bit value. Essential for data
     *          integrity verification in serial communication.
     */
    uint8_t calculateCrc8(const QList<QByteArray>& data);

private:
    /**
     * @brief Updates connection status display
     * @param connected Current connection state (true = connected)
     *
     * @details Modifies both text and styling of the status label:
     *          - Green checkmark and port name when connected
     *          - Red cross when disconnected
     *          - Updates tooltips and accessibility features
     */
    void updateConnectionStatus(bool connected);

    // === Serial communication ===
    QSerialPort *serial;              ///< Handles low-level serial communication
    QPushButton *refreshButton;       ///< Triggers port list refresh (labeled "Ports ▼")
    QPushButton *languageButton;      ///< Toggles the application language
    QPushButton *connectButton;       ///< Toggles connection state (labeled "Connect"/"Disconnect")
    QComboBox *portComboBox;          ///< Dropdown list of available serial ports
    QLabel *statusLabel;              ///< Visual indicator of connection status

    // === Visualization widgets ===
    PlatformViewer *platformViewer;       ///< 3D renderer for platform orientation visualization
    IMUDisplay *imu1Display;              ///< Display widget for first IMU's sensor data
    IMUDisplay *imu2Display;              ///< Display widget for second IMU's sensor data
    ImuGForceWidget *gForceWidget;        ///< Gravity vector visualization component
    HexagonBars *hexagonBars;            ///< Hexagonal servo position indicator
    ImuErrorPlotWidget *errorPlotWidget; ///< Difference plot between two IMUs

    /**
     * @struct ImuData
     * @brief Container for processed IMU sensor values
     */
    struct ImuData {
        float ax;        ///< X-axis acceleration (scaled to m/s²)
        float ay;        ///< Y-axis acceleration (scaled to m/s²)
        float az;        ///< Z-axis acceleration (scaled to m/s²)
        float gx;        ///< X-axis angular velocity (rad/s)
        float gy;        ///< Y-axis angular velocity (rad/s)
        float gz;        ///< Z-axis angular velocity (rad/s)
        bool valid;      ///< Data validity flag (true when current data is usable)

        /**
         * @brief Default constructor initializing all values to zero
         */
        ImuData() : ax(0), ay(0), az(0), gx(0), gy(0), gz(0), valid(false) {}
    };

    ImuData imu1; ///< Data storage for first IMU unit
    ImuData imu2; ///< Data storage for second IMU unit

    /**
     * @brief Handles translation and loading of language files.
     *
     * Used to apply and switch UI language dynamically.
     */
    QTranslator translator;

    /**
     * @brief Stores the current language code (e.g., "en" for English).
     *
     * Keeps track of the selected language for the application.
     */
    QString currentLanguage = "en";

    /**
     * @brief Updates all user-visible strings in the UI to reflect the current language.
     *
     * This function should be called whenever the application language changes,
     * to re-apply translations to all text elements such as labels, titles, and tooltips.
     * It ensures the interface dynamically updates without restarting the application.
     */
    void retranslateUi();

signals:
    /**
     * @brief Signals completed data processing
     * @param intValue Parsed integer value from data stream
     * @param floatValue Parsed floating-point value from data stream
     *
     * @details Emitted after successful parsing and validation of incoming data.
     *          Can be connected to additional processing modules or logging systems.
     */
    void dataProcessed(int intValue, float floatValue);
};

#endif // MAINWINDOW_H
