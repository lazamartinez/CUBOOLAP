#ifndef CONSOLAPROGRESO_H
#define CONSOLAPROGRESO_H

#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>


class ConsolaProgreso : public QWidget {
  Q_OBJECT

public:
  explicit ConsolaProgreso(QWidget *parent = nullptr);

  void actualizarProgresoGeneral(int valor);
  void agregarLog(const QString &mensaje, const QString &tipo = "INFO");
  void mostrarBotonContinuar();

signals:
  void cargaCompletada();

private:
  void configurarUi();

  QProgressBar *barProgreso;
  QTextEdit *txtLog;
  QLabel *lblEstado;
  QPushButton *btnContinuar;
};

#endif // CONSOLAPROGRESO_H
