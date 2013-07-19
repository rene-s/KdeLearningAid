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
 
#ifndef RNDCLASS_H_INCLUDED
#define RNDCLASS_H_INCLUDED

class randomNumbers {
  private: 
    int start, end;
    int makeNumber(int start, int end);

  public:
    randomNumbers();
    ~randomNumbers();
    bool checkNumber(int *cont, int value, int arrsize);
    int singleNumber(int start, int end);
    int *manyNumbers(int start, int end, int amount, bool unique);
    int *ascendingOrder(int start, int end);
};

#endif // RNDCLASS_H_INCLUDED
