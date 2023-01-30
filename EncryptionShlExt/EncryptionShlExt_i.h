

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Tue Jan 19 06:14:07 2038
 */
/* Compiler settings for EncryptionShlExt.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0622 
    protocol : all , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */



/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __EncryptionShlExt_i_h__
#define __EncryptionShlExt_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IShellExtension_FWD_DEFINED__
#define __IShellExtension_FWD_DEFINED__
typedef interface IShellExtension IShellExtension;

#endif 	/* __IShellExtension_FWD_DEFINED__ */


#ifndef __ShellExtension_FWD_DEFINED__
#define __ShellExtension_FWD_DEFINED__

#ifdef __cplusplus
typedef class ShellExtension ShellExtension;
#else
typedef struct ShellExtension ShellExtension;
#endif /* __cplusplus */

#endif 	/* __ShellExtension_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "shobjidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IShellExtension_INTERFACE_DEFINED__
#define __IShellExtension_INTERFACE_DEFINED__

/* interface IShellExtension */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IShellExtension;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("c715a75a-0391-4b5c-83b1-6d713350b738")
    IShellExtension : public IUnknown
    {
    public:
    };
    
    
#else 	/* C style interface */

    typedef struct IShellExtensionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellExtension * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellExtension * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellExtension * This);
        
        END_INTERFACE
    } IShellExtensionVtbl;

    interface IShellExtension
    {
        CONST_VTBL struct IShellExtensionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellExtension_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IShellExtension_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IShellExtension_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IShellExtension_INTERFACE_DEFINED__ */



#ifndef __EncryptionShlExtLib_LIBRARY_DEFINED__
#define __EncryptionShlExtLib_LIBRARY_DEFINED__

/* library EncryptionShlExtLib */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_EncryptionShlExtLib;

EXTERN_C const CLSID CLSID_ShellExtension;

#ifdef __cplusplus

class DECLSPEC_UUID("72c36a7a-ba29-42b2-bb49-bcc58bf4b945")
ShellExtension;
#endif
#endif /* __EncryptionShlExtLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


