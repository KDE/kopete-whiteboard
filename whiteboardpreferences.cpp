/*************************************************************************
 * Copyright <2011>  <Lukas Pohl> <dr.zoidberg1969@yahoo.com>         *
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

// Qt
#include <qlayout.h>
#include <qcheckbox.h>
#include <qgroupbox.h>

// KDE
#include <kdebug.h>
#include <knuminput.h>
#include <kapplication.h>
#include <kgenericfactory.h>
#include <kconfig.h>
// Kopete

//local
#include "kopete_whiteboard.h"
#include "ui_whiteboardprefs.h"
#include "whiteboardpreferences.h"

K_PLUGIN_FACTORY(WhiteboardPreferencesFactory,
        registerPlugin<WhiteboardPreferences>();
        )
K_EXPORT_PLUGIN(WhiteboardPreferencesFactory ( "kcm_kopete_whiteboard", "kopete_whiteboard" ))


WhiteboardPreferences::WhiteboardPreferences ( QWidget *parent, const QVariantList &args )
		: KCModule ( WhiteboardPreferencesFactory::componentData(), parent, args )
{
        kDebug () << "wb preferences start";
        QVBoxLayout* l = new QVBoxLayout( this );
	QWidget* w = new QWidget;
	m_ui = new Ui::WhiteboardPrefsUI;
	m_ui->setupUi( w );
	l->addWidget( w );

	connect(m_ui->chkAutoAccept, SIGNAL(toggled(bool)), this, SLOT(slotAutoAcceptChanged(bool)));
	connect(m_ui->chkNewWindow, SIGNAL(toggled(bool)), this, SLOT(slotNewWindowChanged(bool)));
}

WhiteboardPreferences::~WhiteboardPreferences() {
    kDebug () << "wb preferences finish";
	delete m_ui;
//	delete m_layout;
}

void WhiteboardPreferences::load() {
        kDebug () << "wb preferences load";

        m_ui->chkAutoAccept->setChecked( WhiteboardConfig::self()->autoAccept() );
        m_ui->chkNewWindow->setChecked( WhiteboardConfig::self()->newWindow() );
}

void WhiteboardPreferences::save() {
        kDebug () << "wb preferences save";

        WhiteboardConfig::self()->setAutoAccept(m_ui->chkAutoAccept->isChecked());
        WhiteboardConfig::self()->setNewWindow(m_ui->chkNewWindow->isChecked());

	WhiteboardConfig::self()->writeConfig();
}

void WhiteboardPreferences::defaults() {
    	kDebug () << "wb preferences defaults";

	WhiteboardConfig::self()->setDefaults ();
	load();
	changed();
}

void WhiteboardPreferences::slotAutoAcceptChanged(bool on)
{
	Q_UNUSED(on);
	emit KCModule::changed(true);
}

void WhiteboardPreferences::slotNewWindowChanged(bool on)
{
	Q_UNUSED(on);
	emit KCModule::changed(true);
}

#include "whiteboardpreferences.moc"
