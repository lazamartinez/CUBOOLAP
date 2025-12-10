#include "ConstructorConsultas.h"
#include "Estilos.h"
#include <QFileDialog>
#include <QGroupBox>
#include <QHeaderView>
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

  QLabel *header = new QLabel("Fase 5: Constructor de Consultas", this);
  header->setStyleSheet(R"(
    font-size: 16px;
    font-weight: 700;
    color: #1f2937;
    padding-bottom: 8px;
    border-bottom: 2px solid #6366f1;
  )");
  headerLayout->addWidget(header);
  headerLayout->addStretch();

  // Boton volver a Fase 1
  btnVolverInicio = new QPushButton("Volver al Inicio", this);
  btnVolverInicio->setStyleSheet(R"(
    QPushButton {
      background: #f1f5f9;
      border: 1px solid #e2e8f0;
      border-radius: 6px;
      padding: 6px 16px;
      color: #475569;
      font-size: 11px;
    }
    QPushButton:hover {
      background: #e2e8f0;
    }
  )");
  connect(btnVolverInicio, &QPushButton::clicked, this,
          &ConstructorConsultas::volverAlInicio);
  headerLayout->addWidget(btnVolverInicio);

  mainLayout->addLayout(headerLayout);

  // Splitter principal
  QSplitter *mainSplitter = new QSplitter(Qt::Vertical, this);

  // Panel superior: Constructor
  QWidget *builderWidget = new QWidget(this);
  QHBoxLayout *builderLayout = new QHBoxLayout(builderWidget);
  builderLayout->setSpacing(10);
  builderLayout->setContentsMargins(0, 0, 0, 0);

  // Recursos
  QWidget *resContainer = new QWidget(this);
  resContainer->setStyleSheet(
      "background: white; border: 1px solid #e2e8f0; border-radius: 8px;");
  resContainer->setMaximumWidth(180);
  QVBoxLayout *resLayout = new QVBoxLayout(resContainer);
  resLayout->setContentsMargins(12, 12, 12, 12);
  resLayout->setSpacing(8);

  QLabel *lblDims = new QLabel("Dimensiones", this);
  lblDims->setStyleSheet("font-size: 12px; font-weight: 600; color: #2563eb;");
  resLayout->addWidget(lblDims);

  listaDimensiones = new QListWidget(this);
  listaDimensiones->addItems({"Tiempo", "Producto", "Cliente", "Geografia"});
  listaDimensiones->setDragEnabled(true);
  listaDimensiones->setMaximumHeight(90);
  resLayout->addWidget(listaDimensiones);

  QLabel *lblMeds = new QLabel("Medidas", this);
  lblMeds->setStyleSheet("font-size: 12px; font-weight: 600; color: #10b981;");
  resLayout->addWidget(lblMeds);

  listaMedidas = new QListWidget(this);
  listaMedidas->addItems({"Ventas", "Costo", "Beneficio", "Cantidad"});
  listaMedidas->setDragEnabled(true);
  listaMedidas->setMaximumHeight(90);
  resLayout->addWidget(listaMedidas);

  builderLayout->addWidget(resContainer);

  // Areas de construccion
  QWidget *buildArea = new QWidget(this);
  buildArea->setStyleSheet(
      "background: white; border: 1px solid #e2e8f0; border-radius: 8px;");
  QVBoxLayout *centerLayout = new QVBoxLayout(buildArea);
  centerLayout->setContentsMargins(12, 12, 12, 12);
  centerLayout->setSpacing(8);

  QLabel *lblFilas = new QLabel("Filas (dimension para agrupar)", this);
  lblFilas->setStyleSheet("font-size: 11px; font-weight: 600; color: #374151;");
  centerLayout->addWidget(lblFilas);

  areaFilas = new QListWidget(this);
  areaFilas->setAcceptDrops(true);
  areaFilas->setMaximumHeight(50);
  areaFilas->setStyleSheet(
      "background: #eff6ff; border: 2px dashed #93c5fd; border-radius: 6px;");
  centerLayout->addWidget(areaFilas);

  QLabel *lblCols = new QLabel("Columnas (dimension secundaria)", this);
  lblCols->setStyleSheet("font-size: 11px; font-weight: 600; color: #374151;");
  centerLayout->addWidget(lblCols);

  areaColumnas = new QListWidget(this);
  areaColumnas->setAcceptDrops(true);
  areaColumnas->setMaximumHeight(50);
  areaColumnas->setStyleSheet(
      "background: #eff6ff; border: 2px dashed #93c5fd; border-radius: 6px;");
  centerLayout->addWidget(areaColumnas);

  QLabel *lblMedsSel = new QLabel("Medidas a calcular", this);
  lblMedsSel->setStyleSheet(
      "font-size: 11px; font-weight: 600; color: #374151;");
  centerLayout->addWidget(lblMedsSel);

  areaMedidas = new QListWidget(this);
  areaMedidas->setAcceptDrops(true);
  areaMedidas->setMaximumHeight(50);
  areaMedidas->setStyleSheet(
      "background: #ecfdf5; border: 2px dashed #6ee7b7; border-radius: 6px;");
  centerLayout->addWidget(areaMedidas);

  builderLayout->addWidget(buildArea);

  // Acciones
  QWidget *actContainer = new QWidget(this);
  actContainer->setStyleSheet(
      "background: white; border: 1px solid #e2e8f0; border-radius: 8px;");
  actContainer->setMaximumWidth(150);
  QVBoxLayout *actLayout = new QVBoxLayout(actContainer);
  actLayout->setContentsMargins(12, 12, 12, 12);
  actLayout->setSpacing(8);

  btnEjecutar = new QPushButton("Ejecutar Consulta", this);
  btnEjecutar->setMinimumHeight(36);
  btnEjecutar->setCursor(Qt::PointingHandCursor);
  btnEjecutar->setStyleSheet(Estilos::obtenerEstiloBotonPrimario());
  connect(btnEjecutar, &QPushButton::clicked, this,
          &ConstructorConsultas::ejecutarConsulta);
  actLayout->addWidget(btnEjecutar);

  btnLimpiar = new QPushButton("Limpiar", this);
  btnLimpiar->setStyleSheet(R"(
    QPushButton {
      background: white;
      border: 1px solid #e2e8f0;
      border-radius: 6px;
      padding: 8px;
      color: #475569;
    }
    QPushButton:hover { background: #f1f5f9; }
  )");
  connect(btnLimpiar, &QPushButton::clicked, this,
          &ConstructorConsultas::limpiarConsulta);
  actLayout->addWidget(btnLimpiar);

  actLayout->addStretch();

  // Botones de exportacion
  QLabel *lblExport = new QLabel("Exportar", this);
  lblExport->setStyleSheet(
      "font-size: 11px; font-weight: 600; color: #374151; margin-top: 8px;");
  actLayout->addWidget(lblExport);

  btnExportCSV = new QPushButton("CSV", this);
  btnExportCSV->setStyleSheet(R"(
    QPushButton {
      background: #10b981;
      border: none;
      border-radius: 6px;
      padding: 8px;
      color: white;
      font-weight: 600;
    }
    QPushButton:hover { background: #059669; }
  )");
  connect(btnExportCSV, &QPushButton::clicked, this,
          &ConstructorConsultas::exportarCSV);
  actLayout->addWidget(btnExportCSV);

  btnReporte = new QPushButton("PDF", this);
  btnReporte->setStyleSheet(R"(
    QPushButton {
      background: #ef4444;
      border: none;
      border-radius: 6px;
      padding: 8px;
      color: white;
      font-weight: 600;
    }
    QPushButton:hover { background: #dc2626; }
  )");
  connect(btnReporte, &QPushButton::clicked, this,
          &ConstructorConsultas::generarReporte);
  actLayout->addWidget(btnReporte);

  builderLayout->addWidget(actContainer);

  mainSplitter->addWidget(builderWidget);

  // Resultados
  QWidget *resWidget = new QWidget(this);
  resWidget->setStyleSheet(
      "background: white; border: 1px solid #e2e8f0; border-radius: 8px;");
  QVBoxLayout *resultsLayout = new QVBoxLayout(resWidget);
  resultsLayout->setContentsMargins(12, 12, 12, 12);

  QHBoxLayout *resHeaderLayout = new QHBoxLayout();
  QLabel *lblRes = new QLabel("Resultados de la Consulta", resWidget);
  lblRes->setStyleSheet("font-size: 13px; font-weight: 600; color: #1f2937;");
  resHeaderLayout->addWidget(lblRes);
  resHeaderLayout->addStretch();

  lblInfoResultados = new QLabel("Sin resultados", this);
  lblInfoResultados->setStyleSheet("color: #6b7280; font-size: 11px;");
  resHeaderLayout->addWidget(lblInfoResultados);
  resultsLayout->addLayout(resHeaderLayout);

  tablaResultados = new QTableWidget(this);
  tablaResultados->setColumnCount(4);
  tablaResultados->setHorizontalHeaderLabels(
      {"Dimension", "Periodo", "Ventas", "Beneficio"});
  tablaResultados->horizontalHeader()->setStretchLastSection(true);
  tablaResultados->setAlternatingRowColors(true);
  tablaResultados->setSelectionBehavior(QAbstractItemView::SelectRows);
  resultsLayout->addWidget(tablaResultados);

  mainSplitter->addWidget(resWidget);
  mainSplitter->setStretchFactor(0, 1);
  mainSplitter->setStretchFactor(1, 2);

  mainLayout->addWidget(mainSplitter);
}

bool ConstructorConsultas::validarConsulta() {
  QStringList errores;

  // Validar que haya al menos una dimension en filas o columnas
  if (areaFilas->count() == 0 && areaColumnas->count() == 0) {
    errores << "Debe seleccionar al menos una dimension para filas o columnas";
  }

  // Validar que haya al menos una medida
  if (areaMedidas->count() == 0) {
    errores << "Debe seleccionar al menos una medida a calcular";
  }

  // Validar que no haya dimensiones duplicadas
  QSet<QString> dims;
  for (int i = 0; i < areaFilas->count(); i++) {
    QString text = areaFilas->item(i)->text();
    if (dims.contains(text)) {
      errores << QString("Dimension '%1' esta duplicada").arg(text);
    }
    dims.insert(text);
  }
  for (int i = 0; i < areaColumnas->count(); i++) {
    QString text = areaColumnas->item(i)->text();
    if (dims.contains(text)) {
      errores << QString("Dimension '%1' ya esta en filas").arg(text);
    }
    dims.insert(text);
  }

  if (!errores.isEmpty()) {
    QMessageBox::warning(this, "Validacion de Consulta",
                         "Por favor corrija los siguientes errores:\n\n- " +
                             errores.join("\n- "));
    return false;
  }

  return true;
}

void ConstructorConsultas::ejecutarConsulta() {
  // Validar antes de ejecutar
  if (!validarConsulta()) {
    return;
  }

  lblInfoResultados->setText("Ejecutando...");
  lblInfoResultados->setStyleSheet("color: #f59e0b; font-size: 11px;");

  cargarDatosEjemplo();

  int tiempo = rand() % 50 + 20;
  lblInfoResultados->setText(QString("%1 registros | %2ms | Consulta exitosa")
                                 .arg(tablaResultados->rowCount())
                                 .arg(tiempo));
  lblInfoResultados->setStyleSheet(
      "color: #10b981; font-size: 11px; font-weight: 500;");
}

void ConstructorConsultas::limpiarConsulta() {
  areaFilas->clear();
  areaColumnas->clear();
  areaMedidas->clear();
  tablaResultados->setRowCount(0);
  lblInfoResultados->setText("Sin resultados");
  lblInfoResultados->setStyleSheet("color: #6b7280; font-size: 11px;");
}

void ConstructorConsultas::exportarCSV() {
  // Validar que haya datos
  if (tablaResultados->rowCount() == 0) {
    QMessageBox::warning(
        this, "Exportar CSV",
        "No hay datos para exportar.\n\nEjecute una consulta primero.");
    return;
  }

  QString fileName = QFileDialog::getSaveFileName(
      this, "Exportar CSV", "resultados_olap.csv", "Archivos CSV (*.csv)");

  if (fileName.isEmpty())
    return;

  QFile file(fileName);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::critical(this, "Error", "No se pudo crear el archivo.");
    return;
  }

  QTextStream out(&file);

  // Headers
  QStringList headers;
  for (int c = 0; c < tablaResultados->columnCount(); c++) {
    headers << tablaResultados->horizontalHeaderItem(c)->text();
  }
  out << headers.join(",") << "\n";

  // Datos
  for (int r = 0; r < tablaResultados->rowCount(); r++) {
    QStringList row;
    for (int c = 0; c < tablaResultados->columnCount(); c++) {
      QTableWidgetItem *item = tablaResultados->item(r, c);
      row << (item ? item->text() : "");
    }
    out << row.join(",") << "\n";
  }

  file.close();

  QMessageBox::information(
      this, "Exportar CSV",
      QString("Archivo exportado exitosamente:\n%1\n\n%2 registros guardados.")
          .arg(fileName)
          .arg(tablaResultados->rowCount()));
}

void ConstructorConsultas::generarReporte() {
  if (tablaResultados->rowCount() == 0) {
    QMessageBox::warning(
        this, "Generar PDF",
        "No hay datos para el reporte.\n\nEjecute una consulta primero.");
    return;
  }

  QString fileName = QFileDialog::getSaveFileName(
      this, "Guardar Reporte PDF", "reporte_olap.pdf", "Archivos PDF (*.pdf)");

  if (fileName.isEmpty())
    return;

  // Simular generacion (en produccion usar QPdfWriter)
  QMessageBox::information(
      this, "Generar PDF",
      QString("Reporte PDF generado:\n%1\n\n%2 registros incluidos.\n\n"
              "(En produccion se usaria QPdfWriter)")
          .arg(fileName)
          .arg(tablaResultados->rowCount()));
}

void ConstructorConsultas::cargarDatosEjemplo() {
  QStringList prods = {"Electronica", "Ropa", "Alimentos", "Hogar"};
  QStringList pers = {"Q1 2024", "Q2 2024", "Q3 2024", "Q4 2024"};

  tablaResultados->setRowCount(0);
  int row = 0;

  for (const QString &prod : prods) {
    for (const QString &per : pers) {
      tablaResultados->insertRow(row);

      QTableWidgetItem *itemProd = new QTableWidgetItem(prod);
      QTableWidgetItem *itemPer = new QTableWidgetItem(per);

      double ventas = 5000 + rand() % 15000;
      double beneficio = ventas * (0.15 + (rand() % 20) / 100.0);

      QTableWidgetItem *itemVentas =
          new QTableWidgetItem(QString("$%1").arg(ventas, 0, 'f', 2));
      QTableWidgetItem *itemBenef =
          new QTableWidgetItem(QString("$%1").arg(beneficio, 0, 'f', 2));

      // Color condicional para beneficio
      if (beneficio > 3000) {
        itemBenef->setForeground(QColor("#10b981"));
      } else if (beneficio > 1500) {
        itemBenef->setForeground(QColor("#f59e0b"));
      } else {
        itemBenef->setForeground(QColor("#ef4444"));
      }

      tablaResultados->setItem(row, 0, itemProd);
      tablaResultados->setItem(row, 1, itemPer);
      tablaResultados->setItem(row, 2, itemVentas);
      tablaResultados->setItem(row, 3, itemBenef);

      row++;
    }
  }
}
