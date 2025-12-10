#ifndef INDICADORFASE_H
#define INDICADORFASE_H

#include <QPaintEvent>
#include <QPainter>
#include <QStringList>
#include <QWidget>


/**
 * @brief Indicador visual de progreso entre fases
 *
 * Muestra las 5 fases del sistema con la fase actual resaltada.
 */
class IndicadorFase : public QWidget {
  Q_OBJECT

public:
  explicit IndicadorFase(QWidget *parent = nullptr);

  void setFaseActual(int fase);
  int faseActual() const { return m_faseActual; }

protected:
  void paintEvent(QPaintEvent *event) override;

private:
  int m_faseActual = 0; // 0 = intro, 1-5 = fases
  QStringList m_nombresFases = {"Diagnostico", "Modelado", "Carga", "Explorar",
                                "Reportes"};
};

#endif // INDICADORFASE_H
