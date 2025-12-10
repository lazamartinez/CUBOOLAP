#ifndef CONSTRUCTORCONSULTAS_H
#define CONSTRUCTORCONSULTAS_H

#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>

/**
 * @brief Constructor visual de consultas OLAP (Fase 5)
 *
 * Permite crear consultas arrastrando dimensiones y medidas,
 * ejecutarlas, y exportar los resultados.
 */
class ConstructorConsultas : public QWidget {
  Q_OBJECT

public:
  explicit ConstructorConsultas(QWidget *parent = nullptr);

private slots:
  void ejecutarConsulta();
  void generarReporte();

private:
  void configurarUi();
  void cargarDatosEjemplo();

  // Listas de recursos
  QListWidget *listaDimensiones;
  QListWidget *listaMedidas;

  // Areas de construccion
  QListWidget *areaFilas;
  QListWidget *areaColumnas;
  QListWidget *areaMedidas;

  // Botones
  QPushButton *btnEjecutar;
  QPushButton *btnLimpiar;
  QPushButton *btnReporte;

  // Resultados
  QTableWidget *tablaResultados;
  QLabel *lblInfoResultados;
};

#endif // CONSTRUCTORCONSULTAS_H
