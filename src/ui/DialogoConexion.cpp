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
  setWindowTitle("Conexion - Cubo Vision");
  setFixedSize(400, 480);
  setModal(true);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setSpacing(12);
  mainLayout->setContentsMargins(24, 20, 24, 20);

  // Titulo compacto
  QLabel *lblTitulo = new QLabel("Cubo Vision", this);
  lblTitulo->setStyleSheet(
      "font-size: 20px; font-weight: 700; color: #1f2937;");
  lblTitulo->setAlignment(Qt::AlignCenter);
  mainLayout->addWidget(lblTitulo);

  QLabel *lblSubtitulo = new QLabel("Conectar a PostgreSQL", this);
  lblSubtitulo->setStyleSheet(
      "font-size: 12px; color: #6b7280; margin-bottom: 8px;");
  lblSubtitulo->setAlignment(Qt::AlignCenter);
  mainLayout->addWidget(lblSubtitulo);

  // Formulario compacto
  QWidget *formWidget = new QWidget(this);
  formWidget->setStyleSheet(R"(
    background: #f9fafb;
    border: 1px solid #e5e7eb;
    border-radius: 8px;
    padding: 12px;
  )");
  QFormLayout *formLayout = new QFormLayout(formWidget);
  formLayout->setSpacing(10);
  formLayout->setContentsMargins(12, 12, 12, 12);
  formLayout->setLabelAlignment(Qt::AlignRight);

  QString inputStyle = R"(
    QLineEdit, QSpinBox {
      background: white;
      border: 1px solid #d1d5db;
      border-radius: 6px;
      padding: 8px 10px;
      font-size: 12px;
    }
    QLineEdit:focus, QSpinBox:focus {
      border: 2px solid #2563eb;
    }
  )";

  inputHost = new QLineEdit("localhost", this);
  inputHost->setStyleSheet(inputStyle);

  inputPuerto = new QSpinBox(this);
  inputPuerto->setRange(1, 65535);
  inputPuerto->setValue(5432);
  inputPuerto->setStyleSheet(inputStyle);

  inputUsuario = new QLineEdit("postgres", this);
  inputUsuario->setStyleSheet(inputStyle);

  inputPassword = new QLineEdit("laza", this);
  inputPassword->setEchoMode(QLineEdit::Password);
  inputPassword->setStyleSheet(inputStyle);

  inputBaseDatos = new QLineEdit("bd2025", this);
  inputBaseDatos->setStyleSheet(inputStyle);

  formLayout->addRow("Host:", inputHost);
  formLayout->addRow("Puerto:", inputPuerto);
  formLayout->addRow("Usuario:", inputUsuario);
  formLayout->addRow("Clave:", inputPassword);
  formLayout->addRow("Base:", inputBaseDatos);

  mainLayout->addWidget(formWidget);

  // Boton conectar
  btnConectar = new QPushButton("Conectar", this);
  btnConectar->setMinimumHeight(40);
  btnConectar->setCursor(Qt::PointingHandCursor);
  btnConectar->setStyleSheet(R"(
    QPushButton {
      background: #2563eb;
      border: none;
      border-radius: 8px;
      color: white;
      font-weight: 600;
      font-size: 13px;
    }
    QPushButton:hover {
      background: #1d4ed8;
    }
    QPushButton:pressed {
      background: #1e40af;
    }
  )");
  mainLayout->addWidget(btnConectar);

  // Zona de arrastrar SQL (compacta)
  QLabel *lblSemilla = new QLabel("Importar semilla SQL (opcional)", this);
  lblSemilla->setStyleSheet(
      "font-size: 11px; color: #6b7280; margin-top: 8px;");
  mainLayout->addWidget(lblSemilla);

  dropZone = new QFrame(this);
  dropZone->setStyleSheet(R"(
    QFrame {
      background: #f9fafb;
      border: 1px dashed #d1d5db;
      border-radius: 6px;
    }
  )");
  dropZone->setMinimumHeight(50);
  dropZone->setMaximumHeight(50);
  dropZone->setAcceptDrops(true);

  QVBoxLayout *dropLayout = new QVBoxLayout(dropZone);
  dropLayout->setContentsMargins(8, 8, 8, 8);

  lblDropZone = new QLabel("Arrastrar archivo .sql aqui", dropZone);
  lblDropZone->setStyleSheet("color: #9ca3af; font-size: 11px;");
  lblDropZone->setAlignment(Qt::AlignCenter);
  dropLayout->addWidget(lblDropZone);

  mainLayout->addWidget(dropZone);

  // Estado
  lblEstado = new QLabel("", this);
  lblEstado->setAlignment(Qt::AlignCenter);
  lblEstado->setStyleSheet("color: #6b7280; font-size: 11px;");
  mainLayout->addWidget(lblEstado);

  // Conectar senales
  connect(btnConectar, &QPushButton::clicked, this,
          &DialogoConexion::intentarConexion);

  // Estilo del dialogo
  setStyleSheet("QDialog { background: white; }");
}

void DialogoConexion::crearAnimacionEntrada() {
  // Animacion simple de fade
}

void DialogoConexion::intentarConexion() {
  lblEstado->setText("Conectando...");
  lblEstado->setStyleSheet("color: #f59e0b; font-size: 11px;");
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
    lblEstado->setText("Conectado");
    lblEstado->setStyleSheet(
        "color: #10b981; font-size: 11px; font-weight: 600;");

    if (!m_archivoSemilla.isEmpty()) {
      ejecutarArchivoSQL(m_archivoSemilla);
    }

    QTimer::singleShot(400, this, [this]() {
      emit conexionExitosa();
      accept();
    });
  } else {
    lblEstado->setText("Error de conexion");
    lblEstado->setStyleSheet("color: #ef4444; font-size: 11px;");

    QMessageBox::warning(this, "Error", "No se pudo conectar:\n" + errorMsg);
    btnConectar->setEnabled(true);
  }
}

void DialogoConexion::seleccionarArchivoSQL() {
  QString archivo = QFileDialog::getOpenFileName(this, "Seleccionar SQL", "",
                                                 "Archivos SQL (*.sql)");

  if (!archivo.isEmpty()) {
    m_archivoSemilla = archivo;
    QFileInfo info(archivo);
    lblDropZone->setText(info.fileName());
    lblDropZone->setStyleSheet("color: #10b981; font-size: 11px;");
  }
}

void DialogoConexion::ejecutarArchivoSQL(const QString &rutaArchivo) {
  QFile archivo(rutaArchivo);
  if (!archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return;
  }

  QString contenidoSQL = QString::fromUtf8(archivo.readAll());
  archivo.close();

  QSqlQuery query(GestorBaseDatos::instancia().baseDatos());
  QStringList sentencias = contenidoSQL.split(";", Qt::SkipEmptyParts);

  for (const QString &sentencia : sentencias) {
    QString sql = sentencia.trimmed();
    if (!sql.isEmpty() && !sql.startsWith("--")) {
      query.exec(sql);
    }
  }
}

void DialogoConexion::dragEnterEvent(QDragEnterEvent *event) {
  if (event->mimeData()->hasUrls()) {
    const QList<QUrl> urls = event->mimeData()->urls();
    if (!urls.isEmpty() && urls.first().toLocalFile().endsWith(".sql")) {
      event->acceptProposedAction();
      dropZone->setStyleSheet(R"(
        QFrame {
          background: #dbeafe;
          border: 1px dashed #2563eb;
          border-radius: 6px;
        }
      )");
    }
  }
}

void DialogoConexion::dragLeaveEvent(QDragLeaveEvent *event) {
  Q_UNUSED(event);
  dropZone->setStyleSheet(R"(
    QFrame {
      background: #f9fafb;
      border: 1px dashed #d1d5db;
      border-radius: 6px;
    }
  )");
}

void DialogoConexion::dropEvent(QDropEvent *event) {
  const QMimeData *mimeData = event->mimeData();
  if (mimeData->hasUrls()) {
    QString archivo = mimeData->urls().first().toLocalFile();
    if (archivo.endsWith(".sql")) {
      m_archivoSemilla = archivo;
      QFileInfo info(archivo);
      lblDropZone->setText(info.fileName());
      lblDropZone->setStyleSheet(
          "color: #10b981; font-size: 11px; font-weight: 500;");
      event->acceptProposedAction();
    }
  }
  dropZone->setStyleSheet(R"(
    QFrame {
      background: #f9fafb;
      border: 1px dashed #d1d5db;
      border-radius: 6px;
    }
  )");
}

void DialogoConexion::crearEfectoParticulas() {}
