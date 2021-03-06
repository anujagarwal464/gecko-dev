/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef nsErrorService_h__
#define nsErrorService_h__

#include "mozilla/Attributes.h"

#include "nsIErrorService.h"
#include "nsHashtable.h"

class nsInt2StrHashtable
{
public:
    nsInt2StrHashtable();

    nsresult  Put(uint32_t key, const char* aData);
    char*     Get(uint32_t key);
    nsresult  Remove(uint32_t key);

protected:
    nsObjectHashtable mHashtable;
};

class nsErrorService MOZ_FINAL : public nsIErrorService
{
public:
    NS_DECL_ISUPPORTS
    NS_DECL_NSIERRORSERVICE

    nsErrorService() {}

    static nsresult
    Create(nsISupports* outer, const nsIID& aIID, void* *aInstancePtr);

private:
    ~nsErrorService() {}

protected:
    nsInt2StrHashtable mErrorStringBundleURLMap;
};

#endif // nsErrorService_h__
