#include "DialogoConexion.h"
#include "../core/GestorBaseDatos.h"
#include "Estilos.h"
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
  setFixedSize(420, 520);
  setModal(true);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setSpacing(12);
  mainLayout->setContentsMargins(28, 24, 28, 24);

  // Titulo
  QLabel *lblTitulo = new QLabel("Cubo Vision", this);
  lblTitulo->setStyleSheet(
      "font-size: 22px; font-weight: 700; color: #1e3a8a;");
  lblTitulo->setAlignment(Qt::AlignCenter);
  mainLayout->addWidget(lblTitulo);

  QLabel *lblSubtitulo = new QLabel("Conectar a PostgreSQL", this);
  lblSubtitulo->setStyleSheet(
      "font-size: 12px; color: #64748b; margin-bottom: 12px;");
  lblSubtitulo->setAlignment(Qt::AlignCenter);
  mainLayout->addWidget(lblSubtitulo);

  // Formulario
  QWidget *formWidget = new QWidget(this);
  formWidget->setStyleSheet(R"(
    background: #f8fafc;
    border: 1px solid #e2e8f0;
    border-radius: 10px;
  )");
  QFormLayout *formLayout = new QFormLayout(formWidget);
  formLayout->setSpacing(12);
  formLayout->setContentsMargins(16, 16, 16, 16);
  formLayout->setLabelAlignment(Qt::AlignRight);

  QString inputStyle = R"(
    QLineEdit, QSpinBox {
      background: white;
      border: 1px solid #d1d5db;
      border-radius: 6px;
      padding: 10px 12px;
      font-size: 13px;
    }
    QLineEdit:focus, QSpinBox:focus {
      border: 2px solid #2563eb;
    }
  )";

  QString labelStyle = "font-size: 12px; color: #374151; font-weight: 500;";

  inputHost = new QLineEdit("localhost", this);
  inputHost->setStyleSheet(inputStyle);
  inputHost->setPlaceholderText("ej: localhost o 192.168.1.1");
  QLabel *lblHost = new QLabel("Host:", this);
  lblHost->setStyleSheet(labelStyle);
  formLayout->addRow(lblHost, inputHost);

  inputPuerto = new QSpinBox(this);
  inputPuerto->setRange(1, 65535);
  inputPuerto->setValue(5432);
  inputPuerto->setStyleSheet(inputStyle);
  QLabel *lblPuerto = new QLabel("Puerto:", this);
  lblPuerto->setStyleSheet(labelStyle);
  formLayout->addRow(lblPuerto, inputPuerto);

  inputUsuario = new QLineEdit("postgres", this);
  inputUsuario->setStyleSheet(inputStyle);
  inputUsuario->setPlaceholderText("ej: postgres");
  QLabel *lblUsuario = new QLabel("Usuario:", this);
  lblUsuario->setStyleSheet(labelStyle);
  formLayout->addRow(lblUsuario, inputUsuario);

  inputPassword = new QLineEdit("laza", this);
  inputPassword->setEchoMode(QLineEdit::Password);
  inputPassword->setStyleSheet(inputStyle);
  QLabel *lblPassword = new QLabel("Clave:", this);
  lblPassword->setStyleSheet(labelStyle);
  formLayout->addRow(lblPassword, inputPassword);

  inputBaseDatos = new QLineEdit("bd2025", this);
  inputBaseDatos->setStyleSheet(inputStyle);
  inputBaseDatos->setPlaceholderText("ej: bd2025");
  QLabel *lblBase = new QLabel("Base:", this);
  lblBase->setStyleSheet(labelStyle);
  formLayout->addRow(lblBase, inputBaseDatos);

  mainLayout->addWidget(formWidget);

  // Boton conectar
  btnConectar = new QPushButton("Conectar", this);
  btnConectar->setMinimumHeight(44);
  btnConectar->setCursor(Qt::PointingHandCursor);
  btnConectar->setStyleSheet(R"(
    QPushButton {
      background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
        stop:0 #2563eb, stop:1 #6366f1);
      border: none;
      border-radius: 8px;
      color: white;
      font-weight: 600;
      font-size: 14px;
    }
    QPushButton:hover {
      background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
        stop:0 #1d4ed8, stop:1 #4f46e5);
    }
    QPushButton:disabled {
      background: #94a3b8;
    }
  )");
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

bool DialogoConexion::validarFormulario() {
  QStringList errores;

  // Validar host
  QString host = inputHost->text().trimmed();
  if (host.isEmpty()) {
    errores << "El host es requerido";
    inputHost->setStyleSheet(inputHost->styleSheet() +
                             "border-color: #ef4444;");
  } else {
    // Validar formato de IP o hostname
    QRegularExpression ipRegex("^(\\d{1,3}\\.){3}\\d{1,3}$");
    QRegularExpression hostRegex("^[a-zA-Z][a-zA-Z0-9.-]*$");
    if (!ipRegex.match(host).hasMatch() && !hostRegex.match(host).hasMatch()) {
      errores << "Formato de host invalido";
    }
  }

  // Validar puerto
  int puerto = inputPuerto->value();
  if (puerto < 1 || puerto > 65535) {
    errores << "Puerto debe estar entre 1 y 65535";
  }

  // Validar usuario
  QString usuario = inputUsuario->text().trimmed();
  if (usuario.isEmpty()) {
    errores << "El usuario es requerido";
  } else if (usuario.length() < 2) {
    errores << "Usuario debe tener al menos 2 caracteres";
  }

  // Validar base de datos
  QString baseDatos = inputBaseDatos->text().trimmed();
  if (baseDatos.isEmpty()) {
    errores << "La base de datos es requerida";
  } else if (baseDatos.contains(" ")) {
    errores << "Nombre de base de datos no puede contener espacios";
  }

  if (!errores.isEmpty()) {
    lblEstado->setText("Errores de validacion");
    lblEstado->setStyleSheet("color: #ef4444; font-size: 11px;");

    QMessageBox::warning(this, "Validacion",
                         "Por favor corrija los siguientes errores:\n\n- " +
                             errores.join("\n- "));
    return false;
  }

  return true;
}

void DialogoConexion::intentarConexion() {
  // Validar formulario primero
  if (!validarFormulario()) {
    return;
  }

  lblEstado->setText("Conectando...");
  lblEstado->setStyleSheet("color: #f59e0b; font-size: 11px;");
  btnConectar->setEnabled(false);
  QCoreApplication::processEvents();

  QString host = inputHost->text().trimmed();
  int puerto = inputPuerto->value();
  QString usuario = inputUsuario->text().trimmed();
  QString password = inputPassword->text();
  QString baseDatos = inputBaseDatos->text().trimmed();
  QString errorMsg;

  if (GestorBaseDatos::instancia().conectar(host, puerto, usuario, password,
                                            baseDatos, errorMsg)) {
    lblEstado->setText("Conectado exitosamente");
    lblEstado->setStyleSheet(
        "color: #10b981; font-size: 11px; font-weight: 600;");

    if (!m_archivoSemilla.isEmpty()) {
      lblEstado->setText("Ejecutando semilla SQL...");
      ejecutarArchivoSQL(m_archivoSemilla);
      lblEstado->setText("Conectado y semilla ejecutada");
    }

    QTimer::singleShot(500, this, [this]() {
      emit conexionExitosa();
      accept();
    });
  } else {
    lblEstado->setText("Error de conexion");
    lblEstado->setStyleSheet("color: #ef4444; font-size: 11px;");

    QMessageBox::critical(
        this, "Error de Conexion",
        "No se pudo conectar a la base de datos:\n\n" + errorMsg +
            "\n\nVerifique:\n- Que PostgreSQL este ejecutandose\n- Los datos "
            "de conexion sean correctos\n- El firewall permita la conexion");
    btnConectar->setEnabled(true);
  }
}

void DialogoConexion::seleccionarArchivoSQL() {
  QString archivo = QFileDialog::getOpenFileName(
      this, "Seleccionar SQL", "",
      "Archivos SQL (*.sql);;Todos los archivos (*)");

  if (!archivo.isEmpty()) {
    m_archivoSemilla = archivo;
    QFileInfo info(archivo);
    lblDropZone->setText(info.fileName());
    lblDropZone->setStyleSheet(
        "color: #10b981; font-size: 11px; font-weight: 500;");
  }
}

void DialogoConexion::ejecutarArchivoSQL(const QString &rutaArchivo) {
  QFile archivo(rutaArchivo);
  if (!archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::warning(this, "Error", "No se pudo abrir el archivo SQL.");
    return;
  }

  QString contenidoSQL = QString::fromUtf8(archivo.readAll());
  archivo.close();

  QSqlQuery query(GestorBaseDatos::instancia().baseDatos());
  QStringList sentencias = contenidoSQL.split(";", Qt::SkipEmptyParts);

  int ejecutadas = 0;
  int errores = 0;

  for (const QString &sentencia : sentencias) {
    QString sql = sentencia.trimmed();
    if (!sql.isEmpty() && !sql.startsWith("--")) {
      if (query.exec(sql)) {
        ejecutadas++;
      } else {
        errores++;
      }
    }
  }

  if (errores > 0) {
    QMessageBox::warning(
        this, "Semilla SQL",
        QString(
            "Semilla ejecutada con advertencias:\n%1 sentencias OK, %2 errores")
            .arg(ejecutadas)
            .arg(errores));
  }
}

void DialogoConexion::dragEnterEvent(QDragEnterEvent *event) {
  if (event->mimeData()->hasUrls()) {
    const QList<QUrl> urls = event->mimeData()->urls();
    if (!urls.isEmpty() && urls.first().toLocalFile().endsWith(".sql")) {
      event->acceptProposedAction();
      dropZone->setStyleSheet(R"(
        QFrame {
          background: #eff6ff;
          border: 2px dashed #2563eb;
          border-radius: 8px;
        }
      )");
    }
  }
}

void DialogoConexion::dragLeaveEvent(QDragLeaveEvent *event) {
  Q_UNUSED(event);
  dropZone->setStyleSheet(R"(
    QFrame {
      background: #f8fafc;
      border: 2px dashed #cbd5e1;
      border-radius: 8px;
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
      background: #f8fafc;
      border: 2px dashed #cbd5e1;
      border-radius: 8px;
    }
  )");
}

void DialogoConexion::crearAnimacionEntrada() {}
void DialogoConexion::crearEfectoParticulas() {}
