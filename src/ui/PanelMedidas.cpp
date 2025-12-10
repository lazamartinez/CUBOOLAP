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

  layout->addWidget(listaMedidas);

  cargarMedidasReales();
}

void PanelMedidas::cargarMedidasReales() {
  listaMedidas->clear();
  QSqlDatabase db = QSqlDatabase::database("CuboVisionConnection");
  if (!db.isOpen()) {
    listaMedidas->addItem("Sin conexión");
    return;
  }

  QStringList tablas = db.tables();
  for (const QString &tabla : tablas) {
    if (tabla.startsWith("fact_") || tabla.contains("ventas")) { // Heuristica
      QSqlRecord record = db.record(tabla);
      for (int i = 0; i < record.count(); i++) {
        // Correccion para Qt 6: Usar metaType().id() en lugar de type()
        int typeId = record.field(i).metaType().id();
        if (typeId == QMetaType::Int || typeId == QMetaType::Double ||
            typeId == QMetaType::LongLong) {
          QString col = record.fieldName(i);
          if (!col.endsWith("_id") && !col.startsWith("id_")) {
            listaMedidas->addItem(QString("%1 (SUM)").arg(col));
            listaMedidas->addItem(QString("%1 (AVG)").arg(col));
          }
        }
      }
    }
  }
  if (listaMedidas->count() == 0) {
    listaMedidas->addItem("No se detectaron tablas de hechos");
  }
}
