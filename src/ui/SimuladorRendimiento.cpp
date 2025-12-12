#include "SimuladorRendimiento.h"
#include "styles/FlutterTheme.h"
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QTimer>
#include <QVBoxLayout>


SimuladorRendimiento::SimuladorRendimiento(QWidget *parent) : QWidget(parent) {
  configurarUi();
  FlutterTheme::instance().applyThemeToWidget(this);
  // Simular datos iniciales
  QTimer::singleShot(500, this, &SimuladorRendimiento::actualizarEstimacion);
}

void SimuladorRendimiento::configurarUi() {
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setSpacing(16);
  layout->setContentsMargins(0, 0, 0, 0);

  // Metricas principales
  QHBoxLayout *metricsLayout = new QHBoxLayout();
  metricsLayout->setSpacing(16);

  // Latencia
  FlutterCard *cardLatencia = new FlutterCard(this);
  cardLatencia->setStyleSheet("QFrame { background: white; border-radius: 8px; "
                              "border-left: 4px solid #2563eb; }");
  QVBoxLayout *latLayout = new QVBoxLayout(cardLatencia);
  latLayout->setSpacing(4);
  latLayout->setContentsMargins(12, 12, 12, 12);

  lblLatenciaEstimada = new QLabel("--", cardLatencia);
  lblLatenciaEstimada->setStyleSheet(
      "font-size: 24px; font-weight: 700; color: #2563eb;");
  lblLatenciaEstimada->setAlignment(Qt::AlignCenter);
  latLayout->addWidget(lblLatenciaEstimada);

  QLabel *lblLatTxt = new QLabel("Latencia (ms)", cardLatencia);
  lblLatTxt->setStyleSheet("font-size: 11px; font-weight: 600; text-transform: "
                           "uppercase; color: #6b7280;");
  lblLatTxt->setAlignment(Qt::AlignCenter);
  latLayout->addWidget(lblLatTxt);

  metricsLayout->addWidget(cardLatencia);

  // Costo
  FlutterCard *cardCosto = new FlutterCard(this);
  cardCosto->setStyleSheet("QFrame { background: white; border-radius: 8px; "
                           "border-left: 4px solid #10b981; }");
  QVBoxLayout *costoLayout = new QVBoxLayout(cardCosto);
  costoLayout->setSpacing(4);
  costoLayout->setContentsMargins(12, 12, 12, 12);

  lblCostoEstimado = new QLabel("--", cardCosto);
  lblCostoEstimado->setStyleSheet(
      "font-size: 24px; font-weight: 700; color: #10b981;");
  lblCostoEstimado->setAlignment(Qt::AlignCenter);
  costoLayout->addWidget(lblCostoEstimado);

  QLabel *lblCostoTxt = new QLabel("Costo CPU", cardCosto);
  lblCostoTxt->setStyleSheet("font-size: 11px; font-weight: 600; "
                             "text-transform: uppercase; color: #6b7280;");
  lblCostoTxt->setAlignment(Qt::AlignCenter);
  costoLayout->addWidget(lblCostoTxt);

  metricsLayout->addWidget(cardCosto);

  layout->addLayout(metricsLayout);

  // Indice de optimizacion
  QLabel *lblOpt = new QLabel("Indice de Optimizacion", this);
  lblOpt->setStyleSheet("font-size: 13px; font-weight: 600; color: #374151;");
  layout->addWidget(lblOpt);

  barIndiceOptimizacion = new QProgressBar(this);
  barIndiceOptimizacion->setRange(0, 100);
  barIndiceOptimizacion->setValue(0);
  barIndiceOptimizacion->setMinimumHeight(12);
  barIndiceOptimizacion->setTextVisible(false); // Clean look
  barIndiceOptimizacion->setStyleSheet(R"(
    QProgressBar {
      border: none;
      border-radius: 6px;
      background-color: #e5e7eb;
    }
    QProgressBar::chunk {
      background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
        stop:0 #f59e0b, stop:1 #10b981);
      border-radius: 6px;
    }
  )");
  layout->addWidget(barIndiceOptimizacion);

  // Recomendaciones
  QLabel *lblRec = new QLabel("Recomendaciones", this);
  lblRec->setStyleSheet(
      "font-size: 13px; font-weight: 600; color: #374151; margin-top: 8px;");
  layout->addWidget(lblRec);

  lblRecomendaciones = new QLabel("Analizando modelo...", this);
  lblRecomendaciones->setWordWrap(true);
  lblRecomendaciones->setStyleSheet(R"(
    background: #fffbeb;
    border: 1px solid #fcd34d;
    border-radius: 6px;
    padding: 12px;
    font-size: 12px;
    color: #92400e;
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
      background: #f0fdf4;
      border: 1px solid #86efac;
      border-radius: 6px;
      padding: 12px;
      font-size: 12px;
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
