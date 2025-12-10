#ifndef MOTORCARGA_H
#define MOTORCARGA_H

#include <QObject>
#include <QTimer>

class ConsolaProgreso;

class MotorCarga : public QObject {
  Q_OBJECT

public:
  explicit MotorCarga(ConsolaProgreso *consola, QObject *parent = nullptr);
  void iniciarCarga();

signals:
  void cargaFinalizada();

private slots:
  void ejecutarPaso();

private:
  ConsolaProgreso *m_consola;
  QTimer *m_timer;
  int m_pasoActual;
  int m_progreso;
};

#endif // MOTORCARGA_H
