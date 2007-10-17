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
#include "Vaca/DockArea.h"
#include "Vaca/DockBar.h"
#include "Vaca/Frame.h"
#include "Vaca/Point.h"
#include "Vaca/Debug.h"

using namespace Vaca;

DockArea::DockArea(Side side, Widget *parent, Style style)
  : Widget(DockAreaClass::getClassName(), parent, style)
  , mSide(side)
{
}

DockArea::~DockArea()
{
  dispose();
}

bool DockArea::isHorizontal()
{
  return mSide == TopSide || mSide == BottomSide;
}

bool DockArea::isVertical()
{
  return mSide == LeftSide || mSide == RightSide;
}

Side DockArea::getSide()
{
  return mSide;
}

/**
 * Returns true, because the dock areas are controlled by the Frame,
 * not by the Layout manager.
 * 
 * @see Frame::layout
 */
bool DockArea::isLayoutFree()
{
  return true;
}

/**
 * Adds the @a dockBar in this dock area.
 *
 * @see onAddDockBar
 */
void DockArea::addDockBar(DockBar *dockBar)
{
  addChild(dockBar, true);

  onAddDockBar(dockBar);
}

/**
 * Removes the @a dockBar from this dock area.
 *
 * @see onRemoveDockBar
 */
void DockArea::removeDockBar(DockBar *dockBar)
{
  removeChild(dockBar, true);

  onRemoveDockBar(dockBar);
}

/**
 * You can use onAddDockBar to hook the addDockBar action.
 *
 * @code
 * void MyDockArea::onAddDockBar(DockBar *dockBar)
 * {
 *   // do something, like add the dockBar in
 *   // some internal structure...
 * }
 * @endcode
 */
void DockArea::onAddDockBar(DockBar *dockBar)
{
  // do nothing
}

/**
 * You can use onRemoveDockBar to hook the removeDockBar action.
 *
 * @code
 * void MyDockArea::onRemoveDockBar(DockBar *dockBar)
 * {
 *   // do something, like remove the dockBar from 
 *   // some internal structure...
 * }
 * @endcode
 */
void DockArea::onRemoveDockBar(DockBar *dockBar)
{
  // do nothing
}

/**
 * Event generated when the user start dragging the @a dockBar from
 * this DockArea.
 */
// void DockArea::onBeginDockBarDrag(DockBar *dockBar)
// {
// }

/**
 * Event generated when the user finish the dragging of the @a dockBar
 * that start from this DockArea.
 *
 * @param newDockInfo The new dockInfo where the dockBar will be
 *                    docked. NULL means that dockBar will be
 *                    floating, or the drag operation was canceled.
 */
// void DockArea::onEndDockBarDrag(DockBar *dockBar, DockInfo *newDockInfo)
// {
// }

void DockArea::onRedock(DockBar *dockBar, DockInfo *newDockInfo)
{
}

/**
 * Overrided to remove all dock bars from the dock area before to call
 * Widget::onDestroy (it's to avoid children to be destroyed).
 */
void DockArea::onDestroy()
{
  Widget::Container children = getChildren();
  Widget::Container::iterator it;

  for (it=children.begin(); it!=children.end(); ++it) {
    DockBar *dockBar = static_cast<DockBar *>(*it);

    removeDockBar(dockBar);
    dockBar->mDockArea = NULL;
    dockBar->setVisible(false);
  }

  Widget::onDestroy();
}