/**
 * @file    imudisplay.h
 * @brief   IMU sensor data visualization widget
 *
 * @details Provides a compact display for real-time IMU sensor readings including:
 *          - 3-axis accelerometer data (X, Y, Z)
 *          - 3-axis gyroscope data (X, Y, Z)
 *          - Formatted numerical values with proper units
 *          - Clear visual separation between sensor types
 *
 * @author  Piotr Siembab
 * @date    18.04.2025
 * @version 1.0
 */

#ifndef IMUDISPLAY_H
#define IMUDISPLAY_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>

/**
 * @class IMUDisplay
 * @brief Widget for visualizing IMU sensor data in real-time
 *
 * @details Displays formatted numerical values for:
 *          - Linear acceleration (typically in m/s² or g)
 *          - Angular velocity (typically in rad/s or °/s)
 *
 * The display organizes data in a clean grid layout with:
 * - Separate sections for accelerometer and gyroscope
 * - Clear axis labeling
 * - Consistent numerical formatting
 */
class IMUDisplay : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructs an IMU display widget
     * @param id IMU identifier (used for display differentiation)
     * @param parent Parent widget (default: nullptr)
     *
     * @details Initializes all UI elements including:
     *          - Axis labels for both sensor types
     *          - Value display areas
     *          - Layout organization
     */
    explicit IMUDisplay(bool id, QWidget *parent = nullptr);

    /**
     * @brief Updates all displayed sensor values
     * @param ax X-axis acceleration value
     * @param ay Y-axis acceleration value
     * @param az Z-axis acceleration value
     * @param gx X-axis angular velocity
     * @param gy Y-axis angular velocity
     * @param gz Z-axis angular velocity
     *
     * @details Accepts new sensor readings and:
     *          - Formats values to 2 decimal places
     *          - Updates all display labels
     *          - Maintains consistent units display
     *
     * @note All values should be in consistent units (typically SI units)
     */
    void updateValues(float ax, float ay, float az, float gx, float gy, float gz);

    /**
     * @brief Updates all user-visible strings in the UI to reflect the current language.
     *
     * This function should be called whenever the application language changes,
     * to re-apply translations to all text elements such as labels, titles, and tooltips.
     * It ensures the interface dynamically updates without restarting the application.
     */
    void retranslateUi();

private:
    /**
     * @brief Creates a standardized value display label
     * @return Configured QLabel pointer
     *
     * @details Creates labels with consistent:
     *          - Font styling
     *          - Alignment
     *          - Minimum width
     *          - Numeric formatting
     */
    QLabel *createValueLabel();

    /**
     * @brief Configures display elements for one sensor axis
     * @param layout Target grid layout for placement
     * @param name Axis name identifier ("X", "Y", or "Z")
     * @param row Grid layout row position
     *
     * @details Creates and positions:
     *          - Axis name label
     *          - Value display label
     *          - Proper spacing elements
     */
    void setupAxisDisplay(QGridLayout *layout, const QString &name, int row);

    /**
     * @struct AxisDisplay
     * @brief Container for axis display elements
     */
    struct AxisDisplay {
        QLabel *nameLabel;  ///< Label showing axis identifier (e.g., "Accel X")
        QLabel *valueLabel; ///< Label showing current value (e.g., "1.23 m/s²")
    };

    // Accelerometer displays
    AxisDisplay m_accelX; ///< X-axis acceleration display
    AxisDisplay m_accelY; ///< Y-axis acceleration display
    AxisDisplay m_accelZ; ///< Z-axis acceleration display

    // Gyroscope displays
    AxisDisplay m_gyroX;  ///< X-axis angular velocity display
    AxisDisplay m_gyroY;  ///< Y-axis angular velocity display
    AxisDisplay m_gyroZ;  ///< Z-axis angular velocity display

    /**
 * @brief Main header label displaying the IMU data title.
 *
 * Usually shows text like "IMU Data 1" or "IMU Data 2".
 */
    QLabel *header;

    /**
 * @brief Base title string for the IMU display header.
 *
 * Used as the prefix before the IMU device identifier.
 */
    QString Title;

    /**
 * @brief Identifier suffix string ("1" or "2") for the IMU display.
 *
 * Distinguishes between multiple IMU devices in the UI.
 */
    QString id_str;

};

#endif // IMUDISPLAY_H
