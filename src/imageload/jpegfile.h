#ifndef __jpegfile_h__
#define __jpegfile_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

extern unsigned char* jpg_load(char* fn, int* w, int* h, int* comps, int t_comps);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__jpegfile_h__
