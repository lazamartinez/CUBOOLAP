#ifndef ANALIZADORESQUEMA_H
#define ANALIZADORESQUEMA_H

#include <QMap>
#include <QObject>
#include <QVariantMap>
#include <QVector>

/**
 * @brief Información de una tabla detectada en el esquema
 */
struct InfoTabla {
  QString nombre;
  long long filaEstimada = 0;
  QString tamano;
  long long tamanoBytes = 0;
  int numColumnas = 0;
  QString descripcion;
};

/**
 * @brief Información de una relación Foreign Key
 */
struct InfoRelacionFK {
  QString tablaOrigen;
  QString columnaOrigen;
  QString tablaDestino;
  QString columnaDestino;
  QString nombreConstraint;
};

/**
 * @brief Analizador inteligente de esquemas PostgreSQL
 *
 * Realiza análisis automático de la estructura de la base de datos,
 * detectando tablas, relaciones, tipos de datos, y sugiriendo
 * posibles tablas de hechos y dimensiones para OLAP.
 */
class AnalizadorEsquema : public QObject {
  Q_OBJECT

public:
  explicit AnalizadorEsquema(QObject *parent = nullptr);

  /// Ejecuta el análisis completo del esquema
  void analizar();

  /// Obtiene las tablas detectadas
  QVector<InfoTabla> obtenerTablas() const;

  /// Obtiene las relaciones FK detectadas
  QVector<InfoRelacionFK> obtenerRelaciones() const;

  /// Obtiene el mapa de tipos de datos y sus frecuencias
  QMap<QString, int> obtenerTiposDatos() const;

  /// Obtiene estadísticas generales del análisis
  QVariantMap obtenerEstadisticasGeneral() const;

signals:
  /// Emitida cuando el análisis se completa
  void analisisCompletado();

  /// Emitida durante el progreso del análisis
  void progreso(int porcentaje, QString mensaje);

private:
  QVector<InfoTabla> m_tablas;
  QVector<InfoRelacionFK> m_relacionesFK;
  QMap<QString, int> m_tiposDatos;
  QVariantMap m_estadisticas;

  void analizarTablas();
  void analizarTiposDatos();
  void detectarRelaciones();
  void calcularEstadisticas();
};

#endif // ANALIZADORESQUEMA_H
