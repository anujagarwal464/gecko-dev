/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * The contents of this file are subject to the Netscape Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/NPL/
 *
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is Netscape
 * Communications Corporation.  Portions created by Netscape are
 * Copyright (C) 1998 Netscape Communications Corporation. All
 * Rights Reserved.
 *
 * Contributor(s): 
 */
/* AUTO-GENERATED. DO NOT EDIT!!! */

#ifndef nsIDOMHTMLDirectoryElement_h__
#define nsIDOMHTMLDirectoryElement_h__

#include "nsISupports.h"
#include "nsString.h"
#include "nsIScriptContext.h"
#include "nsIDOMHTMLElement.h"


#define NS_IDOMHTMLDIRECTORYELEMENT_IID \
 { 0xa6cf909c, 0x15b3, 0x11d2, \
  { 0x93, 0x2e, 0x00, 0x80, 0x5f, 0x8a, 0xdd, 0x32 } } 

class NS_NO_VTABLE nsIDOMHTMLDirectoryElement : public nsIDOMHTMLElement {
public:
  static const nsIID& GetIID() { static nsIID iid = NS_IDOMHTMLDIRECTORYELEMENT_IID; return iid; }

  NS_IMETHOD    GetCompact(PRBool* aCompact)=0;
  NS_IMETHOD    SetCompact(PRBool aCompact)=0;
};


#define NS_DECL_IDOMHTMLDIRECTORYELEMENT   \
  NS_IMETHOD    GetCompact(PRBool* aCompact);  \
  NS_IMETHOD    SetCompact(PRBool aCompact);  \



#define NS_FORWARD_IDOMHTMLDIRECTORYELEMENT(_to)  \
  NS_IMETHOD    GetCompact(PRBool* aCompact) { return _to GetCompact(aCompact); } \
  NS_IMETHOD    SetCompact(PRBool aCompact) { return _to SetCompact(aCompact); } \


extern "C" NS_DOM nsresult NS_InitHTMLDirectoryElementClass(nsIScriptContext *aContext, void **aPrototype);

extern "C" NS_DOM nsresult NS_NewScriptHTMLDirectoryElement(nsIScriptContext *aContext, nsISupports *aSupports, nsISupports *aParent, void **aReturn);

#endif // nsIDOMHTMLDirectoryElement_h__
