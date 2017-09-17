#ifndef _btsoccer_dummy_ai_h
#define _btsoccer_dummy_ai_h

/*
  BtSoccer - button football (soccer) game
  Copyright (C) 2008-2015 DNTeam <btsoccer@dnteam.org>

  This file is part of BtSoccer.

  BtSoccer is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  BtSoccer is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with BtSoccer.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "baseai.h"

namespace BtSoccer
{

/*! The DummyAI is the simpliest AI implementation: it only uses
 * the nearest player to the ball that still have action points. */
class DummyAI: public BaseAI
{
   public:
      /*! Constructor */
      DummyAI(BtSoccer::Team* t, Field* f);
      /*! Destructor */
      ~DummyAI();

      /*! Do the disk input-position before freekicks */
      void doDiskPosition(BtSoccer::TeamPlayer* tp);

      /*! Do the goal keeper position before enemy try a shoot */
      void doGoalKeeperPosition(BtSoccer::GoalKeeper* gk);

      void calculateGoalShoot();

   protected:
      /*! The internal AI calculation single step */
      void calculateStep();
};

}

#endif

