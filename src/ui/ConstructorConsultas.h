#ifndef CONSTRUCTORCONSULTAS_H
#define CONSTRUCTORCONSULTAS_H

#include "HistorialConsultas.h"
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>


/**
 * @brief Constructor visual de consultas OLAP (Fase 5) - Avanzado
 *
 * Soporta Drag & Drop avanzado, Filtros, Agregaciones y Historial.
 */
class ConstructorConsultas : public QWidget {
  Q_OBJECT

public:
  explicit ConstructorConsultas(QWidget *parent = nullptr);

signals:
  void volverAlInicio();

private slots:
  void ejecutarConsulta();
  void generarReporte();
  void limpiarConsulta();
  void exportarCSV();
  void restaurarConsulta(const QString &consultaStr);

private:
  void configurarUi();
  void cargarDatosEjemplo();
  bool validarConsulta();
  QString serializarConsulta();

  // Listas de recursos
  QListWidget *listaDimensiones;
  QListWidget *listaMedidas;

  // Areas de construccion
  QListWidget *areaFilas;
  QListWidget *areaColumnas;
  QListWidget *areaMedidas;
  QListWidget *areaFiltros; // Nuevo: Area de filtros

  // Controles de agregacion
  QComboBox *comboAgregacion; // SUM, AVG, MAX, MIN

  // Panel Historial
  HistorialConsultas *panelHistorial;

  // Botones
  QPushButton *btnEjecutar;
  QPushButton *btnLimpiar;
  QPushButton *btnReporte;
  QPushButton *btnExportCSV;
  QPushButton *btnVolverInicio;
  QPushButton *btnAgregarFiltro; // Nuevo boton

  // Resultados
  QTableWidget *tablaResultados;
  QLabel *lblInfoResultados;
};

#endif // CONSTRUCTORCONSULTAS_H
