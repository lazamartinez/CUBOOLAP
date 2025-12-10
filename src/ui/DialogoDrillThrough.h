#ifndef DIALOGODRILLTHROUGH_H
#define DIALOGODRILLTHROUGH_H

#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QVBoxLayout>


// Dialogo que muestra los registros transaccionales (OLTP) de una celda del
// cubo
class DialogoDrillThrough : public QDialog {
  Q_OBJECT
public:
  explicit DialogoDrillThrough(QWidget *parent = nullptr);

  void setTitulo(const QString &dimX, const QString &dimZ, double valor);
  void cargarDatos(const QStringList &columnas,
                   const QVector<QStringList> &filas);
  void setLimit(int limit);

signals:
  void limitCambiado(int nuevoLimit);
  void exportarCSV();

private:
  void configurarUi();
  void aplicarEstilos();

  QTableWidget *tabla;
  QLabel *lblTitulo;
  QLabel *lblRegistros;
  QSpinBox *spinLimit;
  QPushButton *btnExportar;
  QPushButton *btnCerrar;
};

#endif // DIALOGODRILLTHROUGH_H
