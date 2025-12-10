#ifndef PANELMEDIDAS_H
#define PANELMEDIDAS_H

#include <QLabel>
#include <QListWidget>
#include <QMetaType>
#include <QSqlDatabase>
#include <QSqlField>
#include <QSqlRecord>
#include <QVBoxLayout>
#include <QVariant>
#include <QWidget>


class PanelMedidas : public QWidget {
  Q_OBJECT
public:
  explicit PanelMedidas(QWidget *parent = nullptr);
  void cargarMedidasReales();

private:
  void configurarUi();
  QListWidget *listaMedidas;
};

#endif // PANELMEDIDAS_H
