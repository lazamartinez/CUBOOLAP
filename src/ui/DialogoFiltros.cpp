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

  // CORREGIDO: Usar takeItem() en lugar de delete para evitar double-free
  connect(btnAgregar, &QPushButton::clicked, this, [this]() {
    QList<QListWidgetItem *> items = lstValoresDisponibles->selectedItems();
    for (auto item : items) {
      int row = lstValoresDisponibles->row(item);
      QListWidgetItem *takenItem = lstValoresDisponibles->takeItem(row);
      if (takenItem) {
        lstValoresSeleccionados->addItem(takenItem);
      }
    }
  });

  connect(btnQuitar, &QPushButton::clicked, this, [this]() {
    QList<QListWidgetItem *> items = lstValoresSeleccionados->selectedItems();
    for (auto item : items) {
      int row = lstValoresSeleccionados->row(item);
      QListWidgetItem *takenItem = lstValoresSeleccionados->takeItem(row);
      if (takenItem) {
        lstValoresDisponibles->addItem(takenItem);
      }
    }
  });
  connect(btnAceptar, &QPushButton::clicked, this, &QDialog::accept);
  connect(btnCancelar, &QPushButton::clicked, this, &QDialog::reject);
}

void DialogoFiltros::aplicarEstilos() {
  setStyleSheet(R"(
        QDialog {
            background: #ffffff;
        }
        QLabel {
            color: #1e293b;
            font-weight: 600;
            font-size: 11pt;
        }
        QGroupBox {
            color: #475569;
            font-weight: 600;
            border: 2px solid #e2e8f0;
            border-radius: 8px;
            margin-top: 12px;
            padding-top: 14px;
            background: #f8fafc;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 8px;
            background: #ffffff;
        }
        QComboBox {
            background: #ffffff;
            color: #1e293b;
            border: 2px solid #cbd5e1;
            border-radius: 6px;
            padding: 8px 12px;
            font-size: 10pt;
        }
        QComboBox:hover {
            border-color: #3b82f6;
            background: #f8fafc;
        }
        QComboBox:focus {
            border-color: #3b82f6;
        }
        QComboBox::drop-down {
            border: none;
            width: 30px;
        }
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 6px solid #3b82f6;
            margin-right: 8px;
        }
        QListWidget {
            background: #ffffff;
            color: #1e293b;
            border: 2px solid #cbd5e1;
            border-radius: 8px;
            padding: 6px;
            font-size: 10pt;
        }
        QListWidget:hover {
            border-color: #3b82f6;
        }
        QListWidget:focus {
            border-color: #3b82f6;
        }
        QListWidget::item {
            padding: 8px 12px;
            border-radius: 5px;
            margin: 2px;
            color: #1e293b;
        }
        QListWidget::item:hover {
            background: #eff6ff;
            color: #1e40af;
        }
        QListWidget::item:selected {
            background: #3b82f6;
            color: #ffffff;
            font-weight: 600;
        }
        QPushButton {
            background: #3b82f6;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 10px 20px;
            font-weight: 600;
            font-size: 10pt;
        }
        QPushButton:hover {
            background: #2563eb;
        }
        QPushButton:pressed {
            background: #1d4ed8;
        }
        QPushButton#btnCancelar {
            background: #e2e8f0;
            color: #475569;
        }
        QPushButton#btnCancelar:hover {
            background: #cbd5e1;
            color: #1e293b;
        }
    )");

  btnCancelar->setObjectName("btnCancelar");
}

void DialogoFiltros::setDimensionesDisponibles(const QStringList &dimensiones) {
  if (!cmbDimension) {
    qCritical() << "ERROR: cmbDimension es NULL en setDimensionesDisponibles";
    return;
  }

  qDebug() << "setDimensionesDisponibles:" << dimensiones;
  cmbDimension->clear();

  if (!dimensiones.isEmpty()) {
    cmbDimension->addItems(dimensiones);
  } else {
    qWarning() << "ADVERTENCIA: Lista de dimensiones vacía";
  }
}

void DialogoFiltros::setValoresDimension(const QString &dimension,
                                         const QStringList &valores) {
  if (!cmbDimension || !lstValoresDisponibles || !lstValoresSeleccionados) {
    qCritical() << "ERROR: Widgets NULL en setValoresDimension";
    return;
  }

  if (dimension.isEmpty()) {
    qWarning() << "ADVERTENCIA: Dimension vacía en setValoresDimension";
    return;
  }

  qDebug() << "setValoresDimension:" << dimension << "con" << valores.size()
           << "valores";

  m_valoresPorDimension[dimension] = valores;

  // Solo actualizar si es la dimensión actualmente seleccionada
  if (cmbDimension->currentText() == dimension) {
    qDebug() << "Actualizando listas para dimension actual:" << dimension;
    onDimensionCambiada(dimension);
  }
}

void DialogoFiltros::onDimensionCambiada(const QString &dimension) {
  if (!lstValoresDisponibles || !lstValoresSeleccionados) {
    qCritical() << "ERROR: Listas NULL en onDimensionCambiada";
    return;
  }

  qDebug() << "onDimensionCambiada:" << dimension;

  // Limpiar listas de forma segura
  lstValoresDisponibles->clear();
  lstValoresSeleccionados->clear();

  if (dimension.isEmpty()) {
    qWarning() << "ADVERTENCIA: Dimension vacía, no hay valores para mostrar";
    emit dimensionCambiada(dimension);
    return;
  }

  if (m_valoresPorDimension.contains(dimension)) {
    QStringList valores = m_valoresPorDimension[dimension];
    qDebug() << "Agregando" << valores.size() << "valores a la lista";

    if (!valores.isEmpty()) {
      lstValoresDisponibles->addItems(valores);
    } else {
      qWarning() << "ADVERTENCIA: No hay valores para la dimension"
                 << dimension;
    }
  } else {
    qWarning() << "ADVERTENCIA: Dimension" << dimension
               << "no encontrada en m_valoresPorDimension";
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
    QList<QListWidgetItem *> items = lstFiltrosActivos->selectedItems();
    for (auto item : items) {
      QString dim = item->text().split(":").first().trimmed();
      m_filtrosActivos.remove(dim);
      // CORREGIDO: Usar takeItem() y luego delete para evitar double-free
      int row = lstFiltrosActivos->row(item);
      QListWidgetItem *takenItem = lstFiltrosActivos->takeItem(row);
      delete takenItem; // Ahora sí podemos eliminar porque ya no pertenece a la
                        // lista
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
