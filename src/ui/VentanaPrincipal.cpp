#include "VentanaPrincipal.h"
#include "DialogoConexion.h"
#include "Estilos.h"
#include <QMenuBar>
#include <QStatusBar>
#include <QTimer>

VentanaPrincipal::VentanaPrincipal(QWidget *parent)
    : QMainWindow(parent), contenedorCentral(new QStackedWidget(this)) {
  configurarInterfaz();

  // Programar la aparición del diálogo de conexión una vez que el loop de
  // eventos inicie
  QTimer::singleShot(100, this, &VentanaPrincipal::mostrarDialogoConexion);
}

VentanaPrincipal::~VentanaPrincipal() {}

void VentanaPrincipal::configurarInterfaz() {
  setCentralWidget(contenedorCentral);
  setWindowTitle("Cubo Vision - Sistema OLAP Avanzado");
  resize(1440, 900);
  setMinimumSize(1024, 768);

  // Aplicar estilos glassmorphism globales
  setStyleSheet(Estilos::obtenerEstiloGlobal());

  // Configurar barra de menu
  QMenuBar *menuBar = this->menuBar();
  QMenu *menuArchivo = menuBar->addMenu("&Archivo");
  menuArchivo->addAction("Nueva Conexion", this,
                         &VentanaPrincipal::mostrarDialogoConexion);
  menuArchivo->addSeparator();
  menuArchivo->addAction("Salir", this, &QMainWindow::close);

  QMenu *menuVista = menuBar->addMenu("&Vista");
  menuVista->addAction("Pantalla Completa", this, [this]() {
    if (isFullScreen())
      showNormal();
    else
      showFullScreen();
  });

  QMenu *menuAyuda = menuBar->addMenu("A&yuda");
  menuAyuda->addAction("Acerca de Cubo Vision", this, [this]() {
    statusBar()->showMessage(
        "Cubo Vision v1.0 - Sistema OLAP con Visualizacion 2.5D", 5000);
  });

  // Configurar barra de estado
  statusBar()->showMessage(
      "Sistema listo. Conecte a una base de datos para comenzar.");
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
  statusBar()->showMessage(
      "Conexión establecida. Iniciando diagnóstico del esquema...", 0);

  // Iniciar análisis
  m_analizador = new AnalizadorEsquema(this);
  connect(m_analizador, &AnalizadorEsquema::analisisCompletado, this,
          &VentanaPrincipal::alAnalisisCompletado);
  connect(m_analizador, &AnalizadorEsquema::progreso, this,
          [this](int p, QString m) {
            statusBar()->showMessage(
                QString("Analizando: %1 (%2%)").arg(m).arg(p));
          });

  m_analizador->analizar();
}

#include "../core/MotorCarga.h"
#include "ConsolaProgreso.h"
#include "EstudioModelado.h"

void VentanaPrincipal::alAnalisisCompletado() {
  statusBar()->showMessage("Análisis completado. Generando Dashboard...", 2000);

  m_dashboard = new DashboardReconocimiento(this);
  m_dashboard->cargarDatos(m_analizador);

  connect(m_dashboard, &DashboardReconocimiento::confirmarReconocimiento, this,
          &VentanaPrincipal::alConfirmarReconocimiento);

  contenedorCentral->addWidget(m_dashboard);
  contenedorCentral->setCurrentWidget(m_dashboard);
}

void VentanaPrincipal::alConfirmarReconocimiento() {
  statusBar()->showMessage("Iniciando Sesión de Modelado Multidimensional...",
                           3000);

  EstudioModelado *estudio = new EstudioModelado(this);
  contenedorCentral->addWidget(estudio);
  contenedorCentral->setCurrentWidget(estudio);

  // Opcional: conectar señal de EstudioModelado para ir a Fase 3
  connect(estudio, &EstudioModelado::modeloConfirmado, this,
          &VentanaPrincipal::alModeloConfirmado);
}

void VentanaPrincipal::alModeloConfirmado() {
  statusBar()->showMessage("Iniciando Carga de Datos...", 3000);

  ConsolaProgreso *consola = new ConsolaProgreso(this);
  MotorCarga *motor = new MotorCarga(consola, this);

  // Conectar finalización
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
  statusBar()->showMessage("Entorno de Exploración 2.5D Listo.", 0);

  QWidget *containerFase4 = new QWidget(this);
  QHBoxLayout *layout = new QHBoxLayout(containerFase4);
  layout->setContentsMargins(0, 0, 0, 0);

  VisorOlap *visor = new VisorOlap(containerFase4);
  PanelAnalisis *panel = new PanelAnalisis(containerFase4);

  connect(panel, &PanelAnalisis::irAReportes, this,
          &VentanaPrincipal::alIrAReportes);

  layout->addWidget(visor, 4);
  layout->addWidget(panel, 1);

  connect(visor, &VisorOlap::celdaSeleccionada, panel,
          &PanelAnalisis::mostrarInfo);

  contenedorCentral->addWidget(containerFase4);
  contenedorCentral->setCurrentWidget(containerFase4);

  visor->setFocus();
}

#include "ConstructorConsultas.h"

void VentanaPrincipal::alIrAReportes() {
  statusBar()->showMessage("Iniciando Constructor de Consultas (Fase 5)...",
                           3000);

  ConstructorConsultas *constructor = new ConstructorConsultas(this);
  contenedorCentral->addWidget(constructor);
  contenedorCentral->setCurrentWidget(constructor);
}
