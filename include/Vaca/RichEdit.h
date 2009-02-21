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

#ifndef VACA_RICHEDIT_H
#define VACA_RICHEDIT_H

#include "Vaca/base.h"
#include "Vaca/Widget.h"

namespace Vaca {

/// Loads the RichEdit DLL.
/// 
class VACA_DLL RichEditRegister
{
  static HINSTANCE hmod;

public:
  RichEditRegister();
};

/// Widget to edit rich-text.
/// 
class VACA_DLL RichEdit : public RichEditRegister, public Widget
{
public:

  struct VACA_DLL Styles {
    static const Style Default;
    static const Style RightAligned;
    static const Style ReadOnly;
    static const Style AutoHorizontalScroll;
    static const Style AutoVerticalScroll;
  };

  RichEdit(const String& text, Widget* parent, Style style = Styles::Default);
  virtual ~RichEdit();

  virtual void setBgColor(const Color& color);

  size_t getTextLength() const;
  size_t getTextLimit() const;
  void setTextLimit(size_t textLimit);
  
  bool canPaste() const;
  bool canUndo() const;
  bool canRedo() const;
  void undo();
  void redo();

  void setUndoLimit(int maximumActions);

  void setAutoUrlDetect(bool state);
  bool isAutoUrlDetect();
  
  void cut();
  void copy();
  void paste();

  void selectAll();
  void selectRange(int start, int end);
  void deselect();
  
  void getSelection(int& start, int& end);
  String getSelectedText() const;

};

} // namespace Vaca

#endif // VACA_TEXTEDIT_H
