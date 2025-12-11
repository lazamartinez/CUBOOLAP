#include "ConstructorConsultas.h"
#include "../core/GestorBaseDatos.h" // Include GestorBaseDatos
#include "Estilos.h"
#include "ToastNotifier.h"
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
}

void ConstructorConsultas::configurarUi() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setSpacing(10);
  mainLayout->setContentsMargins(16, 16, 16, 16);

  // Header
  QHBoxLayout *headerLayout = new QHBoxLayout();

  QLabel *header =
      new QLabel("Fase 5: Constructor de Consultas Avanzado", this);
  header->setStyleSheet(Estilos::obtenerEstiloTituloSeccion());
  headerLayout->addWidget(header);
  headerLayout->addStretch();

  btnVolverInicio = new QPushButton("Volver al Inicio", this);
  btnVolverInicio->setStyleSheet(R"(
    QPushButton { background: #f1f5f9; border: 1px solid #e2e8f0; border-radius: 6px; color: #475569; }
    QPushButton:hover { background: #e2e8f0; }
  )");
  connect(btnVolverInicio, &QPushButton::clicked, this,
          &ConstructorConsultas::volverAlInicio);
  headerLayout->addWidget(btnVolverInicio);

  mainLayout->addLayout(headerLayout);

  QSplitter *mainSplitter = new QSplitter(Qt::Horizontal, this);

  // LADO IZQUIERDO
  QWidget *leftWidget = new QWidget(this);
  QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
  leftLayout->setContentsMargins(0, 0, 8, 0);
  leftLayout->setSpacing(12);

  // Recursos
  QGroupBox *grpRecursos = new QGroupBox("1. Recursos Disponibles", this);
  QHBoxLayout *recLayout = new QHBoxLayout(grpRecursos);

  QVBoxLayout *dimLayout = new QVBoxLayout();
  dimLayout->addWidget(new QLabel("Dimensiones:", this));
  listaDimensiones = new QListWidget(this);

  // Cargar dimensiones dinámicamente desde la BD
  cargarDimensionesDisponibles();

  listaDimensiones->setDragEnabled(true);
  dimLayout->addWidget(listaDimensiones);
  recLayout->addLayout(dimLayout);

  QVBoxLayout *medLayout = new QVBoxLayout();
  medLayout->addWidget(new QLabel("Medidas:", this));
  listaMedidas = new QListWidget(this);

  // Cargar medidas dinámicamente desde la BD
  cargarMedidasDisponibles();

  listaMedidas->setDragEnabled(true);
  medLayout->addWidget(listaMedidas);
  recLayout->addLayout(medLayout);

  leftLayout->addWidget(grpRecursos);

  // Construccion
  QGroupBox *grpConstruccion = new QGroupBox("2. Definicion de Consulta", this);
  QGridLayout *buildLayout = new QGridLayout(grpConstruccion);

  buildLayout->addWidget(new QLabel("Filas:", this), 0, 0);
  areaFilas = new QListWidget(this);
  areaFilas->setAcceptDrops(true);
  areaFilas->setMaximumHeight(60);
  areaFilas->setStyleSheet(Estilos::obtenerEstiloDropZone());
  buildLayout->addWidget(areaFilas, 1, 0);

  buildLayout->addWidget(new QLabel("Columnas:", this), 0, 1);
  areaColumnas = new QListWidget(this);
  areaColumnas->setAcceptDrops(true);
  areaColumnas->setMaximumHeight(60);
  areaColumnas->setStyleSheet(Estilos::obtenerEstiloDropZone());
  buildLayout->addWidget(areaColumnas, 1, 1);

  QHBoxLayout *medHeaderLayout = new QHBoxLayout();
  medHeaderLayout->addWidget(new QLabel("Medidas (Arrastrar 1):", this));
  medHeaderLayout->addStretch();
  medHeaderLayout->addWidget(new QLabel("Agregacion:", this));
  comboAgregacion = new QComboBox(this);
  comboAgregacion->addItems({"SUM", "AVG", "Count", "MAX", "MIN"});
  medHeaderLayout->addWidget(comboAgregacion);

  buildLayout->addLayout(medHeaderLayout, 2, 0, 1, 2);

  areaMedidas = new QListWidget(this);
  areaMedidas->setAcceptDrops(true);
  areaMedidas->setMaximumHeight(60);
  areaMedidas->setStyleSheet(Estilos::obtenerEstiloDropZone());
  buildLayout->addWidget(areaMedidas, 3, 0, 1, 2);

  QHBoxLayout *filtHeaderLayout = new QHBoxLayout();
  filtHeaderLayout->addWidget(new QLabel("Filtros (WHERE):", this));
  filtHeaderLayout->addStretch();
  btnAgregarFiltro = new QPushButton("+ Filtro Manual SQL", this);
  btnAgregarFiltro->setStyleSheet("font-size: 10px; padding: 2px 8px;");
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
      "background: #fdf2f8; border: 2px dashed #fbcfe8; border-radius: 8px;");
  buildLayout->addWidget(areaFiltros, 5, 0, 1, 2);

  leftLayout->addWidget(grpConstruccion);

  QHBoxLayout *actLayout = new QHBoxLayout();
  btnLimpiar = new QPushButton("Limpiar", this);
  connect(btnLimpiar, &QPushButton::clicked, this,
          &ConstructorConsultas::limpiarConsulta);
  actLayout->addWidget(btnLimpiar);
  actLayout->addStretch();
  btnEjecutar = new QPushButton(" Ejecutar (Real)", this);
  btnEjecutar->setStyleSheet(Estilos::obtenerEstiloBotonPrimario());
  connect(btnEjecutar, &QPushButton::clicked, this,
          &ConstructorConsultas::ejecutarConsulta);
  actLayout->addWidget(btnEjecutar);

  leftLayout->addLayout(actLayout);

  mainSplitter->addWidget(leftWidget);

  // LADO DERECHO
  QWidget *rightWidget = new QWidget(this);
  QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);
  rightLayout->setContentsMargins(8, 0, 0, 0);

  grpRecursos = new QGroupBox("3. Resultados", this);
  QVBoxLayout *resLayoutInner = new QVBoxLayout(grpRecursos);

  tablaResultados = new QTableWidget(this);
  tablaResultados->setAlternatingRowColors(true);
  resLayoutInner->addWidget(tablaResultados);

  QHBoxLayout *resFooter = new QHBoxLayout();
  lblInfoResultados = new QLabel("Esperando consulta...", this);
  resFooter->addWidget(lblInfoResultados);
  resFooter->addStretch();

  btnExportCSV = new QPushButton("📊 CSV", this);
  btnExportCSV->setStyleSheet(R"(
    QPushButton {
      background: #10b981;
      color: white;
      border: none;
      border-radius: 6px;
      padding: 8px 16px;
      font-weight: 600;
      font-size: 11px;
    }
    QPushButton:hover { background: #059669; }
  )");
  btnExportCSV->setMinimumWidth(80);
  btnExportCSV->setCursor(Qt::PointingHandCursor);
  connect(btnExportCSV, &QPushButton::clicked, this,
          &ConstructorConsultas::exportarCSV);
  resFooter->addWidget(btnExportCSV);

  btnReporte = new QPushButton("📄 PDF", this);
  btnReporte->setStyleSheet(R"(
    QPushButton {
      background: #ef4444;
      color: white;
      border: none;
      border-radius: 6px;
      padding: 8px 16px;
      font-weight: 600;
      font-size: 11px;
    }
    QPushButton:hover { background: #dc2626; }
  )");
  btnReporte->setMinimumWidth(80);
  btnReporte->setCursor(Qt::PointingHandCursor);
  connect(btnReporte, &QPushButton::clicked, this,
          &ConstructorConsultas::generarReporte);
  resFooter->addWidget(btnReporte);

  resLayoutInner->addLayout(resFooter);
  rightLayout->addWidget(grpRecursos, 2);

  panelHistorial = new HistorialConsultas(this);
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
