/*
    Copyright (c) 2007 Jaroslav Reznik <rezzabuh@gmail.com>
    Copyright (c) 2010 by  Lukas Pohl <dr.zoidberg1969 AT yahoo.com>

    *************************************************************************
    *                                                                       *
    * This program is free software; you can redistribute it and/or modify  *
    * it under the terms of the GNU General Public License as published by  *
    * the Free Software Foundation; either version 2 of the License, or     *
    * (at your option) any later version.                                   *
    *                                                                       *
    *************************************************************************
*/
#ifndef PALETTEWIDGET_H
#define PALETTEWIDGET_H

#include <QtGui/QWidget>
#include <QtGui/QPushButton>

class PaletteWidget : public QWidget
{
  Q_OBJECT
  QList<QColor> palette;
  QColor m_fgColor, m_bgColor;
  int m_penSize;
  QRect fgRect, bgRect, penSizeRect;
  QList<QRect> rectList;
  public:
    PaletteWidget(QWidget *parent);
    QList<QColor> createTangoPalette();
    void setFgColor(const QColor &c);
    void setBgColor(const QColor &c);
    void setPenSize(int size);
    QColor fgColor() { return m_fgColor; }
    QColor bgColor() { return m_bgColor; }
    int penSize() { return m_penSize; }
  signals:
    // !!! Don't call setFgColor, setBgColor and setPenSize in handlers for this signals  - it will lead to infinite recursion !
    void fgColorChanged(const QColor &);
    void bgColorChanged(const QColor &);
    void penSizeChanged(int size);
  protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *e);
};

#endif // PALETTEWIDGET_H

