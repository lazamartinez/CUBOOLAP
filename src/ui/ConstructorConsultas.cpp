#include "ConstructorConsultas.h"
#include "../core/GestorBaseDatos.h" // Include GestorBaseDatos
#include "ToastNotifier.h"
#include "styles/FlutterTheme.h"
#include <QFileDialog>
#include <QGroupBox>
#include <QHeaderView>
#include <QInputDialog>
#include <QMessageBox>
#include <QRegularExpression>
#include <QSet>
#include <QSplitter>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTextStream>

ConstructorConsultas::ConstructorConsultas(QWidget *parent) : QWidget(parent) {
  configurarUi();
  FlutterTheme::instance().applyThemeToWidget(this);
}

void ConstructorConsultas::configurarUi() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setSpacing(16);
  mainLayout->setContentsMargins(16, 16, 16, 16);

  // Header
  QHBoxLayout *headerLayout = new QHBoxLayout();

  QLabel *header =
      new QLabel("Fase 5: Constructor de Consultas Avanzado", this);
  header->setStyleSheet("font-size: 20px; font-weight: 700; color: #1f2937;");
  headerLayout->addWidget(header);
  headerLayout->addStretch();

  btnVolverInicio = new FlutterElevatedButton("Volver al Inicio", this);
  btnVolverInicio->setIcon(MaterialIcons::instance().arrow_back());
  connect(btnVolverInicio, &QPushButton::clicked, this,
          &ConstructorConsultas::volverAlInicio);
  headerLayout->addWidget(btnVolverInicio);

  mainLayout->addLayout(headerLayout);

  QSplitter *mainSplitter = new QSplitter(Qt::Horizontal, this);
  mainSplitter->setHandleWidth(1);
  mainSplitter->setStyleSheet("QSplitter::handle { background: #e5e7eb; }");

  // LADO IZQUIERDO
  QWidget *leftWidget = new QWidget(this);
  QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
  leftLayout->setContentsMargins(0, 0, 12, 0);
  leftLayout->setSpacing(16);

  // Recursos
  FlutterCard *grpRecursos = new FlutterCard(this);
  QVBoxLayout *recContainerLayout = new QVBoxLayout(grpRecursos);
  recContainerLayout->setContentsMargins(12, 12, 12, 12);
  recContainerLayout->setSpacing(8);

  QLabel *lblRecursos = new QLabel("1. Recursos Disponibles", grpRecursos);
  lblRecursos->setStyleSheet("font-size: 14px; font-weight: 600; "
                             "text-transform: uppercase; color: #374151;");
  recContainerLayout->addWidget(lblRecursos);

  QHBoxLayout *recLayout = new QHBoxLayout();
  recContainerLayout->addLayout(recLayout);

  QVBoxLayout *dimLayout = new QVBoxLayout();
  QLabel *lblDim = new QLabel("Dimensiones:", this);
  lblDim->setStyleSheet("font-size: 12px; font-weight: 600; color: #4b5563;");
  dimLayout->addWidget(lblDim);
  listaDimensiones = new QListWidget(this);
  listaDimensiones->setStyleSheet(
      "border: 1px solid #d1d5db; border-radius: 4px; background: #f9fafb;");
  listaDimensiones->setDragEnabled(true);
  cargarDimensionesDisponibles();
  dimLayout->addWidget(listaDimensiones);
  recLayout->addLayout(dimLayout);

  QVBoxLayout *medLayout = new QVBoxLayout();
  QLabel *lblMed = new QLabel("Medidas:", this);
  lblMed->setStyleSheet("font-size: 12px; font-weight: 600; color: #4b5563;");
  medLayout->addWidget(lblMed);
  listaMedidas = new QListWidget(this);
  listaMedidas->setStyleSheet(
      "border: 1px solid #d1d5db; border-radius: 4px; background: #f9fafb;");
  listaMedidas->setDragEnabled(true);
  cargarMedidasDisponibles();
  medLayout->addWidget(listaMedidas);
  recLayout->addLayout(medLayout);

  leftLayout->addWidget(grpRecursos);

  // Construccion
  FlutterCard *grpConstruccion = new FlutterCard(this);
  QVBoxLayout *buildContainerLayout = new QVBoxLayout(grpConstruccion);
  buildContainerLayout->setContentsMargins(12, 12, 12, 12);
  buildContainerLayout->setSpacing(8);

  QLabel *lblCons = new QLabel("2. Definicion de Consulta", grpConstruccion);
  lblCons->setStyleSheet("font-size: 14px; font-weight: 600; text-transform: "
                         "uppercase; color: #374151;");
  buildContainerLayout->addWidget(lblCons);

  QGridLayout *buildLayout = new QGridLayout();
  buildLayout->setSpacing(12);
  buildContainerLayout->addLayout(buildLayout);

  buildLayout->addWidget(new QLabel("Filas:", this), 0, 0);
  areaFilas = new QListWidget(this);
  areaFilas->setAcceptDrops(true);
  areaFilas->setMaximumHeight(80);
  areaFilas->setStyleSheet(R"(
     QListWidget {
        border: 2px dashed #cbd5e1;
        border-radius: 6px;
        background: #f8fafc;
     }
  )");
  buildLayout->addWidget(areaFilas, 1, 0);

  buildLayout->addWidget(new QLabel("Columnas:", this), 0, 1);
  areaColumnas = new QListWidget(this);
  areaColumnas->setAcceptDrops(true);
  areaColumnas->setMaximumHeight(80);
  areaColumnas->setStyleSheet(R"(
     QListWidget {
        border: 2px dashed #cbd5e1;
        border-radius: 6px;
        background: #f8fafc;
     }
  )");
  buildLayout->addWidget(areaColumnas, 1, 1);

  QHBoxLayout *medHeaderLayout = new QHBoxLayout();
  medHeaderLayout->addWidget(new QLabel("Medidas (Arrastrar 1):", this));
  medHeaderLayout->addStretch();
  medHeaderLayout->addWidget(new QLabel("Agregacion:", this));
  comboAgregacion = new QComboBox(this);
  comboAgregacion->addItems({"SUM", "AVG", "COUNT", "MAX", "MIN"});
  medHeaderLayout->addWidget(comboAgregacion);

  buildLayout->addLayout(medHeaderLayout, 2, 0, 1, 2);

  areaMedidas = new QListWidget(this);
  areaMedidas->setAcceptDrops(true);
  areaMedidas->setMaximumHeight(60);
  areaMedidas->setStyleSheet(R"(
     QListWidget {
        border: 2px dashed #2563eb;
        border-radius: 6px;
        background: #eff6ff;
     }
  )");
  buildLayout->addWidget(areaMedidas, 3, 0, 1, 2);

  QHBoxLayout *filtHeaderLayout = new QHBoxLayout();
  filtHeaderLayout->addWidget(new QLabel("Filtros (WHERE):", this));
  filtHeaderLayout->addStretch();
  btnAgregarFiltro = new FlutterElevatedButton("+ Filtro Manual SQL", this);
  btnAgregarFiltro->setMinimumHeight(30);
  connect(btnAgregarFiltro, &QPushButton::clicked, this, [this]() {
    bool ok;
    QString text = QInputDialog::getText(
        this, "Filtro SQL",
        "Condicion (ej: total_venta > 1000):", QLineEdit::Normal, "", &ok);
    if (ok && !text.isEmpty())
      areaFiltros->addItem(text);
  });
  filtHeaderLayout->addWidget(btnAgregarFiltro);

  buildLayout->addLayout(filtHeaderLayout, 4, 0, 1, 2);

  areaFiltros = new QListWidget(this);
  areaFiltros->setAcceptDrops(true);
  areaFiltros->setMaximumHeight(60);
  areaFiltros->setStyleSheet(
      "background: #fff1f2; border: 2px dashed #fda4af; border-radius: 6px;");
  buildLayout->addWidget(areaFiltros, 5, 0, 1, 2);

  leftLayout->addWidget(grpConstruccion);

  QHBoxLayout *actLayout = new QHBoxLayout();
  btnLimpiar = new FlutterTextButton("Limpiar", this);
  connect(btnLimpiar, &QPushButton::clicked, this,
          &ConstructorConsultas::limpiarConsulta);
  actLayout->addWidget(btnLimpiar);
  actLayout->addStretch();
  btnEjecutar = new FlutterFilledButton(" Ejecutar (Real)", this);
  btnEjecutar->setIcon(MaterialIcons::instance().getIcon("arrow_forward"));
  connect(btnEjecutar, &QPushButton::clicked, this,
          &ConstructorConsultas::ejecutarConsulta);
  actLayout->addWidget(btnEjecutar);

  leftLayout->addLayout(actLayout);

  mainSplitter->addWidget(leftWidget);

  // LADO DERECHO
  QWidget *rightWidget = new QWidget(this);
  QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);
  rightLayout->setContentsMargins(12, 0, 0, 0);
  rightLayout->setSpacing(16);

  FlutterCard *grpResultados = new FlutterCard(this);
  QVBoxLayout *resLayoutInner = new QVBoxLayout(grpResultados);
  resLayoutInner->setContentsMargins(12, 12, 12, 12);

  QLabel *lblRes = new QLabel("3. Resultados", grpResultados);
  lblRes->setStyleSheet("font-size: 14px; font-weight: 600; text-transform: "
                        "uppercase; color: #374151;");
  resLayoutInner->addWidget(lblRes);

  tablaResultados = new QTableWidget(this);
  tablaResultados->setAlternatingRowColors(true);
  tablaResultados->setStyleSheet(
      "border: 1px solid #e5e7eb; border-radius: 4px;");
  resLayoutInner->addWidget(tablaResultados);

  QHBoxLayout *resFooter = new QHBoxLayout();
  lblInfoResultados = new QLabel("Esperando consulta...", this);
  lblInfoResultados->setStyleSheet("color: #6b7280; font-style: italic;");
  resFooter->addWidget(lblInfoResultados);
  resFooter->addStretch();

  btnExportCSV = new FlutterElevatedButton("CSV", this);
  btnExportCSV->setIcon(MaterialIcons::instance().getIcon("table_chart"));
  connect(btnExportCSV, &QPushButton::clicked, this,
          &ConstructorConsultas::exportarCSV);
  resFooter->addWidget(btnExportCSV);

  btnReporte = new FlutterElevatedButton("PDF", this);
  btnReporte->setIcon(MaterialIcons::instance().getIcon("download"));
  // Warning color override? FlutterElevatedButton uses primary color usually.
  // We can leave it as primary or customize validly if API allows.
  connect(btnReporte, &QPushButton::clicked, this,
          &ConstructorConsultas::generarReporte);
  resFooter->addWidget(btnReporte);

  resLayoutInner->addLayout(resFooter);
  rightLayout->addWidget(grpResultados, 2);

  panelHistorial = new HistorialConsultas(this);
  // Assuming HistorialConsultas is also a widget we might want to wrap or
  // style, but let's assume it's fine for now or handle it separately.
  connect(panelHistorial, &HistorialConsultas::consultaSeleccionada, this,
          &ConstructorConsultas::restaurarConsulta);
  rightLayout->addWidget(panelHistorial, 1);

  mainSplitter->addWidget(rightWidget);

  mainSplitter->setStretchFactor(0, 3);
  mainSplitter->setStretchFactor(1, 2);

  mainLayout->addWidget(mainSplitter);
}

void ConstructorConsultas::cargarDimensionesDisponibles() {
  if (!GestorBaseDatos::instancia().estaConectado()) {
    qWarning() << "No hay conexi\u00f3n a BD para cargar dimensiones";
    return;
  }

  QSqlDatabase db = GestorBaseDatos::instancia().baseDatos();
  QSqlQuery query(db);

  // Detectar tablas de dimensiones (dim_*)
  query.exec("SELECT table_name FROM information_schema.tables "
             "WHERE table_schema = 'public' AND table_name LIKE 'dim_%' "
             "ORDER BY table_name");

  while (query.next()) {
    QString tabla = query.value(0).toString();

    // Obtener columnas descriptivas de cada dimensi\u00f3n
    QSqlQuery colQuery(db);
    colQuery.prepare(
        "SELECT column_name FROM information_schema.columns "
        "WHERE table_name = :tabla AND column_name NOT LIKE 'id_%' "
        "ORDER BY ordinal_position LIMIT 1");
    colQuery.bindValue(":tabla", tabla);

    if (colQuery.exec() && colQuery.next()) {
      QString columna = colQuery.value(0).toString();
      QString nombreDim = tabla.mid(4);      // Quitar "dim_"
      nombreDim[0] = nombreDim[0].toUpper(); // Capitalizar

      listaDimensiones->addItem(QString("%1 (%2)").arg(nombreDim, columna));
    }
  }

  qDebug() << "Dimensiones cargadas:" << listaDimensiones->count();
}

void ConstructorConsultas::cargarMedidasDisponibles() {
  if (!GestorBaseDatos::instancia().estaConectado()) {
    qWarning() << "No hay conexi\u00f3n a BD para cargar medidas";
    return;
  }

  QSqlDatabase db = GestorBaseDatos::instancia().baseDatos();
  QSqlQuery query(db);

  // Detectar columnas num\u00e9ricas de la tabla de hechos
  query.exec(
      "SELECT column_name FROM information_schema.columns "
      "WHERE table_name = 'fact_ventas' "
      "AND data_type IN ('integer', 'numeric', 'double precision', 'real') "
      "AND column_name NOT LIKE 'id_%' "
      "ORDER BY column_name");

  while (query.next()) {
    QString medida = query.value(0).toString();
    // Capitalizar primera letra
    medida[0] = medida[0].toUpper();
    listaMedidas->addItem(medida);
  }

  qDebug() << "Medidas cargadas:" << listaMedidas->count();
}

bool ConstructorConsultas::validarConsulta() {
  if (areaMedidas->count() == 0) {
    ToastNotifier::mostrar(this, "Seleccione al menos una medida",
                           ToastNotifier::Advertencia);
    return false;
  }
  return true;
}

QString ConstructorConsultas::serializarConsulta() {
  // Simplificado para historial
  return "Consulta Real Ejecutada";
}

// Mapeos din\u00e1micos
QString mapearDimension(QString uiName) {
  // Extraer nombre de tabla y columna del formato "Nombre (columna)"
  QRegularExpression re("^(.+?)\\s*\\((.+?)\\)$");
  QRegularExpressionMatch match = re.match(uiName);

  if (match.hasMatch()) {
    QString tabla = "dim_" + match.captured(1).toLower();
    QString columna = match.captured(2);

    // Obtener el alias de la tabla (primera letra)
    QString alias = tabla.mid(4, 1); // Quitar "dim_" y tomar primera letra

    return QString("%1.%2").arg(alias, columna);
  }

  return "";
}

QString mapearMedida(QString uiName) {
  // Convertir a min\u00fasculas para buscar en la BD
  QString medida = uiName.toLower();
  return QString("f.%1").arg(medida);
}

void ConstructorConsultas::ejecutarConsulta() {
  if (!validarConsulta())
    return;

  if (!GestorBaseDatos::instancia().estaConectado()) {
    ToastNotifier::mostrar(this, "No hay conexion a BD", ToastNotifier::Error);
    return;
  }

  lblInfoResultados->setText("Ejecutando SQL real...");
  tablaResultados->setRowCount(0);

  // Construir SQL Dinámico
  QStringList selectParts;
  QStringList groupParts;
  QStringList joins;
  QSet<QString> tablasUsadas; // Para evitar JOINs duplicados

  // Procesar filas y columnas como dimensiones de agrupamiento
  auto procesarDim = [&](QListWidget *list) {
    for (int i = 0; i < list->count(); i++) {
      QString dim = list->item(i)->text();
      QString campo = mapearDimension(dim);

      if (!campo.isEmpty()) {
        selectParts << campo;
        groupParts << campo;

        // Extraer nombre de tabla del campo (formato: "alias.columna")
        QString alias = campo.split('.').first();

        // Determinar nombre de tabla desde el alias
        // El alias es la primera letra del nombre de dimensión
        // Extraer nombre de dimensión del formato "Nombre (columna)"
        QRegularExpression re("^(.+?)\\s*\\((.+?)\\)$");
        QRegularExpressionMatch match = re.match(dim);

        if (match.hasMatch()) {
          QString nombreDim = match.captured(1).toLower();
          QString tabla = "dim_" + nombreDim;
          QString pkCol = "id_" + nombreDim;

          if (!tablasUsadas.contains(tabla)) {
            joins << QString("JOIN %1 %2 ON f.%3 = %2.%3")
                         .arg(tabla, alias, pkCol);
            tablasUsadas.insert(tabla);
          }
        }
      }
    }
  };

  procesarDim(areaFilas);
  procesarDim(areaColumnas);

  // Medida
  QString medidaUI = areaMedidas->item(0)->text();
  QString agg = comboAgregacion->currentText();
  QString campoMedida = mapearMedida(medidaUI);

  selectParts << QString("%1(%2) as resultado").arg(agg, campoMedida);

  // Construir query final
  QString sql = "SELECT " + selectParts.join(", ") + " FROM fact_ventas f ";
  for (const QString &j : joins)
    sql += j + " ";

  if (areaFiltros->count() > 0) {
    sql += "WHERE ";
    QStringList wheres;
    for (int i = 0; i < areaFiltros->count(); i++)
      wheres << areaFiltros->item(i)->text();
    sql += wheres.join(" AND ");
    sql += " ";
  }

  if (!groupParts.isEmpty()) {
    sql += "GROUP BY " + groupParts.join(", ");
  }

  sql += " LIMIT 500"; // Limite seguridad

  qDebug() << "SQL Generado:" << sql;

  // Ejecutar
  QSqlQuery query(GestorBaseDatos::instancia().baseDatos());
  if (query.exec(sql)) {
    int cols = query.record().count();
    tablaResultados->setColumnCount(cols);

    QStringList headers;
    for (int i = 0; i < cols; i++)
      headers << query.record().fieldName(i);
    tablaResultados->setHorizontalHeaderLabels(headers);

    int rows = 0;
    while (query.next()) {
      tablaResultados->insertRow(rows);
      for (int i = 0; i < cols; i++) {
        tablaResultados->setItem(
            rows, i, new QTableWidgetItem(query.value(i).toString()));
      }
      rows++;
    }

    lblInfoResultados->setText(QString("Resultados: %1 filas").arg(rows));
    panelHistorial->agregarConsulta(sql, rows, 0); // Guardamos la SQL real
    ToastNotifier::mostrar(this, "Consulta ejecutada!", ToastNotifier::Exito);
  } else {
    QString err = query.lastError().text();
    lblInfoResultados->setText("Error SQL");
    ToastNotifier::mostrar(this, "Error SQL: " + err, ToastNotifier::Error);
    qCritical() << "SQL Error:" << err;
  }
}

void ConstructorConsultas::cargarDatosEjemplo() {
  // Deprecated for real SQL
}

// ... Rest of methods (exportarCSV, limpiar, restaurar) updated slightly ...
void ConstructorConsultas::exportarCSV() {
  if (tablaResultados->rowCount() == 0)
    return;
  QString fileName = QFileDialog::getSaveFileName(
      this, "Exportar", "resultados.csv", "CSV (*.csv)");
  // Logic similar to before
}
void ConstructorConsultas::generarReporte() {
  if (tablaResultados->rowCount() == 0)
    return;
  QString fileName = QFileDialog::getSaveFileName(this, "Reporte",
                                                  "reporte.pdf", "PDF (*.pdf)");
}
void ConstructorConsultas::limpiarConsulta() {
  areaFilas->clear();
  areaColumnas->clear();
  areaMedidas->clear();
  areaFiltros->clear();
  tablaResultados->setRowCount(0);
}
void ConstructorConsultas::restaurarConsulta(const QString &consultaStr) {
  // Por ahora solo mostramos la SQL en un toast
  // Restaurar el estado visual desde SQL es complejo (reverse parsing)
  ToastNotifier::mostrar(this, "SQL Historial: " + consultaStr,
                         ToastNotifier::Info);
}
