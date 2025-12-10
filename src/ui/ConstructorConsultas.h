#ifndef CONSTRUCTORCONSULTAS_H
#define CONSTRUCTORCONSULTAS_H

#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>


class ConstructorConsultas : public QWidget {
  Q_OBJECT

public:
  explicit ConstructorConsultas(QWidget *parent = nullptr);

signals:
  void ejecutarConsulta();
  void generarReporte();

private:
  void configurarUi();

  QListWidget *listaDimensiones;
  QListWidget *listaMedidas;
  QListWidget *areaFilas;
  QListWidget *areaColumnas;
  QPushButton *btnEjecutar;
  QPushButton *btnReporte;
};

#endif // CONSTRUCTORCONSULTAS_H
