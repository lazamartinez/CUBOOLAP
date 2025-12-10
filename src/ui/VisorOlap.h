#ifndef VISOROLAP_H
#define VISOROLAP_H

#include <QMouseEvent>
#include <QPoint>
#include <QTimer>
#include <QWheelEvent>
#include <QWidget>


class VisorOlap : public QWidget {
  Q_OBJECT

public:
  explicit VisorOlap(QWidget *parent = nullptr);
  ~VisorOlap();

  void cargarCuboDummy();

protected:
  void paintEvent(QPaintEvent *event) override;

  // Control de interacción
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;

signals:
  void celdaSeleccionada(const QString &info);

private:
  float m_rotacionX;
  float m_rotacionY;
  float m_zoom;
  QPoint m_lastMousePos;

  void dibujarPrisma(QPainter &painter, float x, float y, float z, float w,
                     float h, float d, const QColor &color);
};

#endif // VISOROLAP_H
