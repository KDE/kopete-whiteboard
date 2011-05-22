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

#ifndef WHITEBOARDPLUGIN_H
#define WHITEBOARDPLUGIN_H

#include <kurl.h>

//local
//#include "whiteboardguiclient.h"
#include "whiteboarddialog.h"
//Kopete
#include "kopeteplugin.h"
#include <kopetemessagehandler.h>
//kde
#include <kdebug.h>
//qt
#include <QPointer>
#include <QDomDocument>



//namespace Kopete { class Message; }
namespace Kopete { class ChatSession; }
namespace Kopete { class MetaContact; }
namespace Kopete { class Contact; }
namespace Kopete { class MessageHandler; }

class WhiteboardPlugin;
class WhiteboardGUIClient;
//class WhiteboardDialog;
//class QImage;


class WhiteboardMessageHandler : public Kopete::MessageHandler
{
    Q_OBJECT
private:
        
	QPointer<WhiteboardPlugin> plugin;
        void handleOutWbMsg( Kopete::MessageEvent * event );
        void handleInWbMsg( Kopete::MessageEvent * event );
        void acceptInvitation( Kopete::MessageEvent * event );
        void rejectInvitation( Kopete::MessageEvent * event );
public:
	WhiteboardMessageHandler( WhiteboardPlugin *plugin ) : plugin(plugin) {
            kDebug() << "MessageHandler created";
	}
	~WhiteboardMessageHandler(){
//            kDebug() << "MessageHandler created";
	}
	void handleMessage( Kopete::MessageEvent *event );
signals:
        void sessionNew( Kopete::MessageEvent *event );
        void sessionAccept( Kopete::MessageEvent *event );
        void processWb(QDomElement, Kopete::MessageEvent *event);
        void userOffline( Kopete::MessageEvent * event );

public slots:
        

private slots:
        
};

class WhiteboardMessageHandlerFactory : public Kopete::MessageHandlerFactory
{
private:
	WhiteboardPlugin *plugin;
	WhiteboardMessageHandler *messageHandler;
        const char *slotNewSession;
        const char *slotTrySessionAccept;
        const char *slotProcessWb;
        const char *slotUserOffline;
        QPointer<QObject> target;
public:
	WhiteboardMessageHandlerFactory( WhiteboardPlugin *plugin, QObject *target,  const char *slotNewSession, const char *slotProcessWb, const char *slotTrySessionAccept, const char *slotUserOffline ) :
            plugin(plugin),
            slotNewSession(slotNewSession),
            slotTrySessionAccept(slotTrySessionAccept),
            slotProcessWb(slotProcessWb),
            slotUserOffline(slotUserOffline),
            target(target) {
        }

	Kopete::MessageHandler *create( Kopete::ChatSession *, Kopete::Message::MessageDirection direction )
	{
            Q_UNUSED(direction)
//            return new WhiteboardMessageHandler(plugin);
            Kopete::MessageHandler *handler = new WhiteboardMessageHandler(plugin);
            QObject::connect( handler, SIGNAL( sessionNew( Kopete::MessageEvent * ) ), target, slotNewSession );
            QObject::connect( handler, SIGNAL( processWb(QDomElement , Kopete::MessageEvent *) ), target, slotProcessWb );
            QObject::connect( handler, SIGNAL( sessionAccept(Kopete::MessageEvent *) ), target, slotTrySessionAccept );
            QObject::connect( handler, SIGNAL( userOffline(Kopete::MessageEvent *) ), target, slotUserOffline );

            return handler;
	}
	int filterPosition( Kopete::ChatSession *, Kopete::Message::MessageDirection )
	{
		return Kopete::MessageHandlerFactory::InStageToSent+1;
	}
};



class WhiteboardPlugin : public Kopete::Plugin {
    Q_OBJECT

    WhiteboardPlugin(const WhiteboardPlugin&);
    WhiteboardPlugin& operator=(const WhiteboardPlugin&);

signals:

public:
    static WhiteboardPlugin  *plugin();
    WhiteboardPlugin(QObject* parent, const QVariantList& args);
    ~WhiteboardPlugin();
    QMap<QString, QString> getMessageCache();
    void newDialog(Kopete::ChatSession * chatSession);

private:
    QList< WhiteboardDialog* > dialogs;
//    Kopete::ChatSession * chatSession;
    static WhiteboardPlugin* pluginStatic_;
    QMap<QString, QString> messageCache;
    WhiteboardMessageHandlerFactory *m_messageHandler;
    void acceptInvitation( Kopete::MessageEvent * event );
    void rejectInvitation( Kopete::MessageEvent * event );
//    QString prepareBody(const QString& parsedBody, uint previewCount = 0);
//    QString createPreviewPicture(const KUrl& url);

public slots:

private slots:
    void slotNewChatSessionWindow(Kopete::ChatSession * );
    void slotAboutToSend(Kopete::Message * );
    void slotSettingsChanged();
    void slotNewSession(Kopete::MessageEvent * );
    void slotTrySessionAccept(Kopete::MessageEvent * );
    void slotProcessWb(QDomElement, Kopete::MessageEvent *);
    void dialogDestroyed(WhiteboardDialog *);
    void slotSendInvitation(Kopete::ChatSession * );
    void slotUserOffline(Kopete::MessageEvent * );


protected slots:
//    void aboutToDisplay(Kopete::Message& message);
//    void readyForUnload();

};


#endif /* WHITEBOARDPLUGIN_H */
