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


#ifndef WBGUICLIENT_H
#define WBGUICLIENT_H

#include <kxmlguiclient.h>

#include <kopetemessage.h>
//qt
#include <QtCore/QObject>

class KActionMenu;
//class KAction;

namespace Kopete { class ChatSession; }
namespace Kopete { class MetaContact; }
namespace Kopete { class Contact; }


class WhiteboardGUIClient : public QObject , public KXMLGUIClient
{
	Q_OBJECT

public:

	WhiteboardGUIClient( Kopete::ChatSession *parent);
	~WhiteboardGUIClient();

private:
//	Kopete::ChatSession *m_manager;
//	KActionMenu *whiteboardActionMenu;
//	KAction *actionEnableOtr;
//	KAction *actionDisableOtr;
//	KAction *actionVerifyFingerprint;
//        HistoryLogger *m_logger;
	Kopete::ChatSession *chatSession;
	//bool m_autoChatWindow;
	//int m_nbAutoChatWindow;
	//unsigned int m_nbChatWindow;

//	KAction *actionPrev;
//	KAction *actionNext;
//	KAction *actionLast;

private slots:
    void slotSentWhiteboardInvitation();
    void slotMessageReceived ( Kopete::Message &, Kopete::ChatSession * );
    void slotSettingsChanged();
//	void slotEnableOtr();
//	void slotDisableOtr();
//        void encryptionEnabled( Kopete::ChatSession* session, int state );
//	void slotVerifyFingerprint();
	
signals:
    void sendInvitation(Kopete::ChatSession * );

};

#endif
