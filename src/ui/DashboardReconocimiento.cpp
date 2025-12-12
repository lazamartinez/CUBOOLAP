#include "DashboardReconocimiento.h"
#include "../core/AnalizadorEsquema.h"
#include "styles/FlutterTheme.h"
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
  FlutterTheme::instance().applyThemeToWidget(this);
}

void DashboardReconocimiento::configurarUi() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setSpacing(16);
  mainLayout->setContentsMargins(16, 16, 16, 16);

  // Header Title
  QLabel *titulo = new QLabel("Diagnostico del Esquema", this);
  titulo->setStyleSheet(
      "font-size: 20px; font-weight: 700; color: #1f2937;"); // Will be
                                                             // overriden by
                                                             // theme if needed
  mainLayout->addWidget(titulo);

  // Stats Row
  QHBoxLayout *statsLayout = new QHBoxLayout();
  statsLayout->setSpacing(12);

  lblTotalTablas = crearCardEstadistica("Tablas", "0", "#2563eb");
  lblTotalFilas = crearCardEstadistica("Registros", "0", "#6366f1");
  lblTotalFK = crearCardEstadistica("Relaciones", "0", "#10b981");
  lblTamanoDB = crearCardEstadistica("Tamano", "0 MB", "#f59e0b");

  statsLayout->addWidget(lblTotalTablas);
  statsLayout->addWidget(lblTotalFilas);
  statsLayout->addWidget(lblTotalFK);
  statsLayout->addWidget(lblTamanoDB);
  mainLayout->addLayout(statsLayout);

  // Charts Row
  QHBoxLayout *chartsLayout = new QHBoxLayout();
  chartsLayout->setSpacing(16);

  // Pie Chart Card
  FlutterCard *pieCard = new FlutterCard(this);
  QVBoxLayout *pieLayout = new QVBoxLayout(pieCard);
  pieLayout->setContentsMargins(12, 12, 12, 12);

  QLabel *lblPie = new QLabel("Distribucion por Tamaño", pieCard);
  lblPie->setStyleSheet("font-size: 14px; font-weight: 600; text-transform: "
                        "uppercase; letter-spacing: 0.5px;");
  pieLayout->addWidget(lblPie);

  QChart *chartPastel = new QChart();
  chartPastel->setBackgroundBrush(Qt::transparent);
  chartPastel->legend()->setVisible(true);
  chartPastel->legend()->setAlignment(Qt::AlignBottom);
  chartPastel->setMargins(QMargins(0, 0, 0, 0));

  graficoPastelView = new QChartView(chartPastel);
  graficoPastelView->setRenderHint(QPainter::Antialiasing);
  graficoPastelView->setMinimumHeight(250);
  graficoPastelView->setStyleSheet("background: transparent;");
  pieLayout->addWidget(graficoPastelView);
  chartsLayout->addWidget(pieCard);

  // Bar Chart Card
  FlutterCard *barCard = new FlutterCard(this);
  QVBoxLayout *barLayout = new QVBoxLayout(barCard);
  barLayout->setContentsMargins(12, 12, 12, 12);

  QLabel *lblBar = new QLabel("Top Tablas con Más Registros", barCard);
  lblBar->setStyleSheet("font-size: 14px; font-weight: 600; text-transform: "
                        "uppercase; letter-spacing: 0.5px;");
  barLayout->addWidget(lblBar);

  QChart *chartBarras = new QChart();
  chartBarras->setBackgroundBrush(Qt::transparent);
  chartBarras->setMargins(QMargins(0, 0, 0, 0));
  chartBarras->legend()->setVisible(false);

  graficoBarrasView = new QChartView(chartBarras);
  graficoBarrasView->setRenderHint(QPainter::Antialiasing);
  graficoBarrasView->setMinimumHeight(250);
  graficoBarrasView->setStyleSheet("background: transparent;");
  barLayout->addWidget(graficoBarrasView);
  chartsLayout->addWidget(barCard);

  mainLayout->addLayout(chartsLayout);

  // Table List Card
  FlutterCard *listCard = new FlutterCard(this);
  QVBoxLayout *listLayout = new QVBoxLayout(listCard);
  QLabel *lblList = new QLabel("Detalle de Tablas Detectadas", listCard);
  lblList->setStyleSheet("font-size: 14px; font-weight: 600;");
  listLayout->addWidget(lblList);

  listaTablas = new QListWidget(listCard);
  listaTablas->setMinimumHeight(150);
  listaTablas->setFrameShape(QFrame::NoFrame);
  listLayout->addWidget(listaTablas);
  mainLayout->addWidget(listCard);

  // Sugerencias Cards (Facts vs Dims)
  QHBoxLayout *sugLayout = new QHBoxLayout();
  sugLayout->setSpacing(12);

  // Facts
  FlutterCard *factCard = new FlutterCard(this);
  factCard->setStyleSheet(
      "QFrame { background-color: #fff7ed; }"); // Orange tint
  QVBoxLayout *fLayout = new QVBoxLayout(factCard);
  QLabel *lblFactTitle = new QLabel("Posibles Tablas de Hechos", factCard);
  lblFactTitle->setStyleSheet("font-weight: 700; color: #c2410c;");
  fLayout->addWidget(lblFactTitle);
  lblHechosSugeridos = new QLabel("Analizando...", factCard);
  lblHechosSugeridos->setStyleSheet("color: #9a3412;");
  lblHechosSugeridos->setWordWrap(true);
  fLayout->addWidget(lblHechosSugeridos);
  sugLayout->addWidget(factCard);

  // Dims
  FlutterCard *dimCard = new FlutterCard(this);
  dimCard->setStyleSheet("QFrame { background-color: #f0fdf4; }"); // Green tint
  QVBoxLayout *dLayout = new QVBoxLayout(dimCard);
  QLabel *lblDimTitle = new QLabel("Posibles Dimensiones", dimCard);
  lblDimTitle->setStyleSheet("font-weight: 700; color: #15803d;");
  dLayout->addWidget(lblDimTitle);
  lblDimensionesSugeridas = new QLabel("Analizando...", dimCard);
  lblDimensionesSugeridas->setStyleSheet("color: #166534;");
  lblDimensionesSugeridas->setWordWrap(true);
  dLayout->addWidget(lblDimensionesSugeridas);
  sugLayout->addWidget(dimCard);

  mainLayout->addLayout(sugLayout);

  // Action Button
  FlutterFilledButton *btnConfirmar =
      new FlutterFilledButton("CONTINUAR A MODELADO", this);
  btnConfirmar->setMinimumHeight(48);
  btnConfirmar->setIcon(MaterialIcons::instance().arrow_forward());
  connect(btnConfirmar, &QPushButton::clicked, this,
          &DashboardReconocimiento::confirmarReconocimiento);
  mainLayout->addWidget(btnConfirmar);
}

QWidget *DashboardReconocimiento::crearCardEstadistica(const QString &etiqueta,
                                                       const QString &valor,
                                                       const QString &color) {
  FlutterCard *card = new FlutterCard(this);
  // Add a colored left border sort of style using a layout trick or just simple
  // styling for now. Using QSS on the card directly.
  card->setStyleSheet(QString(R"(
      QFrame {
        background: white;
        border-radius: 8px;
        border-left: 4px solid %1;
      }
  )")
                          .arg(color));

  QVBoxLayout *layout = new QVBoxLayout(card);
  layout->setContentsMargins(12, 12, 12, 12);
  layout->setAlignment(Qt::AlignCenter);

  QLabel *lblValor = new QLabel(valor, card);
  lblValor->setObjectName("valorCard");
  lblValor->setStyleSheet(
      QString("font-size: 24px; font-weight: 800; color: %1;").arg(color));
  lblValor->setAlignment(Qt::AlignCenter);
  layout->addWidget(lblValor);

  QLabel *lblEtiqueta = new QLabel(etiqueta, card);
  lblEtiqueta->setStyleSheet("font-size: 12px; font-weight: 500; color: "
                             "#6b7280; text-transform: uppercase;");
  lblEtiqueta->setAlignment(Qt::AlignCenter);
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
    QListWidgetItem *item = new QListWidgetItem(
        QString("%1  (%2 filas)").arg(t.nombre).arg(t.filaEstimada));
    item->setIcon(MaterialIcons::instance().table_chart(16));
    listaTablas->addItem(item);
  }

  // Update Charts (Simplified)
  QPieSeries *pieSeries = new QPieSeries();
  pieSeries->setHoleSize(0.35);
  int count = 0;
  for (const auto &t : tablas) {
    if (count++ >= 5 || t.tamanoBytes == 0)
      continue;
    QPieSlice *slice = pieSeries->append(t.nombre, t.tamanoBytes);
    slice->setLabelVisible(true);
  }

  QChart *chartP = graficoPastelView->chart();
  chartP->removeAllSeries();
  chartP->addSeries(pieSeries);

  // Bar Chart
  QHorizontalBarSeries *barSeries = new QHorizontalBarSeries();
  QBarSet *set0 = new QBarSet("Registros");
  set0->setColor(QColor("#3b82f6"));
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
  // Remove old axes
  QList<QAbstractAxis *> axes = chartB->axes();
  for (auto *ax : axes)
    chartB->removeAxis(ax);

  chartB->addSeries(barSeries);

  QBarCategoryAxis *axisY = new QBarCategoryAxis();
  axisY->append(cats);
  chartB->addAxis(axisY, Qt::AlignLeft);
  barSeries->attachAxis(axisY);

  QValueAxis *axisX = new QValueAxis();
  chartB->addAxis(axisX, Qt::AlignBottom);
  barSeries->attachAxis(axisX);

  // Suggestions
  QStringList hechos = stats["posibles_tablas_hechos"].toStringList();
  QStringList dims = stats["posibles_dimensiones"].toStringList();

  lblHechosSugeridos->setText(hechos.isEmpty() ? "No detectadas con certeza."
                                               : hechos.join(", "));
  lblDimensionesSugeridas->setText(dims.isEmpty() ? "No detectadas con certeza."
                                                  : dims.join(", "));
}

void DashboardReconocimiento::actualizarCardValor(QWidget *card,
                                                  const QString &valor) {
  QLabel *lbl = card->findChild<QLabel *>("valorCard");
  if (lbl)
    lbl->setText(valor);
}
