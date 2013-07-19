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

#include "klaidwidget.h"
#include <kuniqueapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <ver.h>
#include <stdio.h>
#include <stdlib.h>

static const char description[] =
    I18N_NOOP("An KVTML file contents viewer");

int main(int argc, char **argv)
{
    KAboutData about("klaid", I18N_NOOP("K Learning Aid"), "0.0", description,
                     KAboutData::License_GPL, "(C) %{YEAR} Rene Schmidt", 0, 0, "rene@reneschmidt.de");
    about.addAuthor( "Rene Schmidt", 0, "rene@reneschmidt.de" );

    KCmdLineArgs::init(argc, argv, &about);

    if (!KUniqueApplication::start())
    {
      fprintf(stderr, "KLAid is already running!\n");
      exit(0);
    }
    KUniqueApplication app;

//     KApplication app;

    KLAidWidget *mainWin = 0;
    mainWin = new KLAidWidget;
    app.setMainWidget( mainWin );
    return app.exec();
}

