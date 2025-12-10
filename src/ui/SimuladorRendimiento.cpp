#include "SimuladorRendimiento.h"

SimuladorRendimiento::SimuladorRendimiento(QWidget *parent) : QWidget(parent) {
  configurarUi();
}

void SimuladorRendimiento::configurarUi() {
  QVBoxLayout *layout = new QVBoxLayout(this);

  QLabel *title = new QLabel("Simulador de Rendimiento Predictivo", this);
  title->setStyleSheet("font-weight: bold; color: #E6E600;");
  layout->addWidget(title);

  layout->addWidget(new QLabel("Latencia Estimada (Consulta Promedio):"));
  lblLatenciaEstimada = new QLabel("--- ms");
  lblLatenciaEstimada->setStyleSheet("font-size: 16px; color: cyan;");
  layout->addWidget(lblLatenciaEstimada);

  layout->addWidget(new QLabel("Índice de Optimización del Modelo:"));
  barIndiceOptimizacion = new QProgressBar(this);
  barIndiceOptimizacion->setRange(0, 100);
  barIndiceOptimizacion->setValue(0);
  layout->addWidget(barIndiceOptimizacion);

  layout->addWidget(new QLabel("Recomendaciones de Indexado:"));
  lblRecomendaciones = new QLabel("Esperando modelo...", this);
  lblRecomendaciones->setWordWrap(true);
  lblRecomendaciones->setStyleSheet("font-style: italic; color: #AAA;");
  layout->addWidget(lblRecomendaciones);

  layout->addStretch();
}

void SimuladorRendimiento::actualizarEstimacion() {
  // Mock logic
  lblLatenciaEstimada->setText("45 ms");
  barIndiceOptimizacion->setValue(85);
  lblRecomendaciones->setText(
      "Sugerencia: Crear índice B-Tree en DimTiempo(anio, mes).");
}
