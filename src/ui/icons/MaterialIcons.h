// material_icons.h
#ifndef MATERIAL_ICONS_H
#define MATERIAL_ICONS_H

#include "../styles/FlutterTheme.h"
#include <QDebug>
#include <QFontDatabase>
#include <QIcon>
#include <QMap>
#include <QPainter>

class MaterialIcons {
public:
  static MaterialIcons &instance() {
    static MaterialIcons instance;
    return instance;
  }

  QIcon getIcon(const QString &iconName, int size = 24,
                const QColor &color = QColor()) {
    if (!m_fontLoaded) {
      loadFont();
    }

    ushort codePoint = getCodePoint(iconName);
    if (codePoint == 0) {
      // Fallback for missing icons or font failure?
      // Maybe just draw a circle or default system icon?
      return QIcon();
    }

    return createIconFromCodePoint(codePoint, size, color);
  }

  // Iconos comunes predefinidos
  QIcon home(int size = 24) { return getIcon("home", size); }
  QIcon person(int size = 24) { return getIcon("person", size); }
  QIcon settings(int size = 24) { return getIcon("settings", size); }
  QIcon search(int size = 24) { return getIcon("search", size); }
  QIcon notifications(int size = 24) { return getIcon("notifications", size); }
  QIcon menu(int size = 24) { return getIcon("menu", size); }
  QIcon close(int size = 24) { return getIcon("close", size); }
  QIcon arrow_forward(int size = 24) { return getIcon("arrow_forward", size); }
  QIcon arrow_back(int size = 24) { return getIcon("arrow_back", size); }
  QIcon check(int size = 24) { return getIcon("check", size); }
  QIcon edit(int size = 24) { return getIcon("edit", size); }
  QIcon delete_icon(int size = 24) { return getIcon("delete", size); }
  QIcon download(int size = 24) { return getIcon("download", size); }
  QIcon star(int size = 24) { return getIcon("star", size); }
  QIcon favorite(int size = 24) { return getIcon("favorite", size); }
  QIcon share(int size = 24) { return getIcon("share", size); }
  QIcon info(int size = 24) { return getIcon("info", size); }
  QIcon warning(int size = 24) { return getIcon("warning", size); }
  QIcon error(int size = 24) { return getIcon("error", size); }
  QIcon dark_mode(int size = 24) { return getIcon("dark_mode", size); }
  QIcon light_mode(int size = 24) { return getIcon("light_mode", size); }
  QIcon database(int size = 24) {
    return getIcon("dns", size);
  } // 'dns' usually looks like DB
  QIcon table_chart(int size = 24) { return getIcon("table_chart", size); }
  QIcon bar_chart(int size = 24) { return getIcon("bar_chart", size); }

private:
  MaterialIcons() : m_fontLoaded(false) { initializeCodePoints(); }

  void loadFont() {
    // Try to load from resource
    int fontId =
        QFontDatabase::addApplicationFont(":/fonts/material-icons.woff2");
    if (fontId == -1) {
      // Fallback to local file if resource missing
      fontId = QFontDatabase::addApplicationFont(
          "assets/fonts/MaterialIcons-Regular.ttf");
    }

    if (fontId != -1) {
      m_fontFamily = QFontDatabase::applicationFontFamilies(fontId).first();
      m_fontLoaded = true;
    } else {
      qWarning() << "Failed to load Material Icons font.";
    }
  }

  QIcon createIconFromCodePoint(ushort codePoint, int size,
                                const QColor &color) {
    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    QFont font(m_fontFamily);
    font.setPixelSize(size);
    painter.setFont(font);

    if (color.isValid()) {
      painter.setPen(color);
    } else {
      painter.setPen(FlutterTheme::instance().darkMode() ? Qt::white
                                                         : Qt::black);
    }

    painter.drawText(pixmap.rect(), Qt::AlignCenter, QString(QChar(codePoint)));

    return QIcon(pixmap);
  }

  ushort getCodePoint(const QString &iconName) {
    return m_codePoints.value(iconName, 0);
  }

  void initializeCodePoints() {
    // Material Icons - Puntos de código comunes (Codepoints may vary by
    // version, using standard hex)
    m_codePoints["home"] = 0xe88a;
    m_codePoints["person"] = 0xe7fd;
    m_codePoints["settings"] = 0xe8b8;
    m_codePoints["search"] = 0xe8b6;
    m_codePoints["notifications"] = 0xe7f4;
    m_codePoints["menu"] = 0xe5d2;
    m_codePoints["close"] = 0xe5cd;
    m_codePoints["arrow_forward"] = 0xe5c8;
    m_codePoints["arrow_back"] = 0xe5c4;
    m_codePoints["check"] = 0xe5ca;
    m_codePoints["edit"] = 0xe254;
    m_codePoints["delete"] = 0xe872;
    m_codePoints["download"] = 0xe2c4;
    m_codePoints["star"] = 0xe838;
    m_codePoints["favorite"] = 0xe87d;
    m_codePoints["share"] = 0xe80d;
    m_codePoints["info"] = 0xe88e;
    m_codePoints["warning"] = 0xe002;
    m_codePoints["error"] = 0xe000;
    m_codePoints["dark_mode"] = 0xe51c;
    m_codePoints["light_mode"] = 0xe518;
    m_codePoints["dns"] = 0xe875;
    m_codePoints["table_chart"] = 0xe265;
    m_codePoints["bar_chart"] = 0xe26b;
  }

  bool m_fontLoaded;
  QString m_fontFamily;
  QMap<QString, ushort> m_codePoints;
};

#endif // MATERIAL_ICONS_H
