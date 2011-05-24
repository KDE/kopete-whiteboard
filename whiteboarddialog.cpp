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
#include "whiteboarddialog.h"
#include "whiteboardpreferences.h"

#include "ui_whiteboardview.h"

#include "kopete_whiteboard.h"
//kopete
#include <kopetemetacontact.h>
#include <kopetechatsessionmanager.h>
//kde
#include <kicon.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kmainwindow.h>
//qt
//#include <kdialog.h>
#include <QPainter>
#include <QMouseEvent>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <qt4/QtXml/qdom.h>
#include <QBuffer>
#include <QPrinter>
#include <QPrintDialog>

WhiteboardDialogSess::~WhiteboardDialogSess()
{
   QDomDocument doc;
    Kopete::Message msg( chatSession->myself(), chatSession->members() );
//
    msg.setDirection( Kopete::Message::Outbound );
    QDomElement wb = doc.createElement ( "wb" );
    wb.setAttribute ( "xmlns", "http://jabber.org/protocol/svgwb");
////    wb.setAttribute ( "session", "http://jabber.org/protocol/svgwb");
////    wb.setAttribute ( "hash", "http://jabber.org/protocol/svgwb");
    QDomElement protocol = doc.createElement("protocol");
    QDomElement rejectInvitation = doc.createElement("user-offline");
    protocol.appendChild(rejectInvitation);
    wb.appendChild(protocol);
    doc.appendChild ( wb );
    msg.setPlainBody(doc.toString());
    chatSession->sendMessage( msg );
}
void WhiteboardDialogSess::userOffline()
{
    emit userOffline(jid);
}

WhiteboardDialog::WhiteboardDialog(Kopete::ChatSession *chatSession, QWidget* parent)
 : KMainWindow(parent)
{
        WhiteboardDialogSess * sess = new WhiteboardDialogSess();
//	mMetaContact = mc;
        Kopete::Contact *contact = chatSession->members().first();
        if (!contact) {
            kDebug()<<"Unable to get contact!";
            return;
        }

        Kopete::MetaContact *mc = contact->metaContact();

        sess->jid = mc -> displayName();
        sess->chatSession = chatSession;

        setCaption( i18n("Whiteboard with %1", mc->displayName()) );
        kDebug() << "WB dialog called with " << mc->displayName();

	// Widgets initializations
        tabWidget = NULL;
	QWidget* w = new QWidget( this );
        w->setBaseSize(800, 640);
	mMainWidget = new Ui::WhiteboardView();
	mMainWidget->setupUi( w );

        mMainWidget->m_paletteWidget->setFgColor(QColor(0, 0, 0));
        mMainWidget->m_paletteWidget->setBgColor(Qt::transparent);
        mMainWidget->m_paletteWidget->setPenSize(1);

        mMainWidget->saveButton->setIcon ( KIcon("document-save") );
        mMainWidget->saveButton->setToolTip ( i18n("Save image") );

        mMainWidget->quitButton->setIcon ( KIcon("dialog-close") );
        mMainWidget->quitButton->setToolTip ( i18n("Close dialog") );

        mMainWidget->printButton->setIcon ( KIcon("document-print") );
        mMainWidget->printButton->setToolTip ( i18n("Print") );

        mMainWidget->selectButton->setIcon ( KIcon("select") );
        mMainWidget->selectButton->setToolTip ( i18n("Select") );

        mMainWidget->eraseButton->setIcon ( KIcon("erase") );
        mMainWidget->eraseButton->setToolTip ( i18n("Erase") );

        mMainWidget->pathButton->setIcon ( KIcon("draw_paths") );
        mMainWidget->pathButton->setToolTip ( i18n("Draw path") );
        mMainWidget->pathButton->setChecked ( true );

        mMainWidget->lineButton->setIcon ( KIcon("draw_lines") );
        mMainWidget->lineButton->setToolTip ( i18n("Draw line") );
        
        mMainWidget->rectangleButton->setIcon ( KIcon("draw_rectangles") );
        mMainWidget->rectangleButton->setToolTip ( i18n("Draw rectangle") );

        mMainWidget->ellipseButton->setIcon ( KIcon("draw_ellipses") );
        mMainWidget->ellipseButton->setToolTip ( i18n("Draw ellipse") );
        
        mMainWidget->imageButton->setIcon ( KIcon("add_image") );
        mMainWidget->imageButton->setToolTip ( i18n("Insert image") );

        mMainWidget->textButton->setIcon ( KIcon("add_text") );
        mMainWidget->textButton->setToolTip ( i18n("Add text") );

        mMainWidget->rotateButton->setIcon ( KIcon("rotate") );
        mMainWidget->rotateButton->setToolTip ( i18n("Rotate element") );

        mMainWidget->scaleButton->setIcon ( KIcon("scale") );
        mMainWidget->scaleButton->setToolTip ( i18n("Scale element") );

        mMainWidget->translateButton->setIcon ( KIcon("translate") );
        mMainWidget->translateButton->setToolTip ( i18n("Shift element") );

        mMainWidget->undoButton->setIcon ( KIcon("undo") );
        mMainWidget->undoButton->setToolTip ( i18n("Undo") );
        mMainWidget->undoButton->setVisible(false);

        mMainWidget->exportButton->setIcon ( KIcon("export") );
        mMainWidget->exportButton->setToolTip ( i18n("Export") );

        mMainWidget->importButton->setIcon ( KIcon("import") );
        mMainWidget->importButton->setToolTip ( i18n("Import") );

        mMainWidget->clearButton->setIcon ( KIcon("clear") );
        mMainWidget->clearButton->setToolTip ( i18n("Clear") );

        mMainWidget->undoButton->setEnabled(false);

        mMainWidget->scrollButton->setIcon ( KIcon("move") );
//        mMainWidget->textButton->setToolTip ( i18n("Add text") );
        
        sess->m_wbwidget = new WbWidget("s1", mc->displayName(), QSize(1920, 1200), mMainWidget->mainFrame);
        
        sess->m_wbwidget->setMode(WbWidget::DrawPath);
        sess->m_wbwidget->setMinimumSize(300, 400);
        sess->m_wbwidget->setSize(QSize(1920,1200));
        sess->m_wbwidget->showMaximized();
        
        mMainWidget->mainFrameLayout->addWidget(sess->m_wbwidget, 0, 0, 1, 1);

        setCentralWidget(w);

        connect(sess->m_wbwidget, SIGNAL(newWb(const QDomElement &)), sess, SLOT(slotElementReady(const QDomElement &)));
        connect(sess, SIGNAL(newWb(const QDomElement &)), this, SLOT(slotElementReady(const QDomElement &)));
        connect(sess, SIGNAL(userOffline(QString)), this, SLOT(slotUserOffline(QString)));

        connect(mMainWidget->m_paletteWidget, SIGNAL(fgColorChanged(const QColor &)), this, SLOT(slotFgColorChanged(const QColor &)));
        connect(mMainWidget->m_paletteWidget, SIGNAL(bgColorChanged(const QColor &)), this, SLOT(slotBgColorChanged(const QColor &)));
        connect(mMainWidget->m_paletteWidget, SIGNAL(penSizeChanged(int)), this, SLOT(slotStrokeSizeChanged(int)));

        connect(mMainWidget->selectButton, SIGNAL(clicked(bool)), this, SLOT(slotSelectClicked()));
        connect(mMainWidget->pathButton, SIGNAL(clicked(bool)), this, SLOT(slotPathClicked()));
        connect(mMainWidget->lineButton, SIGNAL(clicked(bool)), this, SLOT(slotLineClicked()));
        connect(mMainWidget->eraseButton, SIGNAL(clicked(bool)), this, SLOT(slotEraseClicked()));

        connect(mMainWidget->rectangleButton, SIGNAL(clicked(bool)), this, SLOT(slotRectangleClicked()));
        connect(mMainWidget->ellipseButton, SIGNAL(clicked(bool)), this, SLOT(slotEllipseClicked()));
        connect(mMainWidget->imageButton, SIGNAL(clicked(bool)), this, SLOT(slotImageClicked()));
        connect(mMainWidget->textButton, SIGNAL(clicked(bool)), this, SLOT(slotTextClicked()));

        connect(mMainWidget->rotateButton, SIGNAL(clicked(bool)), this, SLOT(slotRotateClicked()));
        connect(mMainWidget->translateButton, SIGNAL(clicked(bool)), this, SLOT(slotTranslateClicked()));
        connect(mMainWidget->scaleButton, SIGNAL(clicked(bool)), this, SLOT(slotScaleClicked()));
        connect(mMainWidget->scrollButton, SIGNAL(clicked(bool)), this, SLOT(slotScrollClicked()));
        connect(mMainWidget->clearButton, SIGNAL(clicked(bool)), this, SLOT(slotClearClicked()));

        connect(mMainWidget->exportButton, SIGNAL(clicked(bool)), this, SLOT(slotExportClicked()));
        connect(mMainWidget->importButton, SIGNAL(clicked(bool)), this, SLOT(slotImportClicked()));
        connect(mMainWidget->saveButton, SIGNAL(clicked(bool)), this, SLOT(slotSaveClicked()));

        connect(mMainWidget->undoButton, SIGNAL(clicked(bool)), this, SLOT(slotUndoClicked()));
        connect(mMainWidget->printButton, SIGNAL(clicked(bool)), this, SLOT(slotPrintClicked()));

        connect(mMainWidget->quitButton, SIGNAL(clicked(bool)), this, SLOT(slotQuitClicked()));

        sessions.append(sess);

	show();
//
}
void WhiteboardDialog::addTab(Kopete::ChatSession * chatSession)
{
    WhiteboardDialogSess * sess = new WhiteboardDialogSess();
    sess->chatSession = chatSession;
    
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

    setCaption( i18n("Whiteboard") );
    
    sess->jid = jid;

    WbWidget * m_wbwidget = new WbWidget("s1", jid, QSize(1920, 1200), mMainWidget->mainFrame);
    sess->m_wbwidget = m_wbwidget;
    sess->m_wbwidget->setMode(sessions.at(0)->m_wbwidget->mode());
    sess->m_wbwidget->setMinimumSize(300, 400);
//    sess->m_wbwidget->setSize(QSize(710,570));
    sess->m_wbwidget->setSize(QSize(1920,1200));
    
    if(tabWidget == NULL) {
        //existing
        tabWidget = new QTabWidget(mMainWidget->mainFrame);
        tabWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        mMainWidget->mainFrameLayout->addWidget(tabWidget, 0, 0, 1, 1);
//        tabWidget -> addTab(mMainWidget->scrollArea, KIcon("online"), sessions.first()->jid);
        tabWidget -> addTab(sessions.at(0)->m_wbwidget, KIcon("online"), sessions.first()->jid);
        connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(slotCloseTab(int)));
        connect(tabWidget, SIGNAL(currentChanged (int)), this, SLOT(slotTabCurrentChanged (int)));
    }
    tabWidget->addTab( m_wbwidget , KIcon("online") ,QString(jid));

    tabWidget->setGeometry(0,0,721,581);
    tabWidget->setTabShape(QTabWidget::Triangular);
    tabWidget->setVisible(true);
    tabWidget->setTabsClosable(true);
    
    
    connect(sess->m_wbwidget, SIGNAL(newWb(const QDomElement &)), sess, SLOT(slotElementReady(const QDomElement &)));
    connect(sess, SIGNAL(newWb(const QDomElement &)), this, SLOT(slotElementReady(const QDomElement &)));
    connect(sess, SIGNAL(userOffline(QString)), this, SLOT(slotUserOffline(QString)));
    
    sessions.append(sess);
}

void WhiteboardDialog::slotTabCurrentChanged(int i)
{
    if(tabWidget != NULL) {
        i = tabWidget->currentIndex();
    } else {
        i = 0;
    }
    if (sessions.at(i)->m_wbwidget->elements.size() > 0) {
        mMainWidget->undoButton->setEnabled(true);
    } else {
        mMainWidget->undoButton->setEnabled(false);
    }
}

void WhiteboardDialog::slotCloseTab(int i)
{
    if(sessions.size() == 1) {
        //close the hole window
        this->close();
    } else {
//        are you sure ?
        tabWidget->removeTab(i);
        delete sessions.at(i);
        sessions.removeAt(i);
    }
}

void WhiteboardDialog::slotFgColorChanged(const QColor & color) {
//    emit fgColorChanged(color);
    kDebug() << "foreground color changed " << color;
    foreach(WhiteboardDialogSess * sess, sessions) {
        sess->m_wbwidget->setStrokeColor(color);
    }
}

void WhiteboardDialog::slotBgColorChanged(const QColor & color) {
    kDebug() << "background color changed "<< color;
    emit bgColorChanged(color);
    foreach(WhiteboardDialogSess * sess, sessions) {
        sess->m_wbwidget->setFillColor(color);
    }
}

void WhiteboardDialog::slotStrokeSizeChanged(int size) {
    kDebug() << "stroke size changed "<< size;
    emit strokeSizeChanged(size);
    foreach(WhiteboardDialogSess * sess, sessions) {
        sess->m_wbwidget->setStrokeWidth(size);
    }
}

void WhiteboardDialog::slotSelectClicked()
{
    foreach(WhiteboardDialogSess * sess, sessions) {
        sess->m_wbwidget->setMode(WbWidget::Select);
    }
//    mMainWidget->selectButton->setChecked(false);
    mMainWidget->pathButton->setChecked(false);
    mMainWidget->eraseButton->setChecked(false);
    mMainWidget->lineButton->setChecked(false);
    mMainWidget->rectangleButton->setChecked(false);
    mMainWidget->ellipseButton->setChecked(false);
    mMainWidget->imageButton->setChecked(false);
    mMainWidget->textButton->setChecked(false);
}
void WhiteboardDialog::slotPathClicked()
{
    foreach(WhiteboardDialogSess * sess, sessions) {
        sess->m_wbwidget->setMode(WbWidget::DrawPath);
    }
    mMainWidget->selectButton->setChecked(false);
//        mMainWidget->pathButton->setChecked(false);
    mMainWidget->eraseButton->setChecked(false);
    mMainWidget->lineButton->setChecked(false);
    mMainWidget->rectangleButton->setChecked(false);
    mMainWidget->ellipseButton->setChecked(false);
    mMainWidget->imageButton->setChecked(false);
    mMainWidget->textButton->setChecked(false);
}
void WhiteboardDialog::slotLineClicked()
{
    foreach(WhiteboardDialogSess * sess, sessions) {
        sess->m_wbwidget->setMode(WbWidget::DrawLine);
    }
    mMainWidget->selectButton->setChecked(false);
    mMainWidget->pathButton->setChecked(false);
    mMainWidget->eraseButton->setChecked(false);
//    mMainWidget->lineButton->setChecked(false);
    mMainWidget->rectangleButton->setChecked(false);
    mMainWidget->ellipseButton->setChecked(false);
    mMainWidget->imageButton->setChecked(false);
    mMainWidget->textButton->setChecked(false);
}

void WhiteboardDialog::slotEraseClicked()
{
    foreach(WhiteboardDialogSess * sess, sessions) {
        sess->m_wbwidget->setMode(WbWidget::Erase);
    }
    mMainWidget->selectButton->setChecked(false);
    mMainWidget->pathButton->setChecked(false);
//    mMainWidget->eraseButton->setChecked(false);
    mMainWidget->lineButton->setChecked(false);
    mMainWidget->rectangleButton->setChecked(false);
    mMainWidget->ellipseButton->setChecked(false);
    mMainWidget->imageButton->setChecked(false);
    mMainWidget->textButton->setChecked(false);
}

void WhiteboardDialog::slotRectangleClicked()
{
    foreach(WhiteboardDialogSess * sess, sessions) {
        sess->m_wbwidget->setMode(WbWidget::DrawRectangle);
    }
    mMainWidget->selectButton->setChecked(false);
    mMainWidget->pathButton->setChecked(false);
    mMainWidget->lineButton->setChecked(false);
    mMainWidget->eraseButton->setChecked(false);
//    mMainWidget->rectangleButton->setChecked(false);
    mMainWidget->ellipseButton->setChecked(false);
    mMainWidget->imageButton->setChecked(false);
    mMainWidget->textButton->setChecked(false);
}

void WhiteboardDialog::slotEllipseClicked()
{
    foreach(WhiteboardDialogSess * sess, sessions) {
        sess->m_wbwidget->setMode(WbWidget::DrawEllipse);
    }
    mMainWidget->selectButton->setChecked(false);
    mMainWidget->pathButton->setChecked(false);
    mMainWidget->lineButton->setChecked(false);
    mMainWidget->eraseButton->setChecked(false);
    mMainWidget->rectangleButton->setChecked(false);
//    mMainWidget->ellipseButton->setChecked(false);
    mMainWidget->imageButton->setChecked(false);
    mMainWidget->textButton->setChecked(false);
}

void WhiteboardDialog::slotImageClicked()
{
    foreach(WhiteboardDialogSess * sess, sessions) {
        sess->m_wbwidget->setMode(WbWidget::DrawImage);
    }
    mMainWidget->selectButton->setChecked(false);
    mMainWidget->pathButton->setChecked(false);
    mMainWidget->lineButton->setChecked(false);
    mMainWidget->eraseButton->setChecked(false);
    mMainWidget->rectangleButton->setChecked(false);
    mMainWidget->ellipseButton->setChecked(false);
//    mMainWidget->imageButton->setChecked(false);
    mMainWidget->textButton->setChecked(false);
}

void WhiteboardDialog::slotTextClicked()
{
    foreach(WhiteboardDialogSess * sess, sessions) {
        sess->m_wbwidget->setMode(WbWidget::DrawText);
    }
    mMainWidget->selectButton->setChecked(false);
    mMainWidget->pathButton->setChecked(false);
    mMainWidget->lineButton->setChecked(false);
    mMainWidget->eraseButton->setChecked(false);
    mMainWidget->rectangleButton->setChecked(false);
    mMainWidget->ellipseButton->setChecked(false);
    mMainWidget->imageButton->setChecked(false);
//    mMainWidget->textButton->setChecked(false);
    mMainWidget->rotateButton->setChecked(false);
    mMainWidget->scaleButton->setChecked(false);
    mMainWidget->translateButton->setChecked(false);
    mMainWidget->scrollButton->setChecked(false);
}

void  WhiteboardDialog::slotRotateClicked() {
    foreach(WhiteboardDialogSess * sess, sessions) {
        sess->m_wbwidget->setMode(WbWidget::Rotate);
    }
    mMainWidget->selectButton->setChecked(false);
    mMainWidget->pathButton->setChecked(false);
    mMainWidget->lineButton->setChecked(false);
    mMainWidget->eraseButton->setChecked(false);
    mMainWidget->rectangleButton->setChecked(false);
    mMainWidget->ellipseButton->setChecked(false);
    mMainWidget->imageButton->setChecked(false);
    mMainWidget->textButton->setChecked(false);
//    mMainWidget->rotateButton->setChecked(false);
    mMainWidget->scaleButton->setChecked(false);
    mMainWidget->translateButton->setChecked(false);
    mMainWidget->scrollButton->setChecked(false);
}

void  WhiteboardDialog::slotTranslateClicked() {
    foreach(WhiteboardDialogSess * sess, sessions) {
        sess->m_wbwidget->setMode(WbWidget::Translate);
    }
    mMainWidget->selectButton->setChecked(false);
    mMainWidget->pathButton->setChecked(false);
    mMainWidget->lineButton->setChecked(false);
    mMainWidget->eraseButton->setChecked(false);
    mMainWidget->rectangleButton->setChecked(false);
    mMainWidget->ellipseButton->setChecked(false);
    mMainWidget->imageButton->setChecked(false);
    mMainWidget->textButton->setChecked(false);
    mMainWidget->rotateButton->setChecked(false);
    mMainWidget->scaleButton->setChecked(false);
//    mMainWidget->translateButton->setChecked(false);
    mMainWidget->scrollButton->setChecked(false);
}

void  WhiteboardDialog::slotScaleClicked() {
    foreach(WhiteboardDialogSess * sess, sessions) {
        sess->m_wbwidget->setMode(WbWidget::Scale);
    }
    mMainWidget->selectButton->setChecked(false);
    mMainWidget->pathButton->setChecked(false);
    mMainWidget->lineButton->setChecked(false);
    mMainWidget->eraseButton->setChecked(false);
    mMainWidget->rectangleButton->setChecked(false);
    mMainWidget->ellipseButton->setChecked(false);
    mMainWidget->imageButton->setChecked(false);
    mMainWidget->textButton->setChecked(false);
    mMainWidget->rotateButton->setChecked(false);
//    mMainWidget->scaleButton->setChecked(false);
    mMainWidget->translateButton->setChecked(false);
    mMainWidget->scrollButton->setChecked(false);
}

void  WhiteboardDialog::slotScrollClicked() {
    foreach(WhiteboardDialogSess * sess, sessions) {
        sess->m_wbwidget->setMode(WbWidget::Scroll);
    }
    mMainWidget->selectButton->setChecked(false);
    mMainWidget->pathButton->setChecked(false);
    mMainWidget->lineButton->setChecked(false);
    mMainWidget->eraseButton->setChecked(false);
    mMainWidget->rectangleButton->setChecked(false);
    mMainWidget->ellipseButton->setChecked(false);
    mMainWidget->imageButton->setChecked(false);
    mMainWidget->textButton->setChecked(false);
    mMainWidget->rotateButton->setChecked(false);
    mMainWidget->scaleButton->setChecked(false);
    mMainWidget->translateButton->setChecked(false);
//    mMainWidget->scrollButton->setChecked(false);
}

void  WhiteboardDialog::slotUndoClicked() {
    int i;
    if(tabWidget != NULL) {
        i = tabWidget->currentIndex();
    } else {
        i = 0;
    }
    sessions.at(i)->m_wbwidget->undo();
}

void  WhiteboardDialog::slotPrintClicked() {
    int i;
    if(tabWidget != NULL) {
        i = tabWidget->currentIndex();
    } else {
        i = 0;
    }
    WbScene * scene = sessions.at(i)->m_wbwidget->scene;

    QPrinter printer;
    QPrintDialog dlg( &printer );

    if( dlg.exec() == QDialog::Accepted )
    {
        QPainter painter( &printer );
        scene->render( &painter, printer.pageRect(), scene->sceneRect(), Qt::KeepAspectRatio );
    }

}

void  WhiteboardDialog::slotQuitClicked() {
//    switch ( KMessageBox::warningYesNoCancel( this,
//        i18n("Are you sure you want to quit?")) ) {
//    case KMessageBox::Yes :
//        close();
//    case KMessageBox::No :
//        return;
//    default: // cancel
//        return;
//    }
    close();
}

void  WhiteboardDialog::slotClearClicked() {
    int i;
    if(tabWidget != NULL) {
        i = tabWidget->currentIndex();
    } else {
        i = 0;
    }
    sessions.at(i)->m_wbwidget->clear();
    mMainWidget->undoButton->setEnabled(false);
}

void  WhiteboardDialog::slotImportClicked() {
    kDebug() << "clicked";
    switch ( KMessageBox::warningYesNoCancel( this, i18n("Import will clear your current screen, are you sure?")) ) {
        case KMessageBox::Yes :
            break;
        case KMessageBox::No :
            return;
        default: // cancel
            return;
    }
    int i;
    if(tabWidget != NULL) {
        i = tabWidget->currentIndex();
    } else {
        i = 0;
    }

    QString filename = QFileDialog::getOpenFileName(this, i18n("Load XML file"), "~/",  i18n("Whiteboard file (*.xml)"));

    mMainWidget->undoButton->setEnabled(false);

    if(!filename.isEmpty()) {
        sessions.at(i)->m_wbwidget->clear();
        QFile file( filename );
        if( !file.open( IO_ReadOnly ) ) {
            QMessageBox::warning( this, "Loading", i18n("Failed to load file.") );
            return;
        }
        QDomDocument doc;
        if( !doc.setContent( &file ) ) {
            QMessageBox::warning( this, "Loading", i18n("Failed to load file.") );
            file.close();
            return;
        }
        file.close();

        QDomNodeList list = doc.elementsByTagName("wb");
        if(list.size() != 1) {
            QMessageBox::warning( this, "Loading", i18n("Failed to load file.") );
            return;
        }
        QDomElement wb = list.at(0).toElement();//first and only element
        wb.setAttribute("session", "s1");

        QDomNodeList news = wb.childNodes();
//        if(news.isEmpty())
        for(int j=0; j< news.size(); j++) {
            news.at(j).toElement().setAttribute("id",    sessions.at(i)->m_wbwidget->scene->newId());
            news.at(j).toElement().setAttribute("index", sessions.at(i)->m_wbwidget->scene->newId());
        }

        sessions.at(i)->m_wbwidget->importWb(wb);

        QDomDocument doc2;
        doc2.createElement("chuj");
        doc2.appendChild(wb);
        kDebug() << doc2.toString();
    }
}

void  WhiteboardDialog::slotExportClicked() {
    kDebug() << "clicked";
    int i;
    if(tabWidget != NULL) {
        i = tabWidget->currentIndex();
    } else {
        i = 0;
    }
    QDomDocument doc;
    QDomElement _wb = doc.createElement("wb");
    _wb.setAttribute("xmlns","http://jabber.org/protocol/svgwb");
    foreach(WbItem* item, sessions.at(i)->m_wbwidget->scene->elements()) {
        QDomElement _new = doc.createElement("new");
        _new.appendChild( item->svg() );
        _wb.appendChild( _new );
        Q_UNUSED(item);
//        doc.appendChild( item->svg() );
    }
    doc.appendChild(_wb);
    QString filename = QFileDialog::getSaveFileName(this, i18n("Save file as"), "~/whiteboard-"+sessions.at(i)->jid+".xml", i18n("Whiteboard file (*.xml)"));

    if(filename.isEmpty()) return;
    
    QFile file( filename );
    if( !file.open( IO_WriteOnly ) )
    {
        QMessageBox::warning( this, "Saving", "Failed to save file." );
        return;
    }
    QTextStream ts( &file );
    ts << doc.toString();
    file.close();
}

void  WhiteboardDialog::slotSaveClicked() {
    int i;
    kDebug() << "clicked";

    if(tabWidget != NULL) {
        i = tabWidget->currentIndex();
    } else {
        i = 0;
    }
//    sessions.at(i)->m_wbwidget->size();
    
    QImage img(sessions.at(i)->m_wbwidget->size(), QImage::Format_RGB32);
//    QImage img();
    QPainter painter(&img);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(0,0,sessions.at(i)->m_wbwidget->size().width(),sessions.at(i)->m_wbwidget->size().height(),Qt::white);
//    painter.setBackgroundColor(QColor( 92,  53, 102));
    sessions.at(i)->m_wbwidget->scene->render(&painter);
    QString filename = QFileDialog::getSaveFileName(this, i18n("Save image as"), "whiteboard-"+sessions.at(i)->jid+".png", i18n("Images (*.png *.jpg)"));
    img.save(filename);
}

void WhiteboardDialog::tabOffline(QString jid) {
    if(tabWidget != NULL) {
        for (int i = 0; i < tabWidget->count(); i++) {
            if(tabWidget->tabText(i).remove("&") == jid) {
                tabWidget->setTabIcon(i, KIcon("offline"));
            }
        }
    } else {
        setCaption( i18n("Whiteboard with %1 *USER OFFLINE*", jid) );
    }
}
void WhiteboardDialog::slotUserOffline(QString jid) {
    kDebug() << jid;
    int i = 0;
    foreach(WhiteboardDialogSess * sess, sessions) {
        if(sess->jid == jid) {
            WhiteboardDialog::tabOffline(jid);
            break;
        }
        i++;
    }
}

void WhiteboardDialog::slotElementReady(const QDomElement &element) {
    Q_UNUSED (element)
    int i;
    if(tabWidget != NULL) {
        i = tabWidget->currentIndex();
    } else {
        i = 0;
    }
    if (sessions.at(i)->m_wbwidget->elements.size() > 0) {
        mMainWidget->undoButton->setEnabled(true);
    }
}

void WhiteboardDialogSess::slotElementReady(const QDomElement &element) {
    kDebug() << "element is ready to be send ";
    emit newWb(element);
//
    QDomDocument doc;
    doc.createElement("element");//it doesnt convert to string without it
    doc.appendChild(element);

//    kDebug() << "wb to send";
    Kopete::Message msg( chatSession->myself(), chatSession->members() );
    msg.setDirection( Kopete::Message::Outbound );
    doc.appendChild ( element );
    msg.setPlainBody(doc.toString());
    chatSession->sendMessage( msg );
}


bool WhiteboardDialog::queryClose()
{
    switch ( KMessageBox::warningYesNoCancel( this,
        i18n("Are you sure you want to quit?")) ) {
    case KMessageBox::Yes :
        return true;
    case KMessageBox::No :
        return false;
    default: // cancel
        return false;
    }
    return true;
}

WhiteboardDialog::~WhiteboardDialog()
{
    emit destroyedDialog(this);
    kDebug() << "called.";
	// end the search function, if it's still running
//	mSearching = false;
    foreach(WhiteboardDialogSess * sess, sessions) {
        delete sess;
    }
    delete mMainWidget;
}


#include "whiteboarddialog.moc"

