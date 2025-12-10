#ifndef DIALOGOFILTROS_H
#define DIALOGOFILTROS_H

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMap>
#include <QPushButton>
#include <QVBoxLayout>


// Dialogo para configurar filtros Slice (una dimension) y Dice (multiples)
class DialogoFiltros : public QDialog {
  Q_OBJECT
public:
  enum Modo { Slice, Dice };

  explicit DialogoFiltros(Modo modo, QWidget *parent = nullptr);

  void setDimensionesDisponibles(const QStringList &dimensiones);
  void setValoresDimension(const QString &dimension,
                           const QStringList &valores);

  QString getDimensionSeleccionada() const;
  QStringList getValoresSeleccionados() const;
  QMap<QString, QStringList> getTodosLosFiltros() const;

private slots:
  void onDimensionCambiada(const QString &dimension);
  void onAgregarFiltro();
  void onQuitarFiltro();

private:
  void configurarUi();
  void aplicarEstilos();

  Modo m_modo;
  QComboBox *cmbDimension;
  QListWidget *lstValoresDisponibles;
  QListWidget *lstValoresSeleccionados;
  QListWidget *lstFiltrosActivos; // Solo para Dice
  QPushButton *btnAgregar;
  QPushButton *btnQuitar;
  QPushButton *btnAceptar;
  QPushButton *btnCancelar;

  QMap<QString, QStringList> m_valoresPorDimension;
  QMap<QString, QStringList> m_filtrosActivos;

signals:
  void dimensionCambiada(const QString &dimension);
};

#endif // DIALOGOFILTROS_H
