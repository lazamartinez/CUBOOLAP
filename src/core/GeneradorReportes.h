#ifndef GENERADORREPORTES_H
#define GENERADORREPORTES_H

#include <QString>

class GeneradorReportes {
public:
  static void generarPDF(const QString &ruta);
  static void generarExcel(const QString &ruta);
};

#endif // GENERADORREPORTES_H
