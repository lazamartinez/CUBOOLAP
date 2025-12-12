#include "HistorialConsultas.h"
#include "styles/FlutterTheme.h"
#include <QDateTime>
#include <QLabel>
#include <QListWidget>
#include <QVBoxLayout>


HistorialConsultas::HistorialConsultas(QWidget *parent) : QWidget(parent) {
  configurarUi();
  FlutterTheme::instance().applyThemeToWidget(this);
}

void HistorialConsultas::configurarUi() {
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(8);

  QLabel *lblTitulo = new QLabel("Historial", this);
  lblTitulo->setStyleSheet(
      "font-size: 14px; font-weight: 600; color: #374151;");
  layout->addWidget(lblTitulo);

  listaHistorial = new QListWidget(this);
  listaHistorial->setMaximumHeight(150);
  listaHistorial->setStyleSheet(R"(
    QListWidget {
      background: #f8fafc;
      border: 1px solid #d1d5db;
      border-radius: 6px;
      font-size: 12px;
      color: #1f2937;
    }
    QListWidget::item {
      padding: 8px;
      border-bottom: 1px solid #e5e7eb;
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

  btnLimpiar = new FlutterTextButton("Limpiar Historial", this);
  // Manual override for danger color if desired, but default is fine or we can
  // set it
  btnLimpiar->setStyleSheet(
      "QPushButton { color: #ef4444; font-weight: 600; font-size: 12px; "
      "border: none; } QPushButton:hover { background: #fee2e2; border-radius: "
      "4px; }");

  connect(btnLimpiar, &QPushButton::clicked, this,
          &HistorialConsultas::limpiarHistorial);
  layout->addWidget(btnLimpiar, 0, Qt::AlignRight);
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
