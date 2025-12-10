#ifndef PANELDIMENSIONES_H
#define PANELDIMENSIONES_H

#include <QLabel>
#include <QSqlDatabase>
#include <QSqlField>
#include <QSqlRecord>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QWidget>


class PanelDimensiones : public QWidget {
  Q_OBJECT
public:
  explicit PanelDimensiones(QWidget *parent = nullptr);
  void cargarDimensionesReales();

private:
  void configurarUi();
  QTreeWidget *arbolDimensiones;
};

#endif // PANELDIMENSIONES_H
