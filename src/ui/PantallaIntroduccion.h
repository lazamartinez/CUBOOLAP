#ifndef PANTALLAINTRODUCCION_H
#define PANTALLAINTRODUCCION_H

#include <QLabel>
#include <QPushButton>
#include <QWidget>

/**
 * @brief Pantalla de introduccion/bienvenida al sistema
 *
 * Muestra una presentacion visual del flujo del sistema OLAP
 * con iconos y descripciones de cada fase.
 */
class PantallaIntroduccion : public QWidget {
  Q_OBJECT

public:
  explicit PantallaIntroduccion(QWidget *parent = nullptr);

signals:
  void iniciarSistema();

protected:
  void paintEvent(QPaintEvent *event) override;

private:
  void configurarUi();
  QWidget *crearTarjetaFase(int numero, const QString &titulo,
                            const QString &descripcion, const QString &icono,
                            const QString &color);
};

#endif // PANTALLAINTRODUCCION_H
