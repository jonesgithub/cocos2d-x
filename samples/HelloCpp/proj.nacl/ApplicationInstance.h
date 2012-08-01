#pragma once

#include <pthread.h>
#include <map>
#include <vector>

#include <ppapi/cpp/instance.h>
#include <ppapi/c/ppb_opengles2.h>
#include <ppapi/cpp/graphics_3d_client.h>
#include <ppapi/cpp/graphics_3d.h>
#include <ppapi/cpp/completion_callback.h>
#include <ppapi/utility/completion_callback_factory.h>

#include "AppDelegate.h"

class ApplicationInstance : public pp::Instance, public pp::Graphics3DClient
{
public:
	explicit ApplicationInstance(PP_Instance instance);
	virtual ~ApplicationInstance();
	
	static ApplicationInstance& sharedApplicationInstance();

	virtual bool Init(uint32_t argc, const char* argn[], const char* argv[]);
	
	virtual void Graphics3DContextLost()
	{
		assert(!"Unexpectedly lost graphics context");
	}
	
	void FileSystemReady();
	
	virtual void DidChangeView(const pp::View& view);
	virtual void HandleMessage(const pp::Var& message);
	void DrawSelf(int32_t result = 0);
protected:
	static ApplicationInstance* sm_pApplicationInstance;

	class FileSystem* mFileSystem;
	
	pp::CompletionCallbackFactory<ApplicationInstance> mFactory;
	
	pp::Graphics3D context_;
	const struct PPB_OpenGLES2* gles2_interface_;

	AppDelegate	mApp;	
};
