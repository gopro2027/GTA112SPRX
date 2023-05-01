#include "stdafx.h"

//ANY OTHER HARDCODED ADDRESSES ARE ON 1.12 BLES
int G_NATIVES;
int GLOBALS_PTR;
int TOC;
int MAIN_HOOK_ADDRESS;

//#include <libpsutil.h> https://github.com/skiff/libpsutil requires sdk 4.75
#include <cellstatus.h>
#include <sys/prx.h>
#include <ppu_asm_intrinsics.h> 
#include <cellstatus.h>
#include <sys/prx.h>
#include <sys/ppu_thread.h>
#include <string.h> 
#include <sys/memory.h>
#include <sys/timer.h>
#include <sys/process.h>
#include <ppu_intrinsics.h>
#include <stdarg.h>
#include <cell/cell_fs.h>
#include <stdio.h>
#include <sysutil\sysutil_msgdialog.h>
#include <sysutil\sysutil_oskdialog.h>
#include <sysutil\sysutil_oskdialog_ext.h>

#include <cell/pad.h>
#include <cell/padfilter.h> 

#include <cstdlib>
#include <math.h>


void debug_write(char *str);

#include "types.h"
#include "utils.h"
#include "GTAStructs.h"
#include "Natives.h"
#include "gtaCode.h"
#include "menu.h"

void main_hook(int r3) {
	*(int*)(*(int*)(r3)+0x0) = 1;//set IS_PLAYER_ONLINE to true
	draw_menu();
}

/*
//now unused stat stuff. This kept causing issues for some reason so it's easier just to edit the file and point it to load from a nicer location than the rpf
void statPatch(Stat *stat);
void statPatches(uint64_t val) {
	while (VALID_ADDR(val)) {
		statPatch((Stat*)val);
		val = *(unsigned int*)(val+0x14);
	}
}

__NO_INLINE int statFileNameHook(uint64_t r3, uint64_t file, uint64_t r5) {
	//hooked at 0x30B064
	printf("Stat: %s\n",file);

	if (strstr((char*)file,"common:/data/mpStatsSetup.xml")) {
		strcpy((char*)file,"/dev_hdd0/tmp/mpStatsSetup.xml");
	}
	if (strstr((char*)file,"common:/data/spStatsSetup.xml")) {
		//strcpy((char*)file,"/dev_hdd0/tmp/spStatsSetup.xml");
	}

	int retval = call<int>(0xA72EF8,r5)(*(int*)0x1C04020, file, "xml", 0);

	
	if (strstr((char*)file,"mpStat")) {
	if (VALID_ADDR(retval)) {
		uint64_t r28 = *(int*)retval;
		if (VALID_ADDR(r28)) {
			r28 = *(int*)(r28+0x18);//r28 is now the stats thing
			while (true) {
				if (VALID_ADDR(r28)) {
					if (strstr((char*)*(int*)r28,"stats")) {
						printf("patching stat!\n");
						statPatches((*(int*)(r28+0x18)));
					}
					r28 = *(int*)(r28 + 0x14);
					if (!(VALID_ADDR(r28))) {
						break;
					}
				}
			}
		}
	}
	}
	return retval;
}



__NO_INLINE __NAKED int statFileNameHook_intermediate() {
	__asm("mflr %r0");
	__asm("stdu %r1, -0x70(%r1)");
	__asm("std %r0, 0x60(%r1)");

	//save old register values in stack
	__asm("std %r25, 0x58(%r1)");//stores r5 in the stack for us, unrelated to the code below but could be if r5 was important and we needed to overwrite it
	
	//move whatever registers and data we want and call our function
	__asm("mr %r5, %r2");
	__asm("bl ._Z16statFileNameHookyyy");

	__asm("ld %r2, 0x58(%r1)");
	__asm("ld %r0, 0x60(%r1)");
	__asm("mtlr %r0");
	__asm("addi %r1, %r1, 0x70");
	__asm("blr");
}



void statPatch(Stat *stat) {
	//uint64_t hasStatName = call<uint64_t>(0xA71078)(stat, "Name");//no longer need to call this
		char *statname = 0;
		char *online = 0;
		char *profile = 0;
		char *SaveCategory = 0;
		char *ServerAuthoritative = 0;
		char *FlushPriority = 0;
		for (int i = 0; i < 20; i++) {
			uint64_t name = *(int*)(stat->arrayOfStatEntries + (i * 4));
			uint64_t val = *(int*)(stat->arrayOfStatEntries + (i * 4) + 4);
			//printf("%s: %s\n",name,val);
			
			
			if (VALID_ADDR(name) && VALID_ADDR(val)) {
				//printf("%i %s %s\n",i,data, val);
				if (strstr((char*)name,"Name")) {
					statname = (char*)val;
				}
				if (strstr((char*)name,"online")) {
					online = (char*)val;
				}
				if (strstr((char*)name,"profile")) {
					profile = (char*)val;
				}
				if (strstr((char*)name,"SaveCategory")) {
					SaveCategory = (char*)val;
				}
				if (strstr((char*)name,"ServerAuthoritative")) {
					ServerAuthoritative = (char*)val;
				}
				if (strstr((char*)name,"FlushPriority")) {
					FlushPriority = (char*)val;
				}
			}
		}
		if (VALID_ADDR(online)) {
			strcpy(online, "false");
			printf("Set %s to offline\n",statname);
		}
}

void patchStatsHooking() {
	//patch stats
	//PatchInJump(0x30AB18, (int)statPatch, true);
	//*(int*)0x30B05C = 0x7C451378;//mr %r5, %r2
	//PatchInJump(0x30B064, (int)statFileNameHook, true);
	//int *FuncBytes = getPatchInJump((int)statFileNameHook, true);
	//write_process((void*)0x30B05C, FuncBytes, 4);
	//write_process((void*)(0x30B05C+0x8), &FuncBytes[1], 4*3);
}
*/

sys_ppu_thread_t ThreadModuleID;
uint64_t main_hook_original_opd;
void init(uint64_t threadinfo) {
	char *str = "SPRX started!\n";
	createFile("/dev_hdd0/tmp/debugfile.txt");
	writeToFile("/dev_hdd0/tmp/debugfile.txt",str,strlen(str));

	debug_append("Loading... if you do not see success after this line, then the sprx has crashed...\n");

	//for disabling the business update so jobs work
	PatchInJump(findEdatPatchPointer(), (int)testHookEdat, true);

	//patchStatsHooking();//this is outdated because it didn't work well for some reason so I gave up and used the code below instead because it's much simpler

	//UNCOMMENT THIS TO KEEP STATS LOADED! MAKE SURE TO HAVE THE 3 XML FILES IN THE PS3_DEBUG FOLDER IN YOUR PS3'S TMP FOLDER. CURRENTLY THIS WILL CAUSE ISSUES IF YOU SAVE YOUR GAME AND RELOAD IT, BUT THIS WILL ALLOW YOU TO EXIT TO SP AND GO BACK TO MP. WE NEED TO FIND OUT WHAT STUFF TO PATCH IN THE SCRIPT FILES TO MAKE THIS WORK AFTER A WHOLE GAME RELOAD! LOOK AT THE GLOBAL RESEARCH DONE IN GTACODE.H
	//strcpy((char*)findDefaultXMLPointer()/*0x174C4B4*/,"/dev_hdd0/tmp/default");//"commoncrc:/data/default"  easier just to tell it to load the file from tmp


	//wait for natives table to be initialized
	sleep(30000);

	//find values
	G_NATIVES = findNativesPointer();// 1.12 BLES 0x1DA7BB0
	GLOBALS_PTR = findGlobalsPointer();// 1.12 BLES 0x1DA7FF0;
	MAIN_HOOK_ADDRESS = getOPDOfNative(0x6E5BDCE2); // NETWORK_IS_SIGNED_ONLINE opd struct 1.12 BLES 0x1B68080
	TOC = *(int*)(MAIN_HOOK_ADDRESS+0x4);

	//create main hook
	main_hook_original_opd = *(uint64_t*)MAIN_HOOK_ADDRESS;
	*(uint64_t*)MAIN_HOOK_ADDRESS = *(uint64_t*)((int)main_hook);

	//create other things
	setup_textmessage_fix();
	setup_stats_fix();


	//debug info if setup was successful
	debug_append("Success!\n");

	sys_ppu_thread_exit(threadinfo);
}

void deinit() {
	//undo main hook
	*(uint64_t*)MAIN_HOOK_ADDRESS = main_hook_original_opd;
}

// Important PRX info
SYS_MODULE_INFO( GTA112SPRX, 0, 1, 1);
SYS_MODULE_START( _GTA112SPRX_prx_entry );

SYS_LIB_DECLARE_WITH_STUB( LIBNAME, SYS_LIB_AUTO_EXPORT, STUBNAME );
SYS_LIB_EXPORT( _GTA112SPRX_export_function, LIBNAME );

// An exported function is needed to generate the project's PRX stub export library
extern "C" int _GTA112SPRX_export_function(void)
{
	deinit();
    return CELL_OK;
}

extern "C" int _GTA112SPRX_prx_entry(void)
{
	sys_ppu_thread_create(&ThreadModuleID, init, 0, 0x5AA, 4096*5, 0, "gopro_2027's Mod Menu");
			
    return SYS_PRX_RESIDENT;
}
