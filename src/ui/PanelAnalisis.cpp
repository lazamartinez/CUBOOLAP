#include "PanelAnalisis.h"
#include "styles/FlutterTheme.h"
#include <QGridLayout>
#include <QGroupBox>

PanelAnalisis::PanelAnalisis(QWidget *parent) : QWidget(parent) {
  configurarUi();
  FlutterTheme::instance().applyThemeToWidget(this);
}

void PanelAnalisis::configurarUi() {
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setSpacing(16);
  layout->setContentsMargins(16, 16, 16, 16);

  // Titulo
  QLabel *title = new QLabel("Panel de Analisis", this);
  title->setStyleSheet("font-size: 20px; font-weight: 700; color: #2563eb;");
  layout->addWidget(title);

  // Seccion de celda seleccionada
  FlutterCard *grpSeleccion = new FlutterCard(this);
  QVBoxLayout *selLayout = new QVBoxLayout(grpSeleccion);
  selLayout->setContentsMargins(12, 12, 12, 12);
  selLayout->setSpacing(8);

  QLabel *lblTitleSel = new QLabel("Celda Seleccionada", grpSeleccion);
  lblTitleSel->setStyleSheet("font-size: 14px; font-weight: 600; "
                             "text-transform: uppercase; color: #374151;");
  selLayout->addWidget(lblTitleSel);

  lblDetalle = new QLabel("Seleccione una celda del cubo para ver detalles.",
                          grpSeleccion);
  lblDetalle->setWordWrap(true);
  lblDetalle->setStyleSheet(
      "color: #4b5563; font-size: 13px; line-height: 1.5;");
  lblDetalle->setMinimumHeight(60);
  selLayout->addWidget(lblDetalle);
  layout->addWidget(grpSeleccion);

  // Seccion de estadisticas
  FlutterCard *grpStats = new FlutterCard(this);
  QVBoxLayout *statsLayout = new QVBoxLayout(grpStats);
  statsLayout->setContentsMargins(12, 12, 12, 12);
  statsLayout->setSpacing(8);

  QLabel *lblTitleStats = new QLabel("Estadisticas de Seleccion", grpStats);
  lblTitleStats->setStyleSheet("font-size: 14px; font-weight: 600; "
                               "text-transform: uppercase; color: #374151;");
  statsLayout->addWidget(lblTitleStats);

  lblEstadisticas = new QLabel("Sin seleccion", grpStats);
  lblEstadisticas->setStyleSheet("color: #6b7280; font-size: 13px;");
  statsLayout->addWidget(lblEstadisticas);
  layout->addWidget(grpStats);

  // Acciones rapidas (Operaciones OLAP)
  FlutterCard *grpAcciones = new FlutterCard(this);
  QVBoxLayout *accContainerLayout = new QVBoxLayout(grpAcciones);
  accContainerLayout->setContentsMargins(12, 12, 12, 12);
  accContainerLayout->setSpacing(8);

  QLabel *lblTitleAcc = new QLabel("Acciones OLAP", grpAcciones);
  lblTitleAcc->setStyleSheet("font-size: 14px; font-weight: 600; "
                             "text-transform: uppercase; color: #374151;");
  accContainerLayout->addWidget(lblTitleAcc);

  QGridLayout *accLayout = new QGridLayout();
  accLayout->setSpacing(8);
  accContainerLayout->addLayout(accLayout);

  // Helper lambda para crear botones
  auto addBtn = [&](QString text, QString tip, int row, int col, auto signal) {
    FlutterElevatedButton *btn = new FlutterElevatedButton(text, this);
    btn->setToolTip(tip);
    // Keep it compact
    btn->setMinimumHeight(36);
    // Styling is handled by FlutterElevatedButton class usually, but
    // constrained here
    connect(btn, &QPushButton::clicked, this, signal);
    accLayout->addWidget(btn, row, col);
    return btn;
  };

  addBtn("🔍 Drill Down", "Ver detalle (Profundizar)", 0, 0,
         &PanelAnalisis::operacionDrillDown);
  addBtn("📊 Roll Up", "Ver general (Subir nivel)", 0, 1,
         &PanelAnalisis::operacionRollUp);

  addBtn("🔪 Slice", "Filtrar 1 dimension", 1, 0,
         &PanelAnalisis::operacionSlice);
  addBtn("🎲 Dice", "Filtros multiples", 1, 1, &PanelAnalisis::operacionDice);

  addBtn("🔄 Pivot", "Rotar ejes", 2, 0, &PanelAnalisis::operacionPivot);
  addBtn("↔️ Swap", "Intercambiar dimensiones", 2, 1,
         &PanelAnalisis::operacionSwap);

  addBtn("📋 Details", "Ver registros (Detalles)", 3, 0,
         &PanelAnalisis::operacionDrillThrough);
  addBtn("🏆 Ranking", "Top N", 3, 1, &PanelAnalisis::operacionRanking);

  FlutterElevatedButton *btnReset =
      new FlutterElevatedButton("🔄 Resetear Vista", this);
  btnReset->setToolTip("Volver al inicio");
  connect(btnReset, &QPushButton::clicked, this,
          &PanelAnalisis::operacionReset);
  accLayout->addWidget(btnReset, 4, 0, 1, 2); // Span 2 columnas

  layout->addWidget(grpAcciones);

  layout->addStretch();

  // Boton principal
  btnReportes = new FlutterFilledButton("📊 Ir a Consultas y Reportes", this);
  btnReportes->setMinimumHeight(48);
  btnReportes->setIcon(MaterialIcons::instance().bar_chart());
  connect(btnReportes, &QPushButton::clicked, this,
          &PanelAnalisis::irAReportes);
  layout->addWidget(btnReportes);

  setMinimumWidth(300);
  setMaximumWidth(360);
}

void PanelAnalisis::mostrarInfo(const QString &info) {
  lblDetalle->setText(info);
  lblDetalle->setStyleSheet(
      "color: #1f2937; font-size: 13px; font-weight: 500;");
}

void PanelAnalisis::actualizarEstadisticas(int cantidadSeleccionadas,
                                           double sumaValores) {
  if (cantidadSeleccionadas == 0) {
    lblEstadisticas->setText("Sin seleccion");
  } else {
    QString stats = QString("Celdas: %1\nSuma: $%2\nPromedio: $%3")
                        .arg(cantidadSeleccionadas)
                        .arg(sumaValores, 0, 'f', 2)
                        .arg(sumaValores / cantidadSeleccionadas, 0, 'f', 2);
    lblEstadisticas->setText(stats);
    lblEstadisticas->setStyleSheet(
        "color: #059669; font-size: 13px; font-weight: 500;");
  }
}
