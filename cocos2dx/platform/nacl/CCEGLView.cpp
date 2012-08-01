#include "CCEGLView.h"
#include "cocoa/CCSet.h"
#include "CCDirector.h"
#include "ccMacros.h"
#include "CCGL.h"

#include <stdlib.h>


NS_CC_BEGIN

CCEGLView::CCEGLView()
{
}

CCEGLView::~CCEGLView()
{
}

bool CCEGLView::isOpenGLReady()
{
    return (m_sSizeInPixel.width != 0 && m_sSizeInPixel.height != 0);
}

void CCEGLView::end()
{
}

void CCEGLView::swapBuffers()
{
}

CCEGLView& CCEGLView::sharedOpenGLView()
{
    static CCEGLView instance;
    return instance;
}

void CCEGLView::setIMEKeyboardState(bool bOpen)
{
}

NS_CC_END

