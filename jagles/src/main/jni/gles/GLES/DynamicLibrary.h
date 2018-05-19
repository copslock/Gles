#pragma once

#include <exception>
#include <string>
#ifdef __ANDROID__
#include <android/log.h>
#define JACONNECT_INFO(...) {__android_log_print(ANDROID_LOG_INFO, "videoconnect", __VA_ARGS__);}
#endif
class DynamicLibrary
{
public:
    DynamicLibrary(const char *fileName);
    ~DynamicLibrary();

    void *getFunctionPtr(const char *name) const;

    DynamicLibrary(const DynamicLibrary &) = delete;
    DynamicLibrary & operator = (const DynamicLibrary &other) = delete;
	bool isSupport;
private:
    void *libHandle;
};

class OpenLibFailedException: public std::exception
{
    virtual const char* what() const throw()
    {
        return "Failed to open Library";
    }
};

