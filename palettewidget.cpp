/*

    Copyright (c) 2007 Jaroslav Reznik <rezzabuh@gmail.com>
    Copyright (c) 2010 Lukas Pohl <dr.zoidberg1969 AT yahoo.com>

    *************************************************************************
    *                                                                       *
    * This program is free software; you can redistribute it and/or modify  *
    * it under the terms of the GNU General Public License as published by  *
    * the Free Software Foundation; either version 2 of the License, or     *
    * (at your option) any later version.                                   *
    *                                                                       *
    *************************************************************************
*/
#include "palettewidget.h"

//kde
#include <kdebug.h>

//qt
#include <QtGui/QGridLayout>
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtGui/QColorDialog>

PaletteWidget::PaletteWidget(QWidget *parent) : QWidget(parent)
{
//  QGridLayout *layout = new QGridLayout(this);
  palette = createTangoPalette();
//  setMinimumSize(260, 30);
  setMinimumSize(30, 300);
}

void PaletteWidget::paintEvent(QPaintEvent *event)
{
  QWidget::paintEvent(event);
  QPainter painter;
  painter.begin(this);
  painter.setPen(QPen(Qt::black));
//  painter.rotate(90);

  int size = rect().width() / 3;
  this->setMouseTracking(true);

  // Draw currently selected colors
  painter.setBrush(m_bgColor);
//  QRect rect(size + 5, size * 0.5 + 5, size * 2 - 2, size * 2 - 2);
  QRect rect(size * 0.5 + 5, size + 5, size * 2 - 2, size * 2 - 2);
  painter.drawRect(rect);
  bgRect = rect;

  painter.setBrush(m_fgColor);
//  rect = QRect (size - 5, size * 0.5 - 5, size * 2 - 2, size * 2 - 2);
  rect = QRect (size * 0.5 - 5, size - 5, size * 2 - 2, size * 2 - 2);
  painter.drawRect(rect);
  fgRect = rect;



//  rect = QRect(6 * size, 0, 2 * size - 2, size - 2);
  rect = QRect(0, 6 * size, size - 2, 1 * size - 2);

  rectList.clear();
  for (int i = 0; i < palette.size(); i++)
  {
    painter.setBrush(QBrush(palette[i]));
    painter.drawRect(rect);
    rectList << rect;
    if (i % 3 == 2 && i != 0)
    {
//      rect.translate(2 * size, -2 * size);
        rect.translate(-2 * size, 1.2 * size);
    }
    else
    {
//      rect.translate(0, size);
      rect.translate(size, 0);
    }
  }
  painter.setBrush(QBrush(Qt::black));
  QPen pen(QPen(QColor(115, 210,  22), 2, Qt::SolidLine));
  painter.setPen(pen);

  if (m_penSize == 1)
    pen.setStyle(Qt::SolidLine);
  else
    pen.setStyle(Qt::NoPen);
  painter.setPen(pen);
//  painter.drawRect(rect.left() + 8, 3, 4, 4);
  painter.drawRect(2, rect.bottom() + 5, 4, 4);
  
  if (m_penSize == 3)
    pen.setStyle(Qt::SolidLine);
  else
    pen.setStyle(Qt::NoPen);
  painter.setPen(pen);
//  painter.drawRect(rect.left() + 7, 10, 6, 6);
  painter.drawRect(8, rect.bottom() + 4, 6, 6);

  if (m_penSize == 6)
    pen.setStyle(Qt::SolidLine);
  else
    pen.setStyle(Qt::NoPen);
  painter.setPen(pen);
//  painter.drawRect(rect.top() + 6, 19, 8, 8);
  painter.drawRect(16, rect.bottom() + 3, 8, 8);
//  penSizeRect = QRect(rect.left() + 6, 0, 20, this->rect().height());
  penSizeRect = QRect(2, rect.bottom(), this->rect().width(), 20);
  painter.end();
}

void PaletteWidget::mouseMoveEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
//    kDebug() << "X: " << e->x() << " Y:"<< e->y();
}

void PaletteWidget::mousePressEvent(QMouseEvent *event)
{
  QColor color;
  if (event->buttons() & (Qt::LeftButton | Qt::RightButton))
  {
    if (fgRect.contains(event->pos()))
    {
      // Foreground color clicked
      if (event->buttons() & Qt::LeftButton)
      {
        color = QColorDialog::getColor(m_fgColor, this);
        if (color.isValid())
          setFgColor(color);
      }
    }
     else if (bgRect.contains(event->pos()))
    {
      // Background color clicked
      if (event->buttons() & Qt::LeftButton)
      {
        color = QColorDialog::getColor(m_bgColor, this);
        if (color.isValid())
          setBgColor(color);
      }
    }
    else if (penSizeRect.contains(event->pos()))
    {
      // Pen size selection clicked
      if (event->buttons() & Qt::LeftButton)
      {
        if (event->x() < 7)
          setPenSize(1);
        else if (event->x() < 16)
          setPenSize(3);
        else
          setPenSize(6);
      }
    }
    else
    {
      for (int i = 0; i < rectList.size(); i++)
      {
        if (rectList[i].contains(event->pos()))
        {
          // Some color clicked
          if (event->buttons() & Qt::LeftButton)
          {
            setFgColor(palette[i]);
          }
          if (event->buttons() & Qt::RightButton)
          {
            setBgColor(palette[i]);
          }
          return;
        }
      }
    }
  }
}

void PaletteWidget::setFgColor(const QColor &c)
{
  m_fgColor = c;
  update();
  emit fgColorChanged(c);
}

void PaletteWidget::setBgColor(const QColor &c)
{
  m_bgColor = c;
  update();
  emit bgColorChanged(c);
}

void PaletteWidget::setPenSize(int size)
{
  m_penSize = size;
  update();
  emit penSizeChanged(size);
}

QList<QColor> PaletteWidget::createTangoPalette()
{
  QList<QColor> l;
  l<<QColor(255, 255, 255) // White
  << QColor(  0,   0,   0) // Black
  << Qt::transparent       // Transparent
  << QColor(252, 233,  79) //	Butter 1
  << QColor(237, 212,   0) //	Butter 2
  << QColor(196, 160,   0) //	Butter 3
  << QColor(138, 226,  52) //	Chameleon 1
  << QColor(115, 210,  22) //	Chameleon 2
  << QColor( 78, 154,   6) //	Chameleon 3
  << QColor(252, 175,  62) //	Orange 1
  << QColor(245, 121,   0) //	Orange 2
  << QColor(206,  92,   0) //	Orange 3
  << QColor(114, 159, 207) //	Sky Blue 1
  << QColor( 52, 101, 164) //	Sky Blue 2
  << QColor( 32,  74, 135) //	Sky Blue 3
  << QColor(173, 127, 168) //	Plum 1
  << QColor(117,  80, 123) //	Plum 2
  << QColor( 92,  53, 102) //	Plum 3
  << QColor(233, 185, 110) //	Chocolate 1
  << QColor(193, 125,  17) //	Chocolate 2
  << QColor(143,  89,   2) //	Chocolate 3
  << QColor(239,  41,  41) //	Scarlet Red 1
  << QColor(204,   0,   0) //	Scarlet Red 2
  << QColor(164,   0,   0) //	Scarlet Red 3
  << QColor(238, 238, 236) //	Aluminium 1
  << QColor(211, 215, 207) //	Aluminium 2
  << QColor(186, 189, 182) //	Aluminium 3
  << QColor(136, 138, 133) //	Aluminium 4
  << QColor( 85,  87,  83) //	Aluminium 5
  << QColor( 46,  52,  54);//	Aluminium 6
  return l;
}
