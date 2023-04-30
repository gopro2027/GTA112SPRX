#include "stdafx.h"

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

int findGlobalsPointer() {
	//1.12 BLES sub_97CD7C
	uint8_t *search = (uint8_t*)"\x3C\xA0\x01\xE7\x39\x00\x00\x00\x3C\x80\x01\xE7\x30\xA5\x03\x70\x61\x06\x00\x00\x30\x84\x04\x70";
	uintptr_t addressptr = FindPattern(0x10200, 0x2269700, search, "xx??xxxxxx??xx??xxxxxx??");
	unsigned int a = *(unsigned short*)(addressptr+2);//lis
	short b = *(short*)(addressptr+0xE);//addic
	return (a << 16) + b;
}

int findNativesPointer() {
	//1.12 BLES sub_97C144
	uint8_t *search = (uint8_t*)"\x3C\x60\x01\xDA\x3C\xC0\x01\xDB\x38\x80\x00\x00\x30\x63\x7B\xB0\x38\xA0\x01\x00\x98\x86\x82\x18\x7C\xA9\x03\xA6\x90\x83\x04\x00\x90\x83\x00\x00\x30\x63\x00\x04\x42\x00\xFF\xF8\x4E\x80\x00\x20";
	uintptr_t addressptr = FindPattern(0x10200, 0x2269700, search, "xx??xx??xxxxxx??xxxx????xxxx????????xxxx????xxxx");
	unsigned int a = *(unsigned short*)(addressptr+2);//lis
	short b = *(short*)(addressptr+0xE);//addic
	return (a << 16) + b;
}

int edatFunctionCallAddress;
int findEdatPatchPointer() {
	//1.12 BLES 0x353D04
	uint8_t *search = (uint8_t*)"\x38\xA0\x01\x00\x00\x00\x00\x00\x80\x7F\x00\x04\x63\xA4\x00\x00\x38\xA0\x00\x01\x38\xC0\x00\x00\x38\xE0\x00\x00";
	uintptr_t addressptr = FindPattern(0x10200, 0x2269700, search, "xxxx????xxxxxxxxxxxxxxxxxxxx");
	edatFunctionCallAddress = addressptr+0x1C + ((*(int*)(addressptr+0x1C))&0xFFFFFF - 1);
	return addressptr + 0x10;
}

int findDefaultXMLPointer() {
	//1.12 BLES 0x174C4B4
	uint8_t *search = (uint8_t*)				   "commoncrc:/data/default";
	return FindPattern(0x10200, 0x2269700, search, "xxxxxxxxxxxxxxxxxxxxxxx");
}


__NO_INLINE uint64_t testHookEdat(uint64_t r3, uint64_t r4) {
	printf("Edat file: %s\n",r4);
	if (strstr((char*)r4, "mpBusiness") > (char*)0x10000) {
		strcpy((char*)r4, "/dev_hdd0/tmp/b.edat");
		//strcpy((char*)r4, "/dev_hdd0/game/BLES01807/USRDIR/dlc/dlc_mpHipser/DLC.edat");
		printf("Overwriting business %s\n",r4);
	}
	
	return call<int>(edatFunctionCallAddress)(r3,r4,1,0,0);//0xA5D800
}



/*
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
}*/

sys_ppu_thread_t ThreadModuleID;
uint64_t main_hook_original_opd;
void init(uint64_t threadinfo) {
	char *str = "SPRX started!\n";
	createFile("/dev_hdd0/tmp/debugfile.txt");
	writeToFile("/dev_hdd0/tmp/debugfile.txt",str,strlen(str));


	//Maybe try editing the stats config file to change all the mp stats to save locally? I know they have some sort of info in it, it's somewhere in the rpf

	//for disabling the business update so jobs work
	PatchInJump(findEdatPatchPointer(), (int)testHookEdat, true);

	//patch stats
	//PatchInJump(0x30AB18, (int)statPatch, true);
	//*(int*)0x30B05C = 0x7C451378;//mr %r5, %r2
	//PatchInJump(0x30B064, (int)statFileNameHook/*_intermediate*/, true);
	//int *FuncBytes = getPatchInJump((int)statFileNameHook, true);
	//write_process((void*)0x30B05C, FuncBytes, 4);
	//write_process((void*)(0x30B05C+0x8), &FuncBytes[1], 4*3);
	char *defaultstring = (char*)findDefaultXMLPointer();
	printf("default string: 0x%X\n",defaultstring);
	strcpy(defaultstring/*0x174C4B4*/,"/dev_hdd0/tmp/default");//"commoncrc:/data/default"


	//some network stuff to force it to return that it is online in a lot of places
	//*(int*)0xCEED88 = 0x38600001;//li %r3, 1 getCloudAvailable1
	//*(int*)0x125B7D0 =0x38600001;
	//*(int*)0x125B6F8 =0x38600001;//125B6F8 Is_Rockstar_Services_Available
	//*(int*)0x125B6FC =0x4E800020;

	//NETWORK_IS_CLOUD_AVAILABLE native bypass... have to leave this in to get the temporary character creation
	//*(int*)0x3C1968 = 0x38600001;
	//*(int*)0x3C196C = 0x4E800020;

	//DLC_CHECK_CLOUD_DATA_CORRECT native bypass
	*(int*)0x3C1598 = 0x38600001;
	*(int*)0x3C159C = 0x4E800020;

	//r* policy bypass (sub_9668C(1) displays it)
	//*(int*)0x96A6C = 0x38600000;//bypass check, this works but eh it's just the native so disable the native instead
	//*(int*)0x14D2BD0 = 0x4E800020;//disable OPEN_ONLINE_POLICIES_MENU

	//ARE_ONLINE_POLICIES_UP_TO_DATE native bypass
	*(int*)0x14D2C04 = 0x38600001;

	//FORCE_CLOUD_MP_STATS_DOWNLOAD_AND_OVERWRITE_LOCAL_SAVE native disable
	*(int*)0x14fa670 = 0x4E800020;

	/*
	0x805d7cbb36fd6c4c,OPEN_ONLINE_POLICIES_MENU
0xf13fe2a80c05c561,ARE_ONLINE_POLICIES_UP_TO_DATE
0x6f72cd94f7b5b68c,IS_ONLINE_POLICIES_MENU_ACTIVE
0x75d3691713c3b05a,OPEN_SOCIAL_CLUB_MENU
0xd2b32be3fc1626c6,CLOSE_SOCIAL_CLUB_MENU
	*/

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


	//if (*(int*)0x0978308 == 0x7FE10808) {
		//fix the trap for jobs? idfk someone sent this out and i mean it can't hurt it's a trap so it would freeze the game either way
		//*(int*)0x0978308 = 0x60000000;
	//}


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
