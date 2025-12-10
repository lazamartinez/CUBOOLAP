#ifndef ESTUDIOMODELADO_H
#define ESTUDIOMODELADO_H

#include <QLabel>
#include <QPushButton>
#include <QSplitter>
#include <QVBoxLayout>
#include <QWidget>


class PanelDimensiones;
class PanelMedidas;

class EstudioModelado : public QWidget {
  Q_OBJECT

public:
  explicit EstudioModelado(QWidget *parent = nullptr);
  ~EstudioModelado();

signals:
  void modeloConfirmado();

private:
  void configurarUi();

  PanelDimensiones *panelDimensiones;
  PanelMedidas *panelMedidas;
  QPushButton *btnConfirmarModelo;
};

#endif // ESTUDIOMODELADO_H
