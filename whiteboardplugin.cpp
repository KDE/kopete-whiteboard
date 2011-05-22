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
#include "whiteboardplugin.h"
#include "whiteboardguiclient.h"
#include "whiteboarddialog.h"
#include "kopete_whiteboard.h"
// Qt
#include <qimage.h>
#include <qregexp.h>
#include <QDomDocument>

// KDE
#include <kdebug.h>
#include <kimageio.h>
#include <kmessagebox.h>
#include <kgenericfactory.h>

// KIO
#include <kio/netaccess.h>

// Kopete
#include <kopetecontactlist.h>
#include <kopetemetacontact.h>
#include <kopetecontactlist.h>
#include <kopetechatsessionmanager.h>
#include <kopetecontact.h>
#include <kopetemessage.h>
#include <kopeteaccount.h>
#include <kopeteaccountmanager.h>
#include <kopetemessageevent.h>
#include <kopeteprotocol.h>
#include <ui/kopeteview.h>
#include <kopetechatsession.h>
#include <kopeteuiglobal.h>


K_PLUGIN_FACTORY( WhiteboardPluginFactory, registerPlugin<WhiteboardPlugin>(); )
K_EXPORT_PLUGIN( WhiteboardPluginFactory( "kopete_whiteboard" ) )

class Private;

WhiteboardPlugin::WhiteboardPlugin ( QObject* parent, const QVariantList& /* args */ )
		: Kopete::Plugin ( WhiteboardPluginFactory::componentData(), parent )
{
        kDebug() << "Whiteboard Plugin loading...";
        
//        m_messageHandler = new WhiteboardMessageHandlerFactory(this);
	m_messageHandler = new WhiteboardMessageHandlerFactory( this, this, 
                SLOT( slotNewSession( Kopete::MessageEvent * ) ),
                SLOT( slotProcessWb ( QDomElement, Kopete::MessageEvent * ) ),
                SLOT( slotTrySessionAccept ( Kopete::MessageEvent * ) ),
                SLOT( slotUserOffline ( Kopete::MessageEvent * ) )
                );

	if( !pluginStatic_ )
		pluginStatic_ = this;

        Kopete::ChatSessionManager * chatSessionManager = Kopete::ChatSessionManager::self();

        connect( chatSessionManager, SIGNAL( chatSessionCreated( Kopete::ChatSession * ) ),
			 this, SLOT( slotNewChatSessionWindow( Kopete::ChatSession * ) ) );
        connect(this, SIGNAL(settingsChanged()), this, SLOT(slotSettingsChanged()));
//        connect ( chatSessionManager, SIGNAL ( aboutToSend ( Kopete::Message& ) ),
//	          this, SLOT ( slotAboutToSend ( Kopete::Message& ) ) );


//	connect ( this, SIGNAL ( readyForUnload() ), this, SLOT ( readyForUnload() ) );

//	m_pic = new QImage;
}

WhiteboardPlugin::~WhiteboardPlugin()
{
        delete m_messageHandler;

	disconnect ( Kopete::ChatSessionManager::self(), SLOT ( chatSessionCreated ( Kopete::ChatSession * ) ) );

//	delete m_pic;

	kDebug() << "Exiting Whiteboard plugin";
}

QMap<QString, QString> WhiteboardPlugin::getMessageCache(){
	return messageCache;
}

WhiteboardPlugin* WhiteboardPlugin::plugin()
{
        return pluginStatic_ ;
}
WhiteboardPlugin* WhiteboardPlugin::pluginStatic_ = 0L;

void WhiteboardPlugin::slotNewChatSessionWindow( Kopete::ChatSession *KMM )
{
        kDebug() << "New Chat Session Window";
	//Check if there is another user in the session.
	//If not it could be a Jabber-MUC
	//If there is more than one member it is a MUC
	// Also don't add the Button on an IRC window!
	if( KMM->members().count() == 1 && (KMM->protocol()) && ( KMM->protocol()->pluginId() == "JabberProtocol" ) ){
		WhiteboardGUIClient * guiClient = new WhiteboardGUIClient( KMM );
                connect(guiClient, SIGNAL(sendInvitation(Kopete::ChatSession * )), this, SLOT(slotSendInvitation(Kopete::ChatSession * )));
	}
}

void WhiteboardPlugin::slotSendInvitation(Kopete::ChatSession * chatSession) {
    if (!dialogs.empty()) {
        foreach( WhiteboardDialog *dialog, dialogs ) {
            Kopete::Contact *contact = chatSession->members().first();
            if (!contact) {
                kDebug()<<"Unable to get contact!";
                return;
            }

            Kopete::MetaContact *mc = contact->metaContact();
            if(!mc) {
                kWarning() << "unable to get metacontact";
                return;
            }
            QString jid = mc -> displayName();
//            if(dialog->jid == jid) {
            foreach( WhiteboardDialogSess * sess, dialog->sessions ) {
                if(sess->jid == jid) {
                    KMessageBox::queuedMessageBox( Kopete::UI::Global::mainWidget(), KMessageBox::Error,
                                    i18n( "<qt>An error has occurred while creating a new chat window. Propably because whiteboard session with user '%1' already exists.</qt>", mc -> displayName()),
                                    i18n( "Error While Creating Chat Window" ) );
                    return;
                }
            }
        }
    }

    QDomDocument doc;
    Kopete::Message msg( chatSession->myself(),chatSession->members() );
    kDebug()<<"Slot whiteboard SendInvitation click!";

    msg.setDirection( Kopete::Message::Outbound );
    QDomElement wb = doc.createElement ( "wb" );
    wb.setAttribute ( "xmlns", "http://jabber.org/protocol/svgwb");
//    wb.setAttribute ( "session", "http://jabber.org/protocol/svgwb");
//    wb.setAttribute ( "hash", "http://jabber.org/protocol/svgwb");
    QDomElement protocol = doc.createElement("protocol");
    QDomElement invitation = doc.createElement("invitation");

    QDomElement feature;
    QDomText text;

//    feature = doc.createElement("feature");
//    text = doc.createTextNode( "http://www.w3.org/TR/SVG11/feature#CoreAttribute" );
//    feature.appendChild(text);
//    invitation.appendChild(feature);

    feature = doc.createElement("feature");
    text = doc.createTextNode( "http://www.w3.org/TR/SVG11/feature#BasicStructure" );
    feature.appendChild(text);
    invitation.appendChild(feature);

    feature = doc.createElement("feature");
    text = doc.createTextNode( "http://www.w3.org/TR/SVG11/feature#BasicPaintAttribute" );
    feature.appendChild(text);
    invitation.appendChild(feature);

    feature = doc.createElement("feature");
    text = doc.createTextNode( "http://www.w3.org/TR/SVG11/feature#Shape" );
    feature.appendChild(text);
    invitation.appendChild(feature);

    protocol.appendChild(invitation);
    wb.appendChild(protocol);
    doc.appendChild ( wb );
//    message.setAttribute ( "timestamp", msg.timestamp().toString ( "ddd MMM d hh:mm:ss yyyy" ) );
//    message.setAttribute ( "formattedTimestamp", msg.timestamp().time().toString ( "hh:mm:ss ap" ) );
//    message.setAttribute ( "subject", msg.subject() );
//    msg.setType(Kopete::Message::TypeFileTransferRequestTypeNormal);
    msg.setPlainBody(doc.toString());
    chatSession->sendMessage( msg );
}


void WhiteboardPlugin::slotAboutToSend( Kopete::Message *message ) {
    kDebug() << "About to send";
    kDebug() << message->plainBody();
}


void WhiteboardPlugin::slotSettingsChanged() {
	kDebug() << "RELOADING CONFIG";
	WhiteboardConfig::self()->readConfig();
}

void WhiteboardPlugin::slotUserOffline(Kopete::MessageEvent * event)
{
    QString jid = event->message().manager()->members().first()->metaContact()->displayName();
    foreach(WhiteboardDialog * dialog, dialogs) {
        foreach(WhiteboardDialogSess * sess, dialog->sessions) {
            if(sess->jid == jid) {
                sess -> userOffline();
            }
        }
    }
}

/**
 * Called when inivitation session is accepted by user, here is checked if the session window
 * already exists, if no/accept invitation, if yes/reject invitation
 * @param event
 */
void WhiteboardPlugin::slotTrySessionAccept(Kopete::MessageEvent * event)
{
    if (!dialogs.empty()) {
        foreach( WhiteboardDialog *dialog, dialogs )
        {
            Kopete::Contact *contact = event->message().manager()->members().first();
            if (!contact) {
                kDebug()<<"Unable to get contact!";
                return;
            }

            Kopete::MetaContact *mc = contact->metaContact();
            if(!mc) {
                kWarning() << "unable to get metacontact";
                return;
            }
            QString jid = mc -> displayName();
            
            foreach( WhiteboardDialogSess * sess, dialog->sessions ) {
                if(sess->jid == jid) {
                    WhiteboardPlugin::rejectInvitation(event);//session already exists
                    return;
                }
    //            list.append( AccountListEntry( contact->contactId(), contact->protocol() ) );
            }
        }
    }
    WhiteboardPlugin::acceptInvitation(event);
    WhiteboardPlugin::newDialog(event->message().manager());
}

void WhiteboardPlugin::newDialog(Kopete::ChatSession * chatSession)
{
    if (dialogs.isEmpty() || WhiteboardConfig::self()->newWindow()) {
        WhiteboardDialog * dialog = new WhiteboardDialog(chatSession);
    //            connect ( dialog, SIGNAL ( newWb ( const QDomElement & ) ),
    //	          this, SLOT ( slotWbToSend ( const QDomElement &) ) );
        connect ( dialog, SIGNAL( destroyedDialog(WhiteboardDialog *) ), this, SLOT( dialogDestroyed(WhiteboardDialog *) ) );
        dialogs.append(dialog);
    } else {
        dialogs.at(0)->addTab(chatSession);//add tab session to first dialog
    }
}

/**
 * Slot to create new session window
 * @param event
 */
void WhiteboardPlugin::slotNewSession(Kopete::MessageEvent * event) {
    WhiteboardPlugin::newDialog(event->message().manager());
//    chatSession = event->message().manager();
//    Kopete::Contact *contact = chatSession->members().first();
//    if (!contact) {
//            kDebug()<<"Unable to get contact!";
//            return;
//    }

//    Kopete::MetaContact *m = contact->metaContact();

//    if(m)
//    {
//        if (dialog == NULL) {
//            //new dialog
//            dialog = new WhiteboardDialog(m);
//        } else {
//            //existing dialog
////            if (WhiteboardConfig::self()->newWindow()) {
//                dialog = new WhiteboardDialog(m);
////            } else {
////                dialog -> addTab(m);
////            }
//        }

//    if(!exists) {
//        WhiteboardDialog * dialog = new WhiteboardDialog(event->message().manager());
////            connect ( dialog, SIGNAL ( newWb ( const QDomElement & ) ),
////	          this, SLOT ( slotWbToSend ( const QDomElement &) ) );
//        connect ( dialog, SIGNAL( destroyedDialog(QString) ), this, SLOT( dialogDestroyed(QString) ) );
//        dialogs.append(dialog);
//    }

//        }
//    }
}

void WhiteboardPlugin::slotProcessWb(QDomElement wb, Kopete::MessageEvent * event)
{
    Kopete::Contact *contact = event->message().manager()->members().first();
    if (!contact) {
        kDebug()<<"Unable to get contact!";
        return;
    }

    Kopete::MetaContact *mc = contact->metaContact();
    if(!mc) {
        kWarning() << "unable to get metacontact";
        return;
    }
    QString jidd = mc -> displayName();
//    Kopete::ChatSession chatSession = event->message().manager();
    foreach( WhiteboardDialog *dialog, dialogs ) {
        foreach( WhiteboardDialogSess *sess, dialog -> sessions ) {
            if(sess->jid == jidd) {
                sess->m_wbwidget->processWb(wb);
            }
        }
//            list.append( AccountListEntry( contact->contactId(), contact->protocol() ) );
    }
//    kDebug() << dialogs
//    if(dialog != NULL) {
//        dialog->processWb(wb);
//    }
    
}

void WhiteboardPlugin::dialogDestroyed(WhiteboardDialog * dialog)
{
    dialogs.removeOne(dialog);//remove(dialog);
//    kDebug() << dialogs.size();
}

void WhiteboardPlugin::acceptInvitation( Kopete::MessageEvent *event ) {
   QDomDocument doc;
    Kopete::ChatSession *chatSession =event->message().manager();
    Kopete::Message msg( chatSession->myself(), chatSession->members() );
//
    msg.setDirection( Kopete::Message::Outbound );
    QDomElement wb = doc.createElement ( "wb" );
    wb.setAttribute ( "xmlns", "http://jabber.org/protocol/svgwb");
////    wb.setAttribute ( "session", "http://jabber.org/protocol/svgwb");
////    wb.setAttribute ( "hash", "http://jabber.org/protocol/svgwb");
    QDomElement protocol = doc.createElement("protocol");
    QDomElement acceptInvitation = doc.createElement("accept-invitation");
    protocol.appendChild(acceptInvitation);
    wb.appendChild(protocol);
    doc.appendChild ( wb );
    msg.setPlainBody(doc.toString());
    chatSession->sendMessage( msg );
}

void WhiteboardPlugin::rejectInvitation( Kopete::MessageEvent *event ) {
   QDomDocument doc;
    Kopete::ChatSession *chatSession =event->message().manager();
    Kopete::Message msg( chatSession->myself(), chatSession->members() );
//
    msg.setDirection( Kopete::Message::Outbound );
    QDomElement wb = doc.createElement ( "wb" );
    wb.setAttribute ( "xmlns", "http://jabber.org/protocol/svgwb");
////    wb.setAttribute ( "session", "http://jabber.org/protocol/svgwb");
////    wb.setAttribute ( "hash", "http://jabber.org/protocol/svgwb");
    QDomElement protocol = doc.createElement("protocol");
    QDomElement rejectInvitation = doc.createElement("abort-negotiation");
    protocol.appendChild(rejectInvitation);
    wb.appendChild(protocol);
    doc.appendChild ( wb );
    msg.setPlainBody(doc.toString());
    chatSession->sendMessage( msg );
}

void WhiteboardMessageHandler::handleMessage( Kopete::MessageEvent *event ){
    kDebug() << "Handle message called";
    Kopete::Message msg = event->message();
    QMap<QString, QString> messageCache = plugin->getMessageCache();
//    Kopete::ChatSession *session = msg.manager();

//    messageCache.clear();
    if( msg.direction() == Kopete::Message::Outbound ){
        //in
        kDebug() << "Outbound message" ;
        if ( msg.plainBody().startsWith( QString::fromLatin1( "<wb" ) ) ) {
            WhiteboardMessageHandler::handleOutWbMsg(event);
            return;
        }
//        event->discard();
//        return;
    } else if( msg.direction() == Kopete::Message::Inbound ) {
        kDebug() << "Inbound message" ;
        if ( msg.plainBody().startsWith( QString::fromLatin1( "<wb" ) ) ) {
            WhiteboardMessageHandler::handleInWbMsg(event);
            return;
        }
        //out

    }

//    event->setMessage( msg );
//    msg.manager()->view()->setCurrentMessage(msg);//manager()->view()->setCurrentMessage(msg);
    MessageHandler::handleMessage( event );
//    QMap<QString, QString> messageCache = plugin->getMessageCache();
//    event->setMessage( msg );
//    MessageHandler::handleMessage( event );
}

/**
 * Main handler for outgoing WB message
 * @param event
 */
void WhiteboardMessageHandler::handleOutWbMsg( Kopete::MessageEvent *event ){
    Kopete::Message msg = event->message();
    QDomDocument doc;
    doc.setContent(msg.plainBody());

    QDomNode wb = doc.firstChild();
    QDomNode element = wb.firstChild();

    //protocol messages
    if (element.nodeName() == "protocol") {
        QDomNode protocolChild = element.firstChild();
        if (protocolChild.nodeName() == "invitation") {
            msg.setPlainBody( "Whiteboard invitation sent" );
            event->setMessage( msg );
//            msg.manager()->view()->setCurrentMessage(msg);//manager()->view()->setCurrentMessage(msg);
            MessageHandler::handleMessage( event );
            return;
        }
    }
    event->discard();
    return;
//    MessageHandler::handleMessage( event );
}

/**
 * Main handler for outgoing WB message
 * @param event
 */
void WhiteboardMessageHandler::handleInWbMsg( Kopete::MessageEvent *event ){
    Kopete::Message msg = event->message();
    QDomDocument doc;
    doc.setContent(msg.plainBody());

    QDomNode wb = doc.firstChild();
    QDomNode element = wb.firstChild();

    //protocol messages
    if (element.nodeName() == "protocol") {
        QDomNode protocolChild = element.firstChild();
        if (protocolChild.nodeName() == "invitation") {
            if (WhiteboardConfig::self()->autoAccept()) {
//                WhiteboardMessageHandler::acceptInvitation(event);
//                WhiteboardMessageHandler::displayView(event);
                emit sessionAccept(event);
            } else {
                Kopete::Contact *contact = msg.manager()->members().first();
                if (!contact) {
                    kWarning()<< "Unable to get contact!";
                    return;
                }

                Kopete::MetaContact *m = contact->metaContact();
                //todo:include in chat window
                int doAccept = KMessageBox::questionYesNo(
                        Kopete::UI::Global::mainWidget(),
                        QString( i18n("User '%1' sent you whiteboard invitation.", m->displayName()) ),
                    i18n("Whiteboard invitation"), KGuiItem( i18n("Accept") ), KGuiItem( i18n("Reject") ) );
                if( doAccept == KMessageBox::Yes ){
//                    WhiteboardMessageHandler::acceptInvitation(event);
//                    WhiteboardMessageHandler::displayView(event);
                    emit sessionAccept(event);
                } else {
                    WhiteboardMessageHandler::rejectInvitation(event);
                }
            }
            return;
        }

        if (protocolChild.nodeName() == "abort-negotiation") {
            msg.setPlainBody( i18n("Whiteboard invitation rejected") );
            event->setMessage( msg );
//            msg.manager()->view()->setCurrentMessage(msg);//manager()->view()->setCurrentMessage(msg);
            MessageHandler::handleMessage( event );
            return;
        }

        if (protocolChild.nodeName() == "accept-invitation") {
//            WhiteboardMessageHandler::displayView(event);
            emit sessionNew(event);
            return;
        }
        if (protocolChild.nodeName() == "user-offline") {
//            WhiteboardMessageHandler::displayView(event);
            emit userOffline(event);
            return;
        }
    } else {
        emit processWb(wb.toElement(), event);
//        if ( dialog != NULL ) {
//            dialog->processWb(wb.toElement());
//        }
        event->discard();
        return;
    }

//    MessageHandler::handleMessage( event );
}


void WhiteboardMessageHandler::acceptInvitation( Kopete::MessageEvent *event ) {
   QDomDocument doc;
    Kopete::ChatSession *chatSession =event->message().manager();
    Kopete::Message msg( chatSession->myself(), chatSession->members() );
//
    msg.setDirection( Kopete::Message::Outbound );
    QDomElement wb = doc.createElement ( "wb" );
    wb.setAttribute ( "xmlns", "http://jabber.org/protocol/svgwb");
////    wb.setAttribute ( "session", "http://jabber.org/protocol/svgwb");
////    wb.setAttribute ( "hash", "http://jabber.org/protocol/svgwb");
    QDomElement protocol = doc.createElement("protocol");
    QDomElement acceptInvitation = doc.createElement("accept-invitation");
    protocol.appendChild(acceptInvitation);
    wb.appendChild(protocol);
    doc.appendChild ( wb );
    msg.setPlainBody(doc.toString());
    chatSession->sendMessage( msg );
}

void WhiteboardMessageHandler::rejectInvitation( Kopete::MessageEvent *event ) {
   QDomDocument doc;
    Kopete::ChatSession *chatSession =event->message().manager();
    Kopete::Message msg( chatSession->myself(), chatSession->members() );
//
    msg.setDirection( Kopete::Message::Outbound );
    QDomElement wb = doc.createElement ( "wb" );
    wb.setAttribute ( "xmlns", "http://jabber.org/protocol/svgwb");
////    wb.setAttribute ( "session", "http://jabber.org/protocol/svgwb");
////    wb.setAttribute ( "hash", "http://jabber.org/protocol/svgwb");
    QDomElement protocol = doc.createElement("protocol");
    QDomElement rejectInvitation = doc.createElement("abort-negotiation");
    protocol.appendChild(rejectInvitation);
    wb.appendChild(protocol);
    doc.appendChild ( wb );
    msg.setPlainBody(doc.toString());
    chatSession->sendMessage( msg );
}

#include "whiteboardplugin.moc"
#include "ui/kopeteview.h"
#include "kopeteuiglobal.h"