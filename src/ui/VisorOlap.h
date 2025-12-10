#ifndef VISOROLAP_H
#define VISOROLAP_H

#include <QMouseEvent>
#include <QPainter>
#include <QPoint>
#include <QTimer>
#include <QToolTip>
#include <QVector>
#include <QWheelEvent>
#include <QWidget>

/**
 * @brief Estructura que representa una celda del cubo OLAP
 */
struct CeldaCubo {
  int dimX = 0;                 // Indice dimension X
  int dimY = 0;                 // Indice dimension Y
  int dimZ = 0;                 // Indice dimension Z
  QString nombreDimX;           // Nombre de la dimension X
  QString nombreDimY;           // Nombre de la dimension Y
  QString nombreDimZ;           // Nombre de la dimension Z
  double valor = 0.0;           // Valor principal (altura del prisma)
  double valorSecundario = 0.0; // Valor secundario (color)
  QString etiqueta;             // Etiqueta para mostrar
  QRectF areaPantalla;          // Area en pantalla para deteccion de clicks
  bool seleccionada = false;
  bool resaltada = false;
};

/**
 * @brief Visor 2.5D interactivo para cubos OLAP
 *
 * Renderiza datos multidimensionales como prismas isometricos con:
 * - Altura proporcional al valor de la medida
 * - Color segun gradiente termico
 * - Seleccion con click
 * - Hover con tooltip
 * - Zoom y pan con mouse
 * - Drill-down con doble click
 */
class VisorOlap : public QWidget {
  Q_OBJECT

public:
  explicit VisorOlap(QWidget *parent = nullptr);
  ~VisorOlap();

  /// Carga datos de ejemplo para demostrar el visor
  void cargarCuboDummy();

  /// Carga datos reales desde una consulta
  void cargarDatos(const QVector<CeldaCubo> &celdas);

  /// Limpia la seleccion actual
  void limpiarSeleccion();

  /// Obtiene las celdas seleccionadas
  QVector<CeldaCubo> obtenerSeleccion() const;

protected:
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;
  void leaveEvent(QEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;

signals:
  /// Emitida cuando se selecciona una celda (click)
  void celdaSeleccionada(const QString &info);

  /// Emitida para drill-down (doble click)
  void drillDown(int dimX, int dimY, int dimZ);

  /// Emitida cuando cambia la seleccion multiple
  void seleccionCambiada(int cantidadSeleccionadas);

private:
  // Geometria y transformacion
  float m_zoom;
  QPointF m_panOffset;
  QPoint m_lastMousePos;
  bool m_arrastrando;

  // Datos del cubo
  QVector<CeldaCubo> m_celdas;
  int m_celdaHover; // -1 si ninguna
  double m_valorMax;
  double m_valorMin;

  // Configuracion visual
  float m_tamanoBase;
  float m_escalaAltura;
  float m_espaciado;

  // Metodos de renderizado
  void dibujarFondo(QPainter &painter);
  void dibujarEjes(QPainter &painter);
  void dibujarPrisma(QPainter &painter, const CeldaCubo &celda, bool hover,
                     bool seleccionada);
  void dibujarTooltip(QPainter &painter, const CeldaCubo &celda,
                      const QPoint &pos);
  void dibujarLeyenda(QPainter &painter);
  void dibujarInfoPanel(QPainter &painter);

  // Utilidades
  QPointF proyectarIso(float x, float y, float z);
  QColor obtenerColorGradiente(double valor, double min, double max);
  int detectarCeldaEnPunto(const QPoint &punto);
  void recalcularAreasPantalla();
};

#endif // VISOROLAP_H
