/***************************************************************************
 *   Copyright (C) 2004 by Rene Schmidt                                    *
 *   rs@reneschmidt.de                                                     *
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
 
#include <cstdlib>
#include <ctime>
#include "./rndclass.h"

using namespace std;
int aux = 0;

/* Konstruktor, initialisiert die RND-Maschine 
*/
randomNumbers::randomNumbers() {
  srand(static_cast<unsigned>(time(0)+(aux++)));
}

/* Destruktor, leer
*/
randomNumbers::~randomNumbers() {}

/* Erzeugt genau EINE Zufallszahl */
int randomNumbers::makeNumber(int von, int bis) {
  if (von>bis || (von == 0 && bis == 0))
    return von+1;
  return rand()%(bis-von+1)+von;
}

/* Liefert eine Zufallszahl  
*/
int randomNumbers::singleNumber(int start, int end) {
  return makeNumber(start, end);
}

/* Liefert eine Reihe Zufallszahlen
*/
int *randomNumbers::manyNumbers(int start, int end, int amount, bool unique) {
  /* Es gibt end-start+1 mögliche verschiedene Zahlen.
  Kann nicht mehr verschiedene Zahlen als möglich liefern, also muss amount <= end-start+1 sein */
  if (end-start+1 < amount) {	amount = end-start+1;	}
  
  /* Wenn end < start wird end zu start+amount */
  if (end < start) { end = start+amount; }
  
  int *cont = new int[amount];
  int t = makeNumber(start, end);
  cont[0] = t;
          
  for (int i = 1; i <= amount-1; i++) {
    if (unique == true) {
      // Solange Zufallszahl erzeugen lassen, bis eine bisher nicht im Array vorhandene geliefert wurde
      while (checkNumber(cont, t, i-1))
        t = makeNumber(start, end);
    } else {
      t = makeNumber(start, end);
    }
    cont[i] = t;
  }
  return cont;
}

/* Überprüft ein Feld auf die Existenz eines Wertes */
bool randomNumbers::checkNumber(int *cont, int value, int arrsize) {
  for (int i = 0; i <= arrsize; i++) {
    if (cont[i] == value) { return true; }
  }
  return false;
}

int *ascendingOrder(int start, int end)
{
  uint amount = end-start+1;
  int *cont = new int[amount];
  for (uint i = 1; i <= amount-1; i++)
    cont[i] = i;
  return cont;
}
