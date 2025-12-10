#include "PanelMedidas.h"

PanelMedidas::PanelMedidas(QWidget *parent) : QWidget(parent) {
  configurarUi();
}

void PanelMedidas::configurarUi() {
  QVBoxLayout *layout = new QVBoxLayout(this);

  QLabel *lbl = new QLabel("Definición de Medidas", this);
  lbl->setStyleSheet("font-weight: bold; margin-bottom: 5px;");
  layout->addWidget(lbl);

  listaMedidas = new QListWidget(this);
  listaMedidas->setStyleSheet("QListWidget { background-color: #252526; color: "
                              "#DDD; border: 1px solid #3E3E42; }");

  // Datos de ejemplo
  listaMedidas->addItem("Ventas Totales (SUM)");
  listaMedidas->addItem("Promedio Diario (AVG)");
  listaMedidas->addItem("Crecimiento Interanual (% VAR)");

  layout->addWidget(listaMedidas);
}
