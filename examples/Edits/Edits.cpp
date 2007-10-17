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

#include "Vaca/Vaca.h"

using namespace Vaca;

class MainFrame : public Frame
{
  Edit mEdit1;
  Edit mEdit2;
  Edit mEdit3;
  MultilineEdit mEdit4;
  Label mLabel1;
  PasswordEdit mPassword;

public:

  MainFrame()
    : Frame("Edits")
    , mEdit1("Default Edit widget with some text that you can edit.", this)
    , mEdit2("An Edit widget without client-edge and right-aligned.",
	     this, EditStyle - ClientEdgeStyle + RightAlignedEditStyle)
    , mEdit3("A read-only Edit widget without client-edge and with modified bgColor",
	     this, EditStyle - ClientEdgeStyle)
    , mEdit4("A MultilineEdit widget\r\nwith multiple lines\r\nof text...", this,
	     MultilineEditStyle + ScrollStyle)
    , mLabel1("Password:", this)
    , mPassword("", this)
  {
    setLayout(new BoxLayout(Vertical, false));
    mEdit4.setConstraint(new BoxConstraint(true));

    mEdit3.setReadOnly(true);
    mEdit3.setBgColor(System::getColor(COLOR_3DFACE));

    setSize(preferredSize());
  }

protected:

  virtual void onClose(CloseEvent &ev)
  {
    Frame::onClose(ev);

    String password = mPassword.getText();
    if (!password.empty())
      msgBox("Your password is:\r\n\""+password+"\"\r\n"+
	     "Keep it safe, don't show it to anyone!\r\n"+
	     "Maybe you forgot it, so I'll show your password\r\n"+
	     "one more time so nobody can see it:\r\n\""+
	     password+"\"\r\n"+"Remember it: \""+password+
	     "\", don't forget it...\r\n\r\n..."+password,
	     "Anti-Security Message",
	     MB_OK | MB_ICONINFORMATION);
  }

};

//////////////////////////////////////////////////////////////////////

class Example : public Application
{
  MainFrame mMainWnd;
public:
  virtual void main(std::vector<String> args) {
    mMainWnd.setVisible(true);
  }
};

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine, int nCmdShow)
{
  Example *app(new Example);
  app->run();
  delete app;
  return 0;
}
