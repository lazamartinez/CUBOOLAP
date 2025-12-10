#include "GestorBaseDatos.h"
#include <QDebug>

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

bool GestorBaseDatos::estaConectado() const { return m_db.isOpen(); }

QSqlDatabase GestorBaseDatos::baseDatos() const { return m_db; }
