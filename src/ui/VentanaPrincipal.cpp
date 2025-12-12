#include "VentanaPrincipal.h"
#include "DialogoConexion.h"
#include "PantallaIntroduccion.h"
#include "ToastNotifier.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QScreen>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStatusBar>
#include <QTimer>
#include <QToolBar>
#include <QVBoxLayout>

#include "../core/AnalizadorEsquema.h"
#include "../core/GeneradorReportes.h" // Assuming might be needed or kept includes
#include "../core/MotorCarga.h"

// UI Components
#include "ConsolaProgreso.h"
#include "ConstructorConsultas.h"
#include "DashboardReconocimiento.h"
#include "DialogoDrillThrough.h"
#include "DialogoFiltros.h"
#include "EstudioModelado.h"
#include "PanelAnalisis.h"
#include "VisorOlap.h"

VentanaPrincipal::VentanaPrincipal(QWidget *parent)
    : QMainWindow(parent), mainContainer(new QWidget(this)),
      contenedorCentral(new QStackedWidget(this)) {
  configurarInterfaz();
  mostrarIntroduccion();
}

VentanaPrincipal::~VentanaPrincipal() {}

void VentanaPrincipal::configurarInterfaz() {
  // Configuración básica de ventana
  setCentralWidget(mainContainer);
  setWindowTitle("Cubo Vision - Sistema OLAP");
  resize(1100, 750); // Default size
  setMinimumSize(900, 650);

  // Layout principal
  QVBoxLayout *layoutPrincipal = new QVBoxLayout(mainContainer);
  layoutPrincipal->setContentsMargins(0, 0, 0, 0);
  layoutPrincipal->setSpacing(0);

  // App Bar
  appBar = new FlutterAppBar("Cubo Vision", this);
  layoutPrincipal->addWidget(appBar);

  // Contenedor Central
  layoutPrincipal->addWidget(contenedorCentral);

  // Centrar en pantalla
  if (QScreen *screen = QApplication::primaryScreen()) {
    QRect geo = screen->availableGeometry();
    move((geo.width() - 1100) / 2, (geo.height() - 750) / 2);
  }

  // Aplicar tema inicial
  FlutterTheme::instance().applyThemeToWidget(this);
  connect(&FlutterTheme::instance(), &FlutterTheme::themeChanged, this,
          [this]() { aplicarTema(); });
}

void VentanaPrincipal::aplicarTema() {
  FlutterTheme::instance().applyThemeToWidget(this);
}

void VentanaPrincipal::actualizarFase(int fase, const QString &titulo) {
  // Update basic logic for phase tracking if needed
  // For now, update AppBar title
  appBar->setTitle(QString("Cubo Vision - %1").arg(titulo));
}

void VentanaPrincipal::mostrarIntroduccion() {
  actualizarFase(0, "Inicio");
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

void VentanaPrincipal::alAnalisisCompletado() {
  actualizarFase(1, "Diagnostico");
  ToastNotifier::mostrar(this, "Analisis completado", ToastNotifier::Exito);

  m_dashboard = new DashboardReconocimiento(this);
  m_dashboard->cargarDatos(m_analizador);

  connect(m_dashboard, &DashboardReconocimiento::confirmarReconocimiento, this,
          &VentanaPrincipal::alConfirmarReconocimiento);

  contenedorCentral->addWidget(m_dashboard);
  contenedorCentral->setCurrentWidget(m_dashboard);
}

void VentanaPrincipal::alConfirmarReconocimiento() {
  actualizarFase(2, "Modelado");
  ToastNotifier::mostrar(this, "Pasando a Fase 2: Modelado",
                         ToastNotifier::Info);

  EstudioModelado *estudio = new EstudioModelado(this);
  contenedorCentral->addWidget(estudio);
  contenedorCentral->setCurrentWidget(estudio);

  connect(estudio, &EstudioModelado::modeloConfirmado, this,
          &VentanaPrincipal::alModeloConfirmado);
}

void VentanaPrincipal::alModeloConfirmado() {
  actualizarFase(3, "Carga");
  ToastNotifier::mostrar(this, "Iniciando carga de datos", ToastNotifier::Info);

  ConsolaProgreso *consola = new ConsolaProgreso(this);
  MotorCarga *motor = new MotorCarga(consola, this);

  connect(consola, &ConsolaProgreso::cargaCompletada, this,
          &VentanaPrincipal::alCargaFinalizada);

  contenedorCentral->addWidget(consola);
  contenedorCentral->setCurrentWidget(consola);

  motor->iniciarCarga();
}

void VentanaPrincipal::alCargaFinalizada() {
  actualizarFase(4, "Explorador OLAP");
  ToastNotifier::mostrar(this, "Carga completada. Cubo listo!",
                         ToastNotifier::Exito);

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

  // 3. Slice (Filtro simple)
  connect(panel, &PanelAnalisis::operacionSlice, this, [visor, this]() {
    try {
      DialogoFiltros *dlg = new DialogoFiltros(DialogoFiltros::Slice, this);
      QStringList dims = visor->obtenerDimensionesDisponibles();
      dlg->blockSignals(true);
      dlg->setDimensionesDisponibles(dims);

      if (!dims.isEmpty()) {
        dlg->setValoresDimension(dims.first(),
                                 visor->obtenerValoresDimension(dims.first()));
      }
      dlg->blockSignals(false);

      connect(dlg, &DialogoFiltros::dimensionCambiada, this,
              [dlg, visor](const QString &dim) {
                dlg->blockSignals(true);
                dlg->setValoresDimension(dim,
                                         visor->obtenerValoresDimension(dim));
                dlg->blockSignals(false);
              });

      if (dlg->exec() == QDialog::Accepted) {
        visor->ejecutarSlice(dlg->getDimensionSeleccionada(),
                             dlg->getValoresSeleccionados());
        ToastNotifier::mostrar(this, "Slice aplicado correctamente",
                               ToastNotifier::Exito);
      }
      dlg->deleteLater();
    } catch (...) {
      // ... exception handling simplified for brevity in this replace text
      // block, can revert to full if needed
      ToastNotifier::mostrar(this, "Error en Slice", ToastNotifier::Error);
    }
  });

  // 4. Dice
  connect(panel, &PanelAnalisis::operacionDice, this, [visor, this]() {
    try {
      DialogoFiltros *dlg = new DialogoFiltros(DialogoFiltros::Dice, this);
      QStringList dims = visor->obtenerDimensionesDisponibles();
      dlg->blockSignals(true);
      dlg->setDimensionesDisponibles(dims);
      if (!dims.isEmpty())
        dlg->setValoresDimension(dims.first(),
                                 visor->obtenerValoresDimension(dims.first()));
      dlg->blockSignals(false);

      connect(dlg, &DialogoFiltros::dimensionCambiada, this,
              [dlg, visor](const QString &dim) {
                dlg->blockSignals(true);
                dlg->setValoresDimension(dim,
                                         visor->obtenerValoresDimension(dim));
                dlg->blockSignals(false);
              });

      if (dlg->exec() == QDialog::Accepted) {
        visor->ejecutarDice(dlg->getTodosLosFiltros());
        ToastNotifier::mostrar(this, "Dice: Sub-cubo generado",
                               ToastNotifier::Exito);
      }
      dlg->deleteLater();
    } catch (...) {
      ToastNotifier::mostrar(this, "Error en Dice", ToastNotifier::Error);
    }
  });

  // 5. Pivot
  connect(panel, &PanelAnalisis::operacionPivot, this, [visor, this]() {
    visor->ejecutarPivot();
    ToastNotifier::mostrar(this, "Ejes rotados (Pivot)", ToastNotifier::Info);
  });

  // 6. Swap
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

  // 8. Drill Through
  auto handleDrillThrough = [this, visor]() {
    auto seleccion = visor->obtenerSeleccion();
    if (seleccion.isEmpty()) {
      ToastNotifier::mostrar(this, "Seleccione una celda primero",
                             ToastNotifier::Advertencia);
      return;
    }

    const CeldaCubo &celda = seleccion.first();
    QString sql = visor->generarConsultaSQL(celda, 1000);

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
        ToastNotifier::mostrar(this, "Error ejecutando SQL",
                               ToastNotifier::Error);
      }
    }
    dlg->exec();
    dlg->deleteLater();
  };

  connect(panel, &PanelAnalisis::operacionDrillThrough, this,
          handleDrillThrough);
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

void VentanaPrincipal::alIrAReportes() {
  actualizarFase(5, "Consultas");
  ToastNotifier::mostrar(this, "Constructor de Consultas", ToastNotifier::Info);

  ConstructorConsultas *constructor = new ConstructorConsultas(this);

  // Conectar volver al inicio
  connect(constructor, &ConstructorConsultas::volverAlInicio, this, [this]() {
    ToastNotifier::mostrar(this, "Volviendo al inicio", ToastNotifier::Info);
    mostrarIntroduccion();
  });

  contenedorCentral->addWidget(constructor);
  contenedorCentral->setCurrentWidget(constructor);
}
