/**
 * @file    hexagon.h
 * @brief   Hexagonal bar platform visualization widget for Qt applications
 *
 * @details Provides a customizable hexagon widget that displays six proportional bars,
 *          used for visualizing servo motor positions
 *
 * @author  Piotr Siembab
 * @date    18.04.2025
 * @version 1.1
 */

#ifndef HEXAGON_H
#define HEXAGON_H

#include <QWidget>
#include <QVector>
#include <QPointF>

/**
 * @class HexagonBars
 * @brief Visualizes six proportional bars in a hexagonal arrangement
 *
 * @details Features include:
 *          - Dynamic value updates (0.0 to 1.0 range)
 *          - Color-coded bars (green-yellow-red gradient)
 *          - Automatic hexagon sizing
 *          - Customizable appearance
 *          - Height indicators for additional data visualization
 *
 * The widget maintains geometric proportions during resizing and
 * provides smooth visual transitions during value changes.
 */
class HexagonBars : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructs a hexagonal bar visualization widget
     * @param parent Parent widget (default: nullptr)
     *
     * @details Initializes:
     *          - Default bar values (all 0.0)
     *          - Hexagon geometry calculations
     *          - Default color scheme
     */
    explicit HexagonBars(QWidget *parent = nullptr);

    /**
     * @brief Sets value for a specific bar
     * @param index Bar index (0-5, clockwise from top-right)
     * @param value Normalized value (0.0 to 1.0)
     *
     * @throws std::out_of_range if index is invalid
     * @note Triggers widget repaint
     * @see getBarValue()
     */
    void setBarValue(int index, float value);

    /**
     * @brief Retrieves current bar value
     * @param index Bar index (0-5)
     * @return Current normalized value
     *
     * @throws std::out_of_range if index is invalid
     * @see setBarValue()
     */
    float getBarValue(int index) const;

    void updateServoAngles(const QVector<int>& angles);  // Nowa funkcja publiczna

protected:
    /**
     * @brief Handles widget painting events
     * @param event Qt paint event object
     *
     * @details Coordinates the rendering of:
     *          - Base hexagon outline
     *          - Colored value bars
     *          - Optional height indicators
     */
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<float> barValues;      ///< Stores normalized values for all six bars
    QVector<QPointF> hexagonPoints;///< Pre-calculated hexagon vertices (in widget coordinates)

    /**
     * @brief Recalculates hexagon geometry
     *
     * @details Computes vertex positions based on current widget dimensions,
     *          maintaining proper aspect ratio and centering.
     */
    void calculateHexagon();

    /**
     * @brief Renders the hexagonal outline
     * @param painter Reference to active QPainter
     *
     * @details Draws:
     *          - Solid hexagon border
     *          - Center point marker
     *          - Axis reference lines (if enabled)
     */
    void drawHexagon(QPainter &painter);

    /**
     * @brief Renders the value bars
     * @param painter Reference to active QPainter
     *
     * @details For each bar:
     *          - Calculates length based on current value
     *          - Applies color gradient
     *          - Adds value text labels (optional)
     */
    void drawBars(QPainter &painter);

    /**
     * @brief Provides recommended minimum widget size
     * @return QSize containing minimum dimensions
     */
    QSize minimumSizeHint() const override;

    /**
     * @brief Provides ideal widget size
     * @return QSize containing preferred dimensions
     */
    QSize sizeHint() const override;

    /**
     * @brief Renders additional height indicators
     * @param painter Reference to active QPainter
     * @param heights Height values for each bar
     *
     * @details Used for visualizing secondary data dimensions:
     *          - Draws as concentric hexagons
     *          - Uses semi-transparent fill
     *          - Supports platform height visualization
     */
    void drawHeightIndicators(QPainter &painter, const QVector<float>& heights);

    /**
     * @brief Generates value-dependent bar color
     * @param value Normalized value (0.0 to 1.0)
     * @return QColor representing value severity
     *
     * @details Color progression:
     *          - 0.0: Green
     *          - 0.5: Yellow
     *          - 1.0: Red
     */
    QColor getBarColor(float value);

    QVector<int> servoAngles = {0, 0, 0, 0, 0, 0};  // Przechowuj rzeczywiste kąty serw
};

#endif // HEXAGON_H
