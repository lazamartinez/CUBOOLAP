#ifndef GESTORBASEDATOS_H
#define GESTORBASEDATOS_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QString>

class GestorBaseDatos : public QObject {
  Q_OBJECT

public:
  static GestorBaseDatos &instancia();

  // Prohíbe copia y asignación
  GestorBaseDatos(const GestorBaseDatos &) = delete;
  void operator=(const GestorBaseDatos &) = delete;

  bool conectar(const QString &host, int puerto, const QString &usuario,
                const QString &password, const QString &baseDatos,
                QString &errorMsg);
  bool estaConectado() const;
  QSqlDatabase baseDatos() const;

private:
  explicit GestorBaseDatos(QObject *parent = nullptr);
  ~GestorBaseDatos();

  QSqlDatabase m_db;
};

#endif // GESTORBASEDATOS_H
