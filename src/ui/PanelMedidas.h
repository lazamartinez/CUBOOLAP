#ifndef PANELMEDIDAS_H
#define PANELMEDIDAS_H

#include <QLabel>
#include <QListWidget>
#include <QVBoxLayout>
#include <QWidget>


class PanelMedidas : public QWidget {
  Q_OBJECT

public:
  explicit PanelMedidas(QWidget *parent = nullptr);

private:
  void configurarUi();
  QListWidget *listaMedidas;
};

#endif // PANELMEDIDAS_H
