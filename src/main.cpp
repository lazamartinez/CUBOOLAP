#include "ui/VentanaPrincipal.h"
#include <QApplication>
#include <QFile>
#include <QMainWindow>

int main(int argc, char *argv[]) {
  QApplication aplicacion(argc, argv);

  // Configuración de metadatos de la aplicación
  aplicacion.setApplicationName("Cubo Visión");
  aplicacion.setApplicationVersion("1.0.0");
  aplicacion.setOrganizationName("OLAP 2025");

  // Estilo base (se puede mejorar luego con QSS)
  aplicacion.setStyle("Fusion");

  // Inicializar y mostrar la ventana principal
  // La conexión se manejará dentro de la ventana principal o un diálogo previo
  VentanaPrincipal ventana;
  ventana.showMaximized();

  return aplicacion.exec();
}
