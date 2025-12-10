#include "GestorBaseDatos.h"
#include <QDebug>
#include <QSqlQuery>

GestorBaseDatos &GestorBaseDatos::instancia() {
  static GestorBaseDatos instancia;
  return instancia;
}

GestorBaseDatos::GestorBaseDatos(QObject *parent) : QObject(parent) {}

GestorBaseDatos::~GestorBaseDatos() {
  if (m_db.isOpen()) {
    m_db.close();
  }
}

bool GestorBaseDatos::conectar(const QString &host, int puerto,
                               const QString &usuario, const QString &password,
                               const QString &baseDatos, QString &errorMsg) {
  if (QSqlDatabase::contains("CuboVisionConnection")) {
    m_db = QSqlDatabase::database("CuboVisionConnection");
  } else {
    m_db = QSqlDatabase::addDatabase("QPSQL", "CuboVisionConnection");
  }

  m_db.setHostName(host);
  m_db.setPort(puerto);
  m_db.setUserName(usuario);
  m_db.setPassword(password);
  m_db.setDatabaseName(baseDatos);

  if (m_db.open()) {
    qDebug() << "Conexión exitosa a la base de datos:" << baseDatos;
    return true;
  } else {
    errorMsg = m_db.lastError().text();
    qCritical() << "Error de conexión:" << errorMsg;
    return false;
  }
}

QStringList GestorBaseDatos::listarBasesDatos(const QString &host, int puerto,
                                              const QString &usuario,
                                              const QString &password,
                                              QString &errorMsg) {
  QStringList basesDeDatos;

  // Conectar a "postgres" para listar otras bases
  QString connectionName = "TempListConnection";
  {
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL", connectionName);
    db.setHostName(host);
    db.setPort(puerto);
    db.setUserName(usuario);
    db.setPassword(password);
    db.setDatabaseName("postgres");

    if (db.open()) {
      QSqlQuery query(db);
      if (query.exec("SELECT datname FROM pg_database WHERE datistemplate = "
                     "false AND datname != 'postgres';")) {
        while (query.next()) {
          basesDeDatos << query.value(0).toString();
        }
      } else {
        errorMsg = query.lastError().text();
      }
      db.close();
    } else {
      errorMsg = db.lastError().text();
    }
  }
  QSqlDatabase::removeDatabase(connectionName);

  return basesDeDatos;
}

bool GestorBaseDatos::estaConectado() const { return m_db.isOpen(); }

QSqlDatabase GestorBaseDatos::baseDatos() const { return m_db; }
