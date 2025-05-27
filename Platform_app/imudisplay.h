#ifndef IMUDISPLAY_H
#define IMUDISPLAY_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>

/**
 * @class IMUDisplay
 * @brief Displays real-time IMU sensor readings such as accelerometer and gyroscope data.
 *
 * This widget is designed to display the sensor values of an IMU, specifically the accelerometer and gyroscope
 * data for the X, Y, and Z axes. It provides an organized layout with labels for each axis and updates in real-time
 * as new data becomes available.
 */
class IMUDisplay : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructs an IMUDisplay object.
     * @param parent The parent widget, or nullptr if none.
     */
    explicit IMUDisplay(bool id, QWidget *parent = nullptr);

    /**
     * @brief Updates the displayed values for the accelerometer and gyroscope.
     * @param ax Accelerometer reading along the X-axis.
     * @param ay Accelerometer reading along the Y-axis.
     * @param az Accelerometer reading along the Z-axis.
     * @param gx Gyroscope reading along the X-axis.
     * @param gy Gyroscope reading along the Y-axis.
     * @param gz Gyroscope reading along the Z-axis.
     *
     * This function is called to update the widget's labels with the new sensor values. The data is updated
     * in real-time as the IMU sends new readings.
     */
    void updateValues(float ax, float ay, float az, float gx, float gy, float gz);

private:
    /**
     * @brief Creates a QLabel to display a value.
     * @return A QLabel widget.
     *
     * This helper function is used to create QLabel objects that are used for displaying individual sensor values.
     */
    QLabel *createValueLabel();

    /**
     * @brief Sets up the display of each axis's values in the provided layout.
     * @param layout The QGridLayout to which the axis labels will be added.
     * @param name The name of the axis to display (e.g., "X", "Y", "Z").
     * @param row The row number in the layout for the axis labels.
     *
     * This function organizes the layout by adding a label for the axis name and its corresponding value.
     */
    void setupAxisDisplay(QGridLayout *layout, const QString &name, int row);

    struct AxisDisplay {
        QLabel *nameLabel; ///< Label for the axis name (e.g., "Accel X").
        QLabel *valueLabel; ///< Label for the axis value (e.g., "0.00").
    };

    // Axis displays for accelerometer and gyroscope values.
    AxisDisplay m_accelX, m_accelY, m_accelZ; ///< Accelerometer axis labels.
    AxisDisplay m_gyroX, m_gyroY, m_gyroZ;    ///< Gyroscope axis labels.
};

#endif // IMUDISPLAY_H
