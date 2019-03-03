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
	int ret = TAI_CONTINUE(int, SceLibPsmAimMgrIsDexHook_ref,);
	
	if(ret > -1)
	{
			return 1;
	}
	sceClibPrintf("[FreePSM] ERROR: SceLibPsmAimMgrIsDex returned %x",ret);
	return ret;
}

SceUID sceKernelLoadStartModule_p(char *path, SceSize args, void *argp, int flags, SceKernelLMOption *option, int *status)
{
	sceClibPrintf("[FreePSM] Starting Module: %s\n",path);
	
	SceUID ret;
	ret = TAI_CONTINUE(SceUID, LoadModuleHook_ref, path, args, argp, flags, option, status);
	
	if(strstr(path,"libpsm.suprx"))
	{
		SceLibPsmAimMgrIsDexHook = taiHookFunctionOffset(&SceLibPsmAimMgrIsDexHook_ref, 
									ret,
									0,
									0x450da, //2.01 Retail Runtime
									1, 
									SceLibPsmAimMgrIsDex_p);
		sceClibPrintf("[FreePSM] SceLibPsmAimMgrIsDexHook %x %x\n",SceLibPsmAimMgrIsDexHook,SceLibPsmAimMgrIsDexHook_ref);
	}
	return ret;
}


void _start() __attribute__ ((weak, alias ("module_start"))); 

void module_start(SceSize argc, const void *args) {
	sceAppMgrAppParamGetString(0, 12, titleid, 256);

	if(!strcmp(titleid,"PCSI00011")) // PSM Runtime
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
