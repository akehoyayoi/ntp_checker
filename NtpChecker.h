//
//  NtpChecker.h
//
//  Created by OKAYA YOHEI on 2015/08/24.
//  Copyright (c) 2015年 usaya. All rights reserved.
//

#ifndef __NtpChecker__
#define __NtpChecker__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "Singleton.h"

USING_NS_CC;
USING_NS_CC_EXT;

enum NtpResult {
    Unknown,
    OK,
    NG
};


typedef void (CCObject::*SEL_NtpCheck)(NtpResult);
#define ntpchecker_selector(_SELECTOR) (SEL_NtpCheck)(&_SELECTOR)

class NtpChecker : public Singleton<NtpChecker>
{
    friend class Singleton<NtpChecker>;
public:
    CCObject* m_target;
    SEL_NtpCheck m_selector;

    bool check(CCObject* target, SEL_NtpCheck selector);
    void checked(CCHttpClient* client, CCHttpResponse* response);

protected:
    NtpChecker()
    : m_target(NULL)
    {}
    virtual ~NtpChecker()
    {
        CC_SAFE_RELEASE_NULL(m_target);
    }
};

#endif /* defined(__NtpChecker__) */
