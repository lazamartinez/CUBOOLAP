#ifndef CONSOLAPROGRESO_H
#define CONSOLAPROGRESO_H

#include "widgets/FlutterWidgets.h"
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>


/**
 * @brief Consola de progreso para Fase 3 (Carga de Datos)
 *
 * Muestra estadisticas en tiempo real, logs de procesamiento,
 * monitor de calidad de datos, y alertas.
 */
class ConsolaProgreso : public QWidget {
  Q_OBJECT

public:
  explicit ConsolaProgreso(QWidget *parent = nullptr);

  /// Actualiza la barra de progreso general (0-100)
  void actualizarProgresoGeneral(int valor);

  /// Actualiza las cards de estadisticas
  void actualizarEstadisticas(int registros, double velocidad,
                              const QString &tiempoRestante, int calidad);

  /// Agrega un mensaje al log de procesamiento
  void agregarLog(const QString &mensaje, const QString &tipo = "INFO");

  /// Agrega una alerta al monitor de calidad
  void agregarAlerta(const QString &mensaje, const QString &severidad = "INFO");

  /// Muestra el boton para continuar a la siguiente fase
  void mostrarBotonContinuar();

signals:
  void cargaCompletada();

private:
  void configurarUi();
  QWidget *crearCardEstadistica(const QString &etiqueta, const QString &valor,
                                const QString &color);

  // Componentes UI
  QProgressBar *barProgreso;
  QLabel *lblEstado;
  QTextEdit *txtLog;
  QTextEdit *txtAlertas;
  QPushButton *btnContinuar;

  // Labels de estadisticas
  QLabel *lblRegistros;
  QLabel *lblVelocidad;
  QLabel *lblTiempo;
  QLabel *lblCalidad;
};

#endif // CONSOLAPROGRESO_H
