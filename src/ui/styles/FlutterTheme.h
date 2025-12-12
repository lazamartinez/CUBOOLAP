// flutter_theme.h
#ifndef FLUTTER_THEME_H
#define FLUTTER_THEME_H

#include <QApplication>
#include <QFile>
#include <QObject>
#include <QPalette>
#include <QString>
#include <QStyle>
#include <QWidget>


class FlutterTheme : public QObject {
  Q_OBJECT
  Q_PROPERTY(bool darkMode READ darkMode WRITE setDarkMode NOTIFY themeChanged)

public:
  static FlutterTheme &instance();

  bool darkMode() const { return m_darkMode; }

  void setDarkMode(bool enabled);

  void toggleTheme() { setDarkMode(!m_darkMode); }

  void applyThemeToWidget(QWidget *widget);

signals:
  void themeChanged();

private:
  FlutterTheme(); // Constructor is private for singleton

  void loadStylesheet();
  void applyTheme();

  bool m_darkMode;
};

#endif // FLUTTER_THEME_H
