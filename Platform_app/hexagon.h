#ifndef HEXAGON_H
#define HEXAGON_H

#include <QWidget>
#include <QVector>
#include <QPointF>

/**
 * @class HexagonBars
 * @brief A custom Qt widget that visualizes six bars arranged in a hexagon.
 *
 * Each bar can be individually set with a value between 0.0 and 1.0,
 * and is drawn extending from the outer hexagon point toward the center.
 * The height and color of each bar visually represent its value.
 */
class HexagonBars : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructor for HexagonBars widget.
     * @param parent Optional parent widget.
     */
    explicit HexagonBars(QWidget *parent = nullptr);

    /**
     * @brief Sets the value of a specific bar.
     * @param index Index of the bar (0–5).
     * @param value Value between 0.0 and 1.0 representing the bar's length/height.
     */
    void setBarValue(int index, float value);

    /**
     * @brief Retrieves the value of a specific bar.
     * @param index Index of the bar (0–5).
     * @return Value of the bar (between 0.0 and 1.0).
     */
    float getBarValue(int index) const;

protected:
    /**
     * @brief Repaints the widget, including hexagon and bars.
     * @param event Qt paint event (unused).
     */
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<float> barValues;      ///< Stores the 6 bar values (0.0–1.0)
    QVector<QPointF> hexagonPoints;///< Stores the calculated 6 hexagon corner points

    /**
     * @brief Calculates corner points of the hexagon based on widget size.
     */
    void calculateHexagon();

    /**
     * @brief Draws the base hexagon shape.
     * @param painter The painter used for rendering.
     */
    void drawHexagon(QPainter &painter);

    /**
     * @brief Draws bars extending from the outer hexagon points toward the center.
     * @param painter The painter used for rendering.
     */
    void drawBars(QPainter &painter);

    /**
     * @brief Draws height indicators for each bar (e.g., platform height effects).
     * @param painter The painter used for rendering.
     * @param heights A list of height values for each bar.
     */
    void drawHeightIndicators(QPainter &painter, const QVector<float>& heights);

    /**
     * @brief Computes a gradient color for a given bar value.
     * @param value A float between 0.0 and 1.0.
     * @return QColor that transitions from green (0.0), to yellow (0.5), to red (1.0).
     */
    QColor getBarColor(float value);
};

#endif // HEXAGON_H
