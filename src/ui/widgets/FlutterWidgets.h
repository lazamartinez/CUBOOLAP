// flutter_widgets.h
#ifndef FLUTTER_WIDGETS_H
#define FLUTTER_WIDGETS_H

#include "../icons/MaterialIcons.h"
#include "../styles/FlutterTheme.h"
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QStyle>
#include <QVBoxLayout>
#include <QWidget>

class FlutterCard : public QFrame {
  Q_OBJECT
  Q_PROPERTY(int elevation READ elevation WRITE setElevation)

public:
  FlutterCard(QWidget *parent = nullptr, int elevation = 1)
      : QFrame(parent), m_elevation(elevation) {
    setFrameShape(QFrame::StyledPanel);
    setProperty("frame-type", "card");
    setElevation(elevation);

    // Ensure theme is applied
    FlutterTheme::instance().applyThemeToWidget(this);
  }

  int elevation() const { return m_elevation; }
  void setElevation(int elevation) {
    m_elevation = elevation;
    setProperty("card-elevation", QString::number(elevation));
    style()->unpolish(this);
    style()->polish(this);
  }

private:
  int m_elevation;
};

class FlutterElevatedButton : public QPushButton {
public:
  FlutterElevatedButton(const QString &text, QWidget *parent = nullptr)
      : QPushButton(text, parent) {
    setProperty("button-type", "elevated");
    setCursor(Qt::PointingHandCursor);
    FlutterTheme::instance().applyThemeToWidget(this);
  }
};

class FlutterTextButton : public QPushButton {
public:
  FlutterTextButton(const QString &text, QWidget *parent = nullptr)
      : QPushButton(text, parent) {
    setProperty("button-type", "text");
    setCursor(Qt::PointingHandCursor);
    FlutterTheme::instance().applyThemeToWidget(this);
  }
};

class FlutterFilledButton : public QPushButton {
public:
  FlutterFilledButton(const QString &text, QWidget *parent = nullptr)
      : QPushButton(text, parent) {
    setProperty("button-type", "filled");
    setCursor(Qt::PointingHandCursor);
    FlutterTheme::instance().applyThemeToWidget(this);
  }
};

class FlutterIconButton : public QPushButton {
  Q_OBJECT
public:
  FlutterIconButton(const QString &iconName, QWidget *parent = nullptr)
      : QPushButton(parent) {
    setIcon(MaterialIcons::instance().getIcon(iconName));
    setProperty("button-type",
                "text"); // Reuse text button styling for transparent background
    setCursor(Qt::PointingHandCursor);
    setFixedSize(40, 40);
    FlutterTheme::instance().applyThemeToWidget(this);
  }
};

class FlutterTextField : public QWidget {
  Q_OBJECT
public:
  FlutterTextField(const QString &placeholder = "", QWidget *parent = nullptr)
      : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    // Label/Placeholder handling could be improved to be floating
    // For now, simpler implementation
    if (!placeholder.isEmpty()) {
      labelWidget = new QLabel(placeholder, this);
      labelWidget->setProperty("floating-label", "true");
      layout->addWidget(labelWidget);
    }

    lineEdit = new QLineEdit(this);
    lineEdit->setPlaceholderText(placeholder);
    layout->addWidget(lineEdit);

    FlutterTheme::instance().applyThemeToWidget(this);
  }

  QString text() const { return lineEdit->text(); }
  void setText(const QString &text) { lineEdit->setText(text); }
  void setPlaceholderText(const QString &text) {
    lineEdit->setPlaceholderText(text);
  }
  void setEchoMode(QLineEdit::EchoMode mode) { lineEdit->setEchoMode(mode); }
  QLineEdit *inner() { return lineEdit; }

private:
  QLineEdit *lineEdit;
  QLabel *labelWidget = nullptr;
};

class FlutterAppBar : public QWidget {
  Q_OBJECT
public:
  FlutterAppBar(const QString &title, QWidget *parent = nullptr)
      : QWidget(parent) {
    setProperty("widget-type", "appbar");
    setFixedHeight(64);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(16, 0, 16, 0);

    // Menu Button
    QPushButton *menuBtn = new QPushButton(this);
    menuBtn->setIcon(MaterialIcons::instance().menu());
    menuBtn->setProperty("button-type", "text");
    menuBtn->setFixedSize(40, 40);
    layout->addWidget(menuBtn);

    titleLabel = new QLabel(title);
    titleLabel->setStyleSheet(
        "font-size: 20px; font-weight: 500; color: white;");
    layout->addWidget(titleLabel);

    layout->addStretch();

    // Botón para cambiar tema
    themeButton = new QPushButton(this);
    themeButton->setFixedSize(40, 40);
    themeButton->setProperty("button-type", "text");
    // Icon gets updated by signal
    updateThemeIcon();

    layout->addWidget(themeButton);

    connect(themeButton, &QPushButton::clicked,
            []() { FlutterTheme::instance().toggleTheme(); });

    connect(&FlutterTheme::instance(), &FlutterTheme::themeChanged, this,
            &FlutterAppBar::updateThemeIcon);
    FlutterTheme::instance().applyThemeToWidget(this);
  }

  void setTitle(const QString &title) {
    if (titleLabel)
      titleLabel->setText(title);
  }

private:
  void updateThemeIcon() {
    bool dark = FlutterTheme::instance().darkMode();
    themeButton->setIcon(dark ? MaterialIcons::instance().light_mode()
                              : MaterialIcons::instance().dark_mode());
  }

  QLabel *titleLabel;
  QPushButton *themeButton;
};

#endif // FLUTTER_WIDGETS_H
