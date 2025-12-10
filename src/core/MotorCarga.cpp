#include "MotorCarga.h"
#include "../ui/ConsolaProgreso.h"

MotorCarga::MotorCarga(ConsolaProgreso *consola, QObject *parent)
    : QObject(parent), m_consola(consola), m_pasoActual(0), m_progreso(0) {
  m_timer = new QTimer(this);
  connect(m_timer, &QTimer::timeout, this, &MotorCarga::ejecutarPaso);
}

void MotorCarga::iniciarCarga() {
  m_consola->agregarLog("Iniciando Motor de Carga Masiva...", "INFO");
  m_timer->start(800); // Simulación de velocidad
}

void MotorCarga::ejecutarPaso() {
  m_pasoActual++;

  switch (m_pasoActual) {
  case 1:
    m_consola->agregarLog("Conectando Extractor Paralelo a PostgreSQL...",
                          "INFO");
    m_progreso = 10;
    break;
  case 2:
    m_consola->agregarLog("Extrayendo dimensiones (Tiempo, Geografía)...",
                          "INFO");
    m_progreso = 30;
    break;
  case 3:
    m_consola->agregarLog(
        "Validando calidad de datos: Integridad Referencial OK.", "INFO");
    m_progreso = 45;
    break;
  case 4:
    m_consola->agregarLog(
        "Detectado valor nulo en 'Region' (ID: 4502). Asignando 'Desconocido'.",
        "WARN");
    m_progreso = 50;
    break;
  case 5:
    m_consola->agregarLog(
        "Procesando Tabla de Hechos (Agregación Hash-Join)...", "INFO");
    m_progreso = 70;
    break;
  case 6:
    m_consola->agregarLog("Generando Caché Estratificado Nivel 1 (RAM LRU)...",
                          "INFO");
    m_progreso = 90;
    break;
  case 7:
    m_consola->agregarLog("Carga y Procesamiento Finalizados con Éxito.",
                          "INFO");
    m_progreso = 100;
    m_timer->stop();
    m_consola->mostrarBotonContinuar();
    emit cargaFinalizada();
    break;
  }

  m_consola->actualizarProgresoGeneral(m_progreso);
}
