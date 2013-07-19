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

#include <kmainwindow.h>
#include <klocale.h>
#include <qstring.h>

#include "klaid.h"
#include "klaidwidget.h"

#include <time.h>

/// @todo: not so good...
QMap<uint,QString> iTitles;
QMap<uint,QString> iTexts;
uint mapIdx;

/**
 * @short KLAid constructor
 * @author Rene Schmidt <rene@reneschmidt.de>
 * @version 0.1
 */
KLAid::KLAid(QWidget * parent) : KMainWindow(0, "KLAid")
{
  this->_defaultConfig();

  // Create DOM parser and connect signals;
  _dom = new DomParser;
  QObject::connect(_dom, SIGNAL(error(uint)), parent, SLOT(receiveError(uint)));
  
  // Create timer and connect slot
  _timer = new QTimer();
  QObject::connect(_timer, SIGNAL(timeout()), this, SLOT(popMark()));

  srand(static_cast<unsigned>(time(0)));
}


/**
 * @short Timer status is open to the public
 * @author Rene Schmidt <rene@reneschmidt.de>
 * @version 0.1
 */
bool KLAid::timerRunning()
{ 
  return _timer->isActive();
}


/**
 * @short Changes timer interval
 * @author Rene Schmidt <rene@reneschmidt.de>
 * @version 0.1
 */
void KLAid::chngInterval(const uint seconds)
{ 
  _pIntval = seconds*1000;
}


/**
 * @short Changes minimum popup stay
 * @author Rene Schmidt <rene@reneschmidt.de>
 * @version 0.1
 */
void KLAid::chngMinStay(const uint seconds)
{ 
  _pMinTimeout = seconds*1000;
}


/**
 * @short Changes popup chance
 * @author Rene Schmidt <rene@reneschmidt.de>
 * @version 0.1
 */
void KLAid::chngChance(const uint chance)
{ 
  _diceThreshold = chance-1;
}


/**
 * @short Enables or disables overlapping popups
 * @author Rene Schmidt <rene@reneschmidt.de>
 * @version 0.1
 */
void KLAid::chngOverlapPops(const bool overlap)
{
  _overlapPops = overlap;
}


/**
 * @short Stops timer
 * @author Rene Schmidt <rene@reneschmidt.de>
 * @version 0.1
 */
void KLAid::stopTimer()
{ 
  if(_timer->isActive()==TRUE)
    _timer->stop();
}


/**
 * @short Starts timer
 * @author Rene Schmidt <rene@reneschmidt.de>
 * @version 0.1
 */
void KLAid::startTimer(const bool force=FALSE)
{ 
  if(_timer->isActive()==FALSE || force==TRUE)
    _timer->start(_pIntval, FALSE);
//     qDebug("Timer started.");
//     qDebug("Next PopUp appearing:");
//     qDebug(projectTime(_pIntval/1000));
}


/**
 * @short Loads and verifies a KVTML file
 * @author Rene Schmidt <rene@reneschmidt.de>
 * @version 0.1
 */
bool KLAid::loadKVTML(const QString datei)
{
  this->_clearConfig();
  
  if(!datei.isEmpty() && _dom->verifyFile(datei))
    _dom->parseFile();
  else
    return FALSE;

  _preWrapText();

  // Create a new set of random numbers
  this->_shuffleNums();

  return TRUE;
}


/**
 * @short Creates a set of random numbers
 * @author Rene Schmidt <rene@reneschmidt.de>
 * @version 0.1
 */
void KLAid::_shuffleNums()
{
  _rndNums = NULL;
  if(_rndSeqMode==TRUE && _uniqItemsMode==TRUE)
  {
    randomNumbers *rnd = new randomNumbers;
    _rndNums = rnd->manyNumbers(0, mapIdx, mapIdx+1, TRUE);
    delete rnd;
  }
}


/**
 * @short Checks for a KVTML file
 * @author Rene Schmidt <rene@reneschmidt.de>
 * @version 0.1
 */
bool KLAid::checkKVTML(const QString datei)
{
  if(!datei.isEmpty() && _dom->verifyFile(datei))
    return TRUE;
  else
    return FALSE;
}


/**
 * @short Clears config vars
 * @author Rene Schmidt <rene@reneschmidt.de>
 * @version 0.1
 */
void KLAid::_clearConfig()
{
  _pCount = 0;
  mapIdx = 0;
  iTitles.clear();
  iTexts.clear();
}


void KLAid::_defaultConfig()
{
    mapIdx = 0;

    _deskW = 1024;
    _pCount = 0;
    
    // Configurables
    _pIntval = 30000;
    _pMinTimeout = 4000;
    _diceThreshold = 100;
    _reps = 999;
    _lFinite = FALSE;
    _overlapPops = FALSE;

    _rndSeqMode = TRUE;
    _uniqItemsMode = TRUE;
}


/**
 * @short Changes repetition mode
 * @author Rene Schmidt <rene@reneschmidt.de>
 * @version 0.1
 */
void KLAid::chngRepMode(const uint mode)
{ 
  _reps = mode;
  _lFinite = (mode == 999 ? FALSE : TRUE);
}


void KLAid::chngRndSeqMode(const bool mode)
{
  _rndSeqMode = mode;
}


void KLAid::chngUniqItemsMode(const bool mode)
{
  _uniqItemsMode = mode;
}


/**
 * @short Pre-wraps text
 * @author Rene Schmidt <rene@reneschmidt.de>
 * @version 0.1
 */
void KLAid::_preWrapText()
{
  uint w=_deskW/3;
  QPainter pnt(this);
  QFontMetrics fm = pnt.fontMetrics();
  
  for (uint i=0; i<mapIdx+1; i++)
  {
    KWordWrap *wwTitle = KWordWrap::formatText(fm, QRect(0, 0, w, -1), 0, iTitles[i]);
    iTitles[i] = wwTitle->wrappedString();
    delete wwTitle;
    KWordWrap *wwText = KWordWrap::formatText(fm, QRect(0, 0, w, -1), 0, iTexts[i]);
    iTexts[i] = wwText->wrappedString();
    delete wwText;
  }
}


/**
 * @short Throws a dice
 * @author Rene Schmidt <rene@reneschmidt.de>
 * @version 0.1
 */
bool KLAid::_throwDice() 
{ 
  return (static_cast<unsigned>(rand()%(99))<=_diceThreshold ? TRUE : FALSE);
} 


/**
 * @short Delivers human readable time
 * @author Rene Schmidt <rene@reneschmidt.de>
 * @version 0.1
 */
QString KLAid::projectTime(const uint seconds)
{
  QTime n(QTime::currentTime());         
  QTime t;
  t = n.addSecs(seconds);          
  return t.toString(Qt::LocalDate);
}


/**
 * @short Emits popup signal
 * @author Rene Schmidt <rene@reneschmidt.de>
 * @version 0.1
 */
void KLAid::popMark()
{
  // Reshuffle if a new cycle begins
  if(_pCount%(mapIdx+1)==0 && this->_rndSeqMode==TRUE && _uniqItemsMode==TRUE)
    this->_shuffleNums();
    
  // End loop?
  if ((_lFinite == TRUE && _pCount>((mapIdx+1)*_reps)-1) || iTitles.isEmpty()==TRUE)
  {
    emit loopExpired();
    return;
  }
  
  // Skip or show popup
  switch(this->_throwDice())
  {
    default:
    case FALSE:
      break;
    case TRUE:
      uint i = _nxtIdx();
      uint tOut = _pMinTimeout+((iTitles[i].length()+iTexts[i].length())*65);
    
      if (_overlapPops==FALSE && tOut>_pIntval) tOut = _pIntval;
      
      emit popNow(iTitles[i], iTexts[i], tOut);
      break;
  }

  _pCount++;
}


uint KLAid::_nxtIdx()
{
  if(this->_rndSeqMode==TRUE)
    return (_uniqItemsMode==TRUE ?      (mapIdx==0 ? 0 : _rndNums[((_pCount)%(mapIdx+1))])        : rand()%(mapIdx+1));
  else
    return _pCount%(mapIdx+1);
}


/**
 * @short Destructor
 * @author Rene Schmidt <rene@reneschmidt.de>
 * @version 0.1
 */
KLAid::~KLAid()
{
  delete _dom;
  delete _timer;
}


////////////////////////////////////////////////

QDomElement root;
bool ready2Parse = FALSE;

DomParser::DomParser()
{}

bool DomParser::verifyFile(const QString &fileName)
{
  QFile file(fileName);
  if(!file.open(IO_ReadOnly))
  {
    emit error(2); // "Failed to load file."
    return FALSE;
  }

  QString errorStr;
  int errorLine;
  int errorColumn;
  QDomDocument doc;
  if (!doc.setContent(&file, TRUE, &errorStr, &errorLine, &errorColumn)) 
  {
    emit error(1); // error in file structure
    return FALSE;
  }

  root = doc.documentElement();
  if (root.tagName() != "kvtml") 
  {
    emit error(4); // not a kvtml file
    return FALSE;
  }

  ready2Parse = TRUE;
  return TRUE;
}

void DomParser::parseFile()
{
  if(ready2Parse==FALSE)
    return;

  int l=-1;
  
  QDomNode node = root.firstChild();
  while (!node.isNull()) 
  {
    if (node.toElement().tagName() == "e")
      _parseEntry(node.toElement(), l);
    node = node.nextSibling();
  }
  mapIdx=l;
}

void DomParser::_parseEntry(const QDomElement &element, int &l)
{
  if(ready2Parse==FALSE)
    return;

  QDomNode node = element.firstChild();
   
  // Block: only save *complete* o-t pairs
  // Drawback: o must be the first element of e. Is that a problem? We'll see.
  if (node.toElement().tagName() == "o") {
    QDomNode c = node.firstChild();
    if (c.nodeType() == QDomNode::TextNode)
    {
      l++;
    }
  }

  while (!node.isNull()) {
    if (node.toElement().tagName() == "e") {
      _parseEntry(node.toElement(), l);
    } else if (node.toElement().tagName() == "o") {
        QDomNode childNode = node.firstChild();
        while (!childNode.isNull()) {
          if (childNode.nodeType() == QDomNode::TextNode) {
            iTitles[l] = QString(childNode.toText().data());
            break;
          }
          childNode = childNode.nextSibling();
        }
    } else if (node.toElement().tagName() == "t") {
      QDomNode childNode = node.firstChild();
      while (!childNode.isNull()) 
      {
        if (childNode.nodeType() == QDomNode::TextNode && !iTitles[l].isEmpty()) {
          iTexts[l] = QString(childNode.toText().data());
          break;
        }
        childNode = childNode.nextSibling();
      }
    }
    node = node.nextSibling();
  }
}

#include "klaid.moc"
