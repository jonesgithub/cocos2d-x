#pragma once

#include "cocoa/CCGeometry.h"
#include "platform/CCEGLViewProtocol.h"
#include "platform/CCPlatFormMacros.h"

NS_CC_BEGIN

class CC_DLL CCEGLView : public CCEGLViewProtocol
{
public:
    CCEGLView();
    virtual ~CCEGLView();

    bool    isOpenGLReady();

    // keep compatible
    void    end();
    void    swapBuffers();
    void    setIMEKeyboardState(bool bOpen);
    
    // static function
    /**
    @brief    get the shared main open gl window
    */
    static CCEGLView& sharedOpenGLView();
};

NS_CC_END