#ifndef SIMULADORRENDIMIENTO_H
#define SIMULADORRENDIMIENTO_H

#include "widgets/FlutterWidgets.h"
#include <QLabel>
#include <QProgressBar>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>


/**
 * @brief Simulador de rendimiento predictivo para modelos OLAP
 *
 * Muestra estimaciones de latencia, costo de CPU, y recomendaciones
 * de optimizacion basadas en el modelo configurado.
 */
class SimuladorRendimiento : public QWidget {
  Q_OBJECT

public:
  explicit SimuladorRendimiento(QWidget *parent = nullptr);

public slots:
  /// Actualiza las estimaciones de rendimiento
  void actualizarEstimacion();

  /// Configura el numero de dimensiones del modelo
  void setDimensiones(int count);

  /// Configura el numero de medidas del modelo
  void setMedidas(int count);

private:
  void configurarUi();

  QLabel *lblLatenciaEstimada;
  QLabel *lblCostoEstimado;
  QProgressBar *barIndiceOptimizacion;
  QLabel *lblRecomendaciones;
};

#endif // SIMULADORRENDIMIENTO_H
