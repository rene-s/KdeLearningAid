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

/// @todo: BUG: repeat steht auf custom, jetzt custom nochmal waehlen, wert aendert sich immer auf 1
 
#include <qlabel.h>

#include "klaidwidget.h"
KLAid * aut;
KSystemTray * tray;
KToggleAction * enPopAction;

QFont * fntCaption;
QFont * fntBody;
QColor * tmpFgColor;
QColor * tmpBgColor;

QString file;
uint oldRepVal;

KURLRequester * urlLine;
QComboBox * repModeBox;
QSpinBox * loopsAmount;
QSpinBox * popUpInterval;
QComboBox * pIntvalUnit;
QSpinBox * popUpChance;
QSpinBox * minStay;
QCheckBox * overlapPops;
QCheckBox * uniqueItemsBox;
QCheckBox * rndSeqBox;

bool cfgOkay;
/**
 * @short Initialize
 * @author Rene Schmidt <rene@reneschmidt.de>
 * @version 0.1
 */
KLAidWidget::KLAidWidget(QWidget* parent, const char* name, WFlags fl)
        : KLAidWidgetBase(parent,name,fl)
{
  this->setCaption(i18n("K Learning Aid %1").arg(VER));

  cfgOkay = FALSE;
  
  oldRepVal = 1;
  
  // Center window
  QDesktopWidget *d = QApplication::desktop();
  int dW = d->width();      int dH = d->height();
  int wW = this->width();   int wH = this->height();
  this->setGeometry((dW-wW)/2, (dH-wH)/2, wW, wH);
  
  // Create tray icon
  tray = new KSystemTray(this, "TimerTray");
  tray->setPixmap(SmallIcon("ktimer"));
  tray->show();

  QToolTip::add(tray, i18n("K Learning Aid %1").arg(VER));
  
  // Create logic backend
  aut = new KLAid(this);
  aut->desktopWidth = d->width();
  aut->setCentralWidget(tray);
  
  QObject::connect(aut, SIGNAL(loopExpired()), this, SLOT(reportExpired()));
  QObject::connect(aut, SIGNAL(popNow( QString&, QString&, uint&)),
                    this, SLOT(showPopUp(QString&, QString&, uint&)));

  // Add some tray menu items
  KPopupMenu * TrayMenu = tray->contextMenu(); 

  enPopAction = new KToggleAction(  i18n("&Enable PopUps"),
                                    0, 
                                    this, 
                                    SLOT(toggleTimer()), 
                                    parentWidget(), 
                                    TOGGLE_ITEM);

  KAction * AboutAction = new KAction(  i18n("&About"), 
                                        0, 
                                        this, 
                                        SLOT(about()), 
                                        parentWidget(), 
                                        ABOUT_ITEM);
      
  TrayMenu->removeItemAt(0);
  TrayMenu->insertTitle(SmallIcon("ktimer"), QString("KLAid"), 0, 0);
  
  enPopAction->plug(TrayMenu, 1);
  enPopAction->setEnabled(FALSE);
  
  AboutAction->plug(TrayMenu, 2);

  connect(popUpInterval, SIGNAL(valueChanged(const QString&)), this, SLOT(sanitizeMinStay()));
  connect(urlLine, SIGNAL(urlSelected(const QString&)), this, SLOT(enableOKButton()));
  connect(minStay, SIGNAL(valueChanged(const QString&)), this, SLOT(sanitizeMinStay()));
  connect(pIntvalUnit, SIGNAL(activated(const QString&)), this, SLOT(sanitizeMinStay()));
  connect(overlapPops, SIGNAL(stateChanged(int)), this, SLOT(sanitizeMinStay()));
  connect(rndSeqBox, SIGNAL(stateChanged(int)), this, SLOT(turnOffUniq()));
  connect(repModeBox, SIGNAL(activated(const QString&)), this, SLOT(changeRepMode()));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelClicked()));
  connect(okButton, SIGNAL(clicked()), this, SLOT(okClicked()));
//  connect(defaultsButton, SIGNAL(clicked()), this, SLOT(resetConfig()));

  // Set default fonts for popups
  fntCaption = new QFont();
  fntCaption->setBold(TRUE);
  kFontRequester1->setFont(*fntCaption);
  fntBody = new QFont();
  kFontRequester2->setFont(*fntBody);

  // Set default fonts for popups
  tmpFgColor = new QColor(this->foregroundColor());
  tmpBgColor = new QColor(this->backgroundColor());

  fgCol->setColor(this->foregroundColor());
  bgCol->setColor(this->backgroundColor());

  this->_readConfig();
  this->show();
}

KLAidWidget::~KLAidWidget()
{
  delete aut;
  delete tray;
  delete enPopAction;
  delete fntBody;
  delete fntCaption;
}


/**
 * @short Checks for valid config and launches everything when OK
 * @author Rene Schmidt <rene@reneschmidt.de>
 * @version 0.1
 */
void KLAidWidget::okClicked()
{
  if (urlLine->url().isEmpty())
  {
    KMessageBox::error(this, i18n("Configuration incomplete. Timer halted."), i18n("Error"));
    return;
  }

  file = urlLine->url();

  this->_commitVars();

  // Konfiguration exportieren
  
  if(aut->loadKVTML(urlLine->url())==FALSE)
  {
    cfgOkay = FALSE;
    urlLine->clear();
    _stopTimer();
    (void)KPassivePopup::message( QString("KLAid information"),
                              i18n("PopUps disabled."),
                              SmallIcon("ktimer"), tray, 0, 8*1000);
    
  } else {
    cfgOkay = TRUE;
    *fntCaption = kFontRequester1->font();
    *fntBody = kFontRequester2->font();
    *tmpFgColor = fgCol->color();
    *tmpBgColor = bgCol->color();
    _startTimer();
  }
}

/**
 * @short Commits variables to backend logic
 * @author Rene Schmidt <rene@reneschmidt.de>
 * @version 0.1
 */
void KLAidWidget::_commitVars()
{
  uint f = (pIntvalUnit->currentItem()==0 ? 1 : 60);
  uint pIntval = popUpInterval->value()*f;
  aut->chngInterval(pIntval);
    
  aut->chngMinStay(minStay->value());
  aut->chngChance(popUpChance->value());
  aut->chngOverlapPops(overlapPops->isChecked());
  aut->chngRepMode(loopsAmount->value());
  aut->chngRndSeqMode(rndSeqBox->isChecked());
  aut->chngUniqItemsMode(uniqueItemsBox->isChecked());
}

/**
 * @short Toggles popup timer.
 * @author Rene Schmidt <rene@reneschmidt.de>
 * @version 0.1
 */
void KLAidWidget::toggleTimer()
{
  switch(aut->timerRunning())
  {
    case TRUE:
      _stopTimer();
      (void)KPassivePopup::message(QString("KLAid ")+i18n("information"),
                              i18n("PopUps disabled."),
                              SmallIcon("ktimer"), tray, 0, 8*1000);
      break;
    case FALSE:
      _startTimer();
      break;
  }
}


/**
 * @short Stops timer.
 * @author Rene Schmidt <rene@reneschmidt.de>
 * @version 0.1
 */
void KLAidWidget::_stopTimer()
{
  // Reset tooltip
  QToolTip::add(tray, i18n("K Learning Aid %1").arg(VER));
    
  aut->stopTimer();
  enPopAction->setChecked(FALSE);
  if(urlLine->url().isEmpty())
  {
    okButton->setEnabled(FALSE);
    enPopAction->setEnabled(FALSE);
    KMessageBox::error(this, i18n("Disabling popups, there is no valid configuration."), i18n("Error"));
  }
}


/**
 * @short Starts timer
 * @author Rene Schmidt <rene@reneschmidt.de>
 * @version 0.1
 */
void KLAidWidget::_startTimer()
{
    // Tell user when the first popup will be shown
    QToolTip::add(tray, i18n("K Learning Aid %1").arg(VER) +
                        i18n("\nFirst PopUp will be shown at ") +
                        aut->projectTime(aut->getInterval()));
                        
//     showPopUp(i18n("KLAid information"),
//     i18n("PopUps enabled. You may stop or quit the program\nvia the KLAid icon in the system tray."),
//     8*1000);
    
    (void)KPassivePopup::message( QString("KLAid")+" "+i18n("information"),
                                  i18n("PopUps enabled. You may stop or quit the program\nvia the KLAid icon in the system tray."),
                                  SmallIcon("ktimer"), tray, 0, 8*1000);
    enPopAction->setChecked(TRUE);
    enPopAction->setEnabled(TRUE);
    this->hide();
    aut->startTimer(TRUE);
}


/**
 * @short Pops an about msg box
 * @author Rene Schmidt <rene@reneschmidt.de>
 * @version 0.1
 */
void KLAidWidget::about()
{
  QMessageBox::about(this, i18n("About"), i18n("<h3>KLAid - K Learning Aid %1</h3><p>(c) 2004 by Rene Schmidt</p><p>This tiny program is free-as-in-freedom software, see GPL.txt for more information. See http://log.reneschmidt.de/ for updates. Icon &quot;borrowed&quot; from the CrystalSVG package by Everaldo.</p>").arg(VER));
}


/**
 * @short Shows a popup
 * @author Rene Schmidt <rene@reneschmidt.de>
 * @version 0.1
 */
void KLAidWidget::showPopUp(QString &caption, QString &text, uint &tOut)
{
  // Tell user when the next popup will be shown
  QToolTip::add(tray, i18n("K Learning Aid %1").arg(VER) + "\nNext PopUp will be shown at " + aut->projectTime(aut->getInterval()));

 // this is a candidate for subclassing... Copied in part from kpassivepopup.cpp (c) by KDE Team
    KPassivePopup *pop = new KPassivePopup( tray );

    pop->setPaletteForegroundColor(*tmpFgColor);
    pop->setPaletteBackgroundColor(*tmpBgColor);
    
    QVBox *vb = new QVBox( pop );
    vb->setSpacing( KDialog::spacingHint() );
 
      QHBox *hb=0;
      hb = new QHBox(vb);
      hb->setMargin(0);
      hb->setSpacing(KDialog::spacingHint() );
      QLabel *ttlIcon=0;
      ttlIcon = new QLabel( hb, "title_icon" );
      ttlIcon->setPixmap( SmallIcon("ktimer") );
      ttlIcon->setAlignment( AlignLeft );

      if ( !caption.isEmpty() ) {
        QLabel *ttl=0;
        ttl = new QLabel(caption, hb ? hb : vb, "title_label");
  
        ttl->setFont( *fntCaption );
        ttl->setAlignment( Qt::AlignHCenter );
            if ( hb )
                hb->setStretchFactor( ttl, 10 ); // enforce centering
      }
  
      if ( !text.isEmpty() ) {
        QLabel *msg=0;
        msg = new QLabel( text, vb, "msg_label" );
        msg->setFont( *fntBody );
        msg->setAlignment( AlignLeft );
      }

    pop->setTimeout(tOut);
    //pop->setView( title, txt, SmallIcon("ktimer"));
    pop->setView(vb);
    pop->show();
}


/**
 * @short Receives error signals from the DOM parser and spits out error msgs
 * @author Rene Schmidt <rene@reneschmidt.de>
 * @version 0.1
 */
void KLAidWidget::receiveError( uint code )
{
  switch(code)
  {
    case 8:
      KMessageBox::error(this, i18n("File is probably well-formed, but\nis containing empty elements. KLAid\ncannot handle such files at the moment.\nYou might consider this a bug. It will be fixed later."), i18n("Error"));
      break;
    case 4:
      KMessageBox::error(this, i18n("File is not in KVTML format."), i18n("Error"));
      break;
    case 2:
      KMessageBox::error(this, i18n("Could not open file. Please check the permissions."), i18n("Error"));
      break;
    case 1:
      KMessageBox::error(this, i18n("KVTML file not well-formed."), i18n("Error"));
      break;
    default:
      KMessageBox::error(this, i18n("Unknown error."), i18n("Error"));
      break;
  }
}


/**
 * @short Checks if minStay is lower than popup interval, avoids overlapping popups
 * @author Rene Schmidt <rene@reneschmidt.de>
 * @version 0.1
 */
void KLAidWidget::sanitizeMinStay()
{
  if(overlapPops->isChecked())
    return;
    
  uint f = (pIntvalUnit->currentItem()==0 ? 1 : 60);
  uint pIntval = popUpInterval->value()*f;
  if(static_cast<unsigned>(minStay->value())>pIntval)
    minStay->setValue(pIntval);
}


/**
 * @short Checks if OK button can be enabled and does so if possible
 * @author Rene Schmidt <rene@reneschmidt.de>
 * @version 0.1
 */
void KLAidWidget::enableOKButton()
{
  if(aut->checkKVTML(urlLine->url())==FALSE)
  {
    cfgOkay=FALSE;
    urlLine->clear();
    enPopAction->setEnabled(FALSE);
    okButton->setEnabled(FALSE);
  } else {
    okButton->setEnabled(TRUE);
    cfgOkay=TRUE;
    this->_commitVars();
  }
}

/**
 * @short Turns off "unique items" checkbox if user wishes not to have random ordered items
 * @author Rene Schmidt <rene@reneschmidt.de>
 * @version 0.1
 */
void KLAidWidget::turnOffUniq()
{
  if(rndSeqBox->isChecked()==FALSE)
  {
    uniqueItemsBox->setChecked(TRUE);
    uniqueItemsBox->setEnabled(FALSE);
  } else {
    uniqueItemsBox->setChecked(aut->getUniqItemsMode());
    uniqueItemsBox->setEnabled(TRUE);
  }
}


/**
 * @short Enables or disables certain widgets depending on users' settings
 * @author Rene Schmidt <rene@reneschmidt.de>
 * @version 0.1
 */
void KLAidWidget::changeRepMode()
{
    switch(repModeBox->currentItem())
    {
    case 1:
      loopsAmount->setEnabled(TRUE);
      loopsAmount->setValue(oldRepVal);
      break;
    case 0:
    default:
      loopsAmount->setEnabled(FALSE);
      oldRepVal = loopsAmount->value();
      loopsAmount->setValue(999);
      break;
    }
}


/**
 * @short Tell user if loop has expired
 * @author Rene Schmidt <rene@reneschmidt.de>
 * @version 0.1
 */
void KLAidWidget::reportExpired()
{
  this->_stopTimer();
  this->show();
  KMessageBox::information(this, i18n("Loop expired, disabling popups."), i18n("Information"));
}


/**
 * @short Just hides the window and restores widget values
 * @author Rene Schmidt <rene@reneschmidt.de>
 * @version 0.1
 */
void KLAidWidget::cancelClicked()
{
  this->hide(); 
  this->_restoreConfig();
}

/**
 * @short Pulls config data from klaid
 * @author Rene Schmidt <rene@reneschmidt.de>
 * @version 0.1
 */
 void KLAidWidget::_readConfig()
{
  KSimpleConfig conf ("klaidrc");
  conf.setGroup("cfg");

  urlLine->setURL(conf.readEntry("lastfile", ""));

  // Set repeat mode
  loopsAmount->setValue(conf.readNumEntry("repeat", aut->getRepMode()));
  switch(loopsAmount->value())
  {
    case 999:
      loopsAmount->setEnabled(FALSE);
      repModeBox->setCurrentItem(0);
      break;
    default:
      loopsAmount->setEnabled(TRUE);
      repModeBox->setCurrentItem(1);
      break;
  }

  // Set interval
  uint pIntval = conf.readNumEntry("interval", aut->getInterval());
  if(pIntval<60)
  {
    popUpInterval->setValue(pIntval);
    pIntvalUnit->setCurrentItem(0);
  } else {
    popUpInterval->setValue(pIntval/60);
    pIntvalUnit->setCurrentItem(1);
  }

  popUpChance->setValue(conf.readNumEntry("chance", aut->getChance()));
  minStay->setValue(conf.readNumEntry("minstay", aut->getMinStay()));

  overlapPops->setChecked(conf.readBoolEntry("overlap", aut->getOverlapPops()));
  uniqueItemsBox->setChecked(conf.readBoolEntry("unique", aut->getUniqItemsMode()));
  rndSeqBox->setChecked(conf.readBoolEntry("random", aut->getRndSeqMode()));

  fgCol->setColor(conf.readColorEntry("fgcol", tmpFgColor));
  bgCol->setColor(conf.readColorEntry("bgcol", tmpBgColor));
  
  kFontRequester1->setFont(conf.readFontEntry("fgfont", fntCaption));
  kFontRequester2->setFont(conf.readFontEntry("bgfont", fntBody));
  
  this->enableOKButton();
}


/**
 * @short Accepts close event, yes this method is kind of redundant
 * @author Rene Schmidt <rene@reneschmidt.de>
 * @version 0.1
 */
void KLAidWidget::closeEvent(QCloseEvent *event)
{
  if(cfgOkay==TRUE)
  {
    KSimpleConfig conf ("klaidrc");
    conf.deleteGroup("cfg");
    conf.setGroup("cfg");
    conf.writeEntry("lastfile", urlLine->url());
    conf.writeEntry("repeat", loopsAmount->value());
    conf.writeEntry("interval", pIntvalUnit->currentItem()==0 ? popUpInterval->value() : popUpInterval->value()*60);
    conf.writeEntry("minstay", minStay->value());
    conf.writeEntry("chance", popUpChance->value());
    conf.writeEntry("random", rndSeqBox->isChecked() ? QString("on") : QString("off"));
    conf.writeEntry("unique", uniqueItemsBox->isChecked() ? QString("on") : QString("off"));
    conf.writeEntry("overlap", overlapPops->isChecked() ? QString("on") : QString("off"));

    conf.writeEntry("fgfont", kFontRequester1->font().toString());
    conf.writeEntry("bgfont", kFontRequester2->font().toString());

    conf.writeEntry("fgcol", fgCol->color().name());
    conf.writeEntry("bgcol", bgCol->color().name());
  }
  
  event->accept();
}


void KLAidWidget::_restoreConfig()
{
  rndSeqBox->setChecked(aut->getRndSeqMode());
  uniqueItemsBox->setChecked(aut->getUniqItemsMode());
  minStay->setValue(aut->getMinStay());
  popUpChance->setValue(aut->getChance());
  overlapPops->setChecked(aut->getOverlapPops());
  urlLine->setURL(file);
  loopsAmount->setValue(aut->getRepMode());
  
  switch(aut->getRepMode())
  {
    case 999:
      repModeBox->setCurrentItem(0);
      this->changeRepMode();
      break;
    default:
      loopsAmount->setValue(aut->getRepMode());
      repModeBox->setCurrentItem(1);
      break;
  }

  uint pIntval = aut->getInterval();
  if(pIntval<60)
  {
    popUpInterval->setValue(pIntval);
    pIntvalUnit->setCurrentItem(0);
  } else {
    popUpInterval->setValue(pIntval/60);
    pIntvalUnit->setCurrentItem(1);
  }

  fgCol->setColor(*tmpFgColor);
  bgCol->setColor(*tmpBgColor);
  kFontRequester1->setFont(*fntCaption);
  kFontRequester2->setFont(*fntBody);
  //this->sanitizeMinStay();
}

#include "klaidwidget.moc"


