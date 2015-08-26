//
//  NtpChecker.cpp
//
//  Created by OKAYA YOHEI on 2015/08/24.
//  Copyright (c) 2015年 usaya. All rights reserved.
//

#include "NtpChecker.h"

#include <time.h>
#include "support/tinyxml2/tinyxml2.h"

std::vector<std::string> split(const std::string &str, char sep)
{
    std::vector<std::string> v;        // 分割結果を格納するベクター
    auto first = str.begin();              // テキストの最初を指すイテレータ
    while( first != str.end() ) {         // テキストが残っている間ループ
        auto last = first;                      // 分割文字列末尾へのイテレータ
        while( last != str.end() && *last != sep )       // 末尾 or セパレータ文字まで進める
            ++last;
        v.push_back(std::string(first, last));       // 分割文字を出力
        if( last != str.end() )
            ++last;
        first = last;          // 次の処理のためにイテレータを設定
    }
    return v;
}

bool NtpChecker::check(CCObject* target, SEL_NtpCheck selector)
{
    CC_SAFE_RELEASE_NULL(m_target);
    m_target = target;
    CC_SAFE_RETAIN(m_target);
    m_selector = selector;
    auto client = CCHttpClient::getInstance();
    auto req = new CCHttpRequest();
    req->setRequestType(CCHttpRequest::kHttpGet);
    req->setUrl("http://ntp-a1.nict.go.jp/cgi-bin/ntp");
    req->setResponseCallback(this, httpresponse_selector(NtpChecker::checked));
    client->setTimeoutForConnect(5); // 時間がかかってもよくないので5秒の応答を期待する
    client->send(req);
    req->release();
    return true;
}



void NtpChecker::checked(CCHttpClient* client, CCHttpResponse* response)
{
    if(response->getResponseCode() != 200) {
        CCLOG("NTP check : Unknown");
        if(m_target){
            (m_target->*m_selector)(Unknown);
        }
        return;
    }

    // 通信内容をParseする(NTPはHTML形式でDLされるため)
    std::vector<char> *buffer = response->getResponseData();
    std::string _xml = std::string(buffer->begin(), buffer->end());

    tinyxml2::XMLDocument doc;
    if (doc.Parse(_xml.c_str()) == tinyxml2::XML_NO_ERROR) {
        const tinyxml2::XMLElement *xml = doc.FirstChildElement();
        for (;xml;xml = xml->NextSiblingElement()) {

            //ここでXMLフアイルの値を取り出します。
            const tinyxml2::XMLElement* _body = xml->FirstChildElement("BODY");
            if(_body != NULL) {
                std::string body = std::string(_body->GetText());
                CCLOG("%s",body.c_str());
                auto bodies = split(body, '.');
                if(bodies.size() == 2) {
                    // UTCは1900からの秒数なので、1900 - 1970分の秒数を引く
                    auto server = atol(bodies.at(0).c_str()) - 2209021200;

                    // 一旦UTCに変換してからUNIX時間に変換する
                    time_t local = time(NULL);
                    struct tm *utc = gmtime(&local);
                    local = mktime(utc);

                    // 誤差5分以内ならOK
                    if(abs(server - local) < 300) {
                        CCLOG("NTP check : OK");
                        if(m_target){
                            (m_target->*m_selector)(OK);
                        }
                        return;
                    } else {
                        CCLOG("NTP check : NG");
                        if(m_target){
                            (m_target->*m_selector)(NG);
                        }
                        return;
                    }
                }
            }
        }
    }

    CCLOG("NTP check : Unknown");
    if(m_target){
        (m_target->*m_selector)(Unknown);
    }
}

