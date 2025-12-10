#include "DialogoFiltros.h"

DialogoFiltros::DialogoFiltros(Modo modo, QWidget *parent)
    : QDialog(parent), m_modo(modo) {
  configurarUi();
  aplicarEstilos();

  setWindowTitle(modo == Slice ? "Slice - Filtrar Dimension"
                               : "Dice - Filtros Multiples");
  setMinimumSize(500, 400);
}

void DialogoFiltros::configurarUi() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setSpacing(12);

  // Selector de dimension
  QHBoxLayout *dimLayout = new QHBoxLayout();
  QLabel *lblDim = new QLabel("Dimension:", this);
  cmbDimension = new QComboBox(this);
  cmbDimension->setMinimumWidth(200);
  dimLayout->addWidget(lblDim);
  dimLayout->addWidget(cmbDimension);
  dimLayout->addStretch();
  mainLayout->addLayout(dimLayout);

  // Area de seleccion de valores
  QHBoxLayout *valoresLayout = new QHBoxLayout();

  // Lista de valores disponibles
  QGroupBox *grpDisponibles = new QGroupBox("Valores Disponibles", this);
  QVBoxLayout *dispLayout = new QVBoxLayout(grpDisponibles);
  lstValoresDisponibles = new QListWidget(this);
  lstValoresDisponibles->setSelectionMode(QAbstractItemView::ExtendedSelection);
  dispLayout->addWidget(lstValoresDisponibles);

  // Botones de transferencia
  QVBoxLayout *botonesLayout = new QVBoxLayout();
  botonesLayout->addStretch();
  btnAgregar = new QPushButton(">", this);
  btnAgregar->setFixedSize(40, 30);
  btnQuitar = new QPushButton("<", this);
  btnQuitar->setFixedSize(40, 30);
  botonesLayout->addWidget(btnAgregar);
  botonesLayout->addWidget(btnQuitar);
  botonesLayout->addStretch();

  // Lista de valores seleccionados
  QGroupBox *grpSeleccionados = new QGroupBox("Valores a Filtrar", this);
  QVBoxLayout *selLayout = new QVBoxLayout(grpSeleccionados);
  lstValoresSeleccionados = new QListWidget(this);
  lstValoresSeleccionados->setSelectionMode(
      QAbstractItemView::ExtendedSelection);
  selLayout->addWidget(lstValoresSeleccionados);

  valoresLayout->addWidget(grpDisponibles, 1);
  valoresLayout->addLayout(botonesLayout);
  valoresLayout->addWidget(grpSeleccionados, 1);
  mainLayout->addLayout(valoresLayout);

  // Para modo Dice: lista de filtros activos
  if (m_modo == Dice) {
    QGroupBox *grpFiltros = new QGroupBox("Filtros Configurados", this);
    QVBoxLayout *filtLayout = new QVBoxLayout(grpFiltros);
    lstFiltrosActivos = new QListWidget(this);
    filtLayout->addWidget(lstFiltrosActivos);

    QPushButton *btnAgregarFiltro =
        new QPushButton("Agregar Filtro a Lista", this);
    connect(btnAgregarFiltro, &QPushButton::clicked, this,
            &DialogoFiltros::onAgregarFiltro);
    filtLayout->addWidget(btnAgregarFiltro);

    mainLayout->addWidget(grpFiltros);
  }

  // Botones de dialogo
  QHBoxLayout *dialogoBtns = new QHBoxLayout();
  dialogoBtns->addStretch();
  btnCancelar = new QPushButton("Cancelar", this);
  btnAceptar = new QPushButton("Aplicar Filtro", this);
  dialogoBtns->addWidget(btnCancelar);
  dialogoBtns->addWidget(btnAceptar);
  mainLayout->addLayout(dialogoBtns);

  // Conexiones
  connect(cmbDimension, &QComboBox::currentTextChanged, this,
          &DialogoFiltros::onDimensionCambiada);
  connect(btnAgregar, &QPushButton::clicked, this, [this]() {
    for (auto item : lstValoresDisponibles->selectedItems()) {
      lstValoresSeleccionados->addItem(item->text());
      delete item;
    }
  });
  connect(btnQuitar, &QPushButton::clicked, this, [this]() {
    for (auto item : lstValoresSeleccionados->selectedItems()) {
      lstValoresDisponibles->addItem(item->text());
      delete item;
    }
  });
  connect(btnAceptar, &QPushButton::clicked, this, &QDialog::accept);
  connect(btnCancelar, &QPushButton::clicked, this, &QDialog::reject);
}

void DialogoFiltros::aplicarEstilos() {
  setStyleSheet(R"(
        QDialog {
            background: #1e293b;
        }
        QLabel {
            color: #e2e8f0;
            font-weight: 500;
        }
        QGroupBox {
            color: #94a3b8;
            font-weight: 600;
            border: 1px solid #334155;
            border-radius: 6px;
            margin-top: 12px;
            padding-top: 12px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 8px;
        }
        QComboBox, QListWidget {
            background: #0f172a;
            color: #e2e8f0;
            border: 1px solid #334155;
            border-radius: 4px;
            padding: 6px;
        }
        QComboBox:hover, QListWidget:hover {
            border-color: #3b82f6;
        }
        QListWidget::item {
            padding: 4px;
        }
        QListWidget::item:selected {
            background: #3b82f6;
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
        QPushButton#btnCancelar {
            background: #475569;
        }
    )");

  btnCancelar->setObjectName("btnCancelar");
}

void DialogoFiltros::setDimensionesDisponibles(const QStringList &dimensiones) {
  cmbDimension->clear();
  cmbDimension->addItems(dimensiones);
}

void DialogoFiltros::setValoresDimension(const QString &dimension,
                                         const QStringList &valores) {
  m_valoresPorDimension[dimension] = valores;
  if (cmbDimension->currentText() == dimension) {
    onDimensionCambiada(dimension);
  }
}

void DialogoFiltros::onDimensionCambiada(const QString &dimension) {
  lstValoresDisponibles->clear();
  lstValoresSeleccionados->clear();

  if (m_valoresPorDimension.contains(dimension)) {
    lstValoresDisponibles->addItems(m_valoresPorDimension[dimension]);
  }

  emit dimensionCambiada(dimension);
}

void DialogoFiltros::onAgregarFiltro() {
  QString dim = cmbDimension->currentText();
  QStringList valores;
  for (int i = 0; i < lstValoresSeleccionados->count(); i++) {
    valores << lstValoresSeleccionados->item(i)->text();
  }

  if (!valores.isEmpty()) {
    m_filtrosActivos[dim] = valores;
    if (lstFiltrosActivos) {
      lstFiltrosActivos->addItem(dim + ": " + valores.join(", "));
    }
  }
}

void DialogoFiltros::onQuitarFiltro() {
  if (lstFiltrosActivos) {
    auto items = lstFiltrosActivos->selectedItems();
    for (auto item : items) {
      QString dim = item->text().split(":").first().trimmed();
      m_filtrosActivos.remove(dim);
      delete item;
    }
  }
}

QString DialogoFiltros::getDimensionSeleccionada() const {
  return cmbDimension->currentText();
}

QStringList DialogoFiltros::getValoresSeleccionados() const {
  QStringList valores;
  for (int i = 0; i < lstValoresSeleccionados->count(); i++) {
    valores << lstValoresSeleccionados->item(i)->text();
  }
  return valores;
}

QMap<QString, QStringList> DialogoFiltros::getTodosLosFiltros() const {
  if (m_modo == Slice) {
    QMap<QString, QStringList> result;
    result[getDimensionSeleccionada()] = getValoresSeleccionados();
    return result;
  }
  return m_filtrosActivos;
}
