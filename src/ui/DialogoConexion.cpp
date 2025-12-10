#include "DialogoConexion.h"
#include "../core/GestorBaseDatos.h"
#include "Estilos.h"
#include <QCoreApplication>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QFormLayout>
#include <QGraphicsDropShadowEffect>
#include <QMessageBox>
#include <QMimeData>
#include <QPropertyAnimation>
#include <QSqlError>
#include <QSqlQuery>
#include <QTimer>

DialogoConexion::DialogoConexion(QWidget *parent) : QDialog(parent) {
  setAcceptDrops(true);
  configurarUi();
}

DialogoConexion::~DialogoConexion() {}

void DialogoConexion::configurarUi() {
  setWindowTitle("Conexion a Base de Datos - Cubo Vision");
  setFixedSize(520, 600);
  setModal(true);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setSpacing(16);
  mainLayout->setContentsMargins(32, 32, 32, 32);

  // ========== ENCABEZADO ==========
  QLabel *lblTitulo = new QLabel("Cubo Vision", this);
  lblTitulo->setStyleSheet(R"(
    font-size: 28px;
    font-weight: 700;
    color: #1f2937;
    letter-spacing: 1px;
  )");
  lblTitulo->setAlignment(Qt::AlignCenter);

  QLabel *lblSubtitulo = new QLabel("Sistema OLAP Multidimensional", this);
  lblSubtitulo->setStyleSheet(R"(
    font-size: 14px;
    color: #6b7280;
    margin-bottom: 16px;
  )");
  lblSubtitulo->setAlignment(Qt::AlignCenter);

  mainLayout->addWidget(lblTitulo);
  mainLayout->addWidget(lblSubtitulo);

  // ========== FORMULARIO DE CONEXION ==========
  QWidget *formWidget = new QWidget(this);
  formWidget->setStyleSheet(R"(
    background: rgba(255, 255, 255, 0.9);
    border: 1px solid #e5e7eb;
    border-radius: 12px;
    padding: 20px;
  )");
  QFormLayout *formLayout = new QFormLayout(formWidget);
  formLayout->setSpacing(14);
  formLayout->setContentsMargins(20, 20, 20, 20);

  inputHost = new QLineEdit("localhost", this);
  inputHost->setPlaceholderText("Ej: localhost o 192.168.1.100");

  inputPuerto = new QSpinBox(this);
  inputPuerto->setRange(1, 65535);
  inputPuerto->setValue(5432);

  inputUsuario = new QLineEdit("postgres", this);
  inputUsuario->setPlaceholderText("Usuario de PostgreSQL");

  inputPassword = new QLineEdit("laza", this);
  inputPassword->setEchoMode(QLineEdit::Password);
  inputPassword->setPlaceholderText("Contrasena");

  inputBaseDatos = new QLineEdit("bd2025", this);
  inputBaseDatos->setPlaceholderText("Nombre de la base de datos");

  formLayout->addRow("Host:", inputHost);
  formLayout->addRow("Puerto:", inputPuerto);
  formLayout->addRow("Usuario:", inputUsuario);
  formLayout->addRow("Contrasena:", inputPassword);
  formLayout->addRow("Base de Datos:", inputBaseDatos);

  mainLayout->addWidget(formWidget);

  // ========== BOTON CONECTAR ==========
  btnConectar = new QPushButton("Conectar", this);
  btnConectar->setMinimumHeight(48);
  btnConectar->setCursor(Qt::PointingHandCursor);
  btnConectar->setStyleSheet(Estilos::obtenerEstiloBotonPrimario());

  QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
  shadow->setBlurRadius(15);
  shadow->setColor(QColor(37, 99, 235, 80));
  shadow->setOffset(0, 4);
  btnConectar->setGraphicsEffect(shadow);

  mainLayout->addWidget(btnConectar);

  // ========== ZONA DE ARRASTRAR SQL ==========
  QLabel *lblSemilla =
      new QLabel("Importar Archivo de Semillas (opcional)", this);
  lblSemilla->setStyleSheet(
      "font-weight: 600; color: #374151; margin-top: 12px;");
  mainLayout->addWidget(lblSemilla);

  dropZone = new QFrame(this);
  dropZone->setStyleSheet(Estilos::obtenerEstiloDropZone());
  dropZone->setMinimumHeight(80);
  dropZone->setAcceptDrops(true);

  QVBoxLayout *dropLayout = new QVBoxLayout(dropZone);
  dropLayout->setAlignment(Qt::AlignCenter);

  lblDropZone = new QLabel(
      "Arrastre un archivo .sql aqui\no haga clic para seleccionar", dropZone);
  lblDropZone->setStyleSheet("color: #6b7280; font-size: 13px;");
  lblDropZone->setAlignment(Qt::AlignCenter);
  dropLayout->addWidget(lblDropZone);

  mainLayout->addWidget(dropZone);

  // Boton alternativo para seleccionar archivo
  QPushButton *btnSeleccionarSQL =
      new QPushButton("Seleccionar Archivo SQL", this);
  btnSeleccionarSQL->setStyleSheet(R"(
    QPushButton {
      background: transparent;
      border: 1px solid #d1d5db;
      border-radius: 6px;
      color: #4b5563;
      padding: 8px 16px;
    }
    QPushButton:hover {
      background: #f3f4f6;
      border-color: #2563eb;
      color: #2563eb;
    }
  )");
  connect(btnSeleccionarSQL, &QPushButton::clicked, this,
          &DialogoConexion::seleccionarArchivoSQL);
  mainLayout->addWidget(btnSeleccionarSQL);

  // ========== ESTADO ==========
  lblEstado = new QLabel("Listo para conectar", this);
  lblEstado->setAlignment(Qt::AlignCenter);
  lblEstado->setStyleSheet("color: #6b7280; font-size: 13px; margin-top: 8px;");
  mainLayout->addWidget(lblEstado);

  mainLayout->addStretch();

  // Conectar senales
  connect(btnConectar, &QPushButton::clicked, this,
          &DialogoConexion::intentarConexion);

  // Estilo global del dialogo
  setStyleSheet(R"(
    QDialog {
      background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
        stop:0 #f0f4f8, stop:0.5 #ffffff, stop:1 #e8f0fe);
    }
    QLineEdit, QSpinBox {
      background-color: white;
      border: 1px solid #d1d5db;
      border-radius: 8px;
      padding: 10px 14px;
      color: #1f2937;
      font-size: 14px;
    }
    QLineEdit:focus, QSpinBox:focus {
      border: 2px solid #2563eb;
    }
    QLabel {
      color: #374151;
    }
  )");

  crearAnimacionEntrada();
}

void DialogoConexion::crearAnimacionEntrada() {
  QPropertyAnimation *animOpacidad =
      new QPropertyAnimation(this, "windowOpacity");
  animOpacidad->setDuration(250);
  animOpacidad->setStartValue(0.0);
  animOpacidad->setEndValue(1.0);
  animOpacidad->setEasingCurve(QEasingCurve::OutCubic);
  animOpacidad->start(QAbstractAnimation::DeleteWhenStopped);
}

void DialogoConexion::intentarConexion() {
  lblEstado->setText("Conectando...");
  lblEstado->setStyleSheet("color: #f59e0b; font-size: 13px;");
  btnConectar->setEnabled(false);
  QCoreApplication::processEvents();

  QString host = inputHost->text();
  int puerto = inputPuerto->value();
  QString usuario = inputUsuario->text();
  QString password = inputPassword->text();
  QString baseDatos = inputBaseDatos->text();
  QString errorMsg;

  if (GestorBaseDatos::instancia().conectar(host, puerto, usuario, password,
                                            baseDatos, errorMsg)) {
    lblEstado->setText("Conexion exitosa");
    lblEstado->setStyleSheet(
        "color: #10b981; font-size: 14px; font-weight: 600;");

    // Ejecutar semilla si hay archivo seleccionado
    if (!m_archivoSemilla.isEmpty()) {
      ejecutarArchivoSQL(m_archivoSemilla);
    }

    QTimer::singleShot(600, this, [this]() {
      emit conexionExitosa();
      accept();
    });
  } else {
    lblEstado->setText("Error de conexion");
    lblEstado->setStyleSheet(
        "color: #ef4444; font-size: 13px; font-weight: 600;");

    QPropertyAnimation *animError = new QPropertyAnimation(this, "pos");
    animError->setDuration(50);
    animError->setLoopCount(4);
    QPoint posOriginal = pos();
    animError->setKeyValueAt(0, posOriginal);
    animError->setKeyValueAt(0.25, posOriginal + QPoint(8, 0));
    animError->setKeyValueAt(0.75, posOriginal + QPoint(-8, 0));
    animError->setKeyValueAt(1, posOriginal);
    animError->start(QAbstractAnimation::DeleteWhenStopped);

    QMessageBox::critical(this, "Error de Conexion",
                          "No se pudo conectar a PostgreSQL:\n\n" + errorMsg);
    btnConectar->setEnabled(true);
  }
}

void DialogoConexion::seleccionarArchivoSQL() {
  QString archivo = QFileDialog::getOpenFileName(
      this, "Seleccionar Archivo SQL", "",
      "Archivos SQL (*.sql);;Todos los archivos (*)");

  if (!archivo.isEmpty()) {
    m_archivoSemilla = archivo;
    QFileInfo info(archivo);
    lblDropZone->setText("Archivo seleccionado:\n" + info.fileName());
    lblDropZone->setStyleSheet(
        "color: #10b981; font-size: 13px; font-weight: 600;");
  }
}

void DialogoConexion::ejecutarArchivoSQL(const QString &rutaArchivo) {
  QFile archivo(rutaArchivo);
  if (!archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::warning(this, "Error",
                         "No se pudo abrir el archivo:\n" + rutaArchivo);
    return;
  }

  QString contenidoSQL = QString::fromUtf8(archivo.readAll());
  archivo.close();

  lblEstado->setText("Ejecutando script SQL...");
  lblEstado->setStyleSheet("color: #f59e0b; font-size: 13px;");
  QCoreApplication::processEvents();

  QSqlQuery query(GestorBaseDatos::instancia().baseDatos());

  // Dividir por sentencias (simplificado, no maneja todos los casos)
  QStringList sentencias = contenidoSQL.split(";", Qt::SkipEmptyParts);

  int ejecutadas = 0;
  int errores = 0;

  for (const QString &sentencia : sentencias) {
    QString sql = sentencia.trimmed();
    if (sql.isEmpty() || sql.startsWith("--"))
      continue;

    if (!query.exec(sql)) {
      errores++;
      qWarning() << "Error SQL:" << query.lastError().text();
    } else {
      ejecutadas++;
    }
  }

  if (errores > 0) {
    lblEstado->setText(
        QString("Ejecutadas: %1, Errores: %2").arg(ejecutadas).arg(errores));
    lblEstado->setStyleSheet("color: #f59e0b; font-size: 13px;");
  } else {
    lblEstado->setText(
        QString("Script ejecutado: %1 sentencias").arg(ejecutadas));
    lblEstado->setStyleSheet(
        "color: #10b981; font-size: 13px; font-weight: 600;");
  }
}

void DialogoConexion::dragEnterEvent(QDragEnterEvent *event) {
  if (event->mimeData()->hasUrls()) {
    const QList<QUrl> urls = event->mimeData()->urls();
    if (!urls.isEmpty() && urls.first().toLocalFile().endsWith(".sql")) {
      event->acceptProposedAction();
      dropZone->setStyleSheet(R"(
        QFrame {
          background: rgba(219, 234, 254, 0.8);
          border: 2px dashed #2563eb;
          border-radius: 12px;
        }
      )");
    }
  }
}

void DialogoConexion::dragLeaveEvent(QDragLeaveEvent *event) {
  Q_UNUSED(event);
  dropZone->setStyleSheet(Estilos::obtenerEstiloDropZone());
}

void DialogoConexion::dropEvent(QDropEvent *event) {
  const QMimeData *mimeData = event->mimeData();
  if (mimeData->hasUrls()) {
    QString archivo = mimeData->urls().first().toLocalFile();
    if (archivo.endsWith(".sql")) {
      m_archivoSemilla = archivo;
      QFileInfo info(archivo);
      lblDropZone->setText("Archivo cargado:\n" + info.fileName());
      lblDropZone->setStyleSheet(
          "color: #10b981; font-size: 13px; font-weight: 600;");
      event->acceptProposedAction();
    }
  }
  dropZone->setStyleSheet(Estilos::obtenerEstiloDropZone());
}

void DialogoConexion::crearEfectoParticulas() {
  // Reservado para efectos visuales adicionales
}
