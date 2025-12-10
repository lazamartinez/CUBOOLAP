#ifndef ESTILOS_H
#define ESTILOS_H

#include <QString>

/**
 * @brief Clase estatica con estilos CSS modernos para la aplicacion
 * Tema: Glassmorphism - Tonos claros con transparencias y buen contraste
 */
class Estilos {
public:
  // ========================================================================
  // PALETA DE COLORES PRINCIPAL - GLASSMORPHISM MODERNO
  // ========================================================================
  static constexpr const char *COLOR_FONDO_PRINCIPAL = "#f0f4f8";
  static constexpr const char *COLOR_FONDO_PANEL = "rgba(255, 255, 255, 0.7)";
  static constexpr const char *COLOR_FONDO_CARD = "rgba(255, 255, 255, 0.85)";
  static constexpr const char *COLOR_BORDE = "rgba(200, 210, 220, 0.5)";
  static constexpr const char *COLOR_TEXTO = "#1a1a2e";
  static constexpr const char *COLOR_TEXTO_SECUNDARIO = "#5a6a7a";

  // Colores de Acento
  static constexpr const char *ACENTO_PRIMARIO = "#2563eb"; // Azul moderno
  static constexpr const char *ACENTO_EXITO = "#10b981";    // Verde esmeralda
  static constexpr const char *ACENTO_ADVERTENCIA = "#f59e0b"; // Ambar
  static constexpr const char *ACENTO_PELIGRO = "#ef4444";     // Rojo
  static constexpr const char *ACENTO_INFO = "#6366f1";        // Indigo

  // ========================================================================
  // ESTILO GLOBAL DE LA APLICACION - GLASSMORPHISM
  // ========================================================================
  static QString obtenerEstiloGlobal() {
    return R"(
            /* ============== VENTANA PRINCIPAL ============== */
            QMainWindow {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                    stop:0 #e0e7ff, stop:0.5 #f0f4f8, stop:1 #e8f0fe);
                color: #1a1a2e;
            }
            
            QWidget {
                background-color: transparent;
                color: #1a1a2e;
                font-family: 'Segoe UI', 'Inter', 'Roboto', sans-serif;
                font-size: 13px;
            }
            
            /* ============== BOTONES ============== */
            QPushButton {
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                    stop:0 #ffffff, stop:1 #f8fafc);
                color: #374151;
                border: 1px solid #d1d5db;
                border-radius: 8px;
                padding: 10px 20px;
                font-weight: 600;
                font-size: 13px;
                min-height: 20px;
            }
            
            QPushButton:hover {
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                    stop:0 #f8fafc, stop:1 #f1f5f9);
                border: 1px solid #2563eb;
                color: #2563eb;
            }
            
            QPushButton:pressed {
                background: #2563eb;
                color: white;
                border: 1px solid #1d4ed8;
            }
            
            QPushButton:disabled {
                background: #f3f4f6;
                border: 1px solid #e5e7eb;
                color: #9ca3af;
            }
            
            /* Boton Primario (azul) */
            QPushButton[class="primary"], QPushButton#btnPrimario {
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                    stop:0 #3b82f6, stop:1 #2563eb);
                border: none;
                color: white;
            }
            
            QPushButton[class="primary"]:hover, QPushButton#btnPrimario:hover {
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                    stop:0 #2563eb, stop:1 #1d4ed8);
            }
            
            /* Boton Exito (verde) */
            QPushButton[class="success"], QPushButton#btnExito {
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                    stop:0 #10b981, stop:1 #059669);
                border: none;
                color: white;
            }
            
            QPushButton[class="success"]:hover, QPushButton#btnExito:hover {
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                    stop:0 #059669, stop:1 #047857);
            }
            
            /* ============== INPUTS ============== */
            QLineEdit, QSpinBox, QDoubleSpinBox, QComboBox {
                background-color: rgba(255, 255, 255, 0.9);
                border: 1px solid #d1d5db;
                border-radius: 6px;
                padding: 10px 14px;
                color: #1f2937;
                selection-background-color: #2563eb;
                selection-color: white;
            }
            
            QLineEdit:focus, QSpinBox:focus, QComboBox:focus {
                border: 2px solid #2563eb;
                background-color: white;
            }
            
            QLineEdit:disabled {
                background-color: #f9fafb;
                color: #9ca3af;
            }
            
            /* ============== LABELS ============== */
            QLabel {
                color: #374151;
                background: transparent;
            }
            
            QLabel[class="titulo"] {
                font-size: 24px;
                font-weight: 700;
                color: #1f2937;
            }
            
            QLabel[class="subtitulo"] {
                font-size: 16px;
                font-weight: 600;
                color: #4b5563;
            }
            
            /* ============== PROGRESS BAR ============== */
            QProgressBar {
                border: none;
                border-radius: 10px;
                background-color: #e5e7eb;
                text-align: center;
                color: #374151;
                font-weight: 600;
                min-height: 20px;
            }
            
            QProgressBar::chunk {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                    stop:0 #2563eb, stop:1 #6366f1);
                border-radius: 10px;
            }
            
            /* ============== SCROLL BARS ============== */
            QScrollBar:vertical {
                background: #f3f4f6;
                width: 10px;
                border-radius: 5px;
                margin: 0;
            }
            
            QScrollBar::handle:vertical {
                background: #cbd5e1;
                border-radius: 5px;
                min-height: 30px;
                margin: 2px;
            }
            
            QScrollBar::handle:vertical:hover {
                background: #94a3b8;
            }
            
            QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
                height: 0;
            }
            
            QScrollBar:horizontal {
                background: #f3f4f6;
                height: 10px;
                border-radius: 5px;
            }
            
            QScrollBar::handle:horizontal {
                background: #cbd5e1;
                border-radius: 5px;
                min-width: 30px;
                margin: 2px;
            }
            
            /* ============== TABLES ============== */
            QTableWidget, QTableView {
                background-color: rgba(255, 255, 255, 0.95);
                alternate-background-color: rgba(248, 250, 252, 0.9);
                border: 1px solid #e5e7eb;
                border-radius: 8px;
                gridline-color: #f1f5f9;
                selection-background-color: #dbeafe;
                selection-color: #1e40af;
            }
            
            QTableWidget::item, QTableView::item {
                padding: 10px;
                border-bottom: 1px solid #f1f5f9;
            }
            
            QHeaderView::section {
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                    stop:0 #f8fafc, stop:1 #f1f5f9);
                color: #374151;
                padding: 12px;
                border: none;
                border-bottom: 2px solid #2563eb;
                font-weight: 600;
            }
            
            /* ============== TREE WIDGET ============== */
            QTreeWidget, QTreeView {
                background-color: rgba(255, 255, 255, 0.95);
                border: 1px solid #e5e7eb;
                border-radius: 8px;
                alternate-background-color: #fafbfc;
                selection-background-color: #dbeafe;
                selection-color: #1e40af;
            }
            
            QTreeWidget::item, QTreeView::item {
                padding: 8px;
                border-radius: 4px;
            }
            
            QTreeWidget::item:hover, QTreeView::item:hover {
                background-color: #f1f5f9;
            }
            
            /* ============== LIST WIDGET ============== */
            QListWidget {
                background-color: rgba(255, 255, 255, 0.95);
                border: 1px solid #e5e7eb;
                border-radius: 8px;
            }
            
            QListWidget::item {
                padding: 12px;
                border-bottom: 1px solid #f1f5f9;
            }
            
            QListWidget::item:selected {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                    stop:0 #dbeafe, stop:1 #e0e7ff);
                color: #1e40af;
                border-radius: 6px;
            }
            
            QListWidget::item:hover:!selected {
                background-color: #f8fafc;
            }
            
            /* ============== TEXT EDIT ============== */
            QTextEdit, QPlainTextEdit {
                background-color: rgba(255, 255, 255, 0.95);
                border: 1px solid #e5e7eb;
                border-radius: 8px;
                color: #1f2937;
                font-family: 'Consolas', 'Fira Code', 'Monaco', monospace;
                font-size: 12px;
                padding: 10px;
            }
            
            /* ============== TAB WIDGET ============== */
            QTabWidget::pane {
                border: 1px solid #e5e7eb;
                background-color: rgba(255, 255, 255, 0.85);
                border-radius: 8px;
            }
            
            QTabBar::tab {
                background: rgba(255, 255, 255, 0.7);
                color: #6b7280;
                padding: 12px 24px;
                border: 1px solid #e5e7eb;
                border-bottom: none;
                border-top-left-radius: 8px;
                border-top-right-radius: 8px;
                margin-right: 2px;
            }
            
            QTabBar::tab:selected {
                background: white;
                color: #2563eb;
                border-bottom: 2px solid #2563eb;
                font-weight: 600;
            }
            
            QTabBar::tab:hover:!selected {
                background: #f8fafc;
                color: #374151;
            }
            
            /* ============== GROUP BOX ============== */
            QGroupBox {
                border: 1px solid #e5e7eb;
                border-radius: 12px;
                margin-top: 20px;
                padding-top: 20px;
                background-color: rgba(255, 255, 255, 0.8);
            }
            
            QGroupBox::title {
                subcontrol-origin: margin;
                subcontrol-position: top left;
                padding: 6px 16px;
                background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                    stop:0 #2563eb, stop:1 #6366f1);
                color: white;
                border-radius: 6px;
                font-weight: 600;
            }
            
            /* ============== SPLITTER ============== */
            QSplitter::handle {
                background: #e5e7eb;
            }
            
            QSplitter::handle:horizontal {
                width: 2px;
            }
            
            QSplitter::handle:vertical {
                height: 2px;
            }
            
            QSplitter::handle:hover {
                background: #2563eb;
            }
            
            /* ============== MENU BAR ============== */
            QMenuBar {
                background-color: rgba(255, 255, 255, 0.95);
                color: #374151;
                border-bottom: 1px solid #e5e7eb;
                padding: 4px;
            }
            
            QMenuBar::item:selected {
                background-color: #dbeafe;
                color: #1e40af;
                border-radius: 4px;
            }
            
            QMenu {
                background-color: white;
                border: 1px solid #e5e7eb;
                border-radius: 8px;
                padding: 4px;
            }
            
            QMenu::item {
                padding: 10px 24px;
                border-radius: 4px;
            }
            
            QMenu::item:selected {
                background-color: #dbeafe;
                color: #1e40af;
            }
            
            /* ============== STATUS BAR ============== */
            QStatusBar {
                background: rgba(255, 255, 255, 0.9);
                color: #4b5563;
                border-top: 1px solid #e5e7eb;
                padding: 6px;
            }
            
            /* ============== DIALOG ============== */
            QDialog {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                    stop:0 #f0f4f8, stop:1 #e8f0fe);
                border: 1px solid #d1d5db;
                border-radius: 16px;
            }
            
            /* ============== TOOLTIPS ============== */
            QToolTip {
                background-color: #1f2937;
                color: white;
                border: none;
                border-radius: 6px;
                padding: 8px 12px;
                font-size: 12px;
            }
            
            /* ============== CHECKBOX & RADIO ============== */
            QCheckBox, QRadioButton {
                color: #374151;
                spacing: 8px;
            }
            
            QCheckBox::indicator, QRadioButton::indicator {
                width: 20px;
                height: 20px;
                border: 2px solid #d1d5db;
                border-radius: 4px;
                background-color: white;
            }
            
            QCheckBox::indicator:checked {
                background-color: #2563eb;
                border-color: #2563eb;
            }
            
            QRadioButton::indicator {
                border-radius: 10px;
            }
            
            QRadioButton::indicator:checked {
                background-color: #2563eb;
                border-color: #2563eb;
            }
        )";
  }

  // ========================================================================
  // ESTILOS ESPECIFICOS PARA COMPONENTES - GLASSMORPHISM
  // ========================================================================
  static QString obtenerEstiloCardGlass() {
    return R"(
            background: rgba(255, 255, 255, 0.8);
            backdrop-filter: blur(10px);
            border: 1px solid rgba(255, 255, 255, 0.5);
            border-radius: 16px;
            padding: 20px;
        )";
  }

  static QString obtenerEstiloTituloSeccion(const QString &color = "#2563eb") {
    return QString(R"(
            font-size: 22px;
            font-weight: 700;
            color: %1;
            padding: 12px 0;
            border-bottom: 3px solid %1;
            margin-bottom: 16px;
        )")
        .arg(color);
  }

  static QString obtenerEstiloBotonPrimario() {
    return R"(
            QPushButton {
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                    stop:0 #3b82f6, stop:1 #2563eb);
                border: none;
                border-radius: 10px;
                color: white;
                font-weight: 600;
                padding: 14px 28px;
                font-size: 14px;
            }
            QPushButton:hover {
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                    stop:0 #2563eb, stop:1 #1d4ed8);
            }
            QPushButton:pressed {
                background: #1d4ed8;
            }
        )";
  }

  static QString obtenerEstiloBotonExito() {
    return R"(
            QPushButton {
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                    stop:0 #10b981, stop:1 #059669);
                border: none;
                border-radius: 10px;
                color: white;
                font-weight: 600;
                padding: 14px 28px;
                font-size: 14px;
            }
            QPushButton:hover {
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                    stop:0 #059669, stop:1 #047857);
            }
            QPushButton:pressed {
                background: #047857;
            }
        )";
  }

  static QString obtenerEstiloPanelGlass() {
    return R"(
            background: rgba(255, 255, 255, 0.85);
            border: 1px solid rgba(200, 210, 220, 0.5);
            border-radius: 12px;
            padding: 16px;
        )";
  }

  static QString obtenerEstiloDropZone() {
    return R"(
            QFrame {
                background: rgba(255, 255, 255, 0.6);
                border: 2px dashed #94a3b8;
                border-radius: 12px;
                min-height: 100px;
            }
            QFrame:hover {
                background: rgba(219, 234, 254, 0.6);
                border-color: #2563eb;
            }
        )";
  }
};

#endif // ESTILOS_H
