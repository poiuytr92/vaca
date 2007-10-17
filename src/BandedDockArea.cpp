// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, David A. Capello
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in
//   the documentation and/or other materials provided with the
//   distribution.
// * Neither the name of the Vaca nor the names of its contributors
//   may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

#include "stdvaca.h"
#include "Vaca/BandedDockArea.h"
#include "Vaca/DockBar.h"
#include "Vaca/Frame.h"
#include "Vaca/Point.h"
#include "Vaca/Debug.h"
#include "Vaca/System.h"

using namespace Vaca;

// Information about the docked position of the DockBar in a BandedDockArea.
class BandedDockInfo : public DockInfo
{
public:

  Side side;         // BandedDockInfo doesn't save a DockArea pointer, just the side of the DockArea.
  int band;          // In which band the DockBar is.
  int offset;        // Offset from the origin of the band.
  Size size;         // Size of the DockBar inside the the DockArea.
  bool preserveBand; // True when the user redock the bar in the same band.
  bool lastMovedInBand; // True means this is the last bar moved in the band

  virtual Size getSize() { return size; }
  virtual Side getSide() { return side; }
};

BandedDockArea::BandedDockArea(Side side, Widget *parent, Style style)
  : DockArea(side, parent, style)
{
}

bool BandedDockArea::hitTest(DockBar *bar, const Point &cursor, const Point &anchor, bool fromInside)
{
  Rect bounds = getAbsoluteClientBounds();
  Size sz;
  Rect rc;

  sz = bounds.getSize();

  if (bar->isFullDrag()) {
    // is floating
    if (bar->isFloating() ||
	// or is docked in other DockArea that isn't this BandeDockArea
	(bar->isDocked() &&
	 (&bar->getDockArea() != this)))
      sz += bar->getDockedSize(getSide());
  }
  else
    sz += bar->getDockedSize(getSide());

  switch (getSide()) {
    case LeftSide:   rc = Rect(bounds.x, bounds.y, sz.w, bounds.h); break;
    case TopSide:    rc = Rect(bounds.x, bounds.y, bounds.w, sz.h); break;
    case RightSide:  rc = Rect(bounds.x+bounds.w-sz.w, bounds.y, sz.w, bounds.h); break;
    case BottomSide: rc = Rect(bounds.x, bounds.y+bounds.h-sz.h, bounds.w, sz.h); break;
  }

  return rc.contains(cursor);
}

DockInfo *BandedDockArea::createDefaultDockInfo(DockBar *bar)
{
  BandedDockInfo *dockInfo = new BandedDockInfo();
  dockInfo->side = getSide();
  dockInfo->band = 0;
  dockInfo->offset = 0;
  dockInfo->size = bar->getDockedSize(getSide());
  dockInfo->preserveBand = false;
  dockInfo->lastMovedInBand = false;
  return dockInfo;
}

DockInfo *BandedDockArea::createDockInfo(DockBar *bar, const Point &cursor, const Point &anchor)
{
  Rect bounds = getAbsoluteClientBounds();
  Size size = bar->getDockedSize(getSide());
  Point origin = cursor - bounds.getOrigin() - anchor;

//   if (isHorizontal()) {
//     if (origin.x < 0)
//       origin.x = 0;
//     else if (origin.x+size.w > bounds.w)
//       origin.x = bounds.w-size.w;
//   }
//   else {
//     if (origin.y < 0)
//       origin.y = 0;
//     else if (origin.y+size.h > bounds.h)
//       origin.y = bounds.h-size.h;
//   }

  // get the band
  int band, count = mBandInfo.size();
  for (band=0; band<count; ++band)
    if (getBandBounds(band).contains(cursor - bounds.getOrigin()))
      break;

  // "band == count" is a valid option (the DockBar must be docked in
  // a new band)

  BandedDockInfo *dockInfo = new BandedDockInfo();
  dockInfo->side = getSide();
  dockInfo->band = band;
  dockInfo->offset = isHorizontal() ? origin.x: origin.y;
  dockInfo->size = size;
  dockInfo->preserveBand = false;
  dockInfo->lastMovedInBand = false;
  return dockInfo;
}

void BandedDockArea::drawXorDockInfoShape(Graphics &g, DockInfo *_dockInfo)
{
  BandedDockInfo *dockInfo = static_cast<BandedDockInfo *>(_dockInfo);
  Rect bounds = getAbsoluteClientBounds();
  Rect externRect;
  Rect internRect;

  externRect = getBandBounds(dockInfo->band);

  if (isHorizontal()) {
    // size of the last band
    if (getSide() == BottomSide)
      externRect.y -= VACA_MAX(externRect.h, dockInfo->size.h) - externRect.h;

    externRect.h = VACA_MAX(externRect.h, dockInfo->size.h);
    internRect = Rect(Point(externRect.x + dockInfo->offset, externRect.y),
		      dockInfo->size);
  }
  else {
    // size of the last band
    if (getSide() == RightSide)
      externRect.x -= VACA_MAX(externRect.w, dockInfo->size.w) - externRect.w;

    externRect.w = VACA_MAX(externRect.w, dockInfo->size.w);
    internRect = Rect(Point(externRect.x, externRect.y + dockInfo->offset),
		      dockInfo->size);
  }

  externRect.offset(bounds.getOrigin());
  internRect.offset(bounds.getOrigin());

  g.setRop2(R2_NOTXORPEN);
  g.drawRect(externRect);
  g.drawRect(internRect.shrink(1));
  g.setRop2(R2_COPYPEN);
}

Size BandedDockArea::preferredSize()
{
  std::vector<BandInfo>::iterator it;
  Size sz(0, 0);
//   int count = 0;

  for (it =mBandInfo.begin();
       it!=mBandInfo.end();
       ++it) {
    if (isHorizontal())
      sz.h += it->size;
    else
      sz.w += it->size;

//     count++;
  }

  return sz//  + (2*VACA_MAX(count-1, 0))
    ;
}

void BandedDockArea::layout()
{
  int bandIndex, bands = mBandInfo.size();

  // for each band
  for (bandIndex=0; bandIndex<bands; ++bandIndex) {
    // get the bounds of this band
    Rect bandBounds = getBandBounds(bandIndex);

    // for each dock bar
//     std::vector<DockBar *>::iterator it = mBandInfo[c].bars.begin();
//     std::vector<DockBar *>::iterator end = mBandInfo[c].bars.end();
    std::vector<Rect> eachBarBounds;
    int barIndex, bars = mBandInfo[bandIndex].bars.size();
    int lastMovedBarIndex = -1;

    // get the real bounds of each bar in this band
    for (barIndex=0; barIndex<bars; ++barIndex) {
      BandedDockInfo *dockInfo =
	static_cast<BandedDockInfo *>(mBandInfo[bandIndex].bars[barIndex]->getDockInfo());

      VACA_ASSERT(dockInfo != NULL);

      // bounds specified by the dockInfo of this DockBar
      Rect bounds;

      if (isHorizontal())
	bounds = Rect(Point(bandBounds.x + dockInfo->offset, bandBounds.y), dockInfo->size);
      else
	bounds = Rect(Point(bandBounds.x, bandBounds.y + dockInfo->offset), dockInfo->size);

      // limit to band's bounds
      if (isHorizontal()) {
	if (bounds.x < 0)
	  bounds.x = 0;
	else if (bounds.x+bounds.w > bandBounds.w)
	  bounds.x = bandBounds.w-bounds.w;
      }
      else {
	if (bounds.y < 0)
	  bounds.y = 0;
	else if (bounds.y+bounds.h > bandBounds.h)
	  bounds.y = bandBounds.h-bounds.h;
      }

      // add to "eachBarBounds"
      eachBarBounds.push_back(bounds);

      if (dockInfo->lastMovedInBand) {
	// we can have just one DockBar with the "lastMovedInBand"
	// flag per band
	VACA_ASSERT(lastMovedBarIndex < 0);
	
	lastMovedBarIndex = barIndex;
      }
    }

    // fit the bars to avoid overlapping
    if (lastMovedBarIndex >= 0)
      fitBounds(bandIndex, lastMovedBarIndex, eachBarBounds);

    // set the bounds of each DockBar
    for (barIndex=0; barIndex<bars; ++barIndex) {
      DockBar *dockBar = mBandInfo[bandIndex].bars[barIndex];
      BandedDockInfo *dockInfo = static_cast<BandedDockInfo *>(dockBar->getDockInfo());

      dockBar->setBounds(eachBarBounds[barIndex]);
      dockBar->layout();
    }
  }

}

void BandedDockArea::onPaint(Graphics &g)
{
#if 0
  Color topLeft = System::getColor(COLOR_3DSHADOW);
  Color bottomRight = System::getColor(COLOR_3DHIGHLIGHT);
  int c, count = mBandInfo.size();

  // for each band
  for (c=0; c<count; ++c) {
    // get the bounds of this band
    Rect bounds = getBandBounds(c);

    if (isHorizontal()) {
      g.setColor(topLeft);
      g.drawLine(bounds.x, bounds.y-2, bounds.x+bounds.w, bounds.y-2);

      g.setColor(bottomRight);
      g.drawLine(bounds.x, bounds.y-1, bounds.x+bounds.w, bounds.y-1);
    }
    else {
      g.setColor(topLeft);
      g.drawLine(bounds.x-2, bounds.y, bounds.x-2, bounds.y+bounds.h);
      
      g.setColor(bottomRight);
      g.drawLine(bounds.x-1, bounds.y, bounds.x-1, bounds.y+bounds.h);
    }
  }
#endif
}

void BandedDockArea::onAddDockBar(DockBar *dockBar)
{
  BandedDockInfo *dockInfo = static_cast<BandedDockInfo *>(dockBar->getDockInfo());
  VACA_ASSERT(dockInfo != NULL);

  // in which band we must to dock this dockBar?
  dockInfo->band = VACA_MID(0, dockInfo->band, mBandInfo.size());

  // add a new band?
  if (dockInfo->band == mBandInfo.size())
    mBandInfo.push_back(BandInfo());

  // add the dock bar to the band
  mBandInfo[dockInfo->band].bars.push_back(dockBar);

  // remove the "lastMovedInBand" flag of each bar that is in the band
  std::vector<DockBar *>::iterator it2 = mBandInfo[dockInfo->band].bars.begin();
  std::vector<DockBar *>::iterator end = mBandInfo[dockInfo->band].bars.end();
  for (; it2 != end; ++it2) {
    DockBar *dockBar2 = *it2;
    BandedDockInfo *dockInfo2 = static_cast<BandedDockInfo *>(dockBar2->getDockInfo());
    dockInfo2->lastMovedInBand = false;
  }

  // last addition in the band
  dockInfo->lastMovedInBand = true;

  // update the size of the band
  updateBandSize(dockInfo->band);
}

void BandedDockArea::onRemoveDockBar(DockBar *dockBar)
{
  BandedDockInfo *dockInfo = static_cast<BandedDockInfo *>(dockBar->getDockInfo());
  VACA_ASSERT(dockInfo != NULL);

  int bandIndex = dockInfo->band;
  VACA_ASSERT(bandIndex >= 0 && bandIndex < mBandInfo.size());

  // remove the dockBar from the band
  remove_element_from_container(mBandInfo[bandIndex].bars, dockBar);

  // preserve band? (this is from onRedock)
  if (dockInfo->preserveBand)
    return;

  // remove the band? (it hasn't bars)
  if (mBandInfo[bandIndex].bars.empty()) {
    std::vector<BandInfo>::iterator it = mBandInfo.begin() + bandIndex;

    mBandInfo.erase(it);

    // update dock bars band indexes
    for (; it!=mBandInfo.end(); ++it) {
      std::vector<DockBar *>::iterator it2 = it->bars.begin();
      std::vector<DockBar *>::iterator end = it->bars.end();
    
      for (; it2!=end; ++it2) {
	DockBar *dockBar2 = *it2;
	BandedDockInfo *dockInfo2 = static_cast<BandedDockInfo *>(dockBar2->getDockInfo());

	VACA_ASSERT(dockInfo2 != NULL);

	dockInfo2->band--;
      }
    }
  }
  // or update the size?
  else
    updateBandSize(bandIndex);
}

void BandedDockArea::onRedock(DockBar *dockBar, DockInfo *_newDockInfo)
{
  BandedDockInfo *oldDockInfo = static_cast<BandedDockInfo *>(dockBar->getDockInfo());
  BandedDockInfo *newDockInfo = static_cast<BandedDockInfo *>(_newDockInfo);

  VACA_ASSERT(oldDockInfo != NULL);
  VACA_ASSERT(newDockInfo != NULL);
  
  if (mBandInfo[oldDockInfo->band].bars.size() == 1) {
    if (newDockInfo->band > oldDockInfo->band) {
      newDockInfo->band--;
    }
    // redocking in the same band?
    else if (newDockInfo->band == oldDockInfo->band) {
      // preserve the band
      oldDockInfo->preserveBand = true;
    }
  }
}

void BandedDockArea::updateBandSize(int bandIndex)
{
  VACA_ASSERT(bandIndex >= 0 && bandIndex < mBandInfo.size());

  std::vector<DockBar *>::iterator it = mBandInfo[bandIndex].bars.begin();
  std::vector<DockBar *>::iterator end = mBandInfo[bandIndex].bars.end();
  int size = 0;
  
  for (; it!=end; ++it) {
    DockBar *dockBar = *it;
    BandedDockInfo *dockInfo = static_cast<BandedDockInfo *>(dockBar->getDockInfo());

    VACA_ASSERT(dockInfo != NULL);

    if (isHorizontal())
      size = VACA_MAX(size, dockInfo->size.h);
    else
      size = VACA_MAX(size, dockInfo->size.w);
  }

  mBandInfo[bandIndex].size = size;
}

Rect BandedDockArea::getBandBounds(int bandIndex)
{
  int count = mBandInfo.size();

  // Warning! we can obtain the bounds of the new band (the one that
  // has bandIndex == count)
  bandIndex = VACA_MID(0, bandIndex, count);

  // get the entire client bounds (all bands size)
  Rect bounds = getClientBounds();

  // displace the bounds to the correct band position
  switch (getSide()) {
    case BottomSide: bounds.y += bounds.h; break;
    case RightSide:  bounds.x += bounds.w; break;
  }
  for (int c=0; c<bandIndex; ++c) {
    switch (getSide()) {
      case TopSide:    bounds.y += mBandInfo[c].size; break;
      case LeftSide:   bounds.x += mBandInfo[c].size; break;
      case BottomSide: bounds.y -= mBandInfo[c].size; break;
      case RightSide:  bounds.x -= mBandInfo[c].size; break;
    }
  }
  if (bandIndex < count)
    switch (getSide()) {
      case BottomSide: bounds.y -= mBandInfo[bandIndex].size; break;
      case RightSide:  bounds.x -= mBandInfo[bandIndex].size; break;
    }

  // reduce the height (or width) to the size of the band on bandIndex
  if (isHorizontal())
    bounds.h = bandIndex < count ? mBandInfo[bandIndex].size: 0;
  else
    bounds.w = bandIndex < count ? mBandInfo[bandIndex].size: 0;

  return bounds;
}

void BandedDockArea::fitBounds(int bandIndex, int barIndex1, std::vector<Rect> &bounds)
{
  Rect bandBounds = getBandBounds(bandIndex);
  int barIndex2, bars = mBandInfo[bandIndex].bars.size();
  BandedDockInfo *di1 =
    static_cast<BandedDockInfo *>(mBandInfo[bandIndex].bars[barIndex1]->getDockInfo());

  for (barIndex2=0; barIndex2<bars; ++barIndex2) {
    if (barIndex1 != barIndex2) {
      BandedDockInfo *di2 =
	static_cast<BandedDockInfo *>(mBandInfo[bandIndex].bars[barIndex2]->getDockInfo());

      if (!di2->lastMovedInBand) {
	
#define FITBOUNDS(x, w)							\
	if (bounds[barIndex1].intersects(bounds[barIndex2])) {		\
	  /*Point center1 = bounds[barIndex1].getCenter();		\
	    Point center2 = bounds[barIndex2].getCenter();*/		\
	  int center1 = di1->offset + di1->size.w/2;			\
	  int center2 = di2->offset + di2->size.w/2;			\
									\
	  /* "di1" must be in the left and "di2" in the right */	\
	  if (center1 < center2) {					\
	    bounds[barIndex2].x =					\
	      bounds[barIndex1].x + bounds[barIndex1].w;		\
									\
	    if (bounds[barIndex2].x + bounds[barIndex2].w >		\
		bandBounds.x + bandBounds.w)				\
	      bounds[barIndex2].x =					\
		bandBounds.x + bandBounds.w - bounds[barIndex2].w;	\
	  }								\
	  /* "di1" must be in the right and "di2" in the left */	\
	  else {							\
	    bounds[barIndex2].x =					\
	      bounds[barIndex1].x - bounds[barIndex2].w;		\
									\
	    if (bounds[barIndex2].x < bandBounds.x)			\
	      bounds[barIndex2].x = bandBounds.x;			\
	  }								\
									\
	  fitBounds(bandIndex, barIndex2, bounds);			\
	}
      
	// horizontal
	if (isHorizontal()) {
	  FITBOUNDS(x, w);
	}
	// vertical
	else {
	  FITBOUNDS(y, h);
	}
      }
    }
  }
}
