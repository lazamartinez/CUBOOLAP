#include "DialogoDrillThrough.h"
#include "styles/FlutterTheme.h"
#include <QFileDialog>
#include <QHeaderView>
#include <QTextStream>

DialogoDrillThrough::DialogoDrillThrough(QWidget *parent) : QDialog(parent) {
  configurarUi();
  aplicarEstilos();

  setWindowTitle("Drill Through - Registros Detallados");
  setMinimumSize(800, 500);
  resize(900, 600);
}

void DialogoDrillThrough::configurarUi() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setSpacing(12);
  mainLayout->setContentsMargins(16, 16, 16, 16);

  // Titulo con informacion de la celda
  lblTitulo = new QLabel("Registros de la celda seleccionada", this);
  mainLayout->addWidget(lblTitulo);

  // Controles
  QHBoxLayout *controlsLayout = new QHBoxLayout();

  QLabel *lblLimit = new QLabel("Mostrar:", this);
  spinLimit = new QSpinBox(this);
  spinLimit->setRange(10, 1000);
  spinLimit->setValue(100);
  spinLimit->setSuffix(" registros");

  lblRegistros = new QLabel("", this);

  controlsLayout->addWidget(lblLimit);
  controlsLayout->addWidget(spinLimit);
  controlsLayout->addWidget(lblRegistros);
  controlsLayout->addStretch();

  btnExportar = new QPushButton("Exportar CSV", this);
  controlsLayout->addWidget(btnExportar);

  mainLayout->addLayout(controlsLayout);

  // Tabla de datos
  tabla = new QTableWidget(this);
  tabla->setAlternatingRowColors(true);
  tabla->setSelectionBehavior(QAbstractItemView::SelectRows);
  tabla->setEditTriggers(QAbstractItemView::NoEditTriggers);
  tabla->horizontalHeader()->setStretchLastSection(true);
  tabla->verticalHeader()->setVisible(false);
  mainLayout->addWidget(tabla);

  // Botones de dialogo
  QHBoxLayout *botonesLayout = new QHBoxLayout();
  botonesLayout->addStretch();
  btnCerrar = new QPushButton("Cerrar", this);
  botonesLayout->addWidget(btnCerrar);
  mainLayout->addLayout(botonesLayout);

  // Conexiones
  connect(spinLimit, QOverload<int>::of(&QSpinBox::valueChanged), this,
          &DialogoDrillThrough::limitCambiado);
  connect(btnExportar, &QPushButton::clicked, this, [this]() {
    QString filename = QFileDialog::getSaveFileName(this, "Exportar a CSV", "",
                                                    "CSV Files (*.csv)");
    if (!filename.isEmpty()) {
      QFile file(filename);
      if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        // Headers
        QStringList headers;
        for (int c = 0; c < tabla->columnCount(); c++) {
          headers << tabla->horizontalHeaderItem(c)->text();
        }
        out << headers.join(",") << "\n";
        // Data
        for (int r = 0; r < tabla->rowCount(); r++) {
          QStringList row;
          for (int c = 0; c < tabla->columnCount(); c++) {
            row << tabla->item(r, c)->text();
          }
          out << row.join(",") << "\n";
        }
        file.close();
      }
    }
  });
  connect(btnCerrar, &QPushButton::clicked, this, &QDialog::accept);
}

void DialogoDrillThrough::aplicarEstilos() {
  bool dark = FlutterTheme::instance().darkMode();
  QString bg = dark ? "#1f2937" : "#ffffff";
  QString text = dark ? "#f3f4f6" : "#1e293b";
  QString border = dark ? "#374151" : "#e2e8f0";
  QString inputBg = dark ? "#111827" : "#f8fafc";

  setStyleSheet(QString(R"(
        QDialog {
            background: %1;
        }
        QLabel {
            color: %2;
        }
        QSpinBox {
            background: %4;
            color: %2;
            border: 1px solid %3;
            border-radius: 4px;
            padding: 4px 8px;
        }
        QTableWidget {
            background: %4;
            color: %2;
            border: 1px solid %3;
            border-radius: 6px;
            gridline-color: %3;
        }
        QTableWidget::item {
            padding: 6px;
        }
        QTableWidget::item:selected {
            background: #3b82f6;
        }
        QHeaderView::section {
            background: %1;
            color: %2;
            font-weight: 600;
            border: none;
            border-bottom: 2px solid #3b82f6;
            padding: 8px;
        }
        QTableWidget::item:alternate {
            background: %1;
        }
        QPushButton {
            background: #3b82f6;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 8px 16px;
            font-weight: 600;
        }
        QPushButton:hover {
            background: #2563eb;
        }
        QPushButton#btnCerrar {
            background: %3;
            color: %2;
        }
    )")
                    .arg(bg, text, border, inputBg));

  lblTitulo->setStyleSheet(
      QString("font-size: 16px; font-weight: 700; color: %1;").arg(text));

  btnCerrar->setObjectName("btnCerrar");
}

void DialogoDrillThrough::setTitulo(const QString &dimX, const QString &dimZ,
                                    double valor) {
  lblTitulo->setText(QString("Drill Through: %1 x %2 (Valor: $%3)")
                         .arg(dimX, dimZ)
                         .arg(valor, 0, 'f', 2));
}

void DialogoDrillThrough::cargarDatos(const QStringList &columnas,
                                      const QVector<QStringList> &filas) {
  tabla->clear();
  tabla->setColumnCount(columnas.size());
  tabla->setHorizontalHeaderLabels(columnas);
  tabla->setRowCount(filas.size());

  for (int r = 0; r < filas.size(); r++) {
    const QStringList &fila = filas[r];
    for (int c = 0; c < fila.size() && c < columnas.size(); c++) {
      tabla->setItem(r, c, new QTableWidgetItem(fila[c]));
    }
  }

  tabla->resizeColumnsToContents();
  lblRegistros->setText(
      QString("(%1 registros encontrados)").arg(filas.size()));
  lblRegistros->setStyleSheet("color: #10b981; font-weight: 500;");
}

void DialogoDrillThrough::setLimit(int limit) { spinLimit->setValue(limit); }
