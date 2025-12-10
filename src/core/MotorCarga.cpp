#include "MotorCarga.h"
#include "../ui/ConsolaProgreso.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>


MotorCarga::MotorCarga(ConsolaProgreso *consola, QObject *parent)
    : QObject(parent), m_consola(consola), m_pasoActual(0), m_progreso(0) {
  m_timer = new QTimer(this);
  connect(m_timer, &QTimer::timeout, this, &MotorCarga::ejecutarPaso);
}

void MotorCarga::iniciarCarga() {
  m_consola->agregarLog("Iniciando Verificacion y Carga de Datos...", "INFO");
  m_timer->start(100);
}

void MotorCarga::ejecutarPaso() {
  m_pasoActual++;
  QSqlDatabase db = QSqlDatabase::database("CuboVisionConnection");

  if (!db.isOpen()) {
    m_consola->agregarLog("Error: No hay conexion a base de datos.", "ERROR");
    m_timer->stop();
    return;
  }

  QSqlQuery query(db);

  switch (m_pasoActual) {
  case 1:
    m_consola->agregarLog("Conectado a: " + db.databaseName(), "INFO");
    m_progreso = 5;
    break;
  case 2:
    if (query.exec("SELECT COUNT(*) FROM dim_tiempo")) {
      query.next();
      m_consola->agregarLog(
          QString("Dimensión Tiempo: %1 registros").arg(query.value(0).toInt()),
          "INFO");
    }
    m_progreso = 20;
    break;
  case 3:
    if (query.exec("SELECT COUNT(*) FROM dim_geografia")) {
      query.next();
      m_consola->agregarLog(QString("Dimensión Geografía: %1 registros")
                                .arg(query.value(0).toInt()),
                            "INFO");
    }
    m_progreso = 35;
    break;
  case 4:
    if (query.exec("SELECT COUNT(*) FROM dim_cliente")) {
      query.next();
      m_consola->agregarLog(QString("Dimensión Cliente: %1 registros")
                                .arg(query.value(0).toInt()),
                            "INFO");
    }
    m_progreso = 50;
    break;
  case 5:
    if (query.exec("SELECT COUNT(*) FROM dim_producto")) {
      query.next();
      m_consola->agregarLog(QString("Dimensión Producto: %1 registros")
                                .arg(query.value(0).toInt()),
                            "INFO");
    }
    m_progreso = 65;
    break;
  case 6: {
    m_consola->agregarLog("Procesando Tabla de Hechos (Ventas)...", "INFO");
    if (query.exec("SELECT COUNT(*), SUM(total_venta) FROM fact_ventas")) {
      query.next();
      int count = query.value(0).toInt();
      double total = query.value(1).toDouble();
      m_consola->agregarLog(
          QString("Hechos Ventas: %1 registros procesados.").arg(count),
          "INFO");
      m_consola->agregarLog(
          QString("Volumen Total de Ventas: $%1").arg(total, 0, 'f', 2),
          "SUCCESS");
    }
  }
    m_progreso = 90;
    break;
  case 7:
    m_consola->agregarLog("Carga de Datos Reales Finalizada.", "INFO");
    m_progreso = 100;
    m_timer->stop();
    m_consola->mostrarBotonContinuar();
    emit cargaFinalizada();
    break;
  }

  m_consola->actualizarProgresoGeneral(m_progreso);
}
