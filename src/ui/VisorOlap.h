#ifndef VISOROLAP_H
#define VISOROLAP_H

#include <QMap>
#include <QPainterPath>
#include <QVector>
#include <QWidget>

// ============================================================================
// ESTRUCTURAS DE DATOS MEJORADAS
// ============================================================================

// Información de una dimensión del cubo
struct DimensionInfo {
  QString nombre;      // Nombre descriptivo (ej: "Producto", "Cliente")
  QString tabla;       // Tabla de origen
  QString columna;     // Columna que contiene los valores
  QString fkColumna;   // Foreign key en tabla de hechos (si aplica)
  QString pkColumna;   // Primary key en tabla de dimensión
  QStringList valores; // Top N valores para visualización
  int cardinalidad;    // Total de valores únicos
  bool esDegenerada;   // Si está en la tabla de hechos directamente

  DimensionInfo() : cardinalidad(0), esDegenerada(false) {}
};

// Metadata completa del cubo cargado
struct MetadataCubo {
  QString nombreTablaHechos;
  QString medidaActual;
  QString medidaFormula; // ej: "SUM(total_venta)"
  QString medidaTipo;    // "SUM", "COUNT", "AVG", etc.

  DimensionInfo dimensionX; // Eje horizontal derecho
  DimensionInfo dimensionY; // Eje vertical (medida)
  DimensionInfo dimensionZ; // Eje horizontal izquierdo

  double valorMinimo;
  double valorMaximo;
  int totalCeldas;    // Total posible (X * Y * Z)
  int celdasConDatos; // Celdas con valores > 0
  int totalRegistros; // Registros en tabla de hechos

  MetadataCubo()
      : valorMinimo(0), valorMaximo(0), totalCeldas(0), celdasConDatos(0),
        totalRegistros(0) {}
};

// Configuración de visualización
struct ConfigVisualizacion {
  int maxCeldasPorEje =
      10; // Aumentado para cubo más denso (10x10 = 100 celdas)
  bool mostrarEjesCompletos = true;
  bool mostrarGrid = true;
  bool mostrarPlanos = false;
  bool mostrarSombras = false; // Desactivar sombras para mejor rendimiento
  bool animarTransiciones = false;
  float opacidadPlanos = 0.08f;
  float opacidadGrid = 0.15f;

  // Colores del tema
  QColor colorEjeX = QColor("#2563eb");   // Azul
  QColor colorEjeY = QColor("#10b981");   // Verde
  QColor colorEjeZ = QColor("#f59e0b");   // Naranja
  QColor colorGrid = QColor("#cbd5e1");   // Gris claro
  QColor colorFondo1 = QColor("#f8fafc"); // Blanco azulado
  QColor colorFondo2 = QColor("#e2e8f0"); // Gris muy claro
};

// Celda del cubo con información completa
struct CeldaCubo {
  int dimX;
  int dimY;
  int dimZ;
  QString nombreDimX;
  QString nombreDimY;
  QString nombreDimZ;
  double valor;
  double valorSecundario;
  QString etiqueta;
  bool seleccionada = false;

  // Cache para interacción
  QRectF areaPantalla;
  QPolygonF poligonoTop;
  QPolygonF poligonoFrontal;
  QPolygonF poligonoLateral;
};

// ============================================================================
// CLASE PRINCIPAL: VISOR OLAP 2.5D
// ============================================================================

class VisorOlap : public QWidget {
  Q_OBJECT

public:
  explicit VisorOlap(QWidget *parent = nullptr);
  ~VisorOlap();

  // Carga de datos
  void cargarCuboDummy();
  void cargarDatos(const QVector<CeldaCubo> &celdas);
  void recargarCubo();

  // Control de selección
  void limpiarSeleccion();
  QVector<CeldaCubo> obtenerSeleccion() const;

  // Acceso a metadata
  MetadataCubo obtenerMetadata() const { return m_metadata; }
  ConfigVisualizacion obtenerConfig() const { return m_config; }
  void aplicarConfig(const ConfigVisualizacion &config);

signals:
  void celdaSeleccionada(const QString &info);
  void seleccionCambiada(int cantidad);
  void estadisticasActualizadas(int cantidad, double suma);
  void drillDown(int x, int y, int z);
  void metadataCambiada(const MetadataCubo &metadata);

protected:
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;
  void leaveEvent(QEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;

private:
  // ========== RENDERIZADO ==========
  void dibujarFondo(QPainter &painter);
  void dibujarGridIsometrico(QPainter &painter);
  void dibujarPlanosReferencia(QPainter &painter);
  void dibujarEjesConEtiquetas(QPainter &painter);
  void dibujarEjeConFlecha(QPainter &painter, int tipoEje,
                           const QString &nombre);
  void dibujarEtiquetasEje(QPainter &painter, int tipoEje,
                           const QStringList &valores);
  void dibujarEscalaEje(QPainter &painter, double minVal, double maxVal);

  void dibujarPrisma(QPainter &painter, const CeldaCubo &celda, bool hover,
                     bool seleccionada);
  void dibujarSombraPrisma(QPainter &painter, const QPointF &base, float altura,
                           float ancho);
  void dibujarCaraSuperior(QPainter &painter, const QPolygonF &poly,
                           const QColor &color);
  void dibujarCaraFrontal(QPainter &painter, const QPolygonF &poly,
                          const QBrush &brush);
  void dibujarCaraLateral(QPainter &painter, const QPolygonF &poly,
                          const QColor &color);
  void dibujarBordeDestacado(QPainter &painter, const CeldaCubo &celda,
                             const QColor &color);

  void dibujarTooltip(QPainter &painter, const CeldaCubo &celda,
                      const QPoint &pos);
  void dibujarLeyenda(QPainter &painter);
  void dibujarPanelControl(QPainter &painter);
  void dibujarInfoDimension(QPainter &painter, int x, int y,
                            const QString &titulo, const DimensionInfo &dim);

  // ========== DETECCIÓN AUTOMÁTICA ==========
  void cargarCuboReal();
  MetadataCubo detectarEsquemaOptimo();
  QStringList detectarMedidasDisponibles(const QString &tablaHechos);
  QVector<DimensionInfo>
  detectarDimensionesDisponibles(const QString &tablaHechos);
  DimensionInfo analizarDimension(const QString &tablaHechos,
                                  const QString &fkCol, const QString &dimTable,
                                  const QString &pkCol);
  DimensionInfo crearDimensionDegenerada(const QString &tablaHechos,
                                         const QString &columna);
  int calcularCardinalidad(const QString &tabla, const QString &columna);

  // ========== UTILIDADES ==========
  QPointF proyectarIso(float x, float y, float z);
  QColor obtenerColorGradiente(double valor, double min, double max);
  QColor obtenerColorPorValor(double valor);
  int detectarCeldaEnPunto(const QPoint &punto);
  void recalcularAreasPantalla();
  float calcularAltura(double valor);
  QPointF calcularPosicionBase(const CeldaCubo &celda);

  QString elegirMejorColumnaDimension(const QString &tabla);
  QStringList obtenerTopKeys(const QString &factTable, const QString &dimTable,
                             const QString &dimCol, const QString &fkCol,
                             const QString &pkCol, int limit);
  void emitirEstadisticasSeleccion();

  // ========== ESTADO ==========
  QVector<CeldaCubo> m_celdas;
  MetadataCubo m_metadata;
  ConfigVisualizacion m_config;

  // Interacción
  float m_zoom;
  QPointF m_panOffset;
  QPoint m_lastMousePos;
  bool m_arrastrando;
  int m_celdaHover;

  // Configuración visual
  double m_valorMax;
  double m_valorMin;
  float m_tamanoBase;
  float m_escalaAltura;
  float m_espaciado;

  // Enums para tipos de ejes
  enum TipoEje { EjeX = 0, EjeY = 1, EjeZ = 2 };
};

#endif // VISOROLAP_H
