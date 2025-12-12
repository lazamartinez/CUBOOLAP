#ifndef DASHBOARDRECONOCIMIENTO_H
#define DASHBOARDRECONOCIMIENTO_H

#include "widgets/FlutterWidgets.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QWidget>
#include <QtCharts/QBarSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>


class AnalizadorEsquema;

/**
 * @brief Dashboard visual de diagnóstico del esquema de base de datos
 *
 * Muestra estadísticas, gráficos y sugerencias automáticas para
 * identificar tablas de hechos y dimensiones OLAP.
 */
class DashboardReconocimiento : public QWidget {
  Q_OBJECT

public:
  explicit DashboardReconocimiento(QWidget *parent = nullptr);

  /// Carga y muestra los datos del analizador
  void cargarDatos(AnalizadorEsquema *analizador);

signals:
  /// Emitida cuando el usuario confirma el diagnóstico
  void confirmarReconocimiento();

private:
  void configurarUi();

  /// Crea una card visual para mostrar una estadistica
  QWidget *crearCardEstadistica(const QString &etiqueta, const QString &valor,
                                const QString &color);

  /// Actualiza el valor de una card de estadística
  void actualizarCardValor(QWidget *card, const QString &valor);

  // Componentes de gráficos
  QChartView *graficoPastelView;
  QChartView *graficoBarrasView;

  // Cards de estadísticas
  QWidget *lblTotalTablas;
  QWidget *lblTotalFilas;
  QWidget *lblTotalFK;
  QWidget *lblTamanoDB;

  // Lista y sugerencias
  QListWidget *listaTablas;
  QLabel *lblHechosSugeridos;
  QLabel *lblDimensionesSugeridas;
};

#endif // DASHBOARDRECONOCIMIENTO_H
