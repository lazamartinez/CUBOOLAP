#ifndef GESTORTEMAS_H
#define GESTORTEMAS_H

#include <QObject>
#include <QString>

/**
 * @brief Gestor de temas para modo oscuro/claro
 *
 * Singleton que maneja el tema actual de la aplicacion
 * y proporciona estilos CSS correspondientes.
 */
class GestorTemas : public QObject {
  Q_OBJECT

public:
  enum Tema { Claro, Oscuro };

  static GestorTemas &instancia() {
    static GestorTemas instance;
    return instance;
  }

  Tema temaActual() const { return m_tema; }
  bool esModoOscuro() const { return m_tema == Oscuro; }

  void setTema(Tema tema);
  void alternarTema();

  QString obtenerEstiloGlobal() const;
  QString obtenerColorFondo() const;
  QString obtenerColorTexto() const;
  QString obtenerColorBorde() const;
  QString obtenerColorAcento() const;

signals:
  void temaCambiado(Tema nuevoTema);

private:
  GestorTemas() : m_tema(Claro) {}
  Tema m_tema;

  QString estiloClaro() const;
  QString estiloOscuro() const;
};

#endif // GESTORTEMAS_H
