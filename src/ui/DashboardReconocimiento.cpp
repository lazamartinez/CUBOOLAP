#include "DashboardReconocimiento.h"
#include "../core/AnalizadorEsquema.h"
#include "Estilos.h"
#include <QGroupBox>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QBarSet>
#include <QtCharts/QChart>
#include <QtCharts/QHorizontalBarSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QValueAxis>

DashboardReconocimiento::DashboardReconocimiento(QWidget *parent)
    : QWidget(parent) {
  configurarUi();
}

void DashboardReconocimiento::configurarUi() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setSpacing(12);
  mainLayout->setContentsMargins(16, 16, 16, 16);

  // Header compacto
  QLabel *titulo = new QLabel("Fase 1: Diagnostico del Esquema", this);
  titulo->setStyleSheet(R"(
    font-size: 16px;
    font-weight: 700;
    color: #1f2937;
    padding-bottom: 8px;
    border-bottom: 2px solid #2563eb;
  )");
  mainLayout->addWidget(titulo);

  // Estadisticas en fila compacta
  QHBoxLayout *statsLayout = new QHBoxLayout();
  statsLayout->setSpacing(10);

  lblTotalTablas = crearCardEstadistica("Tablas", "0", "#2563eb");
  lblTotalFilas = crearCardEstadistica("Registros", "0", "#6366f1");
  lblTotalFK = crearCardEstadistica("Relaciones", "0", "#10b981");
  lblTamanoDB = crearCardEstadistica("Tamano", "0 MB", "#f59e0b");

  statsLayout->addWidget(lblTotalTablas);
  statsLayout->addWidget(lblTotalFilas);
  statsLayout->addWidget(lblTotalFK);
  statsLayout->addWidget(lblTamanoDB);
  mainLayout->addLayout(statsLayout);

  // Graficos lado a lado (compactos)
  QHBoxLayout *chartsLayout = new QHBoxLayout();
  chartsLayout->setSpacing(12);

  // Grafico Pie
  QWidget *pieContainer = new QWidget(this);
  pieContainer->setStyleSheet(
      "background: #f9fafb; border: 1px solid #e5e7eb; border-radius: 8px;");
  QVBoxLayout *pieLayout = new QVBoxLayout(pieContainer);
  pieLayout->setContentsMargins(8, 8, 8, 8);

  QLabel *lblPie = new QLabel("Distribucion", pieContainer);
  lblPie->setStyleSheet("font-size: 12px; font-weight: 600; color: #374151;");
  pieLayout->addWidget(lblPie);

  QChart *chartPastel = new QChart();
  chartPastel->setTheme(QChart::ChartThemeLight);
  chartPastel->setBackgroundBrush(Qt::transparent);
  chartPastel->legend()->setVisible(true);
  chartPastel->legend()->setAlignment(Qt::AlignRight);
  chartPastel->setMargins(QMargins(0, 0, 0, 0));

  graficoPastelView = new QChartView(chartPastel);
  graficoPastelView->setRenderHint(QPainter::Antialiasing);
  graficoPastelView->setMinimumHeight(220);
  graficoPastelView->setStyleSheet("background: transparent;");
  pieLayout->addWidget(graficoPastelView);
  chartsLayout->addWidget(pieContainer);

  // Grafico Barras
  QWidget *barContainer = new QWidget(this);
  barContainer->setStyleSheet(
      "background: #f9fafb; border: 1px solid #e5e7eb; border-radius: 8px;");
  QVBoxLayout *barLayout = new QVBoxLayout(barContainer);
  barLayout->setContentsMargins(8, 8, 8, 8);

  QLabel *lblBar = new QLabel("Top Tablas", barContainer);
  lblBar->setStyleSheet("font-size: 12px; font-weight: 600; color: #374151;");
  barLayout->addWidget(lblBar);

  QChart *chartBarras = new QChart();
  chartBarras->setTheme(QChart::ChartThemeLight);
  chartBarras->setBackgroundBrush(Qt::transparent);
  chartBarras->setMargins(QMargins(5, 5, 5, 5));
  chartBarras->legend()->setVisible(false);

  graficoBarrasView = new QChartView(chartBarras);
  graficoBarrasView->setRenderHint(QPainter::Antialiasing);
  graficoBarrasView->setMinimumHeight(220);
  graficoBarrasView->setStyleSheet("background: transparent;");
  barLayout->addWidget(graficoBarrasView);
  chartsLayout->addWidget(barContainer);

  mainLayout->addLayout(chartsLayout);

  // Lista de tablas (compacta)
  QGroupBox *grpTablas = new QGroupBox("Tablas Detectadas", this);
  grpTablas->setStyleSheet(R"(
    QGroupBox {
      font-size: 12px;
      font-weight: 600;
      border: 1px solid #e5e7eb;
      border-radius: 6px;
      margin-top: 8px;
      padding-top: 8px;
    }
    QGroupBox::title {
      subcontrol-position: top left;
      padding: 2px 8px;
      color: #374151;
    }
  )");
  QVBoxLayout *tablasLayout = new QVBoxLayout(grpTablas);
  tablasLayout->setContentsMargins(8, 16, 8, 8);

  listaTablas = new QListWidget(grpTablas);
  listaTablas->setMinimumHeight(120);
  listaTablas->setStyleSheet("font-size: 11px;");
  tablasLayout->addWidget(listaTablas);
  mainLayout->addWidget(grpTablas);

  // Sugerencias (compactas)
  QHBoxLayout *sugLayout = new QHBoxLayout();
  sugLayout->setSpacing(10);

  QWidget *grpHechos = new QWidget(this);
  grpHechos->setStyleSheet("background: #fef3c7; border: 1px solid #fcd34d; "
                           "border-radius: 6px; padding: 8px;");
  QVBoxLayout *hLayout = new QVBoxLayout(grpHechos);
  hLayout->setContentsMargins(8, 8, 8, 8);
  QLabel *lblHT = new QLabel("Posibles Facts", grpHechos);
  lblHT->setStyleSheet("font-size: 11px; font-weight: 600; color: #b45309;");
  hLayout->addWidget(lblHT);
  lblHechosSugeridos = new QLabel("Analizando...", grpHechos);
  lblHechosSugeridos->setStyleSheet("font-size: 11px; color: #92400e;");
  hLayout->addWidget(lblHechosSugeridos);
  sugLayout->addWidget(grpHechos);

  QWidget *grpDims = new QWidget(this);
  grpDims->setStyleSheet("background: #d1fae5; border: 1px solid #6ee7b7; "
                         "border-radius: 6px; padding: 8px;");
  QVBoxLayout *dLayout = new QVBoxLayout(grpDims);
  dLayout->setContentsMargins(8, 8, 8, 8);
  QLabel *lblDT = new QLabel("Posibles Dims", grpDims);
  lblDT->setStyleSheet("font-size: 11px; font-weight: 600; color: #065f46;");
  dLayout->addWidget(lblDT);
  lblDimensionesSugeridas = new QLabel("Analizando...", grpDims);
  lblDimensionesSugeridas->setStyleSheet("font-size: 11px; color: #047857;");
  dLayout->addWidget(lblDimensionesSugeridas);
  sugLayout->addWidget(grpDims);

  mainLayout->addLayout(sugLayout);

  // Boton continuar
  QPushButton *btnConfirmar = new QPushButton("➡️ Continuar a Fase 2", this);
  btnConfirmar->setMinimumHeight(36);
  btnConfirmar->setCursor(Qt::PointingHandCursor);
  btnConfirmar->setStyleSheet(R"(
    QPushButton {
      background: #10b981;
      border: none;
      border-radius: 6px;
      color: white;
      font-size: 12px;
      font-weight: 600;
    }
    QPushButton:hover { background: #059669; }
  )");
  connect(btnConfirmar, &QPushButton::clicked, this,
          &DashboardReconocimiento::confirmarReconocimiento);
  mainLayout->addWidget(btnConfirmar);
}

QWidget *DashboardReconocimiento::crearCardEstadistica(const QString &etiqueta,
                                                       const QString &valor,
                                                       const QString &color) {

  QWidget *card = new QWidget(this);
  card->setStyleSheet(QString(R"(
    background: white;
    border: 1px solid #e5e7eb;
    border-left: 3px solid %1;
    border-radius: 6px;
  )")
                          .arg(color));

  QVBoxLayout *layout = new QVBoxLayout(card);
  layout->setAlignment(Qt::AlignCenter);
  layout->setSpacing(2);
  layout->setContentsMargins(10, 8, 10, 8);

  QLabel *lblValor = new QLabel(valor, card);
  lblValor->setObjectName("valorCard");
  lblValor->setStyleSheet(
      QString("font-size: 18px; font-weight: 700; color: %1;").arg(color));
  lblValor->setAlignment(Qt::AlignCenter);

  QLabel *lblEtiqueta = new QLabel(etiqueta, card);
  lblEtiqueta->setStyleSheet("font-size: 10px; color: #6b7280;");
  lblEtiqueta->setAlignment(Qt::AlignCenter);

  layout->addWidget(lblValor);
  layout->addWidget(lblEtiqueta);

  return card;
}

void DashboardReconocimiento::cargarDatos(AnalizadorEsquema *analizador) {
  if (!analizador)
    return;

  QVector<InfoTabla> tablas = analizador->obtenerTablas();
  QVariantMap stats = analizador->obtenerEstadisticasGeneral();
  QVector<InfoRelacionFK> relaciones = analizador->obtenerRelaciones();

  actualizarCardValor(lblTotalTablas, QString::number(tablas.size()));

  long long totalFilas = stats["total_filas"].toLongLong();
  QString filasStr = totalFilas > 1000
                         ? QString::number(totalFilas / 1000.0, 'f', 1) + "K"
                         : QString::number(totalFilas);
  actualizarCardValor(lblTotalFilas, filasStr);
  actualizarCardValor(lblTotalFK, QString::number(relaciones.size()));

  long long totalBytes = stats["total_bytes"].toLongLong();
  QString tamanoStr =
      totalBytes > 1048576
          ? QString::number(totalBytes / 1048576.0, 'f', 1) + " MB"
          : QString::number(totalBytes / 1024.0, 'f', 0) + " KB";
  actualizarCardValor(lblTamanoDB, tamanoStr);

  listaTablas->clear();
  for (const auto &t : tablas) {
    listaTablas->addItem(
        QString("%1 - %2 filas").arg(t.nombre).arg(t.filaEstimada));
  }

  // Graficos
  QPieSeries *pieSeries = new QPieSeries();
  pieSeries->setHoleSize(0.4);
  int count = 0;
  for (const auto &t : tablas) {
    if (count++ >= 5)
      break;
    if (t.tamanoBytes > 0) {
      QPieSlice *slice = pieSeries->append(t.nombre, t.tamanoBytes);
      slice->setLabelVisible(true);
      slice->setLabelPosition(QPieSlice::LabelOutside);
      // Mostrar nombre y porcentaje
      slice->setLabel(QString("%1\n%2%").arg(t.nombre).arg(
          100 * slice->percentage(), 0, 'f', 1));
    }
  }
  QChart *chartP = graficoPastelView->chart();
  chartP->removeAllSeries();
  chartP->addSeries(pieSeries);

  QHorizontalBarSeries *barSeries = new QHorizontalBarSeries();
  QBarSet *set0 = new QBarSet("");
  set0->setColor(QColor("#2563eb"));
  QStringList cats;
  count = 0;
  for (const auto &t : tablas) {
    if (count++ >= 5)
      break;
    *set0 << t.filaEstimada;
    cats << t.nombre;
  }
  barSeries->append(set0);

  QChart *chartB = graficoBarrasView->chart();
  chartB->removeAllSeries();
  for (auto *ax : chartB->axes())
    chartB->removeAxis(ax);
  chartB->addSeries(barSeries);

  QBarCategoryAxis *axisY = new QBarCategoryAxis();
  axisY->append(cats);
  chartB->addAxis(axisY, Qt::AlignLeft);
  barSeries->attachAxis(axisY);

  QValueAxis *axisX = new QValueAxis();
  axisX->setLabelFormat("%d");
  chartB->addAxis(axisX, Qt::AlignBottom);
  barSeries->attachAxis(axisX);

  // Sugerencias
  QStringList hechos = stats["posibles_tablas_hechos"].toStringList();
  QStringList dims = stats["posibles_dimensiones"].toStringList();

  lblHechosSugeridos->setText(hechos.isEmpty() ? "No detectadas"
                                               : hechos.join(", "));
  lblDimensionesSugeridas->setText(dims.isEmpty() ? "No detectadas"
                                                  : dims.join(", "));
}

void DashboardReconocimiento::actualizarCardValor(QWidget *card,
                                                  const QString &valor) {
  QLabel *lbl = card->findChild<QLabel *>("valorCard");
  if (lbl)
    lbl->setText(valor);
}
