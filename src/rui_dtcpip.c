/* Copyright (C) 2013 Cable Television Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS
 * IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL CABLE TELEVISION LABS INC. OR ITS
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <glib.h>
#include <string.h>

#include "rui_dtcpip.h"
#include "rui_dll.h"

GST_DEBUG_CATEGORY_EXTERN(gst_dtcpip_debug);
#define GST_CAT_DEFAULT gst_dtcpip_debug

struct dtcpip_untyped_function_entry_s
{
    char *name;
    void (*func)();
};

typedef struct dtcpip_untyped_function_entry_s dtcpip_untyped_function_entry_t;

#define DTCPIP_STUB(ret,func,...) \
    static ret func ## _stub(__VA_ARGS__)

DTCPIP_STUB(int, dtcpip_cmn_init, const char* storage_path);
DTCPIP_STUB(void, dtcpip_cmn_get_version, char* string, size_t length);

DTCPIP_STUB(int, dtcpip_src_init, unsigned short dtcp_port);
DTCPIP_STUB(int, dtcpip_src_open, int* session_handle, int is_audio_only);
DTCPIP_STUB(int, dtcpip_src_alloc_encrypt, int session_handle,
                 unsigned char cci,
                 char* cleartext_data, size_t cleartext_size,
                 char** encrypted_data, size_t* encrypted_size);
DTCPIP_STUB(int, dtcpip_src_free, char* encrypted_data);
DTCPIP_STUB(int, dtcpip_src_close,int session_handle);

DTCPIP_STUB(int, dtcpip_snk_init, void);
DTCPIP_STUB(int, dtcpip_snk_open,
                 char* ip_addr, unsigned short ip_port,
                 int *session_handle);
DTCPIP_STUB(int, dtcpip_snk_alloc_decrypt, int session_handle,
                 char* encrypted_data, size_t encrypted_size,
                 char** cleartext_data, size_t* cleartext_size);
DTCPIP_STUB(int, dtcpip_snk_free, char* cleartext_data);
DTCPIP_STUB(int, dtcpip_snk_close, int session_handle);



#define DTCPIP_INIT(func) \
    { #func, (void(*)()) func ## _stub }
 
static dtcpip_untyped_function_entry_t untyped_functions[] =
{
    DTCPIP_INIT(dtcpip_cmn_init),
    DTCPIP_INIT(dtcpip_cmn_get_version),
    DTCPIP_INIT(dtcpip_src_init),
    DTCPIP_INIT(dtcpip_src_open),
    DTCPIP_INIT(dtcpip_src_alloc_encrypt),
    DTCPIP_INIT(dtcpip_src_free),
    DTCPIP_INIT(dtcpip_src_close),

    DTCPIP_INIT(dtcpip_snk_init),
    DTCPIP_INIT(dtcpip_snk_open),
    DTCPIP_INIT(dtcpip_snk_alloc_decrypt),
    DTCPIP_INIT(dtcpip_snk_free),
    DTCPIP_INIT(dtcpip_snk_close)
};

dtcpip_typed_functions_t* g_dtcpip_ftable = NULL;

static rui_Dlmod dtcp_dll = (rui_Dlmod) 0;

DTCPIP_STUB(int, dtcpip_cmn_init, const char* storage_path)
{
    GST_DEBUG("Not implemented");

    return TRUE;
}

DTCPIP_STUB(void, dtcpip_cmn_get_version, char* string, size_t length)
{
    GST_DEBUG("Not implemented");
}

DTCPIP_STUB(int, dtcpip_src_init, unsigned short dtcp_port)
{
    GST_DEBUG("Not implemented");

    return TRUE;
}

DTCPIP_STUB(int, dtcpip_src_open, int* session_handle, int is_audio_only)
{
    GST_DEBUG("Not implemented");

    return -1;
}

DTCPIP_STUB(int, dtcpip_src_alloc_encrypt, int session_handle,
                 unsigned char cci,
                 char* cleartext_data, size_t cleartext_size,
                 char** encrypted_data, size_t* encrypted_size)
{
    return -1;
}

DTCPIP_STUB(int, dtcpip_src_free, char* encrypted_data)
{
    return -1;
}

DTCPIP_STUB(int, dtcpip_src_close,int session_handle)
{
    return -1;
}

DTCPIP_STUB(int, dtcpip_snk_init, void)
{
    GST_DEBUG("Not implemented");

    return TRUE;
}

DTCPIP_STUB(int, dtcpip_snk_open,
                 char* ip_addr, unsigned short ip_port,
                 int *session_handle)
{
    GST_DEBUG("Not implemented");

    return -1;
}

DTCPIP_STUB(int, dtcpip_snk_alloc_decrypt, int session_handle,
                 char* encrypted_data, size_t encrypted_size,
                 char** cleartext_data, size_t* cleartext_size)
{
    return -1;
}

DTCPIP_STUB(int, dtcpip_snk_free, char* cleartext_data)
{
    return -1;
}

DTCPIP_STUB(int, dtcpip_snk_close, int session_handle)
{
    GST_DEBUG("Not implemented");

    return -1;
}


/**
 * Initializes the DTCP/IP library. If the library cannot be
 * located or does not have all the required functions, DTCP
 * encryption will be disabled and local stub functions will
 * be used instead.
 */
gboolean rui_dtcpip_init()
{
    GST_DEBUG("called");

    gboolean ret_code = FALSE;
    const gchar *dll_path = NULL;

    g_dtcpip_ftable = (dtcpip_typed_functions_t*) &untyped_functions;

    dll_path = getenv(RUIH_GST_DTCP_DLL_ENV);
    if (dll_path == NULL)
    {
        GST_ERROR("dtcp dll env var %s is NULL", RUIH_GST_DTCP_DLL_ENV);
        return FALSE;
    }
    else
    {
        GST_INFO("opening dll using path: %s", dll_path);
        ret_code = rui_dlmodOpen(dll_path, &dtcp_dll);
        if (ret_code == TRUE)
        {
            gint i = 0;
            gint num_funcs = -1;

            GST_INFO("successfully loaded DTCP/IP library from \"%s\".", dll_path);

            // First check that all functions defined in the structure can
            // be located in the supplied DLL.Cleaned up tab/spaces in all files
            num_funcs = sizeof(untyped_functions) / sizeof(dtcpip_untyped_function_entry_t);
            for (i = 0; i < num_funcs; i++)
            {
                void (*func)() = NULL;
                ret_code = rui_dlmodGetSymbol(dtcp_dll, untyped_functions[i].name, (void **) &func);
                if (ret_code == TRUE)
                {
                    GST_DEBUG("successfully located function \"%s\".", untyped_functions[i].name);
                }
                else
                {
                    GST_ERROR("unable to locate function \"%s\".", untyped_functions[i].name);
                    break;
                }
            }

            // If all symbols can be located, go ahead and re-assign all of the function pointers.
            if (ret_code == TRUE)
            {
                for (i = 0; i < num_funcs; i++)
                {
                    ret_code = rui_dlmodGetSymbol(dtcp_dll, untyped_functions[i].name,
                            (void **) &untyped_functions[i].func);
                    if (ret_code != TRUE)
                    {
                        GST_ERROR("unable to get symbol \"%s\".", untyped_functions[i].name);
                        return FALSE;
                    }
                }
            }
        }
        else
        {
            GST_ERROR("unable to load DTCP/IP library from \"%s\": Shared Library error %d.",
                    dll_path, ret_code);
            return FALSE;
        }
    }

    if (ret_code != TRUE)
    {
        GST_WARNING("DTCP/IP disabled.");
        return FALSE;
    }

    gpointer dtcp_storage;
    gsize storage_len = 0;

    const gchar* dtcp_storage_env = getenv(RUIH_GST_DTCP_KEY_STORAGE_ENV);
    if (dtcp_storage_env == NULL)
    {
        GST_INFO("%s not defined.", RUIH_GST_DTCP_KEY_STORAGE_ENV);
        const gchar *last_slash = strrchr(dll_path, '/');
        if (last_slash != NULL)
        {
            storage_len = (int) (last_slash - dll_path);
            dtcp_storage = g_malloc0(storage_len +1);
            memcpy(dtcp_storage, dll_path, storage_len);
            ((gchar*)dtcp_storage)[storage_len] = '\0';
        }
        else
        {
            storage_len = 1;
            dtcp_storage = g_malloc0(storage_len +1);
            ((gchar*)dtcp_storage)[0] = '.';
            ((gchar*)dtcp_storage)[1] = '\0';
        }
    }
    else
    {
        storage_len = strlen(dtcp_storage_env);
        dtcp_storage = g_malloc0(storage_len +1);
        memcpy(dtcp_storage, dtcp_storage_env, storage_len);
        ((gchar*)dtcp_storage)[storage_len] = '\0';
    }

    gint result = 0;
    GST_INFO("using \"%s\" for DTCP/IP library storage.", (gchar*)dtcp_storage);
    result = g_dtcpip_ftable->dtcpip_cmn_init(dtcp_storage);
    if (result != 0)
    {
        GST_ERROR("dtcpip_cmn_init() failed with %d.", result);
        g_free(dtcp_storage);
        return FALSE;
    }

    g_free(dtcp_storage);

    GST_INFO("DTCP/IP enabled.");

    return TRUE;
}
