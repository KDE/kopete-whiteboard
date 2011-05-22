/*************************************************************************
 * Copyright <2011>  <Lukas Pohl> <dr.zoidberg1969@yahoo.com>            *
 *                                                                       *
 * This program is free software; you can redistribute it and/or         *
 * modify it under the terms of the GNU General Public License as        *
 * published by the Free Software Foundation; either version 2 of        *
 * the License or (at your option) version 3 or any later version        *
 * accepted by the membership of KDE e.V. (or its successor approved     *
 * by the membership of KDE e.V.), which shall act as a proxy            *
 * defined in Section 14 of version 3 of the license.                    *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *************************************************************************/


#ifndef WHITEBOARDDIALOG_H
#define	WHITEBOARDDIALOG_H


#include <QtCore/QList>

#include <kmainwindow.h>
#include <kurl.h>
//kopete
#include <kopeteaccount.h>
#include <kopetecontact.h>
#include <kopetemessage.h>
#include <kdebug.h>
//local
#include "palettewidget.h"
#include "wbwidget.h"
//#include <kopetemessage.h>
class PaletteWidget;
class WbWidget;
namespace Ui { class WhiteboardView; }
//namespace Kopete { class ChatSession; }


class WhiteboardDialogSess: public QObject
{
    Q_OBJECT
    public:
        WhiteboardDialogSess(){ }
        ~WhiteboardDialogSess();
    //	Message::MessageDirection direction;
    //	int position;
    //	QPointer<QObject> target;
    //	const char *slot;
        Kopete::ChatSession *chatSession;
        //todo:JID remake to sessionId (so you can make multiple sessions with one user)
        QString jid;
        WbWidget *m_wbwidget;
        void userOffline();
    private slots:
        void slotElementReady(const QDomElement &);
    signals:
        void newWb(const QDomElement &);
        void userOffline(QString);
};

class WhiteboardDialog : public KMainWindow
{
	Q_OBJECT

	public:
		WhiteboardDialog(Kopete::ChatSession *chatSession, QWidget* parent=0);
		~WhiteboardDialog();
                
                /*! \brief Incoming element message.*/
                void processWb(const QDomElement &);
                void addTab(Kopete::ChatSession *);
//                class WhiteboardDialogSess;
                QList<WhiteboardDialogSess *> sessions;

	signals:
            void strokeSizeChanged(int);
            void fgColorChanged(const QColor &);
            void bgColorChanged(const QColor &);
            /*! \brief Emitted when a new whiteboard element is ready to be sent.*/
            void newWb(const QDomElement &);
            void destroyedDialog(WhiteboardDialog *);
        public slots:
            void slotFgColorChanged(const QColor &);
            void slotBgColorChanged(const QColor &);
            void slotStrokeSizeChanged(int);
	private slots:
            void slotSelectClicked();
            void slotPathClicked();
            void slotLineClicked();
            void slotEraseClicked();
            void slotRectangleClicked();
            void slotEllipseClicked();
            void slotImageClicked();
            void slotTextClicked();
            void slotRotateClicked();
            void slotTranslateClicked();
            void slotScaleClicked();
            void slotScrollClicked();
            void slotUndoClicked();
            void slotPrintClicked();
            void slotClearClicked();
            void slotImportClicked();
            void slotExportClicked();
            void slotSaveClicked();
            void slotQuitClicked();
            
            void slotCloseTab(int);
            void slotUserOffline(QString);
            void slotTabCurrentChanged(int i);
            void slotElementReady(const QDomElement &);
            
        protected:
            bool queryClose();
            void tabOffline(QString);
	private:
//            PaletteWidget *mainDraw;
            QTabWidget * tabWidget;
            Ui::WhiteboardView *mMainWidget;
            QScrollArea * scrollArea;
            
//            void mouseMoveEvent(QMouseEvent *event);

};

#endif	/* WHITEBOARDDIALOG_H */

