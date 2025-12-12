#include "FlutterTheme.h"
#include <QDebug>

FlutterTheme &FlutterTheme::instance() {
  static FlutterTheme instance;
  return instance;
}

FlutterTheme::FlutterTheme()
    : m_darkMode(true) { // Default to dark mode for "cyberpunk/modern" feel
  loadStylesheet();
  applyTheme(); // Apply initial theme
}

void FlutterTheme::setDarkMode(bool enabled) {
  if (m_darkMode != enabled) {
    m_darkMode = enabled;
    applyTheme();
    emit themeChanged();
  }
}

void FlutterTheme::applyThemeToWidget(QWidget *widget) {
  if (!widget)
    return;

  widget->setProperty("theme", m_darkMode ? "dark" : "light");
  widget->style()->unpolish(widget);
  widget->style()->polish(widget);

  // Aplicar a hijos recursivamente
  for (auto child : widget->findChildren<QWidget *>()) {
    child->setProperty("theme", m_darkMode ? "dark" : "light");
    child->style()->unpolish(child);
    child->style()->polish(child);
  }
}

void FlutterTheme::loadStylesheet() {
  QFile file(":/styles/flutter_style.qss");
  if (file.open(QFile::ReadOnly)) {
    QString styleSheet = QLatin1String(file.readAll());
    qApp->setStyleSheet(styleSheet);
  } else {
    // Fallback: try loading from relative path if resource not found (during
    // dev)
    QFile localFile("src/ui/styles/flutter_style.qss");
    if (localFile.open(QFile::ReadOnly)) {
      QString styleSheet = QLatin1String(localFile.readAll());
      qApp->setStyleSheet(styleSheet);
    } else {
      qWarning() << "Could not load flutter_style.qss";
    }
  }
}

void FlutterTheme::applyTheme() {
  // Actualizar paleta de la aplicación
  QPalette palette;

  if (m_darkMode) {
    palette.setColor(QPalette::Window, QColor(18, 18, 18));
    palette.setColor(QPalette::WindowText, QColor(255, 255, 255, 222));
    palette.setColor(QPalette::Base, QColor(28, 27, 31));
    palette.setColor(QPalette::AlternateBase, QColor(35, 35, 35));
    palette.setColor(QPalette::ToolTipBase, QColor(40, 40, 40));
    palette.setColor(QPalette::ToolTipText, QColor(255, 255, 255));
    palette.setColor(QPalette::Text, QColor(255, 255, 255, 222));
    palette.setColor(QPalette::Button, QColor(55, 55, 55));
    palette.setColor(QPalette::ButtonText, QColor(255, 255, 255));
    palette.setColor(QPalette::BrightText, QColor(255, 255, 255));
    palette.setColor(QPalette::Highlight, QColor(103, 80, 164));
    palette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
  } else {
    palette.setColor(QPalette::Window, QColor(248, 249, 250));
    palette.setColor(QPalette::WindowText, QColor(0, 0, 0, 222));
    palette.setColor(QPalette::Base, QColor(255, 255, 255));
    palette.setColor(QPalette::AlternateBase, QColor(245, 245, 245));
    palette.setColor(QPalette::ToolTipBase, QColor(255, 255, 255));
    palette.setColor(QPalette::ToolTipText, QColor(0, 0, 0));
    palette.setColor(QPalette::Text, QColor(0, 0, 0, 222));
    palette.setColor(QPalette::Button, QColor(240, 240, 240));
    palette.setColor(QPalette::ButtonText, QColor(0, 0, 0));
    palette.setColor(QPalette::BrightText, QColor(255, 255, 255));
    palette.setColor(QPalette::Highlight, QColor(103, 80, 164));
    palette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
  }

  qApp->setPalette(palette);

  // Also re-apply to Top Level Widgets to force update
  for (QWidget *widget : QApplication::topLevelWidgets()) {
    applyThemeToWidget(widget);
  }
}
