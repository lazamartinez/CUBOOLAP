#ifndef PANELOPERACIONESOLAP_H
#define PANELOPERACIONESOLAP_H

#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

// Panel con botones para las operaciones OLAP clasicas
class PanelOperacionesOlap : public QWidget {
    Q_OBJECT
public:
    explicit PanelOperacionesOlap(QWidget *parent = nullptr);
    
    void setDimensiones(const QStringList &dimensiones);
    void setNivelActual(const QString &nivel);
    void setFiltrosActivos(const QStringList &filtros);

signals:
    void drillDown();
    void rollUp();
    void slice();
    void dice();
    void pivot();
    void ranking();
    void drillThrough();
    void resetView();

private:
    void configurarUi();
    void aplicarEstilos();
    
    QPushButton *btnDrillDown;
    QPushButton *btnRollUp;
    QPushButton *btnSlice;
    QPushButton *btnDice;
    QPushButton *btnPivot;
    QPushButton *btnRanking;
    QPushButton *btnDrillThrough;
    QPushButton *btnReset;
    
    QLabel *lblNivel;
    QLabel *lblFiltros;
};

#endif // PANELOPERACIONESOLAP_H
