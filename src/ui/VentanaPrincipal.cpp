#include "VentanaPrincipal.h"
#include "DialogoConexion.h"
#include "Estilos.h"
#include "PantallaIntroduccion.h"
#include <QApplication>
#include <QMenuBar>
#include <QScreen>
#include <QStatusBar>
#include <QTimer>

VentanaPrincipal::VentanaPrincipal(QWidget *parent)
    : QMainWindow(parent), contenedorCentral(new QStackedWidget(this)) {
  configurarInterfaz();
  mostrarIntroduccion();
}

VentanaPrincipal::~VentanaPrincipal() {}

void VentanaPrincipal::configurarInterfaz() {
  setCentralWidget(contenedorCentral);
  setWindowTitle("Cubo Vision - Sistema OLAP");

  // Ventana compacta
  resize(1100, 700);
  setMinimumSize(900, 600);

  // Centrar en pantalla
  if (QScreen *screen = QApplication::primaryScreen()) {
    QRect geo = screen->availableGeometry();
    move((geo.width() - 1100) / 2, (geo.height() - 700) / 2);
  }

  // Aplicar estilos globales limpios
  setStyleSheet(Estilos::obtenerEstiloGlobal());

  // Menu
  QMenuBar *menuBar = this->menuBar();
  QMenu *menuArchivo = menuBar->addMenu("Archivo");
  menuArchivo->addAction("Nueva Conexion", this,
                         &VentanaPrincipal::mostrarDialogoConexion);
  menuArchivo->addSeparator();
  menuArchivo->addAction("Salir", this, &QMainWindow::close);

  QMenu *menuAyuda = menuBar->addMenu("Ayuda");
  menuAyuda->addAction("Acerca de", this, [this]() {
    statusBar()->showMessage("Cubo Vision v1.0", 3000);
  });

  statusBar()->showMessage("Bienvenido a Cubo Vision");
}

void VentanaPrincipal::mostrarIntroduccion() {
  PantallaIntroduccion *intro = new PantallaIntroduccion(this);
  connect(intro, &PantallaIntroduccion::iniciarSistema, this,
          &VentanaPrincipal::mostrarDialogoConexion);

  contenedorCentral->addWidget(intro);
  contenedorCentral->setCurrentWidget(intro);
}

void VentanaPrincipal::mostrarDialogoConexion() {
  DialogoConexion *dialogo = new DialogoConexion(this);
  connect(dialogo, &DialogoConexion::conexionExitosa, this,
          &VentanaPrincipal::alConectarExitosa);
  dialogo->exec();
}

#include "../core/AnalizadorEsquema.h"
#include "DashboardReconocimiento.h"

void VentanaPrincipal::alConectarExitosa() {
  statusBar()->showMessage("Conectado. Analizando...", 0);

  m_analizador = new AnalizadorEsquema(this);
  connect(m_analizador, &AnalizadorEsquema::analisisCompletado, this,
          &VentanaPrincipal::alAnalisisCompletado);
  connect(m_analizador, &AnalizadorEsquema::progreso, this,
          [this](int p, QString m) {
            statusBar()->showMessage(QString("%1 (%2%)").arg(m).arg(p));
          });

  m_analizador->analizar();
}

#include "../core/MotorCarga.h"
#include "ConsolaProgreso.h"
#include "EstudioModelado.h"

void VentanaPrincipal::alAnalisisCompletado() {
  statusBar()->showMessage("Fase 1: Diagnostico", 0);

  m_dashboard = new DashboardReconocimiento(this);
  m_dashboard->cargarDatos(m_analizador);

  connect(m_dashboard, &DashboardReconocimiento::confirmarReconocimiento, this,
          &VentanaPrincipal::alConfirmarReconocimiento);

  contenedorCentral->addWidget(m_dashboard);
  contenedorCentral->setCurrentWidget(m_dashboard);
}

void VentanaPrincipal::alConfirmarReconocimiento() {
  statusBar()->showMessage("Fase 2: Modelado", 0);

  EstudioModelado *estudio = new EstudioModelado(this);
  contenedorCentral->addWidget(estudio);
  contenedorCentral->setCurrentWidget(estudio);

  connect(estudio, &EstudioModelado::modeloConfirmado, this,
          &VentanaPrincipal::alModeloConfirmado);
}

void VentanaPrincipal::alModeloConfirmado() {
  statusBar()->showMessage("Fase 3: Carga", 0);

  ConsolaProgreso *consola = new ConsolaProgreso(this);
  MotorCarga *motor = new MotorCarga(consola, this);

  connect(consola, &ConsolaProgreso::cargaCompletada, this,
          &VentanaPrincipal::alCargaFinalizada);

  contenedorCentral->addWidget(consola);
  contenedorCentral->setCurrentWidget(consola);

  motor->iniciarCarga();
}

#include "PanelAnalisis.h"
#include "VisorOlap.h"
#include <QHBoxLayout>

void VentanaPrincipal::alCargaFinalizada() {
  statusBar()->showMessage("Fase 4: Explorador OLAP", 0);

  QWidget *containerFase4 = new QWidget(this);
  QHBoxLayout *layout = new QHBoxLayout(containerFase4);
  layout->setContentsMargins(12, 12, 12, 12);
  layout->setSpacing(12);

  VisorOlap *visor = new VisorOlap(containerFase4);
  PanelAnalisis *panel = new PanelAnalisis(containerFase4);

  connect(panel, &PanelAnalisis::irAReportes, this,
          &VentanaPrincipal::alIrAReportes);

  layout->addWidget(visor, 3);
  layout->addWidget(panel, 1);

  connect(visor, &VisorOlap::celdaSeleccionada, panel,
          &PanelAnalisis::mostrarInfo);

  contenedorCentral->addWidget(containerFase4);
  contenedorCentral->setCurrentWidget(containerFase4);

  visor->setFocus();
}

#include "ConstructorConsultas.h"

void VentanaPrincipal::alIrAReportes() {
  statusBar()->showMessage("Fase 5: Consultas", 0);

  ConstructorConsultas *constructor = new ConstructorConsultas(this);
  contenedorCentral->addWidget(constructor);
  contenedorCentral->setCurrentWidget(constructor);
}
