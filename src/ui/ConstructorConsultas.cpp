#include "ConstructorConsultas.h"
#include "Estilos.h"
#include "ToastNotifier.h"
#include <QFileDialog>
#include <QGroupBox>
#include <QHeaderView>
#include <QInputDialog>
#include <QMessageBox>
#include <QSplitter>
#include <QTextStream>

ConstructorConsultas::ConstructorConsultas(QWidget *parent) : QWidget(parent) {
  configurarUi();
}

void ConstructorConsultas::configurarUi() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setSpacing(10);
  mainLayout->setContentsMargins(16, 16, 16, 16);

  // Header con navegacion
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

  // Splitter principal
  QSplitter *mainSplitter = new QSplitter(Qt::Horizontal, this);

  // LADO IZQUIERDO: Constructor
  QWidget *leftWidget = new QWidget(this);
  QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
  leftLayout->setContentsMargins(0, 0, 8, 0);
  leftLayout->setSpacing(12);

  // --- Recursos ---
  QGroupBox *grpRecursos = new QGroupBox("1. Recursos Disponibles", this);
  QHBoxLayout *recLayout = new QHBoxLayout(grpRecursos);

  QVBoxLayout *dimLayout = new QVBoxLayout();
  dimLayout->addWidget(new QLabel("Dimensiones:", this));
  listaDimensiones = new QListWidget(this);
  listaDimensiones->addItems(
      {"Tiempo", "Producto", "Cliente", "Geografia", "Vendedor", "Categoria"});
  listaDimensiones->setDragEnabled(true);
  dimLayout->addWidget(listaDimensiones);
  recLayout->addLayout(dimLayout);

  QVBoxLayout *medLayout = new QVBoxLayout();
  medLayout->addWidget(new QLabel("Medidas:", this));
  listaMedidas = new QListWidget(this);
  listaMedidas->addItems(
      {"Ventas", "Costo", "Beneficio", "Cantidad", "Descuento", "Margen %"});
  listaMedidas->setDragEnabled(true);
  medLayout->addWidget(listaMedidas);
  recLayout->addLayout(medLayout);

  leftLayout->addWidget(grpRecursos);

  // --- Construccion ---
  QGroupBox *grpConstruccion = new QGroupBox("2. Definicion de Consulta", this);
  QGridLayout *buildLayout = new QGridLayout(grpConstruccion);

  // Filas
  buildLayout->addWidget(new QLabel("Filas:", this), 0, 0);
  areaFilas = new QListWidget(this);
  areaFilas->setAcceptDrops(true);
  areaFilas->setMaximumHeight(60);
  areaFilas->setStyleSheet(Estilos::obtenerEstiloDropZone());
  buildLayout->addWidget(areaFilas, 1, 0);

  // Columnas
  buildLayout->addWidget(new QLabel("Columnas:", this), 0, 1);
  areaColumnas = new QListWidget(this);
  areaColumnas->setAcceptDrops(true);
  areaColumnas->setMaximumHeight(60);
  areaColumnas->setStyleSheet(Estilos::obtenerEstiloDropZone());
  buildLayout->addWidget(areaColumnas, 1, 1);

  // Medidas y Agregacion
  QHBoxLayout *medHeaderLayout = new QHBoxLayout();
  medHeaderLayout->addWidget(new QLabel("Medidas:", this));
  medHeaderLayout->addStretch();
  medHeaderLayout->addWidget(new QLabel("Agregacion:", this));
  comboAgregacion = new QComboBox(this);
  comboAgregacion->addItems({"SUM (Suma)", "AVG (Promedio)", "MAX (Maximo)",
                             "MIN (Minimo)", "COUNT (Conteo)"});
  medHeaderLayout->addWidget(comboAgregacion);

  buildLayout->addLayout(medHeaderLayout, 2, 0, 1, 2);

  areaMedidas = new QListWidget(this);
  areaMedidas->setAcceptDrops(true);
  areaMedidas->setMaximumHeight(60);
  areaMedidas->setStyleSheet(Estilos::obtenerEstiloDropZone());
  buildLayout->addWidget(areaMedidas, 3, 0, 1, 2);

  // Filtros
  QHBoxLayout *filtHeaderLayout = new QHBoxLayout();
  filtHeaderLayout->addWidget(new QLabel("Filtros (Opcional):", this));
  filtHeaderLayout->addStretch();
  btnAgregarFiltro = new QPushButton("+ Agregar Filtro Manual", this);
  btnAgregarFiltro->setStyleSheet("font-size: 10px; padding: 2px 8px;");
  connect(btnAgregarFiltro, &QPushButton::clicked, this, [this]() {
    bool ok;
    QString text = QInputDialog::getText(
        this, "Agregar Filtro",
        "Condicion (ej: Ventas > 1000):", QLineEdit::Normal, "", &ok);
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
  areaFiltros->setToolTip(
      "Arrastra dimensiones aqui para filtrar o usa el boton");
  buildLayout->addWidget(areaFiltros, 5, 0, 1, 2);

  leftLayout->addWidget(grpConstruccion);

  // --- Acciones ---
  QHBoxLayout *actLayout = new QHBoxLayout();

  btnLimpiar = new QPushButton("Limpiar Todo", this);
  connect(btnLimpiar, &QPushButton::clicked, this,
          &ConstructorConsultas::limpiarConsulta);
  actLayout->addWidget(btnLimpiar);

  actLayout->addStretch();

  btnEjecutar = new QPushButton(" Ejecutar Consulta", this);
  btnEjecutar->setStyleSheet(Estilos::obtenerEstiloBotonPrimario());
  connect(btnEjecutar, &QPushButton::clicked, this,
          &ConstructorConsultas::ejecutarConsulta);
  actLayout->addWidget(btnEjecutar);

  leftLayout->addLayout(actLayout);

  mainSplitter->addWidget(leftWidget);

  // LADO DERECHO: Resultados e Historial
  QWidget *rightWidget = new QWidget(this);
  QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);
  rightLayout->setContentsMargins(8, 0, 0, 0);

  // Tabla Resultados
  grpRecursos = new QGroupBox(
      "3. Resultados", this); // Reutilizando puntero grpRecursos pero es nuevo
  QVBoxLayout *resLayoutInner = new QVBoxLayout(grpRecursos);

  tablaResultados = new QTableWidget(this);
  tablaResultados->setColumnCount(4);
  tablaResultados->setHorizontalHeaderLabels(
      {"Dimension", "Columna", "Valor", "Metrica"});
  tablaResultados->horizontalHeader()->setStretchLastSection(true);
  tablaResultados->setAlternatingRowColors(true);
  resLayoutInner->addWidget(tablaResultados);

  QHBoxLayout *resFooter = new QHBoxLayout();
  lblInfoResultados = new QLabel("Esperando consulta...", this);
  resFooter->addWidget(lblInfoResultados);
  resFooter->addStretch();

  btnExportCSV = new QPushButton("CSV", this);
  btnExportCSV->setStyleSheet(Estilos::obtenerEstiloBotonExito());
  btnExportCSV->setMaximumWidth(60);
  connect(btnExportCSV, &QPushButton::clicked, this,
          &ConstructorConsultas::exportarCSV);
  resFooter->addWidget(btnExportCSV);

  btnReporte = new QPushButton("PDF", this);
  btnReporte->setStyleSheet(
      "background: #ef4444; color: white; border-radius: 6px; padding: 6px;");
  btnReporte->setMaximumWidth(60);
  connect(btnReporte, &QPushButton::clicked, this,
          &ConstructorConsultas::generarReporte);
  resFooter->addWidget(btnReporte);

  resLayoutInner->addLayout(resFooter);
  rightLayout->addWidget(grpRecursos, 2); // Stretch 2

  // Historial
  panelHistorial = new HistorialConsultas(this);
  connect(panelHistorial, &HistorialConsultas::consultaSeleccionada, this,
          &ConstructorConsultas::restaurarConsulta);
  rightLayout->addWidget(panelHistorial, 1); // Stretch 1

  mainSplitter->addWidget(rightWidget);

  // Ajustar anchos
  mainSplitter->setStretchFactor(0, 3);
  mainSplitter->setStretchFactor(1, 2);

  mainLayout->addWidget(mainSplitter);
}

bool ConstructorConsultas::validarConsulta() {
  QStringList errores;

  if (areaFilas->count() == 0 && areaColumnas->count() == 0)
    errores << "Debe seleccionar al menos una dimension (Filas o Columnas)";

  if (areaMedidas->count() == 0)
    errores << "Debe seleccionar al menos una medida";

  // Validar duplicados
  QSet<QString> usados;
  for (int i = 0; i < areaFilas->count(); i++) {
    if (usados.contains(areaFilas->item(i)->text()))
      errores << "Dimension duplicada en Filas";
    usados.insert(areaFilas->item(i)->text());
  }
  for (int i = 0; i < areaColumnas->count(); i++) {
    if (usados.contains(areaColumnas->item(i)->text()))
      errores << "Dimension duplicada en Columnas";
    usados.insert(areaColumnas->item(i)->text());
  }

  if (!errores.isEmpty()) {
    ToastNotifier::mostrar(this, "Error de validacion: " + errores.first(),
                           ToastNotifier::Advertencia);
    return false;
  }
  return true;
}

QString ConstructorConsultas::serializarConsulta() {
  QStringList parts;
  parts << "FILAS:[";
  QStringList filas;
  for (int i = 0; i < areaFilas->count(); i++)
    filas << areaFilas->item(i)->text();
  parts << filas.join(",") << "] ";

  parts << "COLS:[";
  QStringList cols;
  for (int i = 0; i < areaColumnas->count(); i++)
    cols << areaColumnas->item(i)->text();
  parts << cols.join(",") << "] ";

  parts << "MEDS:[";
  QStringList meds;
  for (int i = 0; i < areaMedidas->count(); i++)
    meds << areaMedidas->item(i)->text();
  parts << meds.join(",") << "] ";

  parts << "AGGR:" << comboAgregacion->currentText() << " ";

  if (areaFiltros->count() > 0) {
    parts << "FILTROS:[";
    QStringList filts;
    for (int i = 0; i < areaFiltros->count(); i++)
      filts << areaFiltros->item(i)->text();
    parts << filts.join(",") << "]";
  }

  return parts.join("");
}

void ConstructorConsultas::ejecutarConsulta() {
  if (!validarConsulta())
    return;

  lblInfoResultados->setText("Ejecutando...");
  cargarDatosEjemplo();

  // Guardar en historial
  QString queryStr = serializarConsulta();
  int rows = tablaResultados->rowCount();
  int ms = rand() % 100 + 20;

  panelHistorial->agregarConsulta(queryStr, rows, ms);
  lblInfoResultados->setText(
      QString("%1 registros | %2ms | %3")
          .arg(rows)
          .arg(ms)
          .arg(comboAgregacion->currentText().split(" ").first()));

  ToastNotifier::mostrar(this, "Consulta ejecutada exitosamente",
                         ToastNotifier::Exito);
}

void ConstructorConsultas::restaurarConsulta(const QString &consultaStr) {
  limpiarConsulta();

  // Parser super basico
  // FILAS:[A,B] COLS:[C] MEDS:[D] AGGR:SUM FILTROS:[F]

  // En una app real usaria JSON o un parser mejor
  // Aqui solo simularemos la carga para demostracion

  ToastNotifier::mostrar(this, "Restaurando consulta...", ToastNotifier::Info);

  // Hack para extraer items y llenar listas
  QString temp = consultaStr;

  // Extraer FILAS
  int idxFilas = temp.indexOf("FILAS:[");
  int idxFilasEnd = temp.indexOf("]", idxFilas);
  if (idxFilas != -1) {
    QString content = temp.mid(idxFilas + 7, idxFilasEnd - (idxFilas + 7));
    for (const QString &s : content.split(",", Qt::SkipEmptyParts))
      areaFilas->addItem(s);
  }

  // Extraer COLS
  int idxCols = temp.indexOf("COLS:[");
  int idxColsEnd = temp.indexOf("]", idxCols);
  if (idxCols != -1) {
    QString content = temp.mid(idxCols + 6, idxColsEnd - (idxCols + 6));
    for (const QString &s : content.split(",", Qt::SkipEmptyParts))
      areaColumnas->addItem(s);
  }

  // Extraer MEDS
  int idxMeds = temp.indexOf("MEDS:[");
  int idxMedsEnd = temp.indexOf("]", idxMeds);
  if (idxMeds != -1) {
    QString content = temp.mid(idxMeds + 6, idxMedsEnd - (idxMeds + 6));
    for (const QString &s : content.split(",", Qt::SkipEmptyParts))
      areaMedidas->addItem(s);
  }

  // Extraer AGGR
  int idxAggr = temp.indexOf("AGGR:");
  if (idxAggr != -1) {
    int idxSpace = temp.indexOf(" ", idxAggr);
    QString aggr = temp.mid(idxAggr + 5, idxSpace - (idxAggr + 5));
    comboAgregacion->setCurrentText(aggr); // Aproximado
  }

  ejecutarConsulta();
}

void ConstructorConsultas::limpiarConsulta() {
  areaFilas->clear();
  areaColumnas->clear();
  areaMedidas->clear();
  areaFiltros->clear();
  tablaResultados->setRowCount(0);
  lblInfoResultados->setText("Esperando consulta...");
}

void ConstructorConsultas::exportarCSV() {
  if (tablaResultados->rowCount() == 0)
    return;
  QString fileName = QFileDialog::getSaveFileName(
      this, "Exportar", "resultados.csv", "CSV (*.csv)");
  if (!fileName.isEmpty()) {
    QMessageBox::information(this, "Exportar",
                             "Datos exportados a " + fileName);
  }
}

void ConstructorConsultas::generarReporte() {
  if (tablaResultados->rowCount() == 0)
    return;
  QString fileName = QFileDialog::getSaveFileName(this, "Reporte",
                                                  "reporte.pdf", "PDF (*.pdf)");
  if (!fileName.isEmpty()) {
    QMessageBox::information(this, "Reporte", "PDF generado en " + fileName);
  }
}

void ConstructorConsultas::cargarDatosEjemplo() {
  tablaResultados->setRowCount(0);
  int rows = 15;

  // Dependiendo de agregacion, cambiar valores
  bool esSuma = comboAgregacion->currentText().startsWith("SUM");

  QStringList dims;
  if (areaFilas->count() > 0)
    dims << areaFilas->item(0)->text();
  else
    dims << "Total";

  QStringList cols;
  if (areaColumnas->count() > 0)
    cols << areaColumnas->item(0)->text();
  else
    cols << "General";

  for (int i = 0; i < rows; ++i) {
    tablaResultados->insertRow(i);
    tablaResultados->setItem(
        i, 0, new QTableWidgetItem(dims[0] + " " + QString::number(i + 1)));
    tablaResultados->setItem(i, 1, new QTableWidgetItem(cols[0]));

    double val = rand() % 10000;
    if (!esSuma)
      val = val / 100.0; // AVG valores mas chicos

    tablaResultados->setItem(
        i, 2, new QTableWidgetItem(QString::number(val, 'f', 2)));
    tablaResultados->setItem(
        i, 3,
        new QTableWidgetItem(areaMedidas->count() > 0
                                 ? areaMedidas->item(0)->text()
                                 : "Ventas"));
  }
}
