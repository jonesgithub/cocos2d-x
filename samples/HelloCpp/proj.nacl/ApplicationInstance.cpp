#include "ApplicationInstance.h"

#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include <cocos2d.h>
#include <support/zip_support/unzip.h>

#include <ppapi/cpp/url_loader.h>
#include <ppapi/cpp/url_request_info.h>
#include <ppapi/cpp/var.h>
#include <ppapi/gles2/gl2ext_ppapi.h>

ApplicationInstance* ApplicationInstance::sm_pApplicationInstance = NULL;
using namespace cocos2d;

#define READ_BUFFER_SIZE 32768

class FileSystem;

ZPOS64_T zip_tell64_file(voidpf opaque, voidpf stream);
long zip_seek64_file(voidpf opaque, voidpf stream, ZPOS64_T offset, int origin);
voidpf zip_open64_file(voidpf opaque, const void* filename, int mode);
uLong zip_read_file(voidpf opaque, voidpf stream, void* buf, uLong size);
uLong zip_write_file(voidpf opaque, voidpf stream, const void* buf, uLong size);
int zip_close_file(voidpf opaque, voidpf stream);
int zip_error_file(voidpf opaque, voidpf stream);


class FileSystem
{
	typedef void (ApplicationInstance::*ReadinessCallback)();	
public:
	FileSystem(ApplicationInstance* instance, const ReadinessCallback callback)
		: mInstance(instance), mCallback(callback), mUrlRequest(instance), mUrlLoader(instance), mFactory(this), mZipFile(0), mOffset(0)
	{
		CC_ASSERT(!sm_pFileSystem);
		sm_pFileSystem = this;
	}
	
	~FileSystem()
	{
		if (mZipFile != 0)
			unzClose(mZipFile);
		sm_pFileSystem = NULL;
	}
	
	static FileSystem& sharedFileSystem()
	{
		CC_ASSERT(sm_pFileSystem);
		return *sm_pFileSystem;
	}
	
	void DownloadArchive()
	{
		mUrlRequest.SetURL("/Resources.zip");
		mUrlRequest.SetMethod("GET");
		mUrlRequest.SetRecordDownloadProgress(true);
		
		pp::CompletionCallback cc = mFactory.NewCallback(&FileSystem::OnOpen);
		mUrlLoader.Open(mUrlRequest, cc);
	}
	
	void ZipOpen()
	{
		mOffset = 0;
	}
	
	long ZipSeek(ZPOS64_T offset, int origin)
	{
		if (origin == 0)
			mOffset = offset;
		else if (origin == 1)
			mOffset += offset;
		else if (origin == 2)
			mOffset = mArchiveData.size() - offset;
		return 0;		
	}
	
	ZPOS64_T ZipTell()
	{
		return mOffset;
	}
	
	uLong ZipReadFile(void* buf, uLong size)
	{
		memcpy(buf, mArchiveData.data() + mOffset, size);
		mOffset += size;
		return size;
	}
	
	unsigned char* GetFileData(const char* pszFileName, const char* pszMode, unsigned long* pSize)
	{
		unsigned char* pBuffer = NULL;
		*pSize = 0;
		if (UNZ_OK == unzLocateFile(mZipFile, pszFileName, 0))
		{
			unz_file_info64 file_info;
			unzGetCurrentFileInfo64(mZipFile,
                         &file_info,
                         NULL, 0,
                         NULL, 0,
                         NULL, 0);
			*pSize = file_info.uncompressed_size;
			pBuffer = new unsigned char[*pSize];
			
			unzOpenCurrentFile(mZipFile);
			unzReadCurrentFile(mZipFile, pBuffer, *pSize);
			unzCloseCurrentFile(mZipFile);
		}
		return pBuffer;
	}
protected:
	ApplicationInstance* mInstance;
	ReadinessCallback	mCallback;
	
	pp::URLRequestInfo	mUrlRequest;
	pp::URLLoader 		mUrlLoader;
	pp::CompletionCallbackFactory<FileSystem> mFactory;
	char				mBuffer[READ_BUFFER_SIZE];
	std::vector<char>	mArchiveData;
	unzFile				mZipFile;
	ZPOS64_T			mOffset;

	static FileSystem* sm_pFileSystem;
	
	void OnOpen(int32_t result)
	{
		if (result != PP_OK)
		{
			CCLog("pp::URLLoader::Open() failed");
			return;
		}		
		int64_t bytes_received = 0;
		int64_t total_bytes_to_be_received = 0;
		if (mUrlLoader.GetDownloadProgress(&bytes_received, &total_bytes_to_be_received)) 
		{
			if (total_bytes_to_be_received > 0)
				mArchiveData.reserve(total_bytes_to_be_received);
		}
		mUrlRequest.SetRecordDownloadProgress(false);		
		
		ReadBody();
	}
	
	void AppendDataBytes(const char* buffer, int32_t num_bytes)
	{
		if (num_bytes <= 0)
			return;
		// Make sure we don't get a buffer overrun.
		num_bytes = std::min(READ_BUFFER_SIZE, num_bytes);
		
		// Note that we do *not* try to minimally increase the amount of allocated
		// memory here by calling mArchiveData.reserve().  Doing so causes a
		// lot of string reallocations that kills performance for large files.
		mArchiveData.insert(mArchiveData.end(),
                            buffer,
                            buffer + num_bytes);
	}

	void OnRead(int32_t result)
	{
		if (result == PP_OK)
		{
			zlib_filefunc64_def ffunc;
			
			ffunc.zopen64_file = zip_open64_file;
			ffunc.zread_file = zip_read_file;
			ffunc.zwrite_file = zip_write_file;
			ffunc.ztell64_file = zip_tell64_file;
			ffunc.zseek64_file = zip_seek64_file;
			ffunc.zclose_file = zip_close_file;
			ffunc.zerror_file = zip_error_file;
			ffunc.opaque = this;
	
			char zipfilename[] = "";
			mZipFile = unzOpen2_64(zipfilename, &ffunc);
		
			(mInstance->*mCallback)();
		}
		else if (result > 0)
		{
			// The URLLoader just filled "result" number of bytes into our buffer.
			// Save them and perform another read.
			AppendDataBytes(mBuffer, result);
			ReadBody();
		}
		else 
			CCLog("pp::URLLoader::ReadResponseBody() result<0");
	}

	void ReadBody()
	{
		// Note that you specifically want an "optional" callback here. This will
		// allow ReadBody() to return synchronously, ignoring your completion
		// callback, if data is available. For fast connections and large files,
		// reading as fast as we can will make a large performance difference
		// However, in the case of a synchronous return, we need to be sure to run
		// the callback we created since the loader won't do anything with it.
		pp::CompletionCallback cc =	mFactory.NewOptionalCallback(&FileSystem::OnRead);
		int32_t result = PP_OK;
		do
		{
			result = mUrlLoader.ReadResponseBody(mBuffer, READ_BUFFER_SIZE, cc);
			// Handle streaming data directly. Note that we *don't* want to call
			// OnRead here, since in the case of result > 0 it will schedule
			// another call to this function. If the network is very fast, we could
			// end up with a deeply recursive stack.
			if (result > 0)
				AppendDataBytes(mBuffer, result);
		} while (result > 0);

		if (result != PP_OK_COMPLETIONPENDING)
		{
			// Either we reached the end of the stream (result == PP_OK) or there was
			// an error. We want OnRead to get called no matter what to handle
			// that case, whether the error is synchronous or asynchronous. If the
			// result code *is* COMPLETIONPENDING, our callback will be called
			// asynchronously.
			cc.Run(result);
		}
	}
};

ZPOS64_T zip_tell64_file(voidpf opaque, voidpf stream)
{
	return ((FileSystem*)opaque)->ZipTell();
}

long zip_seek64_file(voidpf opaque, voidpf stream, ZPOS64_T offset, int origin)
{
	return ((FileSystem*)opaque)->ZipSeek(offset, origin);
}

voidpf zip_open64_file(voidpf opaque, const void* filename, int mode)
{
	static int stream = 0xDEADBEEF;
	((FileSystem*)opaque)->ZipOpen();
	return &stream;
}

uLong zip_read_file(voidpf opaque, voidpf stream, void* buf, uLong size)
{	
	return ((FileSystem*)opaque)->ZipReadFile(buf, size);
}

uLong zip_write_file(voidpf opaque, voidpf stream, const void* buf, uLong size)
{
	return size;
}

int zip_close_file(voidpf opaque, voidpf stream)
{
	return 0;
}

int zip_error_file(voidpf opaque, voidpf stream)
{
	return 0;
}

FileSystem* FileSystem::sm_pFileSystem = NULL;

NS_CC_BEGIN

#define MAX_LEN         (cocos2d::kMaxLogLen + 1)

void CCLog(const char * pszFormat, ...)
{
    char buf[MAX_LEN];
	
    va_list args;
    va_start(args, pszFormat);
    vsprintf(buf, pszFormat, args);    
    va_end(args);
	
	for (size_t i = MAX_LEN - 1; i >= 6; i--)
		buf[i] = buf[i-6];
	memcpy(buf, "CCLog:", 6);
	
	ApplicationInstance::sharedApplicationInstance().PostMessage(pp::Var(buf));
}

void CCMessageBox(const char * pszMsg, const char * pszTitle)
{
	char buf[256];
	sprintf(buf, "CCMessageBox:%s", pszMsg);
	ApplicationInstance::sharedApplicationInstance().PostMessage(pp::Var(buf));
}

unsigned char* CCFileUtils::getFileData(const char* pszFileName, const char* pszMode, unsigned long* pSize)
{    
	CCAssert(pszFileName != NULL && pSize != NULL && pszMode != NULL, "Invaild parameters.");
	unsigned char* pBuffer = FileSystem::sharedFileSystem().GetFileData(pszFileName, pszMode, pSize);

    if (! pBuffer && isPopupNotify())
    {
        std::string title = "Notification";
        std::string msg = "Get data from file(";
        msg.append(pszFileName).append(") failed!");

        CCMessageBox(msg.c_str(), title.c_str());
    }
    return pBuffer;
}


NS_CC_END

ApplicationInstance::ApplicationInstance(PP_Instance instance)
    : pp::Instance(instance), pp::Graphics3DClient(this), mFactory(this)
{
    CC_ASSERT(!sm_pApplicationInstance);
    sm_pApplicationInstance = this;
	
	mFileSystem = new FileSystem(this, &ApplicationInstance::FileSystemReady);
}

ApplicationInstance::~ApplicationInstance()
{
	glSetCurrentContextPPAPI(context_.pp_resource());
	glSetCurrentContextPPAPI(0);

	delete mFileSystem;
	CC_ASSERT(this == sm_pApplicationInstance);
    sm_pApplicationInstance = NULL;
}

ApplicationInstance& ApplicationInstance::sharedApplicationInstance()
{
	CC_ASSERT(sm_pApplicationInstance);
    return *sm_pApplicationInstance;
}

bool ApplicationInstance::Init(uint32_t /* argc */, const char* /* argn */[], const char* /* argv */[])
{
	mFileSystem->DownloadArchive();
	return true;
}

void ApplicationInstance::FileSystemReady()
{	
	mApp.applicationDidFinishLaunching();
}

void ApplicationInstance::HandleMessage(const pp::Var& message)
{
}

void ApplicationInstance::DidChangeView(const pp::View& view)
{	
	pp::Size size = view.GetRect().size();
	if (context_.is_null())
	{
		pp::Module* module = pp::Module::Get();
		assert(module);
		gles2_interface_ = static_cast<const struct PPB_OpenGLES2*>(
			module->GetBrowserInterface(PPB_OPENGLES2_INTERFACE));
		assert(gles2_interface_);
	  
		int32_t attribs[] = {
			PP_GRAPHICS3DATTRIB_ALPHA_SIZE, 8,
			PP_GRAPHICS3DATTRIB_DEPTH_SIZE, 24,
			PP_GRAPHICS3DATTRIB_STENCIL_SIZE, 8,
			PP_GRAPHICS3DATTRIB_SAMPLES, 0,
			PP_GRAPHICS3DATTRIB_SAMPLE_BUFFERS, 0,
			PP_GRAPHICS3DATTRIB_WIDTH, size.width(),
			PP_GRAPHICS3DATTRIB_HEIGHT, size.height(),
			PP_GRAPHICS3DATTRIB_NONE
		};
		context_ = pp::Graphics3D(this, pp::Graphics3D(), attribs);
		if (context_.is_null())
			glSetCurrentContextPPAPI(0);
		else
			BindGraphics(context_);
			
        CCEGLView* view = &CCEGLView::sharedOpenGLView();
        view->setFrameSize(size.width(), size.height());
	}
	glSetCurrentContextPPAPI(0);
	context_.ResizeBuffers(size.width(), size.height());
	glSetCurrentContextPPAPI(context_.pp_resource());
	glViewport(0, 0, size.width(), size.height());
	DrawSelf();	
}

void ApplicationInstance::DrawSelf(int32_t result)
{
	glSetCurrentContextPPAPI(context_.pp_resource());
	CCDirector::sharedDirector()->mainLoop();
	context_.SwapBuffers(mFactory.NewCallback(&ApplicationInstance::DrawSelf));
}
