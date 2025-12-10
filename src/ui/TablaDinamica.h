#ifndef TABLADINAMICA_H
#define TABLADINAMICA_H

#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>


class TablaDinamica : public QWidget {
  Q_OBJECT

public:
  explicit TablaDinamica(QWidget *parent = nullptr);
  void mostrarResultadosSimulados();

private:
  void configurarUi();
  QTableWidget *tabla;
};

#endif // TABLADINAMICA_H
