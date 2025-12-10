#ifndef TOASTNOTIFIER_H
#define TOASTNOTIFIER_H

#include <QHBoxLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include <QString>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>


/**
 * @brief Notificaciones Toast flotantes
 *
 * Muestra mensajes temporales elegantes en la esquina de la pantalla.
 */
class ToastNotifier : public QWidget {
  Q_OBJECT
  Q_PROPERTY(qreal opacity READ windowOpacity WRITE setWindowOpacity)

public:
  enum TipoToast { Info, Exito, Advertencia, Error };

  static void mostrar(QWidget *parent, const QString &mensaje,
                      TipoToast tipo = Info, int duracion = 3000);

private:
  explicit ToastNotifier(QWidget *parent, const QString &mensaje,
                         TipoToast tipo, int duracion);

  void configurarUi(const QString &mensaje, TipoToast tipo);
  void iniciarAnimacion(int duracion);

  QString obtenerIcono(TipoToast tipo);
  QString obtenerColor(TipoToast tipo);
};

#endif // TOASTNOTIFIER_H
