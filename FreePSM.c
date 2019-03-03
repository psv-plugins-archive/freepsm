#include <taihen.h>
#include <vitasdk.h>
#include <string.h>

static SceUID LoadModuleHook = -1;
static tai_hook_ref_t LoadModuleHook_ref;

static SceUID SceLibPsmAimMgrIsDexHook = -1;
static tai_hook_ref_t SceLibPsmAimMgrIsDexHook_ref;

char titleid[12];



int SceLibPsmAimMgrIsDex_p()
{
	return 1;
}

SceUID sceKernelLoadStartModule_p(char *path, SceSize args, void *argp, int flags, SceKernelLMOption *option, int *status)
{
	sceClibPrintf("[FreePSM] Starting Module: %s\n",path);
	
	SceUID ret;
	ret = TAI_CONTINUE(SceUID, LoadModuleHook_ref, path, args, argp, flags, option, status);
	
	if(strstr(path,"libpsm.suprx")) //PSM
	{
		SceLibPsmAimMgrIsDexHook = taiHookFunctionOffset(&SceLibPsmAimMgrIsDexHook_ref, 
									ret,
									0,
									0x450da, //SceLibPsmAimMgrIsDex
									1, 
									SceLibPsmAimMgrIsDex_p);
		sceClibPrintf("[FreePSM] SceLibPsmAimMgrIsDexHook %x %x\n",SceLibPsmAimMgrIsDexHook,SceLibPsmAimMgrIsDexHook_ref);
	}
	
	if(strstr(path,"libScePsmInAppPurchase.suprx")) //PSM Unity
	{
		tai_module_info_t tai_info;
		tai_info.size = sizeof(tai_module_info_t);
  	    taiGetModuleInfo("SceLibPsmInAppPurchase", &tai_info);

		switch (tai_info.module_nid) {
			case 0x1E9E7B88: //1.00 - 1.01 Unity Runtime
				SceLibPsmAimMgrIsDexHook = taiHookFunctionOffset(&SceLibPsmAimMgrIsDexHook_ref, 
											ret,
											0,
											0x3672, //SceLibPsmAimMgrIsDex
											1, 
											SceLibPsmAimMgrIsDex_p);
				sceClibPrintf("[FreePSM] SceLibPsmAimMgrIsDexHook %x %x\n",SceLibPsmAimMgrIsDexHook,SceLibPsmAimMgrIsDexHook_ref);
				break;
			case 0x4E7A28FC: //1.02 Unity Runtime
				SceLibPsmAimMgrIsDexHook = taiHookFunctionOffset(&SceLibPsmAimMgrIsDexHook_ref, 
											ret,
											0,
											0x366a, //SceLibPsmAimMgrIsDex
											1, 
											SceLibPsmAimMgrIsDex_p);
				sceClibPrintf("[FreePSM] SceLibPsmAimMgrIsDexHook %x %x\n",SceLibPsmAimMgrIsDexHook,SceLibPsmAimMgrIsDexHook_ref);
				break;
		}
	}
	
	return ret;
}


void _start() __attribute__ ((weak, alias ("module_start"))); 

void module_start(SceSize argc, const void *args) {
	sceAppMgrAppParamGetString(0, 12, titleid, 256);

	if(!strcmp(titleid,"PCSI00011") || !strcmp(titleid,"PCSI00010")) // PSM Runtime & PSM Unity Runtime
	{
		sceClibPrintf("[FreePSM] Silca: I like to see girls die :3\n");
		sceClibPrintf("[FreePSM] Loaded!\n");
		sceClibPrintf("[FreePSM] Running on %s\n",titleid);

		LoadModuleHook = taiHookFunctionImport(&LoadModuleHook_ref, 
										  TAI_MAIN_MODULE,
										  TAI_ANY_LIBRARY,
										  0x2DCC4AFA, //sceKernelLoadStartModule
										  sceKernelLoadStartModule_p);

		sceClibPrintf("[FreePSM] LoadModuleHook %x, %x\n",LoadModuleHook,LoadModuleHook_ref);
	}
}

int module_stop(SceSize argc, const void *args) {

	// release hooks
	return SCE_KERNEL_STOP_SUCCESS;
}
