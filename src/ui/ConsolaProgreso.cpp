#include "ConsolaProgreso.h"
#include "styles/FlutterTheme.h"
#include <QDateTime>
#include <QGroupBox>
#include <QScrollBar>

ConsolaProgreso::ConsolaProgreso(QWidget *parent) : QWidget(parent) {
  configurarUi();
  FlutterTheme::instance().applyThemeToWidget(this);
}

void ConsolaProgreso::configurarUi() {
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setSpacing(16);
  layout->setContentsMargins(16, 16, 16, 16);

  // Header
  QLabel *header = new QLabel("Fase 3: Carga de Datos", this);
  header->setStyleSheet("font-size: 20px; font-weight: 700; color: #1f2937;");
  layout->addWidget(header);

  // Stats compactas
  QHBoxLayout *statsLayout = new QHBoxLayout();
  statsLayout->setSpacing(12);

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
  barProgreso->setMinimumHeight(8); // Thinner, more Material
  barProgreso->setMaximumHeight(8);
  barProgreso->setTextVisible(false);
  barProgreso->setStyleSheet(R"(
    QProgressBar {
      border: none;
      border-radius: 4px;
      background-color: #e5e7eb;
    }
    QProgressBar::chunk {
      background-color: #2563eb;
      border-radius: 4px;
    }
  )");
  layout->addWidget(barProgreso);

  lblEstado = new QLabel("Preparando...", this);
  lblEstado->setStyleSheet(
      "color: #6b7280; font-size: 14px; font-weight: 500;");
  layout->addWidget(lblEstado);

  // Logs
  QHBoxLayout *logsLayout = new QHBoxLayout();
  logsLayout->setSpacing(16);

  // Log principal
  FlutterCard *logContainer = new FlutterCard(this);
  QVBoxLayout *lLayout = new QVBoxLayout(logContainer);
  lLayout->setContentsMargins(12, 12, 12, 12);
  lLayout->setSpacing(8);

  QLabel *lblLog = new QLabel("Bitancora del Proceso", logContainer);
  lblLog->setStyleSheet("font-size: 14px; font-weight: 600; text-transform: "
                        "uppercase; color: #374151;");
  lLayout->addWidget(lblLog);

  txtLog = new QTextEdit(this);
  txtLog->setReadOnly(true);
  txtLog->setMinimumHeight(250);
  txtLog->setStyleSheet(R"(
    QTextEdit {
      background: #1f2937;
      color: #10b981;
      font-family: 'Consolas', monospace;
      font-size: 13px;
      border: none;
      border-radius: 4px;
      padding: 8px;
    }
  )");
  lLayout->addWidget(txtLog);
  logsLayout->addWidget(logContainer, 2);

  // Alertas
  FlutterCard *alertContainer = new FlutterCard(this);
  alertContainer->setStyleSheet(
      "QFrame { background-color: #fffbeb; }"); // Yellow tint base
  QVBoxLayout *aLayout = new QVBoxLayout(alertContainer);
  aLayout->setContentsMargins(12, 12, 12, 12);
  aLayout->setSpacing(8);

  QLabel *lblAlert = new QLabel("Alertas de Calidad", alertContainer);
  lblAlert->setStyleSheet("font-size: 14px; font-weight: 600; text-transform: "
                          "uppercase; color: #92400e;");
  aLayout->addWidget(lblAlert);

  txtAlertas = new QTextEdit(this);
  txtAlertas->setReadOnly(true);
  txtAlertas->setMinimumHeight(250);
  txtAlertas->setPlaceholderText("Sin alertas de calidad detectadas.");
  txtAlertas->setStyleSheet(R"(
    QTextEdit {
      background: transparent;
      color: #92400e;
      font-size: 13px;
      border: none;
    }
  )");
  aLayout->addWidget(txtAlertas);
  logsLayout->addWidget(alertContainer, 1);

  layout->addLayout(logsLayout);

  // Boton continuar
  btnContinuar = new FlutterFilledButton("🔍 Explorar Cubo (Fase 4)", this);
  btnContinuar->setMinimumHeight(48);
  btnContinuar->setIcon(MaterialIcons::instance().search());
  btnContinuar->setVisible(false);
  btnContinuar->setEnabled(false);
  connect(btnContinuar, &QPushButton::clicked, this,
          &ConsolaProgreso::cargaCompletada);
  layout->addWidget(btnContinuar);
}

QWidget *ConsolaProgreso::crearCardEstadistica(const QString &etiqueta,
                                               const QString &valor,
                                               const QString &color) {
  FlutterCard *card = new FlutterCard(this);
  card->setStyleSheet(QString(R"(
      QFrame {
        background: white;
        border-radius: 8px;
        border-left: 4px solid %1;
      }
  )")
                          .arg(color));

  QVBoxLayout *layout = new QVBoxLayout(card);
  layout->setSpacing(2);
  layout->setContentsMargins(12, 12, 12, 12);

  QLabel *lblValor = new QLabel(valor, card);
  lblValor->setObjectName("valorCard");
  lblValor->setStyleSheet(
      QString("font-size: 24px; font-weight: 700; color: %1;").arg(color));
  lblValor->setAlignment(Qt::AlignCenter);

  QLabel *lblEtiqueta = new QLabel(etiqueta, card);
  lblEtiqueta->setStyleSheet("font-size: 12px; font-weight: 600; "
                             "text-transform: uppercase; color: #6b7280;");
  lblEtiqueta->setAlignment(Qt::AlignCenter);

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
