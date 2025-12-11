#include "PanelAnalisis.h"
#include "Estilos.h"
#include <QGroupBox>

PanelAnalisis::PanelAnalisis(QWidget *parent) : QWidget(parent) {
  configurarUi();
}

void PanelAnalisis::configurarUi() {
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setSpacing(16);
  layout->setContentsMargins(16, 16, 16, 16);

  // Titulo
  QLabel *title = new QLabel("Panel de Analisis", this);
  title->setStyleSheet(R"(
    font-size: 18px;
    font-weight: 700;
    color: #2563eb;
    padding-bottom: 8px;
    border-bottom: 2px solid #2563eb;
  )");
  layout->addWidget(title);

  // Seccion de celda seleccionada
  QGroupBox *grpSeleccion = new QGroupBox("Celda Seleccionada", this);
  QVBoxLayout *selLayout = new QVBoxLayout(grpSeleccion);

  lblDetalle =
      new QLabel("Seleccione una celda del cubo para ver detalles.", this);
  lblDetalle->setWordWrap(true);
  lblDetalle->setStyleSheet(
      "color: #374151; font-size: 13px; line-height: 1.5;");
  lblDetalle->setMinimumHeight(80);
  selLayout->addWidget(lblDetalle);
  layout->addWidget(grpSeleccion);

  // Seccion de estadisticas
  QGroupBox *grpStats = new QGroupBox("Estadisticas de Seleccion", this);
  QVBoxLayout *statsLayout = new QVBoxLayout(grpStats);

  lblEstadisticas = new QLabel("Sin seleccion", this);
  lblEstadisticas->setStyleSheet("color: #6b7280; font-size: 12px;");
  statsLayout->addWidget(lblEstadisticas);
  layout->addWidget(grpStats);

  // Acciones rapidas (Operaciones OLAP)
  QGroupBox *grpAcciones = new QGroupBox("Acciones OLAP", this);
  QGridLayout *accLayout = new QGridLayout(grpAcciones);
  accLayout->setSpacing(8);

  // Helper lambda para crear botones
  auto addBtn = [&](QString text, QString tip, int row, int col, auto signal) {
    QPushButton *btn = new QPushButton(text, this);
    btn->setToolTip(tip);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setStyleSheet(R"(
        QPushButton {
          background: #f3f4f6; border: 1px solid #d1d5db;
          border-radius: 4px; padding: 6px; color: #374151; font-weight: 500;
        }
        QPushButton:hover { background: #e5e7eb; border-color: #2563eb; color: #2563eb; }
        QPushButton:pressed { background: #dbeafe; }
      )");
    connect(btn, &QPushButton::clicked, this, signal);
    accLayout->addWidget(btn, row, col);
    return btn;
  };

  addBtn("Drill Down", "Ver detalle (Profundizar)", 0, 0,
         &PanelAnalisis::operacionDrillDown);
  addBtn("Roll Up", "Ver general (Subir nivel)", 0, 1,
         &PanelAnalisis::operacionRollUp);

  addBtn("Slice", "Filtrar 1 dimension", 1, 0, &PanelAnalisis::operacionSlice);
  addBtn("Dice", "Filtros multiples", 1, 1, &PanelAnalisis::operacionDice);

  addBtn("Pivot", "Rotar ejes", 2, 0, &PanelAnalisis::operacionPivot);
  addBtn("Swap", "Intercambiar dimensiones", 2, 1,
         &PanelAnalisis::operacionSwap);

  addBtn("Drill Through", "Ver registros (Detalles)", 3, 0,
         &PanelAnalisis::operacionDrillThrough);
  addBtn("Ranking", "Top N", 3, 1, &PanelAnalisis::operacionRanking);

  QPushButton *btnReset = addBtn("Resetear Vista", "Volver al inicio", 4, 0,
                                 &PanelAnalisis::operacionReset);
  accLayout->addWidget(btnReset, 4, 0, 1, 2); // Span 2 columnas

  layout->addWidget(grpAcciones);

  layout->addStretch();

  // Boton principal
  btnReportes = new QPushButton("Ir a Consultas y Reportes", this);
  btnReportes->setMinimumHeight(44);
  btnReportes->setCursor(Qt::PointingHandCursor);
  btnReportes->setStyleSheet(Estilos::obtenerEstiloBotonPrimario());
  connect(btnReportes, &QPushButton::clicked, this,
          &PanelAnalisis::irAReportes);
  layout->addWidget(btnReportes);

  // Estilo general del panel
  setStyleSheet(R"(
    QWidget {
      background: rgba(255, 255, 255, 0.95);
    }
    QGroupBox {
      border: 1px solid #e5e7eb;
      border-radius: 8px;
      margin-top: 12px;
      padding-top: 12px;
      background: rgba(248, 250, 252, 0.9);
    }
    QGroupBox::title {
      subcontrol-origin: margin;
      subcontrol-position: top left;
      padding: 4px 12px;
      background: #2563eb;
      color: white;
      border-radius: 4px;
      font-weight: 600;
      font-size: 11px;
    }
  )");

  setMinimumWidth(260);
  setMaximumWidth(320);
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
        "color: #059669; font-size: 12px; font-weight: 500;");
  }
}
