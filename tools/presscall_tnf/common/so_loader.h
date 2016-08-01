
#ifndef _SO_FILE_H_
#define _SO_FILE_H_

#include <dlfcn.h>
#include <string>
#include <stdio.h>
#include <assert.h>

class CSoLoader
{
public:
    CSoLoader() : m_handle(NULL){}
    ~CSoLoader()
    {
        if (m_handle ) {
            dlclose(m_handle);
        }
        m_handle = NULL;
    }

    bool Open(const std::string& som_path)
    {
        if (m_handle) {
            dlclose(m_handle);
            m_handle = NULL;
        }
        m_path = som_path;
        m_handle = dlopen (m_path.c_str(), RTLD_LAZY | RTLD_GLOBAL);
        if (m_handle == NULL) {
            fprintf(stderr, "%s\n", dlerror());
            return false;
        }
        return true;
    }

    void* GetFunc(const std::string& func_name)
    {
        assert(m_handle != NULL);
        dlerror();    /* Clear any existing error */
        void* ret = dlsym(m_handle, func_name.c_str());

        char *error;
        if ((error = dlerror()) != NULL) {
            // fprintf(stderr, "%s\n", error);
            return NULL;
        }
        else
            return ret;
    }

private:
    std::string m_path;
    void* m_handle;
};


//////////////////////////////////////////////////////////////////////////
#endif//_SO_FILE_H_
///:~
