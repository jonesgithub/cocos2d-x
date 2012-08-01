/****************************************************************************
Copyright (c) 2010 cocos2d-x.org

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
#define __CC_PLATFORM_FILEUTILS_CPP__
#include "platform/CCFileUtilsCommon_cpp.h"
#include "CCDirector.h"
#include "CCCommon.h"

#define CC_RETINA_DISPLAY_FILENAME_SUFFIX "-hd"
#define CC_IPAD_FILENAME_SUFFIX "-ipad"
#define CC_IPAD_DISPLAY_RETINA_SUPPFIX "-ipadhd"

using namespace std;

NS_CC_BEGIN

static char s_pszResourcePath[2048] = {0};
static CCFileUtils* s_pFileUtils = NULL;

CCFileUtils* CCFileUtils::sharedFileUtils()
{
    if (s_pFileUtils == NULL)
    {
        s_pFileUtils = new CCFileUtils();
    }
    return s_pFileUtils;
}

void CCFileUtils::purgeFileUtils()
{
    if (s_pFileUtils != NULL)
    {
        s_pFileUtils->purgeCachedEntries();
    }
    
    CC_SAFE_DELETE(s_pFileUtils);
}

void CCFileUtils::purgeCachedEntries()
{

}

void CCFileUtils::setResourcePath(const char *pszResourcePath)
{
	CCLog("setResourcePath pszResourcePath='%s'", pszResourcePath);
	strcpy(s_pszResourcePath, pszResourcePath);
}

const char* CCFileUtils::fullPathFromRelativePath(const char *pszRelativePath, ccResolutionType *pResolutionType)
{	
    CCString * pRet = new CCString();
    pRet->autorelease();
	
	pRet->m_sString = s_pszResourcePath;
    pRet->m_sString += pszRelativePath;
 
    return pRet->m_sString.c_str();
}

const char *CCFileUtils::fullPathFromRelativeFile(const char *pszFilename, const char *pszRelativeFile)
{
	CCLog("fullPathFromRelativeFile");

    CCString *pRet = new CCString();
    pRet->autorelease();
    return pRet->m_sString.c_str();
}

string CCFileUtils::getWriteablePath()
{
	CCLog("getWriteablePath");
    string ret;
    return ret;
}

NS_CC_END
