#ifndef PANELANALISIS_H
#define PANELANALISIS_H

#include "widgets/FlutterWidgets.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>


/**
 * @brief Panel lateral de analisis para Fase 4
 *
 * Muestra informacion de la celda seleccionada en el visor OLAP,
 * estadisticas de seleccion multiple, y acciones disponibles.
 */
class PanelAnalisis : public QWidget {
  Q_OBJECT

public:
  explicit PanelAnalisis(QWidget *parent = nullptr);

public slots:
  /// Muestra informacion de la celda seleccionada
  void mostrarInfo(const QString &info);

  /// Actualiza estadisticas cuando hay multi-seleccion
  void actualizarEstadisticas(int cantidadSeleccionadas, double sumaValores);

  // Senales de operaciones OLAP
signals:
  void operacionDrillDown();
  void operacionRollUp();
  void operacionSlice();
  void operacionDice();
  void operacionPivot();
  void operacionSwap();
  void operacionRanking();
  void operacionDrillThrough();
  void operacionReset();

  void irAReportes();

private:
  void configurarUi();

  QLabel *lblDetalle;
  QLabel *lblEstadisticas;
  QPushButton *btnReportes;
};

#endif // PANELANALISIS_H
