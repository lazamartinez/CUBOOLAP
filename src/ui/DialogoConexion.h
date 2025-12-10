#ifndef DIALOGOCONEXION_H
#define DIALOGOCONEXION_H

#include <QComboBox>
#include <QDialog>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>


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

private:
  void configurarUi();
  void intentarConexion();
  void seleccionarArchivoSQL();
  void crearEfectoParticulas();
  void crearAnimacionEntrada();
  void ejecutarArchivoSQL(const QString &rutaArchivo);
  bool validarFormulario();
  void actualizarBasesDatos();

  // Campos de conexion
  QLineEdit *inputHost;
  QSpinBox *inputPuerto;
  QLineEdit *inputUsuario;
  QLineEdit *inputPassword;
  QComboBox *comboBaseDatos;
  QPushButton *btnActualizarBD;

  QPushButton *btnConectar;
  QLabel *lblEstado;

  // Zona Drop
  QFrame *dropZone;
  QLabel *lblDropZone;
  QString m_archivoSemilla;
};

#endif // DIALOGOCONEXION_H
