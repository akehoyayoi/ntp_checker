//
//  Singleton.h
//
//  Created by OKAYA YOHEI on 2015/04/18.
//  Copyright (c) 2015年 usaya. All rights reserved.
//

#ifndef __Singleton__
#define __Singleton__

#include "cocos2d.h"

template<class T>
class Singleton: public cocos2d::CCObject
{
public:
    class Auto {
    public:
        T* m_instance;
        Auto() : m_instance(0) {}
        virtual ~Auto() {
            destroy();
        }
        void destroy() {
            if( m_instance ) {
                delete m_instance;
                m_instance = 0;
                m_flag = false;
            }
        }
    };

    static Singleton::Auto m_auto;

public:
    static T *sharedInstance(){
        if( m_flag == false ) {
            if( !m_auto.m_instance ) {
                m_auto.m_instance = new T();
                Singleton<T> *instance = m_auto.m_instance;
                if(!instance->init()) return NULL;
            }
            m_flag = true;
            return m_auto.m_instance;
        } else {
            return m_auto.m_instance;
        }
    }

    static bool isCreate() {
        return m_flag;
    }

    static bool isDestroy() {
        return !m_flag;
    }

    static void destroy () {
        m_auto.destroy();
        m_flag = false;
    }

protected:
    virtual bool init() { return true; };

    Singleton() {}
    //    Singleton(StaticVirtualFunc *a_init = init):m_init(a_init){}
    virtual ~Singleton() {}
private:
    static T *m_instance;

    /** シングルトンオブジェクトが生成されているかどうか */
    static volatile bool m_flag;

    /** コピーコンストラクタも隠蔽 */
    Singleton( Singleton const & );
    Singleton &operator=( Singleton const & );
};

template<class T>
typename Singleton<T>::Auto Singleton<T>::m_auto;

template<class T>
T *Singleton<T>::m_instance = 0;

template<class T>
volatile bool Singleton<T>::m_flag = false;

#endif /* defined(__Singleton__) */
