#include "ConsolaProgreso.h"
#include "Estilos.h"
#include <QDateTime>
#include <QGroupBox>
#include <QScrollBar>

ConsolaProgreso::ConsolaProgreso(QWidget *parent) : QWidget(parent) {
  configurarUi();
}

void ConsolaProgreso::configurarUi() {
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setSpacing(10);
  layout->setContentsMargins(16, 16, 16, 16);

  // Header
  QLabel *header = new QLabel("Fase 3: Carga de Datos", this);
  header->setStyleSheet(R"(
    font-size: 16px;
    font-weight: 700;
    color: #1f2937;
    padding-bottom: 8px;
    border-bottom: 2px solid #10b981;
  )");
  layout->addWidget(header);

  // Stats compactas
  QHBoxLayout *statsLayout = new QHBoxLayout();
  statsLayout->setSpacing(8);

  QWidget *cardRegistros = crearCardEstadistica("Registros", "0", "#2563eb");
  statsLayout->addWidget(cardRegistros);
  lblRegistros = cardRegistros->findChild<QLabel *>("valorCard");

  QWidget *cardVelocidad = crearCardEstadistica("Reg/seg", "0", "#10b981");
  statsLayout->addWidget(cardVelocidad);
  lblVelocidad = cardVelocidad->findChild<QLabel *>("valorCard");

  QWidget *cardTiempo = crearCardEstadistica("Restante", "--:--", "#f59e0b");
  statsLayout->addWidget(cardTiempo);
  lblTiempo = cardTiempo->findChild<QLabel *>("valorCard");

  QWidget *cardCalidad = crearCardEstadistica("Calidad", "100%", "#6366f1");
  statsLayout->addWidget(cardCalidad);
  lblCalidad = cardCalidad->findChild<QLabel *>("valorCard");

  layout->addLayout(statsLayout);

  // Barra de progreso
  barProgreso = new QProgressBar(this);
  barProgreso->setRange(0, 100);
  barProgreso->setValue(0);
  barProgreso->setMinimumHeight(20);
  barProgreso->setMaximumHeight(20);
  barProgreso->setStyleSheet(R"(
    QProgressBar {
      border: none;
      border-radius: 10px;
      background: #e5e7eb;
      text-align: center;
      font-size: 10px;
    }
    QProgressBar::chunk {
      background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #10b981, stop:1 #6366f1);
      border-radius: 10px;
    }
  )");
  layout->addWidget(barProgreso);

  lblEstado = new QLabel("Preparando...", this);
  lblEstado->setStyleSheet("color: #6b7280; font-size: 11px;");
  layout->addWidget(lblEstado);

  // Logs
  QHBoxLayout *logsLayout = new QHBoxLayout();
  logsLayout->setSpacing(10);

  // Log principal
  QWidget *logContainer = new QWidget(this);
  logContainer->setStyleSheet(
      "background: #f9fafb; border: 1px solid #e5e7eb; border-radius: 6px;");
  QVBoxLayout *lLayout = new QVBoxLayout(logContainer);
  lLayout->setContentsMargins(8, 8, 8, 8);

  QLabel *lblLog = new QLabel("Bitacora", logContainer);
  lblLog->setStyleSheet("font-size: 11px; font-weight: 600; color: #374151;");
  lLayout->addWidget(lblLog);

  txtLog = new QTextEdit(this);
  txtLog->setReadOnly(true);
  txtLog->setMaximumHeight(150);
  txtLog->setStyleSheet(R"(
    QTextEdit {
      background: #1f2937;
      color: #10b981;
      font-family: Consolas, monospace;
      font-size: 10px;
      border: none;
      border-radius: 4px;
    }
  )");
  lLayout->addWidget(txtLog);
  logsLayout->addWidget(logContainer, 2);

  // Alertas
  QWidget *alertContainer = new QWidget(this);
  alertContainer->setStyleSheet(
      "background: #fefce8; border: 1px solid #fef08a; border-radius: 6px;");
  QVBoxLayout *aLayout = new QVBoxLayout(alertContainer);
  aLayout->setContentsMargins(8, 8, 8, 8);

  QLabel *lblAlert = new QLabel("Alertas", alertContainer);
  lblAlert->setStyleSheet("font-size: 11px; font-weight: 600; color: #854d0e;");
  aLayout->addWidget(lblAlert);

  txtAlertas = new QTextEdit(this);
  txtAlertas->setReadOnly(true);
  txtAlertas->setMaximumHeight(150);
  txtAlertas->setPlaceholderText("Sin alertas");
  txtAlertas->setStyleSheet(R"(
    QTextEdit {
      background: transparent;
      color: #92400e;
      font-size: 10px;
      border: none;
    }
  )");
  aLayout->addWidget(txtAlertas);
  logsLayout->addWidget(alertContainer, 1);

  layout->addLayout(logsLayout);

  // Boton continuar
  btnContinuar = new QPushButton("Explorar Cubo (Fase 4)", this);
  btnContinuar->setMinimumHeight(36);
  btnContinuar->setCursor(Qt::PointingHandCursor);
  btnContinuar->setStyleSheet(R"(
    QPushButton {
      background: #10b981;
      border: none;
      border-radius: 6px;
      color: white;
      font-size: 12px;
      font-weight: 600;
    }
    QPushButton:hover { background: #059669; }
  )");
  btnContinuar->setVisible(false);
  btnContinuar->setEnabled(false);
  connect(btnContinuar, &QPushButton::clicked, this,
          &ConsolaProgreso::cargaCompletada);
  layout->addWidget(btnContinuar);
}

QWidget *ConsolaProgreso::crearCardEstadistica(const QString &etiqueta,
                                               const QString &valor,
                                               const QString &color) {
  QWidget *card = new QWidget(this);
  card->setStyleSheet(QString(R"(
    background: white;
    border: 1px solid #e5e7eb;
    border-left: 3px solid %1;
    border-radius: 6px;
  )")
                          .arg(color));

  QVBoxLayout *layout = new QVBoxLayout(card);
  layout->setSpacing(2);
  layout->setContentsMargins(8, 6, 8, 6);

  QLabel *lblValor = new QLabel(valor, card);
  lblValor->setObjectName("valorCard");
  lblValor->setStyleSheet(
      QString("font-size: 16px; font-weight: 700; color: %1;").arg(color));

  QLabel *lblEtiqueta = new QLabel(etiqueta, card);
  lblEtiqueta->setStyleSheet("font-size: 9px; color: #6b7280;");

  layout->addWidget(lblValor);
  layout->addWidget(lblEtiqueta);

  return card;
}

void ConsolaProgreso::actualizarProgresoGeneral(int valor) {
  barProgreso->setValue(valor);
}

void ConsolaProgreso::actualizarEstadisticas(int registros, double velocidad,
                                             const QString &tiempoRestante,
                                             int calidad) {
  if (lblRegistros)
    lblRegistros->setText(QString::number(registros));
  if (lblVelocidad)
    lblVelocidad->setText(QString::number((int)velocidad));
  if (lblTiempo)
    lblTiempo->setText(tiempoRestante);
  if (lblCalidad)
    lblCalidad->setText(QString::number(calidad) + "%");
}

void ConsolaProgreso::agregarAlerta(const QString &mensaje,
                                    const QString &severidad) {
  QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss");
  txtAlertas->append(QString("[%1] %2").arg(timestamp).arg(mensaje));
}

void ConsolaProgreso::agregarLog(const QString &mensaje, const QString &tipo) {
  QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss");
  QString color = tipo == "ERROR" ? "#ef4444" : "#10b981";
  txtLog->insertHtml(QString("<span style='color:#6b7280;'>[%1]</span> <span "
                             "style='color:%2;'>%3</span><br>")
                         .arg(timestamp)
                         .arg(color)
                         .arg(mensaje));
  txtLog->verticalScrollBar()->setValue(txtLog->verticalScrollBar()->maximum());
  lblEstado->setText(mensaje);
}

void ConsolaProgreso::mostrarBotonContinuar() {
  btnContinuar->setVisible(true);
  btnContinuar->setEnabled(true);
}
