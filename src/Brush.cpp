// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
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
// * Neither the name of the author nor the names of its contributors
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

#include "Vaca/Brush.h"
#include "Vaca/Color.h"
#include "Vaca/win32.h"
#include <cassert>

using namespace Vaca;

Brush::Brush()
  : SharedPtr<GdiObject<HBRUSH> >(new GdiObject<HBRUSH>(CreateSolidBrush(RGB(0, 0, 0))))
{
}

Brush::Brush(const Brush& brush)
  : SharedPtr<GdiObject<HBRUSH> >(brush)
{
}

Brush::Brush(const Color& color)
  : SharedPtr<GdiObject<HBRUSH> >(new GdiObject<HBRUSH>(CreateSolidBrush(convert_to<COLORREF>(color))))
{
}

Brush::~Brush()
{
}

Brush& Brush::operator=(const Brush& brush)
{
  SharedPtr<GdiObject<HBRUSH> >::operator=(brush);
  return *this;
}

Color Brush::getColor() const
{
  LOGBRUSH lb;
  assert(getHandle());
  ::GetObject(getHandle(), sizeof(LOGBRUSH), &lb); 
  return convert_to<Color>(lb.lbColor);
}

HBRUSH Brush::getHandle() const
{
  return get()->getHandle();
}
