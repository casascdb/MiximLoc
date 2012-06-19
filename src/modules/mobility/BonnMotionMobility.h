//
// Copyright (C) 2005 Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

#ifndef BONNMOTION_MOBILITY_H
#define BONNMOTION_MOBILITY_H

#include "MiXiMDefs.h"
#include "LineSegmentsMobilityBase.h"
#include "BonnMotionFileCache.h"


/**
 * @brief Uses the BonnMotion native file format. See NED file for more info.
 *
 * @ingroup mobility
 * @author Andras Varga
 */
class MIXIM_API BonnMotionMobility : public LineSegmentsMobilityBase
{
  private:
	/** @brief Copy constructor is not allowed.
	 */
	BonnMotionMobility(const BonnMotionMobility&);
	/** @brief Assignment operator is not allowed.
	 */
	BonnMotionMobility& operator=(const BonnMotionMobility&);

  protected:
    // state
    const BonnMotionFile::Line *vecp;
    unsigned vecpos;
    bool     bIs3D;

  public:
    BonnMotionMobility()
    	: LineSegmentsMobilityBase()
    	, vecp(NULL)
    	, vecpos(0)
    	, bIs3D(false)
    {}

    //Module_Class_Members(BonnMotionMobility, LineSegmentsMobilityBase, 0);
    ~BonnMotionMobility();

    /** @brief Initializes mobility model parameters.*/
    virtual void initialize(int);

  protected:
    /** @brief Overridden from LineSegmentsMobilityBase.*/
    virtual void setTargetPosition();

    /** @brief Overridden from LineSegmentsMobilityBase.*/
    virtual void fixIfHostGetsOutside();
};

#endif

