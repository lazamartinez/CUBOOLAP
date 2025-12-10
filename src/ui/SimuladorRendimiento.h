#ifndef SIMULADORRENDIMIENTO_H
#define SIMULADORRENDIMIENTO_H

#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QWidget>


class SimuladorRendimiento : public QWidget {
  Q_OBJECT

public:
  explicit SimuladorRendimiento(QWidget *parent = nullptr);
  void actualizarEstimacion();

private:
  void configurarUi();

  QLabel *lblLatenciaEstimada;
  QProgressBar *barIndiceOptimizacion;
  QLabel *lblRecomendaciones;
};

#endif // SIMULADORRENDIMIENTO_H
