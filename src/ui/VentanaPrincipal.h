#ifndef VENTANAPRINCIPAL_H
#define VENTANAPRINCIPAL_H

#include <QMainWindow>
#include <QStackedWidget>

// Forward declarations
// Forward declarations
class DialogoConexion;
class DashboardReconocimiento;
class AnalizadorEsquema;

class VentanaPrincipal : public QMainWindow {
  Q_OBJECT

public:
  explicit VentanaPrincipal(QWidget *parent = nullptr);
  ~VentanaPrincipal();

private slots:
  void mostrarDialogoConexion();
  void alConectarExitosa();
  void alAnalisisCompletado();
  void alConfirmarReconocimiento();
  void alModeloConfirmado();
  void alCargaFinalizada();
  void alIrAReportes();

private:
  void configurarInterfaz();
  void mostrarIntroduccion();
  void aplicarTema();

  QStackedWidget *contenedorCentral;
  DashboardReconocimiento *m_dashboard;
  AnalizadorEsquema *m_analizador;
  class IndicadorFase *m_indicadorFase;
};

#endif // VENTANAPRINCIPAL_H
