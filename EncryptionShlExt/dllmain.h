// dllmain.h : Declaration of module class.

class CEncryptionShlExtModule : public ATL::CAtlDllModuleT< CEncryptionShlExtModule >
{
public :
	DECLARE_LIBID(LIBID_EncryptionShlExtLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_ENCRYPTIONSHLEXT, "{6d7ffa90-8339-45fa-8a0e-510b26868b29}")
};

extern class CEncryptionShlExtModule _AtlModule;
