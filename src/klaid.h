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


#ifndef _KLAID_H_
#define _KLAID_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qtimer.h>
#include <qstring.h>
#include <qdatetime.h>
#include <qmessagebox.h>
#include <qdom.h>
#include <qfile.h>
#include <qpainter.h>

#include <kmainwindow.h>
#include <kapplication.h>
#include <kpassivepopup.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kwordwrap.h>

#include <rndclass.h>
#include <ver.h>

/**
 * @short Application Main Window
 * @author Rene Schmidt <rene@reneschmidt.de>
 * @version 0.1
 */


class DomParser;
class randomNumbers;

class KLAid : public KMainWindow
{
    Q_OBJECT
    
public:
    /**
     * Default Constructor
     */
    KLAid(QWidget * parent);

    /**
     * Default Destructor
     */
    virtual ~KLAid();
    
    void chngInterval(const uint seconds);        uint getInterval() { return _pIntval/1000; }
    void chngMinStay(const uint seconds);         uint getMinStay() { return _pMinTimeout/1000; }
    void chngChance(const uint chance);           uint getChance() { return _diceThreshold+1; }
    void chngOverlapPops(const bool overlap);     bool getOverlapPops() { return _overlapPops; }
    void chngRepMode(const uint mode);            uint getRepMode() { return _reps; }

    void chngRndSeqMode(const bool mode);         bool getRndSeqMode() { return _rndSeqMode; }
    void chngUniqItemsMode(const bool mode);      bool getUniqItemsMode() { return _uniqItemsMode; }

    void startTimer(bool force);
    void stopTimer();
    bool timerRunning();
    
    bool loadKVTML(const QString datei);
    bool checkKVTML(const QString datei);
    
    QString projectTime(const uint seconds);

    int desktopWidth;
        
public slots:
    void popMark();

signals:
    void loopExpired();
    void popNow(QString&, QString&, uint&);
    
private:
    bool _throwDice();
    void _preWrapText();
    void _clearConfig();
    void _defaultConfig();
    uint _nxtIdx();
    void _shuffleNums();

    QTimer * _timer;
    DomParser * _dom;
    
    int *_rndNums;
    uint _deskW;
    uint _pCount;
    
    // Configurables
    uint _pIntval;
    uint _pMinTimeout;
    uint _diceThreshold;
    uint _reps;
    bool _lFinite;
    bool _overlapPops;
    bool _uniqItemsMode;
    bool _rndSeqMode;
};



class QDomElement;
class QIODevice;

class DomParser : public QObject
{
      Q_OBJECT
      
public:
    DomParser();
    void parseFile();
    
public slots:
    bool verifyFile(const QString &fileName);
    
signals:
    void error(uint);

private:
    void _parseEntry(const QDomElement &element, int &l);

  
};

#endif // _KLAID_H_
