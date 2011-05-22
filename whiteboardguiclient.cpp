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

//local
#include "whiteboardguiclient.h"
#include "whiteboardplugin.h"
#include "whiteboarddialog.h"

#include "kopete_whiteboard.h"
#include "whiteboardpreferences.h"

//kde
#include <kaction.h>
#include <klocale.h>
#include <kactionmenu.h>

#include <kgenericfactory.h>
#include <ui/kopeteview.h>
#include <kopetemessage.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <kicon.h>
#include <kactioncollection.h>
//kopete
#include <kopetemetacontact.h>
#include <kopetechatsession.h>


//qt
//#include <qdom.h>
#include <QDomDocument>
//#include <QDomElement>
//#include <QtXml/QDomDocument>


WhiteboardGUIClient::WhiteboardGUIClient( Kopete::ChatSession *parent )
: QObject( parent ), KXMLGUIClient( parent )
{
        kDebug() << "WhiteboardGUIClient start";
        setComponentData( WhiteboardPlugin::plugin()->componentData() );

//        JabberAccount *account=0L;
//        if(!accountid.isEmpty())
//        {
//            account=static_cast<JabberAccount*>(Kopete::AccountManager::self()->findAccount("JabberProtocol" , accountid));
//        }
        chatSession = parent;
        QList<Kopete::Contact*> mb=chatSession->members();
//	connect( WhiteboardPlugin::plugin(), SIGNAL( destroyed( QObject * ) ), this, SLOT( deleteLater() ) );
        
        KAction *sendWbInvitaion = new KAction( KIcon("view-pim-summary"), i18n("&Whiteboard" ), this );
	actionCollection()->addAction( "start_whiteboard", sendWbInvitaion );
//	sendWbInvitaion->setShortcut(KShortcut (Qt::CTRL + Qt::Key_H));
        sendWbInvitaion->setHelpText( QString( i18n("Start whiteboard session") ) );
        
	connect(sendWbInvitaion, SIGNAL(triggered(bool)), this, SLOT(slotSentWhiteboardInvitation()));

        connect ( parent, SIGNAL ( messageReceived ( Kopete::Message &, Kopete::ChatSession * ) ), this, SLOT ( slotMessageReceived ( Kopete::Message &, Kopete::ChatSession * ) ) );

//        connect(this, SIGNAL(settingsChanged()), this, SLOT(slotSettingsChanged()));

	setXMLFile("whiteboardchatui.rc");
//	setupGUI();
}

WhiteboardGUIClient::~WhiteboardGUIClient()
{
}


void WhiteboardGUIClient::slotSentWhiteboardInvitation()
{
    emit sendInvitation(chatSession);
}

void WhiteboardGUIClient::slotMessageReceived ( Kopete::Message &message, Kopete::ChatSession * session)
{
    Q_UNUSED(session);
    
    kDebug() << "Message recieved";
    
//    session -> view() -> msgManager() -> clear();
//    message.setPlainBody ( viewPlugin );
//    message . setRequestedPlugin(viewPlugin);

//    message . setHtmlBody("<input type='button' value='Accept' /><input type='button' value='Reject' />");
    kDebug() << message.plainBody();
}

void WhiteboardGUIClient::slotSettingsChanged()
{
	kDebug() << "RELOADING CONFIG";
	WhiteboardConfig::self()->readConfig();
}


#include "whiteboardguiclient.moc"
