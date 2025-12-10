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
  layout->setSpacing(16);
  layout->setContentsMargins(24, 24, 24, 24);

  // Header
  QLabel *header = new QLabel("Fase 3: Carga y Procesamiento de Datos", this);
  header->setStyleSheet(Estilos::obtenerEstiloTituloSeccion("#10b981"));
  layout->addWidget(header);

  // Panel superior con estadisticas
  QHBoxLayout *statsLayout = new QHBoxLayout();
  statsLayout->setSpacing(16);

  // Card: Registros procesados
  QWidget *cardRegistros = crearCardEstadistica("Registros", "0", "#2563eb");
  statsLayout->addWidget(cardRegistros);
  lblRegistros = cardRegistros->findChild<QLabel *>("valorCard");

  // Card: Velocidad
  QWidget *cardVelocidad =
      crearCardEstadistica("Registros/seg", "0", "#10b981");
  statsLayout->addWidget(cardVelocidad);
  lblVelocidad = cardVelocidad->findChild<QLabel *>("valorCard");

  // Card: Tiempo estimado
  QWidget *cardTiempo =
      crearCardEstadistica("Tiempo restante", "--:--", "#f59e0b");
  statsLayout->addWidget(cardTiempo);
  lblTiempo = cardTiempo->findChild<QLabel *>("valorCard");

  // Card: Calidad de datos
  QWidget *cardCalidad = crearCardEstadistica("Calidad", "100%", "#6366f1");
  statsLayout->addWidget(cardCalidad);
  lblCalidad = cardCalidad->findChild<QLabel *>("valorCard");

  layout->addLayout(statsLayout);

  // Barra de progreso
  QGroupBox *grpProgreso = new QGroupBox("Progreso General", this);
  QVBoxLayout *progLayout = new QVBoxLayout(grpProgreso);

  barProgreso = new QProgressBar(this);
  barProgreso->setRange(0, 100);
  barProgreso->setValue(0);
  barProgreso->setMinimumHeight(28);
  barProgreso->setStyleSheet(R"(
    QProgressBar {
      border: none;
      border-radius: 14px;
      background-color: #e5e7eb;
      text-align: center;
      color: #374151;
      font-weight: 600;
    }
    QProgressBar::chunk {
      background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
        stop:0 #10b981, stop:1 #6366f1);
      border-radius: 14px;
    }
  )");
  progLayout->addWidget(barProgreso);

  lblEstado = new QLabel("Preparando carga de datos...", this);
  lblEstado->setStyleSheet("color: #6b7280; font-size: 12px; margin-top: 4px;");
  progLayout->addWidget(lblEstado);

  layout->addWidget(grpProgreso);

  // Panel de calidad de datos
  QHBoxLayout *panelsLayout = new QHBoxLayout();
  panelsLayout->setSpacing(16);

  // Log de procesamiento
  QGroupBox *grpLog = new QGroupBox("Bitacora de Procesamiento", this);
  QVBoxLayout *logLayout = new QVBoxLayout(grpLog);

  txtLog = new QTextEdit(this);
  txtLog->setReadOnly(true);
  txtLog->setStyleSheet(R"(
    QTextEdit {
      background-color: #1f2937;
      color: #10b981;
      font-family: 'Consolas', 'Fira Code', monospace;
      font-size: 11px;
      border: 1px solid #374151;
      border-radius: 8px;
      padding: 8px;
    }
  )");
  logLayout->addWidget(txtLog);
  panelsLayout->addWidget(grpLog, 2);

  // Panel de alertas de calidad
  QGroupBox *grpCalidad = new QGroupBox("Monitor de Calidad", this);
  QVBoxLayout *calidadLayout = new QVBoxLayout(grpCalidad);

  txtAlertas = new QTextEdit(this);
  txtAlertas->setReadOnly(true);
  txtAlertas->setStyleSheet(R"(
    QTextEdit {
      background-color: #fefce8;
      color: #854d0e;
      font-family: 'Segoe UI', sans-serif;
      font-size: 12px;
      border: 1px solid #fef08a;
      border-radius: 8px;
      padding: 8px;
    }
  )");
  txtAlertas->setPlaceholderText("Sin alertas de calidad detectadas.");
  calidadLayout->addWidget(txtAlertas);
  panelsLayout->addWidget(grpCalidad, 1);

  layout->addLayout(panelsLayout);

  // Boton continuar
  btnContinuar = new QPushButton("Explorar Cubo OLAP (Fase 4)", this);
  btnContinuar->setMinimumHeight(48);
  btnContinuar->setCursor(Qt::PointingHandCursor);
  btnContinuar->setStyleSheet(Estilos::obtenerEstiloBotonExito());
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
    background: rgba(255, 255, 255, 0.9);
    border: 1px solid #e5e7eb;
    border-left: 4px solid %1;
    border-radius: 8px;
    padding: 12px;
  )")
                          .arg(color));

  QVBoxLayout *layout = new QVBoxLayout(card);
  layout->setSpacing(4);
  layout->setContentsMargins(12, 8, 12, 8);

  QLabel *lblValor = new QLabel(valor, card);
  lblValor->setObjectName("valorCard");
  lblValor->setStyleSheet(
      QString("font-size: 22px; font-weight: 700; color: %1;").arg(color));

  QLabel *lblEtiqueta = new QLabel(etiqueta, card);
  lblEtiqueta->setStyleSheet("font-size: 11px; color: #6b7280;");

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
  if (lblRegistros) {
    QString regStr = registros > 1000
                         ? QString::number(registros / 1000.0, 'f', 1) + "K"
                         : QString::number(registros);
    lblRegistros->setText(regStr);
  }
  if (lblVelocidad)
    lblVelocidad->setText(QString::number((int)velocidad));
  if (lblTiempo)
    lblTiempo->setText(tiempoRestante);
  if (lblCalidad) {
    lblCalidad->setText(QString::number(calidad) + "%");
    if (calidad < 80) {
      lblCalidad->setStyleSheet(
          "font-size: 22px; font-weight: 700; color: #ef4444;");
    } else if (calidad < 95) {
      lblCalidad->setStyleSheet(
          "font-size: 22px; font-weight: 700; color: #f59e0b;");
    }
  }
}

void ConsolaProgreso::agregarAlerta(const QString &mensaje,
                                    const QString &severidad) {
  QString icono = "INFO";
  QString color = "#1e40af";

  if (severidad == "WARN") {
    icono = "ADVERTENCIA";
    color = "#b45309";
  } else if (severidad == "ERROR") {
    icono = "ERROR";
    color = "#dc2626";
  }

  QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss");
  QString entry = QString("<span style='color: #666;'>[%1]</span> "
                          "<b style='color: %2;'>[%3]</b> %4<br>")
                      .arg(timestamp)
                      .arg(color)
                      .arg(icono)
                      .arg(mensaje);

  txtAlertas->insertHtml(entry);
}

void ConsolaProgreso::agregarLog(const QString &mensaje, const QString &tipo) {
  QString color = "#10b981"; // INFO
  if (tipo == "WARN")
    color = "#fbbf24";
  if (tipo == "ERROR")
    color = "#ef4444";
  if (tipo == "SUCCESS")
    color = "#22c55e";

  QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss");
  QString logEntry = QString("<span style='color: #6b7280;'>[%1]</span> "
                             "<span style='color: %2;'>[%3] %4</span><br>")
                         .arg(timestamp)
                         .arg(color)
                         .arg(tipo)
                         .arg(mensaje);

  txtLog->insertHtml(logEntry);
  txtLog->verticalScrollBar()->setValue(txtLog->verticalScrollBar()->maximum());
  lblEstado->setText(mensaje);
}

void ConsolaProgreso::mostrarBotonContinuar() {
  btnContinuar->setVisible(true);
  btnContinuar->setEnabled(true);
}
