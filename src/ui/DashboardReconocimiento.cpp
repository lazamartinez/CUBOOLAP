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
  mainLayout->setSpacing(20);
  mainLayout->setContentsMargins(24, 24, 24, 24);

  // ========== HEADER ==========
  QLabel *titulo = new QLabel("Fase 1: Diagnostico del Esquema", this);
  titulo->setStyleSheet(Estilos::obtenerEstiloTituloSeccion("#2563eb"));
  titulo->setAlignment(Qt::AlignLeft);
  mainLayout->addWidget(titulo);

  // ========== ESTADISTICAS RAPIDAS ==========
  QWidget *statsWidget = new QWidget(this);
  QHBoxLayout *statsLayout = new QHBoxLayout(statsWidget);
  statsLayout->setSpacing(16);

  lblTotalTablas = crearCardEstadistica("Tablas", "0", "#2563eb");
  lblTotalFilas = crearCardEstadistica("Registros", "0", "#6366f1");
  lblTotalFK = crearCardEstadistica("Relaciones", "0", "#10b981");
  lblTamanoDB = crearCardEstadistica("Tamano DB", "0 MB", "#f59e0b");

  statsLayout->addWidget(lblTotalTablas);
  statsLayout->addWidget(lblTotalFilas);
  statsLayout->addWidget(lblTotalFK);
  statsLayout->addWidget(lblTamanoDB);
  mainLayout->addWidget(statsWidget);

  // ========== GRAFICOS ==========
  QHBoxLayout *chartsLayout = new QHBoxLayout();
  chartsLayout->setSpacing(20);

  // Grafico de distribucion (Pastel)
  QWidget *pieContainer = new QWidget(this);
  pieContainer->setStyleSheet(Estilos::obtenerEstiloPanelGlass());
  QVBoxLayout *pieLayout = new QVBoxLayout(pieContainer);

  QLabel *lblPie = new QLabel("Distribucion por Tamano", pieContainer);
  lblPie->setStyleSheet("font-size: 15px; font-weight: 600; color: #374151;");
  pieLayout->addWidget(lblPie);

  QChart *chartPastel = new QChart();
  chartPastel->setTitle("");
  chartPastel->setTheme(QChart::ChartThemeLight);
  chartPastel->setBackgroundBrush(QBrush(Qt::transparent));
  chartPastel->legend()->setVisible(true);
  chartPastel->legend()->setAlignment(Qt::AlignRight);

  graficoPastelView = new QChartView(chartPastel);
  graficoPastelView->setRenderHint(QPainter::Antialiasing);
  graficoPastelView->setMinimumHeight(260);
  graficoPastelView->setStyleSheet("background: transparent;");
  pieLayout->addWidget(graficoPastelView);
  chartsLayout->addWidget(pieContainer);

  // Grafico de barras (Top Tablas)
  QWidget *barContainer = new QWidget(this);
  barContainer->setStyleSheet(Estilos::obtenerEstiloPanelGlass());
  QVBoxLayout *barLayout = new QVBoxLayout(barContainer);

  QLabel *lblBar = new QLabel("Top 10 Tablas por Registros", barContainer);
  lblBar->setStyleSheet("font-size: 15px; font-weight: 600; color: #374151;");
  barLayout->addWidget(lblBar);

  QChart *chartBarras = new QChart();
  chartBarras->setTitle("");
  chartBarras->setTheme(QChart::ChartThemeLight);
  chartBarras->setBackgroundBrush(QBrush(Qt::transparent));

  graficoBarrasView = new QChartView(chartBarras);
  graficoBarrasView->setRenderHint(QPainter::Antialiasing);
  graficoBarrasView->setMinimumHeight(260);
  graficoBarrasView->setStyleSheet("background: transparent;");
  barLayout->addWidget(graficoBarrasView);
  chartsLayout->addWidget(barContainer);

  mainLayout->addLayout(chartsLayout);

  // ========== TABLAS DETECTADAS ==========
  QGroupBox *grpTablas = new QGroupBox("Tablas Detectadas", this);
  QVBoxLayout *tablasLayout = new QVBoxLayout(grpTablas);
  listaTablas = new QListWidget(grpTablas);
  listaTablas->setMinimumHeight(120);
  listaTablas->setMaximumHeight(160);
  tablasLayout->addWidget(listaTablas);
  mainLayout->addWidget(grpTablas);

  // ========== SUGERENCIAS OLAP ==========
  QHBoxLayout *sugerenciasLayout = new QHBoxLayout();
  sugerenciasLayout->setSpacing(16);

  QGroupBox *grpHechos = new QGroupBox("Posibles Tablas de Hechos", this);
  QVBoxLayout *hechosLayout = new QVBoxLayout(grpHechos);
  lblHechosSugeridos = new QLabel("Analizando...", grpHechos);
  lblHechosSugeridos->setWordWrap(true);
  lblHechosSugeridos->setStyleSheet("color: #f59e0b; font-size: 13px;");
  hechosLayout->addWidget(lblHechosSugeridos);
  sugerenciasLayout->addWidget(grpHechos);

  QGroupBox *grpDims = new QGroupBox("Posibles Dimensiones", this);
  QVBoxLayout *dimsLayout = new QVBoxLayout(grpDims);
  lblDimensionesSugeridas = new QLabel("Analizando...", grpDims);
  lblDimensionesSugeridas->setWordWrap(true);
  lblDimensionesSugeridas->setStyleSheet("color: #10b981; font-size: 13px;");
  dimsLayout->addWidget(lblDimensionesSugeridas);
  sugerenciasLayout->addWidget(grpDims);

  mainLayout->addLayout(sugerenciasLayout);

  // ========== BOTON CONFIRMAR ==========
  QPushButton *btnConfirmar =
      new QPushButton("Confirmar y Continuar a Fase 2", this);
  btnConfirmar->setMinimumHeight(48);
  btnConfirmar->setCursor(Qt::PointingHandCursor);
  btnConfirmar->setStyleSheet(Estilos::obtenerEstiloBotonExito());
  connect(btnConfirmar, &QPushButton::clicked, this,
          &DashboardReconocimiento::confirmarReconocimiento);
  mainLayout->addWidget(btnConfirmar);
}

QWidget *DashboardReconocimiento::crearCardEstadistica(const QString &etiqueta,
                                                       const QString &valor,
                                                       const QString &color) {

  QWidget *card = new QWidget(this);
  card->setStyleSheet(QString(R"(
    background: rgba(255, 255, 255, 0.9);
    border: 1px solid #e5e7eb;
    border-left: 4px solid %1;
    border-radius: 10px;
    padding: 16px;
  )")
                          .arg(color));

  QVBoxLayout *layout = new QVBoxLayout(card);
  layout->setAlignment(Qt::AlignCenter);
  layout->setSpacing(6);

  QLabel *lblValor = new QLabel(valor, card);
  lblValor->setObjectName("valorCard");
  lblValor->setStyleSheet(
      QString("font-size: 28px; font-weight: 700; color: %1;").arg(color));
  lblValor->setAlignment(Qt::AlignCenter);

  QLabel *lblEtiqueta = new QLabel(etiqueta, card);
  lblEtiqueta->setStyleSheet(
      "font-size: 13px; color: #6b7280; font-weight: 500;");
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

  // Actualizar cards de estadisticas
  actualizarCardValor(lblTotalTablas, QString::number(tablas.size()));

  long long totalFilas = stats["total_filas"].toLongLong();
  QString filasStr;
  if (totalFilas > 1000000) {
    filasStr = QString::number(totalFilas / 1000000.0, 'f', 1) + "M";
  } else if (totalFilas > 1000) {
    filasStr = QString::number(totalFilas / 1000.0, 'f', 1) + "K";
  } else {
    filasStr = QString::number(totalFilas);
  }
  actualizarCardValor(lblTotalFilas, filasStr);

  actualizarCardValor(lblTotalFK, QString::number(relaciones.size()));

  long long totalBytes = stats["total_bytes"].toLongLong();
  QString tamanoStr;
  if (totalBytes > 1073741824) {
    tamanoStr = QString::number(totalBytes / 1073741824.0, 'f', 2) + " GB";
  } else {
    tamanoStr = QString::number(totalBytes / 1048576.0, 'f', 1) + " MB";
  }
  actualizarCardValor(lblTamanoDB, tamanoStr);

  // Llenar lista de tablas
  listaTablas->clear();
  for (const auto &t : tablas) {
    QString texto = QString("%1  |  %2 filas  |  %3")
                        .arg(t.nombre, -25)
                        .arg(t.filaEstimada)
                        .arg(t.tamano);
    listaTablas->addItem(texto);
  }

  // Grafico Pie (distribucion por tamano)
  QPieSeries *pieSeries = new QPieSeries();
  int count = 0;
  long long totalBytesChart = 0;
  for (const auto &t : tablas)
    totalBytesChart += t.tamanoBytes;

  for (const auto &t : tablas) {
    if (count++ >= 8)
      break;
    if (t.tamanoBytes > 0) {
      QPieSlice *slice = pieSeries->append(t.nombre, t.tamanoBytes);
      slice->setLabelVisible(t.tamanoBytes > totalBytesChart * 0.05);
    }
  }
  pieSeries->setHoleSize(0.45);

  QChart *chartP = graficoPastelView->chart();
  chartP->removeAllSeries();
  chartP->addSeries(pieSeries);

  // Grafico Barras (top tablas por filas)
  QHorizontalBarSeries *barSeries = new QHorizontalBarSeries();
  QBarSet *set0 = new QBarSet("Registros");
  set0->setColor(QColor("#2563eb"));
  QStringList categorias;

  count = 0;
  for (const auto &t : tablas) {
    if (count++ >= 10)
      break;
    *set0 << t.filaEstimada;
    categorias << t.nombre;
  }
  barSeries->append(set0);

  QChart *chartB = graficoBarrasView->chart();
  chartB->removeAllSeries();

  const auto axes = chartB->axes();
  for (auto *axis : axes) {
    chartB->removeAxis(axis);
  }

  chartB->addSeries(barSeries);

  QBarCategoryAxis *axisY = new QBarCategoryAxis();
  axisY->append(categorias);
  chartB->addAxis(axisY, Qt::AlignLeft);
  barSeries->attachAxis(axisY);

  QValueAxis *axisX = new QValueAxis();
  axisX->setLabelFormat("%d");
  chartB->addAxis(axisX, Qt::AlignBottom);
  barSeries->attachAxis(axisX);

  // Mostrar sugerencias OLAP
  QStringList hechos = stats["posibles_tablas_hechos"].toStringList();
  QStringList dims = stats["posibles_dimensiones"].toStringList();

  if (hechos.isEmpty()) {
    lblHechosSugeridos->setText(
        "No se detectaron tablas de hechos automaticamente.\nBusque tablas con "
        "muchas filas y varias FKs.");
  } else {
    lblHechosSugeridos->setText(hechos.join("\n"));
    lblHechosSugeridos->setStyleSheet(
        "color: #d97706; font-size: 13px; font-weight: 500;");
  }

  if (dims.isEmpty()) {
    lblDimensionesSugeridas->setText(
        "No se detectaron dimensiones automaticamente.\nBusque tablas "
        "referenciadas por FKs.");
  } else {
    lblDimensionesSugeridas->setText(dims.join("\n"));
    lblDimensionesSugeridas->setStyleSheet(
        "color: #059669; font-size: 13px; font-weight: 500;");
  }
}

void DashboardReconocimiento::actualizarCardValor(QWidget *card,
                                                  const QString &valor) {
  QLabel *lbl = card->findChild<QLabel *>("valorCard");
  if (lbl) {
    lbl->setText(valor);
  }
}
