#ifndef HISTORIALCONSULTAS_H
#define HISTORIALCONSULTAS_H

#include <QDateTime>
#include <QListWidget>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <QVector>
#include <QWidget>


/**
 * @brief Panel de historial de consultas ejecutadas
 *
 * Permite ver y re-ejecutar consultas anteriores.
 */
class HistorialConsultas : public QWidget {
  Q_OBJECT

public:
  explicit HistorialConsultas(QWidget *parent = nullptr);

  struct EntradaHistorial {
    QString consulta;
    QDateTime fecha;
    int registros;
    int tiempoMs;
  };

  void agregarConsulta(const QString &consulta, int registros, int tiempoMs);
  void limpiarHistorial();

signals:
  void consultaSeleccionada(const QString &consulta);

private:
  void configurarUi();

  QListWidget *listaHistorial;
  QPushButton *btnLimpiar;
  QVector<EntradaHistorial> m_historial;
};

#endif // HISTORIALCONSULTAS_H
