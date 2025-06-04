#include "imudisplay.h"
#include <QFont>
#include <QFrame>

// Konstruktor klasy IMUDisplay — przyjmuje identyfikator i wskaznik na rodzica QWidget
IMUDisplay::IMUDisplay(bool id, QWidget *parent) : QWidget(parent) {
    QGridLayout *layout = new QGridLayout(this); // Ustawiamy siatke jako glowny layout
    layout->setSpacing(5);
    layout->setContentsMargins(10, 10, 10, 10);

    // Tworzymy naglowek z napisem "IMU Data 1" lub "IMU Data 2"
    const QString Title = "IMU Data ";
    const QString id_str = id ? "2" : "1";
    QLabel *header = new QLabel(Title + id_str);
    QFont headerFont = header->font();
    headerFont.setBold(true);
    headerFont.setPointSize(12);
    header->setFont(headerFont);
    layout->addWidget(header, 0, 0, 1, 3, Qt::AlignCenter);  // Naglowek rozciagniety na 3 kolumny

    // Separator poziomy
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    layout->addWidget(line, 1, 0, 1, 3);

    // Wyswietlacze akcelerometru
    setupAxisDisplay(layout, "Accel X", 2);
    setupAxisDisplay(layout, "Accel Y", 3);
    setupAxisDisplay(layout, "Accel Z", 4);

    // Drugi separator
    line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    layout->addWidget(line, 5, 0, 1, 3);

    // Wyswietlacze zyroskopu
    setupAxisDisplay(layout, "Gyro X", 6);
    setupAxisDisplay(layout, "Gyro Y", 7);
    setupAxisDisplay(layout, "Gyro Z", 8);

    // Stylizacja calego widgetu i etykiet
    this->setStyleSheet(
        "IMUDisplay {"
        "   background-color: #404040;"  // Ciemne tlo widgetu
        "}"
        "QLabel {"
        "   color: white;"  // Domyslny kolor tekstu dla etykiet
        "}"
        "QLabel[accessibleName='valueLabel'] {"  // Styl tylko dla etykiet z wynikami
        "   background-color: #353535;"
        "   border: 1px solid #555555;"
        "   border-radius: 4px;"
        "   padding: 4px;"
        "   min-width: 50px;"
        "   font-family: 'Courier New', monospace;"
        "   color: black;"  // UWAGA: tekst moze byc malo czytelny na ciemnym tle
        "}"
        );
}

// Tworzy etykiete z wartoscia (poczatkowo 0.00), z odpowiednim stylem i wyrownaniem
QLabel *IMUDisplay::createValueLabel() {
    QLabel *label = new QLabel("0.00");
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter); // Wyrownanie tekstu do prawej
    label->setMinimumWidth(50);  // Stala szerokosc

    // Stylizacja ramki
    label->setFrameStyle(QFrame::Box | QFrame::Plain);
    label->setStyleSheet("QLabel { background-color: grey; padding: 3px; border: 1px solid black }");
    return label;
}

// Funkcja pomocnicza do tworzenia wiersza z nazwa, wartoscia i jednostka
void IMUDisplay::setupAxisDisplay(QGridLayout *layout, const QString &name, int row) {
    AxisDisplay *display = nullptr;

    // Rozrozniamy pomiedzy akcelerometrem i zyroskopem na podstawie nazwy
    if (name.startsWith("Accel")) {
        if (name.endsWith("X")) display = &m_accelX;
        else if (name.endsWith("Y")) display = &m_accelY;
        else display = &m_accelZ;
    } else {
        if (name.endsWith("X")) display = &m_gyroX;
        else if (name.endsWith("Y")) display = &m_gyroY;
        else display = &m_gyroZ;
    }

    // Tworzymy etykiety nazwy i wartosci
    display->nameLabel = new QLabel(name);
    display->valueLabel = createValueLabel();
    display->valueLabel->setText("0.00");

    // Ustawienia rozmiaru i wyrownania
    display->nameLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    display->nameLabel->setAlignment(Qt::AlignCenter);

    // Dodajemy etykiety i jednostke do siatki
    layout->addWidget(display->nameLabel, row, 0);
    layout->addWidget(display->valueLabel, row, 1);

    QLabel *unit = new QLabel(name.startsWith("Accel") ? "m/s²" : "°/s");
    layout->addWidget(unit, row, 2);
}

// Aktualizacja wartosci wyswietlanych dla akcelerometru i zyroskopu
void IMUDisplay::updateValues(float ax, float ay, float az, float gx, float gy, float gz) {
    // Przypisujemy nowe wartosci do etykiet
    m_accelX.valueLabel->setText(QString::number(ax, 'f', 2));
    m_accelY.valueLabel->setText(QString::number(ay, 'f', 2));
    m_accelZ.valueLabel->setText(QString::number(az, 'f', 2));

    m_gyroX.valueLabel->setText(QString::number(gx, 'f', 2));
    m_gyroY.valueLabel->setText(QString::number(gy, 'f', 2));
    m_gyroZ.valueLabel->setText(QString::number(gz, 'f', 2));

    // Wymuszenie natychmiastowego odswiezenia interfejsu
    this->update();
}
