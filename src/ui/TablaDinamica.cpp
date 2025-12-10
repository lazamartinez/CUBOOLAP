#include "TablaDinamica.h"

TablaDinamica::TablaDinamica(QWidget *parent) : QWidget(parent) {
  configurarUi();
}

void TablaDinamica::configurarUi() {
  QVBoxLayout *layout = new QVBoxLayout(this);

  tabla = new QTableWidget(this);
  tabla->setRowCount(5);
  tabla->setColumnCount(4);
  tabla->setHorizontalHeaderLabels(
      {"Región", "Q1 2024", "Q2 2024", "Crecimiento"});
  tabla->alternatingRowColors();
  tabla->setStyleSheet(
      "QTableWidget { background-color: #1E1E1E; color: white; gridline-color: "
      "#333; } QHeaderView::section { background-color: #333; color: white; }");

  layout->addWidget(tabla);
}

void TablaDinamica::mostrarResultadosSimulados() {
  // Datos Dummy
  tabla->setItem(0, 0, new QTableWidgetItem("Norte"));
  tabla->setItem(0, 1, new QTableWidgetItem("$150,000"));
  tabla->setItem(0, 2, new QTableWidgetItem("$165,000"));
  tabla->setItem(0, 3, new QTableWidgetItem("+10%"));

  tabla->setItem(1, 0, new QTableWidgetItem("Sur"));
  tabla->setItem(1, 1, new QTableWidgetItem("$120,000"));
  tabla->setItem(1, 2, new QTableWidgetItem("$130,000"));
  tabla->setItem(1, 3, new QTableWidgetItem("+8.3%"));

  // ... más datos
}
