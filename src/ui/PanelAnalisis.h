#ifndef PANELANALISIS_H
#define PANELANALISIS_H

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

class PanelAnalisis : public QWidget {
  Q_OBJECT

public:
  explicit PanelAnalisis(QWidget *parent = nullptr);
  void mostrarInfo(const QString &info);

signals:
  void irAReportes();

private:
  void configurarUi();
  QLabel *lblDetalle;
  QPushButton *btnReportes;
};

#endif // PANELANALISIS_H
