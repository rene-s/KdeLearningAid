/***************************************************************************
 *   Copyright (C) 2004 by Rene Schmidt                                    *
 *   rene@reneschmidt.de                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifndef _KLAIDWIDGET_H_
#define _KLAIDWIDGET_H_

#include "klaidwidgetbase.h"
#include <ver.h>

#include <qspinbox.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qtooltip.h>
#include <qlabel.h>
#include <qvbox.h>
#include <qcolor.h>
#include <qhbox.h>
#include <qpushbutton.h>
#include <qfont.h>

#include <ksimpleconfig.h>
#include <kmessagebox.h>
#include <kcolorcombo.h>
#include <kurlrequester.h>
#include <kfontrequester.h>
#include <klaid.h>
#include <kcolorbutton.h>
#include <ksystemtray.h>
#include <kpopupmenu.h>
#include <kaction.h>
#include <kiconloader.h>
#include <kaboutdata.h>
#include <kaboutdialog.h>
#include <kaboutapplication.h>
#include <klocale.h>
#include <khelpmenu.h>

#define TOGGLE_ITEM "TOGGLE_ITEM"
#define ABOUT_ITEM  "ABOUT_ITEM"

class KLAidWidget : public KLAidWidgetBase
{
    Q_OBJECT

public:
    KLAidWidget(QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~KLAidWidget();

public slots:
    /*$PUBLIC_SLOTS$*/
    virtual void okClicked();
    virtual void closeEvent(QCloseEvent *event);
    virtual void toggleTimer();
    virtual void about();
    virtual void showPopUp(QString &caption, QString &text, uint &tOut);
    virtual void receiveError(uint code);
    
    virtual void turnOffUniq();
    virtual void cancelClicked();
    
    virtual void reportExpired();
    virtual void changeRepMode();
    virtual void enableOKButton();
    virtual void sanitizeMinStay();

        
protected:
    /*$PROTECTED_FUNCTIONS$*/
    void _stopTimer();
    void _startTimer();
    void _restoreConfig();
    void _commitVars();
    void _readConfig();
    QFont * fntCaption;
    QFont * fntBody;
    
protected slots:
    /*$PROTECTED_SLOTS$*/


};

#endif

