#include "PanelMedidas.h"
#include "styles/FlutterTheme.h"

PanelMedidas::PanelMedidas(QWidget *parent) : QWidget(parent) {
  configurarUi();
}

void PanelMedidas::configurarUi() {
  QVBoxLayout *layout = new QVBoxLayout(this);

  QLabel *lbl = new QLabel("Definición de Medidas", this);
  lbl->setStyleSheet("font-weight: bold; margin-bottom: 5px;");
  layout->addWidget(lbl);

  listaMedidas = new QListWidget(this);

  bool dark = FlutterTheme::instance().darkMode();
  QString bg = dark ? "#252526" : "#ffffff";
  QString text = dark ? "#DDD" : "#1e293b";
  QString border = dark ? "#3E3E42" : "#e2e8f0";

  listaMedidas->setStyleSheet(
      QString("QListWidget { background-color: %1; color: "
              "%2; border: 1px solid %3; }")
          .arg(bg, text, border));

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
