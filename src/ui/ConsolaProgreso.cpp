#include "ConsolaProgreso.h"
#include <QDateTime>
#include <QScrollBar>

ConsolaProgreso::ConsolaProgreso(QWidget *parent) : QWidget(parent) {
  configurarUi();
}

void ConsolaProgreso::configurarUi() {
  QVBoxLayout *layout = new QVBoxLayout(this);

  QLabel *header = new QLabel("Fase 3: Carga Optimizada y Procesamiento", this);
  header->setStyleSheet("font-size: 18px; font-weight: bold; color: #4FC3F7; "
                        "margin-bottom: 20px;");
  layout->addWidget(header);

  layout->addWidget(new QLabel("Progreso General:", this));
  barProgreso = new QProgressBar(this);
  barProgreso->setRange(0, 100);
  barProgreso->setValue(0);
  barProgreso->setStyleSheet(
      "QProgressBar { border: 2px solid grey; border-radius: 5px; text-align: "
      "center; } QProgressBar::chunk { background-color: #007ACC; }");
  layout->addWidget(barProgreso);

  lblEstado = new QLabel("Iniciando motores...", this);
  lblEstado->setStyleSheet("color: #AAA; font-style: italic; margin-top: 5px;");
  layout->addWidget(lblEstado);

  layout->addWidget(new QLabel("Bitácora de Procesamiento:", this));
  txtLog = new QTextEdit(this);
  txtLog->setReadOnly(true);
  txtLog->setStyleSheet(
      "background-color: #1E1E1E; color: #00FF00; font-family: Consolas, "
      "Monospace; border: 1px solid #333;");
  layout->addWidget(txtLog);

  btnContinuar = new QPushButton("Explorar Cubo (Fase 4)", this);
  btnContinuar->setStyleSheet("background-color: #28a745; color: white; "
                              "padding: 10px; font-weight: bold;");
  btnContinuar->setVisible(false);
  btnContinuar->setEnabled(false);

  connect(btnContinuar, &QPushButton::clicked, this,
          &ConsolaProgreso::cargaCompletada);
  layout->addWidget(btnContinuar);
}

void ConsolaProgreso::actualizarProgresoGeneral(int valor) {
  barProgreso->setValue(valor);
}

void ConsolaProgreso::agregarLog(const QString &mensaje, const QString &tipo) {
  QString color = "#00FF00"; // INFO
  if (tipo == "WARN")
    color = "#FFFF00";
  if (tipo == "ERROR")
    color = "#FF0000";

  QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss");
  QString logEntry = QString("<span style='color: #888;'>[%1]</span> <span "
                             "style='color: %2;'>[%3] %4</span>")
                         .arg(timestamp)
                         .arg(color)
                         .arg(tipo)
                         .arg(mensaje);

  txtLog->append(logEntry);

  // Auto-scroll
  txtLog->verticalScrollBar()->setValue(txtLog->verticalScrollBar()->maximum());

  // Actualizar estado corto
  lblEstado->setText(mensaje);
}

void ConsolaProgreso::mostrarBotonContinuar() {
  btnContinuar->setVisible(true);
  btnContinuar->setEnabled(true);
}
