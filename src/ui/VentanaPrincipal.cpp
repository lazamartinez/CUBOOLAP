#include "VentanaPrincipal.h"
#include "DialogoConexion.h"
#include "GestorTemas.h"
#include "IndicadorFase.h"
#include "PantallaIntroduccion.h"
#include "ToastNotifier.h"
#include <QApplication>
#include <QMenuBar>
#include <QScreen>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStatusBar>
#include <QTimer>
#include <QToolBar>


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
  resize(1100, 750);
  setMinimumSize(900, 650);

  // Centrar en pantalla
  if (QScreen *screen = QApplication::primaryScreen()) {
    QRect geo = screen->availableGeometry();
    move((geo.width() - 1100) / 2, (geo.height() - 750) / 2);
  }

  // Aplicar tema
  aplicarTema();
  connect(&GestorTemas::instancia(), &GestorTemas::temaCambiado, this,
          [this]() { aplicarTema(); });

  // Menu
  QMenuBar *menuBar = this->menuBar();
  QMenu *menuArchivo = menuBar->addMenu("Archivo");
  menuArchivo->addAction("Nueva Conexion", this,
                         &VentanaPrincipal::mostrarDialogoConexion);
  menuArchivo->addSeparator();
  menuArchivo->addAction("Salir", this, &QMainWindow::close);

  QMenu *menuVer = menuBar->addMenu("Ver");
  QAction *actTema = menuVer->addAction("Modo Oscuro", this, [this]() {
    GestorTemas::instancia().alternarTema();
    ToastNotifier::mostrar(this,
                           GestorTemas::instancia().esModoOscuro()
                               ? "Modo oscuro activado"
                               : "Modo claro activado",
                           ToastNotifier::Info);
  });
  actTema->setCheckable(true);

  QMenu *menuAyuda = menuBar->addMenu("Ayuda");
  menuAyuda->addAction("Acerca de", this, [this]() {
    ToastNotifier::mostrar(this, "Cubo Vision v1.0 - Sistema OLAP Avanzado",
                           ToastNotifier::Info, 4000);
  });

  // Toolbar con indicador de fase
  QToolBar *toolbar = new QToolBar("Navegacion", this);
  toolbar->setMovable(false);
  toolbar->setStyleSheet(
      "QToolBar { border: none; background: transparent; padding: 4px; }");

  m_indicadorFase = new IndicadorFase(this);
  toolbar->addWidget(m_indicadorFase);

  addToolBar(Qt::TopToolBarArea, toolbar);

  statusBar()->showMessage("Bienvenido a Cubo Vision");
}

void VentanaPrincipal::aplicarTema() {
  setStyleSheet(GestorTemas::instancia().obtenerEstiloGlobal());
}

void VentanaPrincipal::mostrarIntroduccion() {
  m_indicadorFase->setFaseActual(0);

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
  ToastNotifier::mostrar(this, "Conexion exitosa", ToastNotifier::Exito);
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
  m_indicadorFase->setFaseActual(1);
  ToastNotifier::mostrar(this, "Analisis completado", ToastNotifier::Exito);
  statusBar()->showMessage("Fase 1: Diagnostico", 0);

  m_dashboard = new DashboardReconocimiento(this);
  m_dashboard->cargarDatos(m_analizador);

  connect(m_dashboard, &DashboardReconocimiento::confirmarReconocimiento, this,
          &VentanaPrincipal::alConfirmarReconocimiento);

  contenedorCentral->addWidget(m_dashboard);
  contenedorCentral->setCurrentWidget(m_dashboard);
}

void VentanaPrincipal::alConfirmarReconocimiento() {
  m_indicadorFase->setFaseActual(2);
  ToastNotifier::mostrar(this, "Pasando a Fase 2: Modelado",
                         ToastNotifier::Info);
  statusBar()->showMessage("Fase 2: Modelado", 0);

  EstudioModelado *estudio = new EstudioModelado(this);
  contenedorCentral->addWidget(estudio);
  contenedorCentral->setCurrentWidget(estudio);

  connect(estudio, &EstudioModelado::modeloConfirmado, this,
          &VentanaPrincipal::alModeloConfirmado);
}

void VentanaPrincipal::alModeloConfirmado() {
  m_indicadorFase->setFaseActual(3);
  ToastNotifier::mostrar(this, "Iniciando carga de datos", ToastNotifier::Info);
  statusBar()->showMessage("Fase 3: Carga", 0);

  ConsolaProgreso *consola = new ConsolaProgreso(this);
  MotorCarga *motor = new MotorCarga(consola, this);

  connect(consola, &ConsolaProgreso::cargaCompletada, this,
          &VentanaPrincipal::alCargaFinalizada);

  contenedorCentral->addWidget(consola);
  contenedorCentral->setCurrentWidget(consola);

  motor->iniciarCarga();
}

#include "DialogoDrillThrough.h"
#include "DialogoFiltros.h"
#include "PanelAnalisis.h"
#include "PanelOperacionesOlap.h"
#include "VisorOlap.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

void VentanaPrincipal::alCargaFinalizada() {
  m_indicadorFase->setFaseActual(4);
  ToastNotifier::mostrar(this, "Carga completada. Cubo listo!",
                         ToastNotifier::Exito);
  statusBar()->showMessage("Fase 4: Explorador OLAP", 0);

  QWidget *containerFase4 = new QWidget(this);
  QVBoxLayout *mainLayout = new QVBoxLayout(containerFase4);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);

  // Panel de operaciones OLAP (barra superior)
  PanelOperacionesOlap *panelOps = new PanelOperacionesOlap(containerFase4);
  mainLayout->addWidget(panelOps);

  // Contenedor del visor y panel de analisis
  QWidget *containerVisor = new QWidget(containerFase4);
  QHBoxLayout *visorLayout = new QHBoxLayout(containerVisor);
  visorLayout->setContentsMargins(12, 12, 12, 12);
  visorLayout->setSpacing(12);

  VisorOlap *visor = new VisorOlap(containerVisor);
  PanelAnalisis *panel = new PanelAnalisis(containerVisor);

  visorLayout->addWidget(visor, 3);
  visorLayout->addWidget(panel, 1);
  mainLayout->addWidget(containerVisor, 1);

  // Conexiones basicas
  connect(panel, &PanelAnalisis::irAReportes, this,
          &VentanaPrincipal::alIrAReportes);
  connect(visor, &VisorOlap::celdaSeleccionada, panel,
          &PanelAnalisis::mostrarInfo);
  connect(visor, &VisorOlap::estadisticasActualizadas, panel,
          &PanelAnalisis::actualizarEstadisticas);

  // Conexiones de operaciones OLAP
  connect(panelOps, &PanelOperacionesOlap::drillDown, visor,
          &VisorOlap::ejecutarDrillDown);
  connect(panelOps, &PanelOperacionesOlap::rollUp, visor,
          &VisorOlap::ejecutarRollUp);
  connect(panelOps, &PanelOperacionesOlap::pivot, visor,
          &VisorOlap::ejecutarPivot);
  connect(panelOps, &PanelOperacionesOlap::drillThrough, visor,
          &VisorOlap::ejecutarDrillThrough);
  connect(panelOps, &PanelOperacionesOlap::resetView, visor,
          &VisorOlap::resetearVista);

  // Slice: Mostrar dialogo de filtro por una dimension
  connect(panelOps, &PanelOperacionesOlap::slice, this, [visor, this]() {
    DialogoFiltros *dlg = new DialogoFiltros(DialogoFiltros::Slice, this);
    dlg->setDimensionesDisponibles(visor->obtenerDimensionesDisponibles());

    connect(dlg, &DialogoFiltros::dimensionCambiada, this,
            [dlg, visor](const QString &dim) {
              dlg->setValoresDimension(dim,
                                       visor->obtenerValoresDimension(dim));
            });

    // Cargar valores iniciales
    if (!visor->obtenerDimensionesDisponibles().isEmpty()) {
      QString primeraDim = visor->obtenerDimensionesDisponibles().first();
      dlg->setValoresDimension(primeraDim,
                               visor->obtenerValoresDimension(primeraDim));
    }

    if (dlg->exec() == QDialog::Accepted) {
      visor->ejecutarSlice(dlg->getDimensionSeleccionada(),
                           dlg->getValoresSeleccionados());
    }
    dlg->deleteLater();
  });

  // Dice: Dialogo de filtros multiples
  connect(panelOps, &PanelOperacionesOlap::dice, this, [visor, this]() {
    DialogoFiltros *dlg = new DialogoFiltros(DialogoFiltros::Dice, this);
    dlg->setDimensionesDisponibles(visor->obtenerDimensionesDisponibles());

    connect(dlg, &DialogoFiltros::dimensionCambiada, this,
            [dlg, visor](const QString &dim) {
              dlg->setValoresDimension(dim,
                                       visor->obtenerValoresDimension(dim));
            });

    if (!visor->obtenerDimensionesDisponibles().isEmpty()) {
      QString primeraDim = visor->obtenerDimensionesDisponibles().first();
      dlg->setValoresDimension(primeraDim,
                               visor->obtenerValoresDimension(primeraDim));
    }

    if (dlg->exec() == QDialog::Accepted) {
      visor->ejecutarDice(dlg->getTodosLosFiltros());
    }
    dlg->deleteLater();
  });

  // Ranking: Dialogo simple para Top N
  connect(panelOps, &PanelOperacionesOlap::ranking, this, [visor]() {
    visor->ejecutarRanking(10, true); // Top 10 descendente por defecto
  });

  // Drill Through: Mostrar dialogo con registros
  connect(
      visor, &VisorOlap::solicitarDrillThrough, this,
      [this, visor](const QString &dimX, const QString &dimZ, double valor) {
        DialogoDrillThrough *dlg = new DialogoDrillThrough(this);
        dlg->setTitulo(dimX, dimZ, valor);

        // Obtener datos de drill-through desde la BD
        QSqlDatabase db = QSqlDatabase::database("CuboVisionConnection");
        if (db.isOpen()) {
          QString sql =
              QString(
                  "SELECT t.fecha, p.nombre as producto, c.nombre as cliente, "
                  "f.cantidad, f.total_venta, f.ganancia "
                  "FROM fact_ventas f "
                  "JOIN dim_tiempo t ON f.id_tiempo = t.id_tiempo "
                  "JOIN dim_producto p ON f.id_producto = p.id_producto "
                  "JOIN dim_cliente c ON f.id_cliente = c.id_cliente "
                  "WHERE p.id_producto::text = '%1' OR c.id_cliente::text = "
                  "'%2' "
                  "ORDER BY t.fecha DESC LIMIT 100")
                  .arg(dimX, dimZ);

          QSqlQuery q(db);
          if (q.exec(sql)) {
            QStringList cols = {"Fecha",    "Producto", "Cliente",
                                "Cantidad", "Total",    "Ganancia"};
            QVector<QStringList> filas;
            while (q.next()) {
              QStringList fila;
              for (int i = 0; i < 6; i++) {
                fila << q.value(i).toString();
              }
              filas << fila;
            }
            dlg->cargarDatos(cols, filas);
          }
        }

        dlg->exec();
        dlg->deleteLater();
      });

  // Actualizar panel de operaciones cuando cambian filtros
  connect(visor, &VisorOlap::nivelCambiado, panelOps,
          &PanelOperacionesOlap::setNivelActual);
  connect(visor, &VisorOlap::filtrosActualizados, panelOps,
          &PanelOperacionesOlap::setFiltrosActivos);

  contenedorCentral->addWidget(containerFase4);
  contenedorCentral->setCurrentWidget(containerFase4);

  visor->setFocus();
}

#include "ConstructorConsultas.h"

void VentanaPrincipal::alIrAReportes() {
  m_indicadorFase->setFaseActual(5);
  ToastNotifier::mostrar(this, "Constructor de Consultas", ToastNotifier::Info);
  statusBar()->showMessage("Fase 5: Consultas", 0);

  ConstructorConsultas *constructor = new ConstructorConsultas(this);

  // Conectar volver al inicio
  connect(constructor, &ConstructorConsultas::volverAlInicio, this, [this]() {
    ToastNotifier::mostrar(this, "Volviendo al inicio", ToastNotifier::Info);
    mostrarIntroduccion();
  });

  contenedorCentral->addWidget(constructor);
  contenedorCentral->setCurrentWidget(constructor);
}
