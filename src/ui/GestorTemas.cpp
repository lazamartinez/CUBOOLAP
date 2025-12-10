#include "GestorTemas.h"

void GestorTemas::setTema(Tema tema) {
  if (m_tema != tema) {
    m_tema = tema;
    emit temaCambiado(tema);
  }
}

void GestorTemas::alternarTema() { setTema(m_tema == Claro ? Oscuro : Claro); }

QString GestorTemas::obtenerColorFondo() const {
  return m_tema == Claro ? "#ffffff" : "#0f172a";
}

QString GestorTemas::obtenerColorTexto() const {
  return m_tema == Claro ? "#1e293b" : "#f1f5f9";
}

QString GestorTemas::obtenerColorBorde() const {
  return m_tema == Claro ? "#e2e8f0" : "#334155";
}

QString GestorTemas::obtenerColorAcento() const { return "#2563eb"; }

QString GestorTemas::obtenerEstiloGlobal() const {
  return m_tema == Claro ? estiloClaro() : estiloOscuro();
}

QString GestorTemas::estiloClaro() const {
  return R"(
    * { font-family: 'Segoe UI', sans-serif; }
    QMainWindow, QWidget { background-color: #ffffff; color: #1e293b; }
    QLabel { color: #1e293b; background: transparent; }
    QPushButton {
      background-color: #f1f5f9; border: 1px solid #e2e8f0;
      border-radius: 6px; padding: 8px 16px; color: #334155;
    }
    QPushButton:hover { background-color: #e2e8f0; }
    QLineEdit, QSpinBox, QComboBox {
      background-color: #ffffff; border: 1px solid #e2e8f0;
      border-radius: 6px; padding: 8px 12px; color: #1e293b;
    }
    QLineEdit:focus { border: 2px solid #2563eb; }
    QProgressBar {
      border: none; border-radius: 4px; background-color: #e2e8f0;
    }
    QProgressBar::chunk { background-color: #2563eb; border-radius: 4px; }
    QTableWidget { background-color: #ffffff; gridline-color: #e2e8f0; }
    QHeaderView::section {
      background-color: #f8fafc; color: #475569; padding: 10px;
      border: none; border-bottom: 2px solid #e2e8f0;
    }
    QListWidget { background-color: #ffffff; border: 1px solid #e2e8f0; }
    QListWidget::item:hover { background-color: #f1f5f9; }
    QListWidget::item:selected { background-color: #dbeafe; color: #1e40af; }
    QGroupBox {
      background-color: #ffffff; border: 1px solid #e2e8f0;
      border-radius: 8px; margin-top: 16px; padding-top: 16px;
    }
    QMenuBar { background-color: #ffffff; border-bottom: 1px solid #e2e8f0; }
    QMenuBar::item:selected { background-color: #f1f5f9; }
    QMenu { background-color: #ffffff; border: 1px solid #e2e8f0; }
    QMenu::item:selected { background-color: #dbeafe; }
    QStatusBar { background-color: #f8fafc; border-top: 1px solid #e2e8f0; color: #64748b; }
    QScrollBar:vertical { background: #f1f5f9; width: 8px; }
    QScrollBar::handle:vertical { background: #cbd5e1; border-radius: 4px; }
    QTextEdit { background-color: #ffffff; border: 1px solid #e2e8f0; }
  )";
}

QString GestorTemas::estiloOscuro() const {
  return R"(
    * { font-family: 'Segoe UI', sans-serif; }
    QMainWindow, QWidget { background-color: #0f172a; color: #f1f5f9; }
    QLabel { color: #f1f5f9; background: transparent; }
    QPushButton {
      background-color: #1e293b; border: 1px solid #334155;
      border-radius: 6px; padding: 8px 16px; color: #e2e8f0;
    }
    QPushButton:hover { background-color: #334155; }
    QLineEdit, QSpinBox, QComboBox {
      background-color: #1e293b; border: 1px solid #334155;
      border-radius: 6px; padding: 8px 12px; color: #f1f5f9;
    }
    QLineEdit:focus { border: 2px solid #3b82f6; }
    QProgressBar {
      border: none; border-radius: 4px; background-color: #334155;
    }
    QProgressBar::chunk { background-color: #3b82f6; border-radius: 4px; }
    QTableWidget { background-color: #1e293b; gridline-color: #334155; color: #f1f5f9; }
    QHeaderView::section {
      background-color: #0f172a; color: #94a3b8; padding: 10px;
      border: none; border-bottom: 2px solid #334155;
    }
    QListWidget { background-color: #1e293b; border: 1px solid #334155; color: #f1f5f9; }
    QListWidget::item:hover { background-color: #334155; }
    QListWidget::item:selected { background-color: #1e40af; color: #ffffff; }
    QGroupBox {
      background-color: #1e293b; border: 1px solid #334155;
      border-radius: 8px; margin-top: 16px; padding-top: 16px; color: #f1f5f9;
    }
    QGroupBox::title { color: #94a3b8; }
    QMenuBar { background-color: #0f172a; border-bottom: 1px solid #334155; color: #f1f5f9; }
    QMenuBar::item:selected { background-color: #1e293b; }
    QMenu { background-color: #1e293b; border: 1px solid #334155; color: #f1f5f9; }
    QMenu::item:selected { background-color: #1e40af; }
    QStatusBar { background-color: #0f172a; border-top: 1px solid #334155; color: #94a3b8; }
    QScrollBar:vertical { background: #1e293b; width: 8px; }
    QScrollBar::handle:vertical { background: #475569; border-radius: 4px; }
    QTextEdit { background-color: #1e293b; border: 1px solid #334155; color: #f1f5f9; }
    QTreeWidget { background-color: #1e293b; border: 1px solid #334155; color: #f1f5f9; }
  )";
}
