#include "PanelDimensiones.h"

PanelDimensiones::PanelDimensiones(QWidget *parent) : QWidget(parent) {
  configurarUi();
}

void PanelDimensiones::configurarUi() {
  QVBoxLayout *layout = new QVBoxLayout(this);

  QLabel *lbl = new QLabel("Explorador Jerárquico de Dimensiones", this);
  lbl->setStyleSheet("font-weight: bold; margin-bottom: 5px;");
  layout->addWidget(lbl);

  arbolDimensiones = new QTreeWidget(this);
  arbolDimensiones->setHeaderLabel("Dimensiones / Niveles");
  arbolDimensiones->setStyleSheet("QTreeWidget { background-color: #252526; "
                                  "color: #DDD; border: 1px solid #3E3E42; }");

  // Datos de ejemplo
  QTreeWidgetItem *dimTiempo = new QTreeWidgetItem(arbolDimensiones);
  dimTiempo->setText(0, "Tiempo");
  dimTiempo->setIcon(
      0, QIcon::fromTheme("appointment-new")); // Icono genérico si existe

  new QTreeWidgetItem(dimTiempo, QStringList() << "Año");
  new QTreeWidgetItem(dimTiempo, QStringList() << "Trimestre");
  new QTreeWidgetItem(dimTiempo, QStringList() << "Mes");

  QTreeWidgetItem *dimGeo = new QTreeWidgetItem(arbolDimensiones);
  dimGeo->setText(0, "Geografía");

  new QTreeWidgetItem(dimGeo, QStringList() << "País");
  new QTreeWidgetItem(dimGeo, QStringList() << "Ciudad");

  layout->addWidget(arbolDimensiones);
}
