#ifndef VISOROLAP_H
#define VISOROLAP_H

#include <QPainterPath>
#include <QVector>
#include <QWidget>

struct CeldaCubo {
  int dimX;
  int dimY;
  int dimZ;
  QString nombreDimX;
  QString nombreDimY;
  QString nombreDimZ;
  double valor;
  double valorSecundario;
  QString etiqueta;
  bool seleccionada = false;

  // Cache para interaccion
  QRectF areaPantalla;
};

class VisorOlap : public QWidget {
  Q_OBJECT

public:
  explicit VisorOlap(QWidget *parent = nullptr);
  ~VisorOlap();

  void cargarCuboDummy();
  void cargarDatos(const QVector<CeldaCubo> &celdas);
  void limpiarSeleccion();
  QVector<CeldaCubo> obtenerSeleccion() const;

signals:
  void celdaSeleccionada(const QString &info);
  void seleccionCambiada(int cantidad);
  void drillDown(int x, int y, int z);

protected:
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;
  void leaveEvent(QEvent *event) override;

private:
  void dibujarFondo(QPainter &painter);
  void dibujarEjes(QPainter &painter);
  void dibujarPrisma(QPainter &painter, const CeldaCubo &celda, bool hover,
                     bool seleccionada);
  void dibujarTooltip(QPainter &painter, const CeldaCubo &celda,
                      const QPoint &pos);
  void dibujarLeyenda(QPainter &painter);
  void dibujarInfoPanel(QPainter &painter);

  QPointF proyectarIso(float x, float y, float z);
  QColor obtenerColorGradiente(double valor, double min, double max);
  int detectarCeldaEnPunto(const QPoint &punto);
  void recalcularAreasPantalla();
  void cargarCuboReal();

  // Estado
  QVector<CeldaCubo> m_celdas;
  float m_zoom;
  QPointF m_panOffset;
  QPoint m_lastMousePos;
  bool m_arrastrando;
  int m_celdaHover;

  // Configuracion Visual
  double m_valorMax;
  double m_valorMin;
  float m_tamanoBase;
  float m_escalaAltura;
  float m_espaciado;
};

#endif // VISOROLAP_H
