#include "PanelDimensiones.h"
#include "styles/FlutterTheme.h"

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

  bool dark = FlutterTheme::instance().darkMode();
  QString bg = dark ? "#252526" : "#ffffff";
  QString text = dark ? "#DDD" : "#1e293b";
  QString border = dark ? "#3E3E42" : "#e2e8f0";

  arbolDimensiones->setStyleSheet(QString("QTreeWidget { background-color: %1; "
                                          "color: %2; border: 1px solid %3; }")
                                      .arg(bg, text, border));

  layout->addWidget(arbolDimensiones);

  cargarDimensionesReales();
}

void PanelDimensiones::cargarDimensionesReales() {
  arbolDimensiones->clear();

  QSqlDatabase db = QSqlDatabase::database("CuboVisionConnection");
  if (!db.isOpen()) {
    QTreeWidgetItem *err = new QTreeWidgetItem(arbolDimensiones);
    err->setText(0, "Sin conexión a BD");
    return;
  }

  QStringList tablas = db.tables();
  for (const QString &tabla : tablas) {
    if (!tabla.startsWith("pg_") &&
        !tabla.startsWith("sql_")) { // Filtrar sistema
      QTreeWidgetItem *itemTabla = new QTreeWidgetItem(arbolDimensiones);
      itemTabla->setText(0, tabla);
      itemTabla->setIcon(0, QIcon::fromTheme("folder"));

      QSqlRecord record = db.record(tabla);
      for (int i = 0; i < record.count(); i++) {
        QTreeWidgetItem *itemCol = new QTreeWidgetItem(itemTabla);
        itemCol->setText(0, record.fieldName(i));
        itemCol->setToolTip(0, QString(record.value(i).typeName()));
      }
    }
  }
  arbolDimensiones->expandAll();
}
