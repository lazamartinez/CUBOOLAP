#include "HistorialConsultas.h"
#include <QLabel>

HistorialConsultas::HistorialConsultas(QWidget *parent) : QWidget(parent) {
  configurarUi();
}

void HistorialConsultas::configurarUi() {
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(8);

  QLabel *lblTitulo = new QLabel("Historial", this);
  lblTitulo->setStyleSheet(
      "font-size: 12px; font-weight: 600; color: #374151;");
  layout->addWidget(lblTitulo);

  listaHistorial = new QListWidget(this);
  listaHistorial->setMaximumHeight(150);
  listaHistorial->setStyleSheet(R"(
    QListWidget {
      background: #f8fafc;
      border: 1px solid #e2e8f0;
      border-radius: 6px;
      font-size: 11px;
    }
    QListWidget::item {
      padding: 8px;
      border-bottom: 1px solid #e2e8f0;
    }
    QListWidget::item:hover {
      background: #eff6ff;
    }
    QListWidget::item:selected {
      background: #dbeafe;
      color: #1e40af;
    }
  )");

  connect(listaHistorial, &QListWidget::itemDoubleClicked, this,
          [this](QListWidgetItem *item) {
            int idx = listaHistorial->row(item);
            if (idx >= 0 && idx < m_historial.size()) {
              emit consultaSeleccionada(m_historial[idx].consulta);
            }
          });

  layout->addWidget(listaHistorial);

  btnLimpiar = new QPushButton("Limpiar Historial", this);
  btnLimpiar->setStyleSheet(R"(
    QPushButton {
      background: transparent;
      border: none;
      color: #ef4444;
      font-size: 10px;
      padding: 4px;
    }
    QPushButton:hover {
      text-decoration: underline;
    }
  )");
  connect(btnLimpiar, &QPushButton::clicked, this,
          &HistorialConsultas::limpiarHistorial);
  layout->addWidget(btnLimpiar);
}

void HistorialConsultas::agregarConsulta(const QString &consulta, int registros,
                                         int tiempoMs) {
  EntradaHistorial entrada;
  entrada.consulta = consulta;
  entrada.fecha = QDateTime::currentDateTime();
  entrada.registros = registros;
  entrada.tiempoMs = tiempoMs;

  m_historial.prepend(entrada);

  // Limitar a 20 entradas
  if (m_historial.size() > 20) {
    m_historial.removeLast();
  }

  // Actualizar lista visual
  QString texto = QString("%1 | %2 regs | %3ms")
                      .arg(entrada.fecha.toString("HH:mm"))
                      .arg(registros)
                      .arg(tiempoMs);

  listaHistorial->insertItem(0, texto);

  if (listaHistorial->count() > 20) {
    delete listaHistorial->takeItem(listaHistorial->count() - 1);
  }
}

void HistorialConsultas::limpiarHistorial() {
  m_historial.clear();
  listaHistorial->clear();
}
