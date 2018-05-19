#include "DynamicLibrary.h"

#include <iostream>
#include <dlfcn.h>

DynamicLibrary::DynamicLibrary(const char *fileName)
{
    libHandle = dlopen(fileName, RTLD_LAZY);
	if (!libHandle) //throw OpenLibFailedException();
	{
		isSupport = false;
	}
	else
	{
		JACONNECT_INFO("can dlopen this lib");
		isSupport = true;
	}
}

DynamicLibrary::~DynamicLibrary()
{
    if (libHandle) dlclose(libHandle);
}

void *DynamicLibrary::getFunctionPtr(const char *name) const
{
    void * ret = (void *)dlsym(libHandle, name);
    if (ret == NULL) {
        std::cerr << "Failed to get function " << name << std::endl;
    }
    return ret;
}

