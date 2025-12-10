#include "PanelAnalisis.h"

PanelAnalisis::PanelAnalisis(QWidget *parent) : QWidget(parent) {
  configurarUi();
}

void PanelAnalisis::configurarUi() {
  QVBoxLayout *layout = new QVBoxLayout(this);

  QLabel *title = new QLabel("Panel de Análisis Contextual", this);
  title->setStyleSheet("font-weight: bold; color: #4FC3F7; border-bottom: 1px "
                       "solid #555; padding-bottom: 5px;");
  layout->addWidget(title);

  lblDetalle =
      new QLabel("Seleccione una celda del cubo para ver detalles.", this);
  lblDetalle->setWordWrap(true);
  lblDetalle->setStyleSheet("color: #DDD; margin-top: 10px;");
  layout->addWidget(lblDetalle);

  layout->addStretch();

  btnReportes = new QPushButton("Ir a Consultas y Reportes (Fase 5)", this);
  btnReportes->setStyleSheet("background-color: #6f42c1; color: white; "
                             "margin-top: 10px; padding: 5px;");
  connect(btnReportes, &QPushButton::clicked, this,
          &PanelAnalisis::irAReportes);
  layout->addWidget(btnReportes);

  setStyleSheet("background-color: #252526; border-left: 1px solid #333;");
}

void PanelAnalisis::mostrarInfo(const QString &info) {
  lblDetalle->setText(info);
}
