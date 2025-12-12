#include "DialogoConexion.h"
#include "../core/GestorBaseDatos.h"
#include "ToastNotifier.h"
#include "styles/FlutterTheme.h"
#include "widgets/FlutterWidgets.h"
#include <QCoreApplication>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QFormLayout>
#include <QMessageBox>
#include <QMimeData>
#include <QPropertyAnimation>
#include <QRegularExpression>
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
  setFixedSize(420, 600);
  setModal(true);

  // Apply Theme
  FlutterTheme::instance().applyThemeToWidget(this);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setSpacing(16);
  mainLayout->setContentsMargins(24, 24, 24, 24);

  // Titulo
  QLabel *lblTitulo = new QLabel("Cubo Vision", this);
  lblTitulo->setStyleSheet(
      "font-size: 24px; font-weight: 700; color: #6750a4;"); // Primary
  lblTitulo->setAlignment(Qt::AlignCenter);
  mainLayout->addWidget(lblTitulo);

  QLabel *lblSubtitulo = new QLabel("Conectar a PostgreSQL", this);
  lblSubtitulo->setStyleSheet(
      "font-size: 14px; margin-bottom: 12px; opacity: 0.7;");
  lblSubtitulo->setAlignment(Qt::AlignCenter);
  mainLayout->addWidget(lblSubtitulo);

  // Card Container for Form
  FlutterCard *card = new FlutterCard(this, 1);
  QVBoxLayout *cardLayout = new QVBoxLayout(card);
  cardLayout->setSpacing(16);
  cardLayout->setContentsMargins(16, 24, 16, 24);

  // Form Inputs
  inputHost = new FlutterTextField("Host (ej: localhost)", this);
  inputHost->setText("localhost");
  cardLayout->addWidget(inputHost);

  // Puerto (SpinBox needs native styling or wrapper, keeping simple for now)
  QLabel *lblPort = new QLabel("Puerto", this);
  lblPort->setProperty("floating-label", "true");
  cardLayout->addWidget(lblPort);

  inputPuerto = new QSpinBox(this);
  inputPuerto->setRange(1, 65535);
  inputPuerto->setValue(5432);
  inputPuerto->setFixedHeight(40);
  cardLayout->addWidget(inputPuerto);

  inputUsuario = new FlutterTextField("Usuario", this);
  inputUsuario->setText("postgres");
  cardLayout->addWidget(inputUsuario);

  inputPassword = new FlutterTextField("Contraseña", this);
  inputPassword->setText("laza");
  inputPassword->setEchoMode(QLineEdit::Password);
  cardLayout->addWidget(inputPassword);

  // Database
  QHBoxLayout *bdLayout = new QHBoxLayout();
  comboBaseDatos = new QComboBox(this);
  comboBaseDatos->setEditable(true);
  comboBaseDatos->setPlaceholderText("bd2025");
  comboBaseDatos->addItem("bd2025");
  comboBaseDatos->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  comboBaseDatos->setFixedHeight(40);

  btnActualizarBD = new QPushButton("🔄", this);
  btnActualizarBD->setFixedWidth(40);
  btnActualizarBD->setFixedHeight(40);
  // TODO: Use FlutterIconButton if time permits replacing this
  connect(btnActualizarBD, &QPushButton::clicked, this,
          &DialogoConexion::actualizarBasesDatos);

  bdLayout->addWidget(comboBaseDatos);
  bdLayout->addWidget(btnActualizarBD);
  cardLayout->addLayout(bdLayout);

  mainLayout->addWidget(card);

  // Boton conectar
  btnConectar = new FlutterElevatedButton("CONECTAR", this);
  btnConectar->setMinimumHeight(48);
  mainLayout->addWidget(btnConectar);

  // Zona SQL
  QLabel *lblSemilla = new QLabel("Importar semilla SQL (opcional)", this);
  lblSemilla->setStyleSheet(
      "font-size: 11px; color: #64748b; margin-top: 8px;");
  mainLayout->addWidget(lblSemilla);

  dropZone = new QFrame(this);
  dropZone->setStyleSheet(R"(
    QFrame {
      background: #f8fafc;
      border: 2px dashed #cbd5e1;
      border-radius: 8px;
    }
  )");
  dropZone->setMinimumHeight(50);
  dropZone->setMaximumHeight(50);
  dropZone->setAcceptDrops(true);

  QVBoxLayout *dropLayout = new QVBoxLayout(dropZone);
  dropLayout->setContentsMargins(8, 8, 8, 8);

  lblDropZone = new QLabel("Arrastrar archivo .sql aqui", dropZone);
  lblDropZone->setStyleSheet("color: #94a3b8; font-size: 11px;");
  lblDropZone->setAlignment(Qt::AlignCenter);
  dropLayout->addWidget(lblDropZone);

  mainLayout->addWidget(dropZone);

  // Estado
  lblEstado = new QLabel("", this);
  lblEstado->setAlignment(Qt::AlignCenter);
  lblEstado->setStyleSheet(
      "color: #64748b; font-size: 11px; min-height: 20px;");
  mainLayout->addWidget(lblEstado);

  // Conectar senales
  connect(btnConectar, &QPushButton::clicked, this,
          &DialogoConexion::intentarConexion);

  // Estilo del dialogo
  setStyleSheet("QDialog { background: white; }");
}

void DialogoConexion::actualizarBasesDatos() {
  QString host = inputHost->text();
  int puerto = inputPuerto->value();
  QString usuario = inputUsuario->text();
  QString password = inputPassword->text();

  if (host.isEmpty() || usuario.isEmpty()) {
    ToastNotifier::mostrar(this, "Ingrese Host y Usuario",
                           ToastNotifier::Advertencia);
    return;
  }

  btnActualizarBD->setEnabled(false);
  lblEstado->setText("Listando bases de datos...");
  QCoreApplication::processEvents();

  QString errorMsg;
  QStringList dbs = GestorBaseDatos::instancia().listarBasesDatos(
      host, puerto, usuario, password, errorMsg);

  if (errorMsg.isEmpty()) {
    comboBaseDatos->clear();
    comboBaseDatos->addItems(dbs);
    ToastNotifier::mostrar(this, "Bases de datos actualizadas",
                           ToastNotifier::Exito);
    lblEstado->setText("Lista actualizada");
  } else {
    ToastNotifier::mostrar(this, "Error al listar bases: " + errorMsg,
                           ToastNotifier::Error);
    lblEstado->setText("Error de conexion");
  }

  btnActualizarBD->setEnabled(true);
}

bool DialogoConexion::validarFormulario() {
  QStringList errores;

  if (inputHost->text().trimmed().isEmpty())
    errores << "Host requerido";
  if (inputUsuario->text().trimmed().isEmpty())
    errores << "Usuario requerido";

  QString baseDatos = comboBaseDatos->currentText().trimmed();
  if (baseDatos.isEmpty())
    errores << "Base de datos requerida";
  else if (baseDatos.contains(" "))
    errores << "Nombre BD sin espacios";

  if (!errores.isEmpty()) {
    lblEstado->setText("Errores de validacion");
    lblEstado->setStyleSheet("color: #ef4444; font-size: 11px;");
    ToastNotifier::mostrar(this, errores.first(), ToastNotifier::Advertencia);
    return false;
  }

  return true;
}

void DialogoConexion::intentarConexion() {
  if (!validarFormulario())
    return;

  lblEstado->setText("Conectando...");
  lblEstado->setStyleSheet("color: #f59e0b; font-size: 11px;");
  btnConectar->setEnabled(false);
  QCoreApplication::processEvents();

  QString host = inputHost->text().trimmed();
  int puerto = inputPuerto->value();
  QString usuario = inputUsuario->text().trimmed();
  QString password = inputPassword->text();
  QString baseDatos = comboBaseDatos->currentText().trimmed();
  QString errorMsg;

  if (GestorBaseDatos::instancia().conectar(host, puerto, usuario, password,
                                            baseDatos, errorMsg)) {
    lblEstado->setText("Conectado exitosamente");
    lblEstado->setStyleSheet(
        "color: #10b981; font-size: 11px; font-weight: 600;");

    if (!m_archivoSemilla.isEmpty()) {
      ejecutarArchivoSQL(m_archivoSemilla);
    }

    QTimer::singleShot(500, this, [this]() {
      emit conexionExitosa();
      accept();
    });
  } else {
    lblEstado->setText("Error de conexion");
    lblEstado->setStyleSheet("color: #ef4444; font-size: 11px;");
    ToastNotifier::mostrar(this, "Error: " + errorMsg, ToastNotifier::Error);
    btnConectar->setEnabled(true);
  }
}

void DialogoConexion::ejecutarArchivoSQL(const QString &rutaArchivo) {
  lblEstado->setText("Ejecutando semilla SQL...");
  ToastNotifier::mostrar(this, "Iniciando carga de SQL...",
                         ToastNotifier::Info);
  QCoreApplication::processEvents();

  QFile archivo(rutaArchivo);
  if (!archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
    ToastNotifier::mostrar(this, "No se pudo leer archivo SQL",
                           ToastNotifier::Error);
    return;
  }

  QString contenidoSQL = QString::fromUtf8(archivo.readAll());
  archivo.close();

  QSqlQuery query(GestorBaseDatos::instancia().baseDatos());
  QStringList sentencias = contenidoSQL.split(";", Qt::SkipEmptyParts);

  int total = 0;
  int errores = 0;

  // Contar sentencias reales (ignorando comentarios puros si los split deja
  // algo)
  int totalReal = 0;
  for (const QString &s : sentencias) {
    if (!s.trimmed().isEmpty())
      totalReal++;
  }

  int progreso = 0;
  for (const QString &sentencia : sentencias) {
    QString sql = sentencia.trimmed();
    if (!sql.isEmpty() && !sql.startsWith("--")) {
      total++;
      if (!query.exec(sql)) {
        errores++;
        qDebug() << "Error SQL:" << query.lastError().text() << " en "
                 << sql.left(50);
      }
      progreso++;
      if (progreso % 10 == 0) {
        lblEstado->setText(
            QString("Ejecutando SQL (%1/%2)...").arg(progreso).arg(totalReal));
        QCoreApplication::processEvents();
      }
    }
  }

  if (errores == 0) {
    ToastNotifier::mostrar(
        this, QString("Semilla cargada: %1 sentencias OK").arg(total),
        ToastNotifier::Exito);
    lblEstado->setText("Semilla cargada exitosamente");
  } else {
    ToastNotifier::mostrar(
        this,
        QString("Carga con errores: %1/%2 fallos").arg(errores).arg(total),
        ToastNotifier::Advertencia);
    lblEstado->setText("Semilla cargada con advertencias");
  }
}

// ... dragged event methods remain largely the same, included below for
// completeness of replacement
void DialogoConexion::seleccionarArchivoSQL() { /* Unused directly now */ }
void DialogoConexion::crearAnimacionEntrada() {}
void DialogoConexion::crearEfectoParticulas() {}

void DialogoConexion::dragEnterEvent(QDragEnterEvent *event) {
  if (event->mimeData()->hasUrls() &&
      event->mimeData()->urls().first().toLocalFile().endsWith(".sql")) {
    event->acceptProposedAction();
    dropZone->setStyleSheet("QFrame { background: #eff6ff; border: 2px dashed "
                            "#2563eb; border-radius: 8px; }");
  }
}
void DialogoConexion::dragLeaveEvent(QDragLeaveEvent *event) {
  Q_UNUSED(event);
  dropZone->setStyleSheet("QFrame { background: #f8fafc; border: 2px dashed "
                          "#cbd5e1; border-radius: 8px; }");
}
void DialogoConexion::dropEvent(QDropEvent *event) {
  const QMimeData *mimeData = event->mimeData();
  if (mimeData->hasUrls()) {
    QString archivo = mimeData->urls().first().toLocalFile();
    if (archivo.endsWith(".sql")) {
      m_archivoSemilla = archivo;
      lblDropZone->setText("Archivo listo: " + QFileInfo(archivo).fileName());
      lblDropZone->setStyleSheet("color: #10b981; font-weight: bold;");
      event->acceptProposedAction();
    }
  }
  dropZone->setStyleSheet("QFrame { background: #f8fafc; border: 2px dashed "
                          "#cbd5e1; border-radius: 8px; }");
}
