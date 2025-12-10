#ifndef DIALOGOCONEXION_H
#define DIALOGOCONEXION_H

#include <QDialog>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileInfo>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

/**
 * @brief Dialogo de conexion a base de datos PostgreSQL
 *
 * Permite configurar la conexion y opcionalmente importar
 * archivos SQL de semilla mediante drag-drop o seleccion.
 */
class DialogoConexion : public QDialog {
  Q_OBJECT

public:
  explicit DialogoConexion(QWidget *parent = nullptr);
  ~DialogoConexion();

signals:
  void conexionExitosa();

protected:
  void dragEnterEvent(QDragEnterEvent *event) override;
  void dragLeaveEvent(QDragLeaveEvent *event) override;
  void dropEvent(QDropEvent *event) override;

private slots:
  void intentarConexion();
  void seleccionarArchivoSQL();

private:
  void configurarUi();
  void crearEfectoParticulas();
  void crearAnimacionEntrada();
  void ejecutarArchivoSQL(const QString &rutaArchivo);

  // Campos de conexion
  QLineEdit *inputHost;
  QSpinBox *inputPuerto;
  QLineEdit *inputUsuario;
  QLineEdit *inputPassword;
  QLineEdit *inputBaseDatos;
  QPushButton *btnConectar;
  QLabel *lblEstado;

  // Zona de drag-drop
  QFrame *dropZone;
  QLabel *lblDropZone;
  QString m_archivoSemilla;
};

#endif // DIALOGOCONEXION_H
