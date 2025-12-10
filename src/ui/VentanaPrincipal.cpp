#include "VentanaPrincipal.h"
#include "DialogoConexion.h"
#include "Estilos.h"
#include <QApplication>
#include <QMenuBar>
#include <QScreen>
#include <QStatusBar>
#include <QTimer>


VentanaPrincipal::VentanaPrincipal(QWidget *parent)
    : QMainWindow(parent), contenedorCentral(new QStackedWidget(this)) {
  configurarInterfaz();

  // Programar la aparicion del dialogo de conexion
  QTimer::singleShot(100, this, &VentanaPrincipal::mostrarDialogoConexion);
}

VentanaPrincipal::~VentanaPrincipal() {}

void VentanaPrincipal::configurarInterfaz() {
  setCentralWidget(contenedorCentral);
  setWindowTitle("Cubo Vision - Sistema OLAP");

  // Ventana compacta (no pantalla completa)
  resize(1100, 700);
  setMinimumSize(900, 600);

  // Centrar en pantalla
  if (QScreen *screen = QApplication::primaryScreen()) {
    QRect screenGeometry = screen->availableGeometry();
    int x = (screenGeometry.width() - 1100) / 2;
    int y = (screenGeometry.height() - 700) / 2;
    move(x, y);
  }

  // Aplicar estilos limpios y modernos
  setStyleSheet(Estilos::obtenerEstiloGlobal());

  // Barra de menu compacta
  QMenuBar *menuBar = this->menuBar();
  menuBar->setStyleSheet(R"(
    QMenuBar {
      background: white;
      border-bottom: 1px solid #e5e7eb;
      padding: 2px 8px;
      font-size: 12px;
    }
    QMenuBar::item {
      padding: 6px 12px;
      border-radius: 4px;
    }
    QMenuBar::item:selected {
      background: #f1f5f9;
    }
  )");

  QMenu *menuArchivo = menuBar->addMenu("Archivo");
  menuArchivo->addAction("Nueva Conexion", this,
                         &VentanaPrincipal::mostrarDialogoConexion);
  menuArchivo->addSeparator();
  menuArchivo->addAction("Salir", this, &QMainWindow::close);

  QMenu *menuAyuda = menuBar->addMenu("Ayuda");
  menuAyuda->addAction("Acerca de", this, [this]() {
    statusBar()->showMessage("Cubo Vision v1.0 - Sistema OLAP", 3000);
  });

  // Barra de estado compacta
  statusBar()->setStyleSheet(R"(
    QStatusBar {
      background: white;
      border-top: 1px solid #e5e7eb;
      color: #6b7280;
      font-size: 11px;
      padding: 4px 12px;
    }
  )");
  statusBar()->showMessage("Listo");
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
  statusBar()->showMessage("Conectado. Analizando esquema...", 0);

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
  statusBar()->showMessage("Analisis completado", 2000);

  m_dashboard = new DashboardReconocimiento(this);
  m_dashboard->cargarDatos(m_analizador);

  connect(m_dashboard, &DashboardReconocimiento::confirmarReconocimiento, this,
          &VentanaPrincipal::alConfirmarReconocimiento);

  contenedorCentral->addWidget(m_dashboard);
  contenedorCentral->setCurrentWidget(m_dashboard);
}

void VentanaPrincipal::alConfirmarReconocimiento() {
  statusBar()->showMessage("Fase 2: Modelado", 2000);

  EstudioModelado *estudio = new EstudioModelado(this);
  contenedorCentral->addWidget(estudio);
  contenedorCentral->setCurrentWidget(estudio);

  connect(estudio, &EstudioModelado::modeloConfirmado, this,
          &VentanaPrincipal::alModeloConfirmado);
}

void VentanaPrincipal::alModeloConfirmado() {
  statusBar()->showMessage("Fase 3: Carga de datos", 2000);

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
  layout->setContentsMargins(8, 8, 8, 8);
  layout->setSpacing(8);

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
  statusBar()->showMessage("Fase 5: Consultas", 2000);

  ConstructorConsultas *constructor = new ConstructorConsultas(this);
  contenedorCentral->addWidget(constructor);
  contenedorCentral->setCurrentWidget(constructor);
}
