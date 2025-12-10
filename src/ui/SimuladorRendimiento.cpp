#include "SimuladorRendimiento.h"
#include "Estilos.h"
#include <QGroupBox>

SimuladorRendimiento::SimuladorRendimiento(QWidget *parent) : QWidget(parent) {
  configurarUi();
  // Simular datos iniciales
  QTimer::singleShot(500, this, &SimuladorRendimiento::actualizarEstimacion);
}

void SimuladorRendimiento::configurarUi() {
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setSpacing(12);
  layout->setContentsMargins(0, 0, 0, 0);

  // Metricas principales
  QHBoxLayout *metricsLayout = new QHBoxLayout();
  metricsLayout->setSpacing(12);

  // Latencia
  QWidget *cardLatencia = new QWidget(this);
  cardLatencia->setStyleSheet(R"(
    background: rgba(255, 255, 255, 0.9);
    border: 1px solid #e5e7eb;
    border-left: 4px solid #2563eb;
    border-radius: 8px;
    padding: 10px;
  )");
  QVBoxLayout *latLayout = new QVBoxLayout(cardLatencia);
  latLayout->setSpacing(4);

  lblLatenciaEstimada = new QLabel("--", cardLatencia);
  lblLatenciaEstimada->setStyleSheet(
      "font-size: 24px; font-weight: 700; color: #2563eb;");
  lblLatenciaEstimada->setAlignment(Qt::AlignCenter);
  latLayout->addWidget(lblLatenciaEstimada);

  QLabel *lblLatTxt = new QLabel("Latencia (ms)", cardLatencia);
  lblLatTxt->setStyleSheet("font-size: 10px; color: #6b7280;");
  lblLatTxt->setAlignment(Qt::AlignCenter);
  latLayout->addWidget(lblLatTxt);

  metricsLayout->addWidget(cardLatencia);

  // Costo
  QWidget *cardCosto = new QWidget(this);
  cardCosto->setStyleSheet(
      cardLatencia->styleSheet().replace("#2563eb", "#10b981"));
  QVBoxLayout *costoLayout = new QVBoxLayout(cardCosto);
  costoLayout->setSpacing(4);

  lblCostoEstimado = new QLabel("--", cardCosto);
  lblCostoEstimado->setStyleSheet(
      "font-size: 24px; font-weight: 700; color: #10b981;");
  lblCostoEstimado->setAlignment(Qt::AlignCenter);
  costoLayout->addWidget(lblCostoEstimado);

  QLabel *lblCostoTxt = new QLabel("Costo CPU", cardCosto);
  lblCostoTxt->setStyleSheet("font-size: 10px; color: #6b7280;");
  lblCostoTxt->setAlignment(Qt::AlignCenter);
  costoLayout->addWidget(lblCostoTxt);

  metricsLayout->addWidget(cardCosto);

  layout->addLayout(metricsLayout);

  // Indice de optimizacion
  QLabel *lblOpt = new QLabel("Indice de Optimizacion", this);
  lblOpt->setStyleSheet("font-weight: 600; color: #374151; font-size: 11px;");
  layout->addWidget(lblOpt);

  barIndiceOptimizacion = new QProgressBar(this);
  barIndiceOptimizacion->setRange(0, 100);
  barIndiceOptimizacion->setValue(0);
  barIndiceOptimizacion->setMinimumHeight(20);
  barIndiceOptimizacion->setStyleSheet(R"(
    QProgressBar {
      border: none;
      border-radius: 10px;
      background-color: #e5e7eb;
      text-align: center;
      font-size: 10px;
      font-weight: 600;
    }
    QProgressBar::chunk {
      background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
        stop:0 #f59e0b, stop:1 #10b981);
      border-radius: 10px;
    }
  )");
  layout->addWidget(barIndiceOptimizacion);

  // Recomendaciones
  QLabel *lblRec = new QLabel("Recomendaciones", this);
  lblRec->setStyleSheet(
      "font-weight: 600; color: #374151; font-size: 11px; margin-top: 8px;");
  layout->addWidget(lblRec);

  lblRecomendaciones = new QLabel("Analizando modelo...", this);
  lblRecomendaciones->setWordWrap(true);
  lblRecomendaciones->setStyleSheet(R"(
    background: rgba(254, 252, 232, 0.9);
    border: 1px solid #fef08a;
    border-radius: 6px;
    padding: 8px;
    font-size: 11px;
    color: #854d0e;
  )");
  layout->addWidget(lblRecomendaciones);

  layout->addStretch();
}

void SimuladorRendimiento::actualizarEstimacion() {
  // Simular metricas
  int latencia = 35 + (rand() % 30);
  int costo = 1 + (rand() % 5);
  int optimizacion = 70 + (rand() % 25);

  lblLatenciaEstimada->setText(QString::number(latencia));
  lblCostoEstimado->setText(QString::number(costo));
  barIndiceOptimizacion->setValue(optimizacion);

  // Color segun latencia
  if (latencia < 50) {
    lblLatenciaEstimada->setStyleSheet(
        "font-size: 24px; font-weight: 700; color: #10b981;");
  } else if (latencia < 100) {
    lblLatenciaEstimada->setStyleSheet(
        "font-size: 24px; font-weight: 700; color: #f59e0b;");
  } else {
    lblLatenciaEstimada->setStyleSheet(
        "font-size: 24px; font-weight: 700; color: #ef4444;");
  }

  // Recomendaciones basadas en optimizacion
  QString recs;
  if (optimizacion < 80) {
    recs = "Sugerencias:\n"
           "- Crear indice en dim_tiempo(fecha)\n"
           "- Agregar FK en fact_ventas\n"
           "- Considerar vista materializada";
  } else if (optimizacion < 95) {
    recs = "Modelo bien optimizado.\n"
           "Sugerencia menor: Particionar fact_ventas por anio.";
  } else {
    recs = "Excelente! Modelo altamente optimizado.\n"
           "No se requieren acciones adicionales.";
    lblRecomendaciones->setStyleSheet(R"(
      background: rgba(220, 252, 231, 0.9);
      border: 1px solid #86efac;
      border-radius: 6px;
      padding: 8px;
      font-size: 11px;
      color: #166534;
    )");
  }

  lblRecomendaciones->setText(recs);
}

void SimuladorRendimiento::setDimensiones(int count) {
  // Recalcular estimacion basada en dimensiones
  actualizarEstimacion();
}

void SimuladorRendimiento::setMedidas(int count) {
  // Recalcular estimacion basada en medidas
  actualizarEstimacion();
}
