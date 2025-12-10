#ifndef ESTILOS_H
#define ESTILOS_H

#include <QString>

/**
 * @brief Sistema de estilos moderno con fondo blanco limpio
 *
 * Paleta de colores consistente con buen contraste.
 */
class Estilos {
public:
  // Colores principales
  static constexpr const char *COLOR_FONDO = "#ffffff";
  static constexpr const char *COLOR_FONDO_SECUNDARIO = "#f8fafc";
  static constexpr const char *COLOR_BORDE = "#e2e8f0";
  static constexpr const char *COLOR_TEXTO = "#1e293b";
  static constexpr const char *COLOR_TEXTO_SECUNDARIO = "#64748b";

  // Acentos
  static constexpr const char *ACENTO_PRIMARIO = "#2563eb";
  static constexpr const char *ACENTO_EXITO = "#10b981";
  static constexpr const char *ACENTO_ALERTA = "#f59e0b";
  static constexpr const char *ACENTO_ERROR = "#ef4444";

  static QString obtenerEstiloGlobal() {
    return R"(
      * {
        font-family: 'Segoe UI', 'SF Pro Display', -apple-system, sans-serif;
      }
      
      QMainWindow, QWidget {
        background-color: #ffffff;
        color: #1e293b;
      }
      
      QLabel {
        color: #1e293b;
        background: transparent;
      }
      
      QPushButton {
        background-color: #f1f5f9;
        border: 1px solid #e2e8f0;
        border-radius: 6px;
        padding: 8px 16px;
        color: #334155;
        font-weight: 500;
      }
      
      QPushButton:hover {
        background-color: #e2e8f0;
        border-color: #cbd5e1;
      }
      
      QPushButton:pressed {
        background-color: #cbd5e1;
      }
      
      QLineEdit, QSpinBox, QComboBox {
        background-color: #ffffff;
        border: 1px solid #e2e8f0;
        border-radius: 6px;
        padding: 8px 12px;
        color: #1e293b;
        selection-background-color: #2563eb;
      }
      
      QLineEdit:focus, QSpinBox:focus, QComboBox:focus {
        border: 2px solid #2563eb;
        outline: none;
      }
      
      QProgressBar {
        border: none;
        border-radius: 4px;
        background-color: #e2e8f0;
        text-align: center;
        color: #475569;
      }
      
      QProgressBar::chunk {
        background-color: #2563eb;
        border-radius: 4px;
      }
      
      QScrollBar:vertical {
        background: #f1f5f9;
        width: 8px;
        border-radius: 4px;
      }
      
      QScrollBar::handle:vertical {
        background: #cbd5e1;
        border-radius: 4px;
        min-height: 30px;
      }
      
      QScrollBar::handle:vertical:hover {
        background: #94a3b8;
      }
      
      QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
        background: transparent;
      }
      
      QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
        height: 0;
      }
      
      QTableWidget {
        background-color: #ffffff;
        gridline-color: #e2e8f0;
        border: 1px solid #e2e8f0;
        border-radius: 6px;
      }
      
      QTableWidget::item {
        padding: 8px;
        color: #1e293b;
      }
      
      QTableWidget::item:selected {
        background-color: #dbeafe;
        color: #1e40af;
      }
      
      QHeaderView::section {
        background-color: #f8fafc;
        color: #475569;
        padding: 10px;
        border: none;
        border-bottom: 2px solid #e2e8f0;
        font-weight: 600;
      }
      
      QListWidget {
        background-color: #ffffff;
        border: 1px solid #e2e8f0;
        border-radius: 6px;
        color: #1e293b;
      }
      
      QListWidget::item {
        padding: 8px;
        border-radius: 4px;
      }
      
      QListWidget::item:hover {
        background-color: #f1f5f9;
      }
      
      QListWidget::item:selected {
        background-color: #dbeafe;
        color: #1e40af;
      }
      
      QTreeWidget {
        background-color: #ffffff;
        border: 1px solid #e2e8f0;
        border-radius: 6px;
        color: #1e293b;
      }
      
      QTreeWidget::item {
        padding: 6px;
      }
      
      QTreeWidget::item:hover {
        background-color: #f1f5f9;
      }
      
      QTreeWidget::item:selected {
        background-color: #dbeafe;
        color: #1e40af;
      }
      
      QTextEdit {
        background-color: #ffffff;
        border: 1px solid #e2e8f0;
        border-radius: 6px;
        color: #1e293b;
        padding: 8px;
      }
      
      QGroupBox {
        background-color: #ffffff;
        border: 1px solid #e2e8f0;
        border-radius: 8px;
        margin-top: 16px;
        padding-top: 16px;
        font-weight: 600;
        color: #374151;
      }
      
      QGroupBox::title {
        subcontrol-origin: margin;
        subcontrol-position: top left;
        padding: 4px 12px;
        background-color: #f1f5f9;
        border-radius: 4px;
        color: #374151;
      }
      
      QSplitter::handle {
        background-color: #e2e8f0;
      }
      
      QSplitter::handle:horizontal {
        width: 2px;
      }
      
      QSplitter::handle:vertical {
        height: 2px;
      }
      
      QTabWidget::pane {
        border: 1px solid #e2e8f0;
        border-radius: 6px;
        background: #ffffff;
      }
      
      QTabBar::tab {
        background: #f1f5f9;
        border: 1px solid #e2e8f0;
        padding: 8px 16px;
        margin-right: 2px;
        border-top-left-radius: 6px;
        border-top-right-radius: 6px;
        color: #64748b;
      }
      
      QTabBar::tab:selected {
        background: #ffffff;
        border-bottom-color: #ffffff;
        color: #2563eb;
        font-weight: 600;
      }
      
      QMenuBar {
        background-color: #ffffff;
        border-bottom: 1px solid #e2e8f0;
        padding: 4px;
        color: #374151;
      }
      
      QMenuBar::item {
        padding: 6px 12px;
        border-radius: 4px;
      }
      
      QMenuBar::item:selected {
        background-color: #f1f5f9;
      }
      
      QMenu {
        background-color: #ffffff;
        border: 1px solid #e2e8f0;
        border-radius: 8px;
        padding: 4px;
      }
      
      QMenu::item {
        padding: 8px 24px;
        border-radius: 4px;
        color: #374151;
      }
      
      QMenu::item:selected {
        background-color: #dbeafe;
        color: #1e40af;
      }
      
      QStatusBar {
        background-color: #f8fafc;
        border-top: 1px solid #e2e8f0;
        color: #64748b;
        padding: 4px 12px;
      }
      
      QToolTip {
        background-color: #1e293b;
        color: #ffffff;
        border: none;
        border-radius: 6px;
        padding: 8px 12px;
      }
    )";
  }

  static QString obtenerEstiloTituloSeccion(const QString &color = "#2563eb") {
    return QString(R"(
      font-size: 16px;
      font-weight: 700;
      color: %1;
      padding-bottom: 8px;
      border-bottom: 2px solid %1;
      background: transparent;
    )")
        .arg(color);
  }

  static QString obtenerEstiloBotonPrimario() {
    return R"(
      QPushButton {
        background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
          stop:0 #3b82f6, stop:1 #2563eb);
        border: none;
        border-radius: 8px;
        color: white;
        font-weight: 600;
        padding: 10px 24px;
      }
      QPushButton:hover {
        background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
          stop:0 #2563eb, stop:1 #1d4ed8);
      }
      QPushButton:pressed {
        background: #1e40af;
      }
    )";
  }

  static QString obtenerEstiloBotonExito() {
    return R"(
      QPushButton {
        background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
          stop:0 #22c55e, stop:1 #16a34a);
        border: none;
        border-radius: 8px;
        color: white;
        font-weight: 600;
        padding: 10px 24px;
      }
      QPushButton:hover {
        background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
          stop:0 #16a34a, stop:1 #15803d);
      }
    )";
  }

  static QString obtenerEstiloPanelGlass() {
    return R"(
      background: #ffffff;
      border: 1px solid #e2e8f0;
      border-radius: 8px;
    )";
  }

  static QString obtenerEstiloCardGlass() {
    return R"(
      background: #ffffff;
      border: 1px solid #e2e8f0;
      border-radius: 8px;
      padding: 12px;
    )";
  }

  static QString obtenerEstiloDropZone() {
    return R"(
      background: #f8fafc;
      border: 2px dashed #cbd5e1;
      border-radius: 8px;
    )";
  }
};

#endif // ESTILOS_H
