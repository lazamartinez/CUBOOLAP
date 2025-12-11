#include "VentanaPrincipal.h"
#include "DialogoConexion.h"
#include "GestorTemas.h"
#include "IndicadorFase.h"
#include "PantallaIntroduccion.h"
#include "ToastNotifier.h"
#include <QApplication>
#include <QInputDialog>
#include <QMenuBar>
#include <QScreen>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
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

// Headers necesarios
#include "DialogoDrillThrough.h"
#include "DialogoFiltros.h"
#include "PanelAnalisis.h"
#include "VisorOlap.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

void VentanaPrincipal::alCargaFinalizada() {
  m_indicadorFase->setFaseActual(4);
  ToastNotifier::mostrar(this, "Carga completada. Cubo listo!",
                         ToastNotifier::Exito);
  statusBar()->showMessage("Fase 4: Explorador OLAP", 0);

  QWidget *containerFase4 = new QWidget(this);
  QHBoxLayout *mainLayout = new QHBoxLayout(containerFase4);
  mainLayout->setContentsMargins(12, 12, 12, 12);
  mainLayout->setSpacing(12);

  // Visor (Izquierda - Centro)
  VisorOlap *visor = new VisorOlap(containerFase4);

  // Panel Analisis + Acciones (Derecha)
  PanelAnalisis *panel = new PanelAnalisis(containerFase4);

  mainLayout->addWidget(visor, 3);
  mainLayout->addWidget(panel, 1);

  // === CONEXIONES BASICAS ===
  connect(panel, &PanelAnalisis::irAReportes, this,
          &VentanaPrincipal::alIrAReportes);
  connect(visor, &VisorOlap::celdaSeleccionada, panel,
          &PanelAnalisis::mostrarInfo);
  connect(visor, &VisorOlap::estadisticasActualizadas, panel,
          &PanelAnalisis::actualizarEstadisticas);

  // === OPERACIONES OLAP ===

  // 1. Drill Down
  connect(panel, &PanelAnalisis::operacionDrillDown, visor,
          &VisorOlap::ejecutarDrillDown);
  connect(visor, &VisorOlap::drillDownActivado, this, [this](int, int, int) {
    ToastNotifier::mostrar(this, "Drill Down: Detalle aplicado",
                           ToastNotifier::Info);
  });
  connect(visor, &VisorOlap::nivelCambiado, this, [this](const QString &nivel) {
    statusBar()->showMessage("Nivel actual: " + nivel);
  });

  // 2. Roll Up
  connect(panel, &PanelAnalisis::operacionRollUp, visor,
          &VisorOlap::ejecutarRollUp);
  // (El feedback visual es el cambio de nivel)

  // 3. Slice (Filtro simple)
  connect(panel, &PanelAnalisis::operacionSlice, this, [visor, this]() {
    DialogoFiltros *dlg = new DialogoFiltros(DialogoFiltros::Slice, this);
    QStringList dims = visor->obtenerDimensionesDisponibles();
    dlg->setDimensionesDisponibles(dims);

    connect(dlg, &DialogoFiltros::dimensionCambiada, this,
            [dlg, visor](const QString &dim) {
              dlg->setValoresDimension(dim,
                                       visor->obtenerValoresDimension(dim));
            });

    if (!dims.isEmpty()) {
      QString primeraDim = dims.first();
      dlg->setValoresDimension(primeraDim,
                               visor->obtenerValoresDimension(primeraDim));
    }

    if (dlg->exec() == QDialog::Accepted) {
      visor->ejecutarSlice(dlg->getDimensionSeleccionada(),
                           dlg->getValoresSeleccionados());
      ToastNotifier::mostrar(this, "Slice aplicado correctamente",
                             ToastNotifier::Exito);
    }
    dlg->deleteLater();
  });

  // 4. Dice (Filtros multiples)
  connect(panel, &PanelAnalisis::operacionDice, this, [visor, this]() {
    DialogoFiltros *dlg = new DialogoFiltros(DialogoFiltros::Dice, this);
    // Configurar igual que Slice pero modo Dice
    QStringList dims = visor->obtenerDimensionesDisponibles();
    dlg->setDimensionesDisponibles(dims);

    connect(dlg, &DialogoFiltros::dimensionCambiada, this,
            [dlg, visor](const QString &dim) {
              dlg->setValoresDimension(dim,
                                       visor->obtenerValoresDimension(dim));
            });

    if (!dims.isEmpty()) {
      QString primeraDim = dims.first();
      dlg->setValoresDimension(primeraDim,
                               visor->obtenerValoresDimension(primeraDim));
    }

    if (dlg->exec() == QDialog::Accepted) {
      visor->ejecutarDice(dlg->getTodosLosFiltros());
      ToastNotifier::mostrar(this, "Dice: Sub-cubo generado",
                             ToastNotifier::Exito);
    }
    dlg->deleteLater();
  });

  // 5. Pivot
  connect(panel, &PanelAnalisis::operacionPivot, this, [visor, this]() {
    visor->ejecutarPivot();
    ToastNotifier::mostrar(this, "Ejes rotados (Pivot)", ToastNotifier::Info);
  });

  // 6. Swap (Por ahora lo mismo que Pivot o podria ser Drag&Drop en futuro)
  connect(panel, &PanelAnalisis::operacionSwap, this, [visor, this]() {
    visor->ejecutarPivot();
    ToastNotifier::mostrar(this, "Dimensiones intercambiadas",
                           ToastNotifier::Info);
  });

  // 7. Ranking (Top N)
  connect(panel, &PanelAnalisis::operacionRanking, this, [visor, this]() {
    bool ok;
    int n = QInputDialog::getInt(
        this, "Top N", "Cantidad de elementos a mostrar:", 10, 1, 100, 1, &ok);
    if (ok) {
      visor->ejecutarRanking(n, true); // true = descendente
      ToastNotifier::mostrar(this, QString("Top %1 aplicado").arg(n),
                             ToastNotifier::Exito);
    }
  });

  // 8. Drill Through (Detalles) - SQL Generico
  auto handleDrillThrough = [this, visor]() {
    auto seleccion = visor->obtenerSeleccion();
    if (seleccion.isEmpty()) {
      ToastNotifier::mostrar(this, "Seleccione una celda primero",
                             ToastNotifier::Advertencia);
      return;
    }

    const CeldaCubo &celda = seleccion.first();
    QString sql = visor->generarConsultaSQL(celda, 1000); // Limit 1000

    if (sql.isEmpty()) {
      ToastNotifier::mostrar(this, "Error generando consulta",
                             ToastNotifier::Error);
      return;
    }

    DialogoDrillThrough *dlg = new DialogoDrillThrough(this);
    dlg->setTitulo(celda.nombreDimX, celda.nombreDimZ, celda.valor);

    QSqlDatabase db = QSqlDatabase::database("CuboVisionConnection");
    if (db.isOpen()) {
      QSqlQuery q(db);
      if (q.exec(sql)) {
        // Obtener nombres de columnas dinamicamente
        QStringList cols;
        QSqlRecord rec = q.record();
        for (int i = 0; i < rec.count(); ++i)
          cols << rec.fieldName(i);

        QVector<QStringList> filas;
        while (q.next()) {
          QStringList fila;
          for (int i = 0; i < rec.count(); ++i)
            fila << q.value(i).toString();
          filas << fila;
        }
        dlg->cargarDatos(cols, filas);
      } else {
        qDebug() << "SQL Error:" << q.lastError().text();
        ToastNotifier::mostrar(this, "Error ejecutando SQL",
                               ToastNotifier::Error);
      }
    }

    dlg->exec();
    dlg->deleteLater();
  };

  connect(panel, &PanelAnalisis::operacionDrillThrough, this,
          handleDrillThrough);
  // Tambien conectar la señal directa del visor si se usa menu contextual o
  // doble click raro
  connect(visor, &VisorOlap::solicitarDrillThrough, this, handleDrillThrough);

  // 9. Reset
  connect(panel, &PanelAnalisis::operacionReset, this, [visor, this]() {
    visor->resetearVista();
    ToastNotifier::mostrar(this, "Vista reseteada", ToastNotifier::Info);
  });

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
