// colors.h - Paleta de colores estilo Flutter
#ifndef FLUTTER_COLORS_H
#define FLUTTER_COLORS_H

#include <QColor>

namespace FlutterColors {
// Colores primarios (Material Design)
const QColor primary = QColor(103, 80, 164);         // Morado Flutter
const QColor primaryVariant = QColor(156, 39, 176);  // Morado más oscuro
const QColor secondary = QColor(3, 218, 197);        // Cian Flutter
const QColor secondaryVariant = QColor(0, 191, 165); // Cian más oscuro

// Colores de superficie
const QColor surfaceLight = QColor(255, 255, 255);
const QColor surfaceDark = QColor(28, 27, 31);

// Colores de fondo
const QColor backgroundLight = QColor(248, 249, 250);
const QColor backgroundDark = QColor(18, 18, 18);

// Colores de error y éxito
const QColor error = QColor(207, 102, 121);
const QColor success = QColor(76, 175, 80);

// Colores de texto
const QColor onPrimary = QColor(255, 255, 255);
const QColor onSecondary = QColor(0, 0, 0);
const QColor onSurfaceLight = QColor(0, 0, 0, 222);
const QColor onSurfaceDark = QColor(255, 255, 255, 222);

// Elevación (sombras/superficies)
const QColor elevationLight[5] = {
    QColor(0, 0, 0, 5),  // dp0
    QColor(0, 0, 0, 10), // dp1
    QColor(0, 0, 0, 15), // dp2
    QColor(0, 0, 0, 20), // dp3
    QColor(0, 0, 0, 25)  // dp4
};

const QColor elevationDark[5] = {
    QColor(255, 255, 255, 5), QColor(255, 255, 255, 10),
    QColor(255, 255, 255, 15), QColor(255, 255, 255, 20),
    QColor(255, 255, 255, 25)};
} // namespace FlutterColors

#endif // FLUTTER_COLORS_H
