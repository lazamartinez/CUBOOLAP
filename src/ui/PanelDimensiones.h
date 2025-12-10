#ifndef PANELDIMENSIONES_H
#define PANELDIMENSIONES_H

#include <QLabel>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QWidget>


class PanelDimensiones : public QWidget {
  Q_OBJECT

public:
  explicit PanelDimensiones(QWidget *parent = nullptr);

private:
  void configurarUi();
  QTreeWidget *arbolDimensiones;
};

#endif // PANELDIMENSIONES_H
