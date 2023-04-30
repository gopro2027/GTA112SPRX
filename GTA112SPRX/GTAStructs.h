//LITERALLY ALL OF THESE ARE "OUTDATED" BECAUSE THEY ARE 1.27!!! ASSUME THE WORST!


//some useful defines
#define VALID_ADDR(x) ((unsigned int)(x) > 0x10000)
typedef unsigned long long ui64;
typedef long long s64;
typedef unsigned long long u64;
typedef unsigned int ui32;
typedef int s32;
typedef unsigned char u8;
typedef uint64_t(*one_hook_function_call)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);
#define TRAP() __asm("trap")
#define SC() __asm("sc")
#define TRAP_BYTES (0x7FE00008) 
#define SC_BYTES (0x44000002)
#define BIT_FROM_LEFT(x) (0x80000000>>x)
#define BIT_FROM_RIGHT(x) (1<<x)
#define B(address_jump_at, address_jump_to) (0x48000000 & ((address_jump_to-address_jump_at) & 0xFFFFFF))
#define BL(address_jump_at, address_jump_to) (B(address_jump_at, address_jump_to) + 1)
#define BA(address_jump_to) (B(0,address_jump_to) + 2)
#define BLA(address_jump_to) (B(0,address_jump_to) + 3)
#define MAX_B 0xffffff
//typedef ui64 (*fnptr)(...);
//template <typename RET>
//typedef RET (*fnptr)(...);
#define PLAYER_VALID(i) (VALID_ADDR(getCNetGamePlayer(i)))
#define IS_LOADED_INTO_LOBBY ((bool)(*(int*)(*(int*)0x1CF72C4+0xA098) == 8))    //from NETWORK_IS_SESSION_ACTIVE function   and more like... IS_STARTING_TRANSITION_TO_LOBBY
#define MY_PLAYER_PED_ADDRESS (*(volatile unsigned int*)( *(volatile unsigned int*)0x1CEFCF8 + 0x4) + 0)   //from the PLAYER_PED_ID function
#define I_AM_ONLINE (*(char*)0x223F0C0)
#define BLACK_SCREEN_DIALOG_OPEN (*(char*)0x1D63D1F > 0)
char global_temporary_buf[200];


#define PAD(x,s) char unk##x[s]


class Hooker { 
public:
	char orig[4 * 4];
	char patch[4 * 4];
	int address;
	Hooker() {}
	void init(int addressOfOriginalFunction, int opdOfDetour) {
		address = addressOfOriginalFunction;
		memcpy((void*)orig, (void*)address, 4 * 4);
		PatchInJump(address, opdOfDetour, false);
		memcpy((void*)patch, (void*)address, 4 * 4);
	}
	void unhook() {
		memcpy((void*)address, (void*)orig, 4 * 4);
		//sync_mem(address);//optional
	}
	void rehook() {
		memcpy((void*)address, (void*)patch, 4 * 4);
		//sync_mem(address);//optional
	}
	template <typename T>
	T(*call())(...) {
		volatile opd_s opd = { address,TOC };
		T(*func)(...) = (T(*)(...)) & opd;
		return func;
	}
};

//https://gta.nick7.com/stuff/gta5/ps3-classes.txt
/*
Entity layout:
017237C8:class CPhysical: public CDynamicEntity  I CALL THIS CObj
01788D48:  class CVehicle: public CPhysical
01788D58:    class CAutomobile: public CVehicle
01788D68:      class CQuadBike: public CAutomobile
01788DC0:      class CRotaryWingAircraft: public CAutomobile
01788DD0:        class CHeli: public CRotaryWingAircraft
01788DF0:          class CBlimp: public CHeli
01788DE0:        class CAutogyro: public CRotaryWingAircraft
01788E80:      class CTrailer: public CAutomobile
01788E60:      class CPlane: public CAutomobile
01788D90:    class CBike: public CVehicle
01788EB0:      class CBmx: public CBike
01788EC0:    class CBoat: public CVehicle
01788F08:    class CSubmarine: public CVehicle
01788E28:    class CTrain: public CVehicle
0178FB60:  class CObject: public CPhysical
0178ADB0:    class CProjectile: public CObject
0178AE00:      class CProjectileRocket: public CProjectile
0178AEA8:      class CProjectileThrown: public CProjectile
0178FB90:    class CDoor: public CObject
01791928:    class CPickup: public CObject
0178B678:  class CPed: public CPhysical
*/
char* getClassName(unsigned int classAddress);
struct ClassData;
struct CNonPhysicalPlayerData;
struct CPlayerInfo;
struct CNetGamePlayer;
struct GtaThread;
struct CGameScriptHandlerNetwork;
struct CGameScriptHandlerNetComponent;
struct CGameScriptId;
struct CObj;
struct CNetObj;
struct CFingerOfGodPresenceEvent;
class Global;
class Struct;
typedef CObj CPed;
typedef struct Vector3
{
	float x;
	float y;
	float z;
	Vector3 add(float x, float y, float z) {
		Vector3 n = { x + this->x,y + this->y,z + this->z };
		return n;
	}
};
typedef struct
{
	float x;
	float y;
} Vector2;

struct ClassData {//this would be the 0x19...
	unsigned int base;

	unsigned int getClassDataAddress() {
		return (unsigned int)&base;
	}
	char* className() {
		return getClassName(getClassDataAddress());
	}
	//alternate us using the typedef fnptr and doing fnptr call(unsigned int offset)...
	template <typename T>
	T(*call(unsigned int offset))(...) {//currently untested
		unsigned int __opd_s = *(volatile unsigned int*)(getClassDataAddress() + offset);
		T(*func)(...) = (T(*)(...)) & __opd_s;
		return func;
	}
	/*template <typename RET>
	fnptr<RET> call(unsigned int offset) {
		unsigned int opd_s = *(volatile unsigned int*)(getClassDataAddress()+offset);
		fnptr<RET> func = fnptr&opd_s;
		return func;
	}*/
};
struct CNonPhysicalPlayerData {
	ClassData* classData;//0x0
	unsigned int unk1;//0x4
	unsigned int playerIndex;//0x8
};
struct CPlayerInfo {//this is at offset 0xBD0 from entity class, such as CPed
	ClassData* classData;//0x0
	unsigned char unk1[0x4 * 3];//0x4
	char name[0x4 * 5];//0x10
	char region[0x4 * 2];//0x24
	PAD(2, 0x40 - 0x2C);//0x2C
	ui64 uuid;//0x40
	PAD(3, 0x160 - 0x48);//0x48
	CObj* cPed;//0x160  this is the CPed of the player
	//spoofed name is at 0x84 (0x10 + 0x74)
};
struct StatEntry {
	char *name;
	//char *value;
};
struct Stat {//size of 0x14
	char *xmlName;
	unsigned int arrayOfStatEntries;

};
struct Metric {
	virtual void unkfunc1();
	virtual void unkfunc2();
	virtual void return4();
	virtual void return0();
	virtual char* getMetricName();
	virtual bool storeJsonData(unsigned int unk);
	virtual int getMetricID();
};
#pragma pack(1)
struct CNetGamePlayer {
	ClassData* classData;//0x0
	unsigned int unk1;//0x4
	CNonPhysicalPlayerData* nonPhysicalPlayerData;//0x8
	unsigned char unk2[0xC];//0xC
	unsigned int unk3;//0x18
	unsigned char unk4[0xD];//0x1C
	unsigned char playerIndex;//0x29   0 through 17 ya know
	unsigned char unk41[0x38 - 0x2A];//0x2A
	unsigned int team;//0x38
	unsigned int unk5;//0x3C
	CNetGamePlayer* myCNetgamePlayer;//0x40
	unsigned char unk6[0x4 * 3];//0x44
	CNetGamePlayer* myCNetgamePlayer1;//0x50
	unsigned char unk7[0x4 * 3];//0x54
	CNetGamePlayer* myCNetgamePlayer2;//0x60
	unsigned char unk8[0x4 * 5];//0x64
	CPlayerInfo* playerInfo;//0x78
	unsigned char unk9[0xF4];//0x7C
	unsigned char unk10[0x1];//0x170
	unsigned char rockstarDev;//0x171
	unsigned char rockstarQA;//0x172
	unsigned char isCheater;//0x173
	unsigned char unk12[0xFFFFFF];//0x174 just here to note that it's not at 0x174

	//offset 0xBE is crew name
	//offset 0xD7 is crew tag
	void* getWeirdNetInfo() {
		return classData->call<void*>(0x1C)(this);//this returns CPlayerInfo + 0x10
	}
};
#pragma pack(4)
struct GtaThread {
	ClassData* classData;//0x0
	unsigned int threadID;//0x4
	unsigned int nameHash;//0x8
	//unsigned char unk1[0x5C];//0xC
	PAD(0, 0x54 - 0xC);//0xC
	void* localDataPointer;//0x54
	PAD(1, 0x68 - 0x58);//0x58
	char threadName[0x4 * 6];//0x68
	//30CGameScriptHandlerNetComponent is 0xAC
	/*
	virtual void unkfunc1();//0x0
	virtual void unkfunc2();//0x4
	virtual void unkfunc3();//0x8
	virtual void unkfunc4();//0xC
	virtual void unkfunc5();//0x10
	virtual void terminate_thread();//0x14
	*/

	void setLocal(int local, int value) {
		if (VALID_ADDR(localDataPointer))
			*(int*)((int)localDataPointer + (local * 4)) = value;
	}
	void terminate() {
		//call<void>(0x46D6F8)(&classData);
		classData->call<int>(0x14)(this);
		//terminate_thread();//I wonder if this will work at all hmm
	}

};
struct CGameScriptId {
	ClassData* classData;//0x0
	unsigned int unk1;
	char scriptName[0x4 * 0x8];//max size, but might be less
};
struct CGameScriptHandlerNetwork {
	ClassData* classData;//0x0
	unsigned char unk1[0x8];//0x4
	GtaThread* gtaThread;//0xC
	unsigned char unk2[0x14];//0x10
	CGameScriptHandlerNetComponent* cGameScriptHandlerNetComponentSyncOne;//0x24   this one is used in CGameScriptHandlerNetComponent__setMemorySyncPlayers and CGameScriptHandlerNetComponent__setMemorySyncHost as r3
	unsigned char unk3[0x30];//0x28
	CGameScriptHandlerNetComponent* cGameScriptHandlerNetComponent;//0x58
	unsigned char unk4[0x10];//0x5C
	CGameScriptId cGameScriptId;//0x6C         //0x38 - //this may be bigger in size, unsure 
};
#pragma pack(1)
struct CGameScriptHandlerNetComponent {//this is used for setting memory in script to be shared
	ClassData* classData;//0x0
	CGameScriptHandlerNetwork* cGameScriptHandlerNetwork;//0x4
	unsigned int unk1;//0x8  -- used in some places
	//0x34 is a cnetgameplayer
	int getHost() {
		unsigned int unkStruct = *(unsigned int*)((unsigned int)this + 0x1C);
		if (unkStruct == 0)
			return -1;
		CNetGamePlayer* p = (CNetGamePlayer*) *(int*)(unkStruct + 0x8);
		return p->playerIndex;
	}
};

struct CGamePlayerBroadcastDataHandler_Remote {//size of 0xF8
	//There is a pool of these at 0x1CF7318, max size of 000001F4, entry size of 000000F8
	ClassData* classData;//0x0
	CGamePlayerBroadcastDataHandler_Remote* next;//0x4
	CGamePlayerBroadcastDataHandler_Remote* previous;//0x8
	PAD(1, 0x5E - 0xC);//0xC
	short maxSize;//0x5E  this is the max size it can be that iis shared
	PAD(15, 0x70 - 0x60);
	int* pointerToDataSyncLocation;//0x70
	PAD(2, 0xA0 - 0x74);//0x74
	unsigned int sizeOfSyncData;//0xA0   modifyy this on host broadcast to be smaller than it should be and it can cause a crash
	PAD(3, 0xB0 - 0xA4);//0xA4
	unsigned int affectedPlayerId;//0xB0
	unsigned int unkValue131234;//0xB4   this looks like a boolean
	unsigned int currentNumberWeird;//0xB8    this value is shared. Like if you use the player sync this value will be used 18 times
	CGameScriptId cGameScriptId;//0xBC
	PAD(4, 0x130 - (0xBC + sizeof(CGameScriptId/*should be 0x28*/)));//sizeof(CGameScriptId/*should be 0x28*/)
	char scriptName[7 * 4];//0x130 idk the size of it, I just know it's at 0x130
	//unsure after this because idk the size of CGameScriptId
	unsigned int getAddressOfCopiedOverData() {
		return *(unsigned int*)((unsigned int)this + 0xD0);
	}
};
struct CGamePlayerBroadcastDataHandler_Local : CGamePlayerBroadcastDataHandler_Remote {};//size of 0x164 according to address 0x130C858
struct CGameHostBroadcastDataHandler : CGamePlayerBroadcastDataHandler_Remote {};//basically the same as this, but the important ones are pointerToDataSyncLocation and 
#pragma pack(4)
struct eventArgumentData {
	unsigned char data[0xF];//this is an array, not of size 0xFFFFFFFF but doesn't matter
	unsigned int* getArr(unsigned int offset = 0) {
		return (unsigned int*)(&(data[offset]));
	}
	int getInt(int index) {
		return *(volatile int*)&(getArr()[index]);
	}
	unsigned int getUInt(int index) {
		return getArr()[index];
	}
	int* getpInt(int index) {
		return (int*)&(getArr()[index]);
	}
	unsigned int* getpUInt(int index) {
		return &(getArr()[index]);
	}
};
struct eventArgumentInfo {//this is r3
	eventArgumentData* data;
	//more data within the r3
};
struct phInstGta {
	ClassData* classData;//0x0
};
struct CObjModelInfo {
	ClassData* classData;//0x0
	PAD(1, 0xC - 0x4);//0x4
	unsigned int modelHash;//0xC
	PAD(2, 0x20 - 0x10);//0x10
	Vector3 modelDimensionMin;//0x20
	PAD(3, 0x30 - 0x2C);//0x2C
	Vector3 modelDimensionMax;//0x30
	PAD(4, 0x160 - 0x3C);//0x3C
	//0x6B is like 5 if model is a vehicle... idk lmao
	char modelName[0x4];//0x160 is where it starts but idk where it ends
	PAD(5, 0x1B8 - 0x164);//0x164
	unsigned int modelType;//0x1B8
};
struct CPedDrawHandler {
	ClassData* classData;//0x0
};
struct CNetBlenderPed {
	ClassData* classData;//0x0
};
struct CPedInventory {
	ClassData* classData;//0x0
};
struct CWeaponInfo {
	ClassData* classData;//0x0
};
struct CPedWeaponManager {
	ClassData* classData;//0x0
	PAD(0, 0x4);//0x4
	CPed* unkCPed;//0x8
	unsigned int unkValue;//0xC
	CWeaponInfo* cWeaponInfo;//0x10
};
struct CNetObj_CSyncData {//N10CNetObjPed12CPedSyncDataE
	ClassData* classData;//0x0

};
#pragma pack(1)
struct CNetObj {
	ClassData* classData;//0x0
	unsigned short unk;//0x4
	unsigned short netID;//0x6
	//0x8 is CNetworkSyncData, but not a pointer to it. like *(int*)(this+0x8) = the classData for CNetworkSyncData aka 0x018F84B8
	//you can use A076C0 getCNetGamePlayerFromCNetObjPlayer to get the cnetgamme player for a cnetobj
	//unsigned char unk1[0x4*7];//0x8
	ClassData* N4rage18CNetworkSyncDataULILj18EEE;//0x8
	CNetObj_CSyncData* syncData;//0xC
	PAD(0, 0x24 - 0x10);//0x10
	unsigned int ownerData;//0x24
	unsigned int unk2;//0x28
	CObj* cObj;//0x2C - CPed...
	CNetBlenderPed* cNetBlenderPed;//0x30
	PAD(3, 0x374 - 0x34);//0x34
	short netID2;//0x374 used in other places, different than netID
	int getCurrentOwner() {//0x25
		int data = ownerData & 0x00FF0000;
		data = data >> 16;
		return data;
	}
	bool isCurrentOwner() {//0x27
		int data = ownerData & 0x000000FF;
		return data == 0;//data should be 0 or 1, 0 if I am owner.... unless I am mistaken and this is the same as 0x25
	}
	//this is important too, just not got stuff in it yet
};
#pragma pack(4)
#pragma pack(1)
struct CObj {//getEntityFromIndex, entityToIndex, CPed...

	//byte at 0x6DC contains the bit for visibility at insrwi    r3, r31, 1,29

	ClassData* classData;//0x0
	unsigned char unk[0xC];//0x4
	CObjModelInfo* modelInfo;//0x10
	unsigned int unk_bitset;//0x14    the first byte of this has values, and 3 is checks in delete entity
	unsigned char unk1[0x4];//0x18
	phInstGta* unk_phInstGta;//0x1C
	unsigned char unk2[0x8];//0x20
	CPedDrawHandler* cPedDrawHandler;//0x28
	unsigned char unk3[0x20];//0x2C  //used to be 0x10 of size but realized I fucked up
	CNetObj* cNetObj;//0x4C  Player associated with it
	unsigned char unk4[0x10];//0x50
	ClassData* classDataForStartOfOtherClass;//0x60
	unsigned char unk5[0x50];//0x64
	unsigned int proofsBitset;//0xB4  //0xA4 is what it shows though
	unsigned char proofsBitsetByte;//0xB8
	unsigned char unk6[3];//filling up the rest of that
	unsigned char unk7[0xB4];//0xBC
	float health;//0x170
	float maxHealth;//0x174
	unsigned char unk8[0xA58];//0x178
	CPlayerInfo* cPlayerInfo;//0xBD0  This CObj can be refernced back through this struct
	CPedInventory* cPedInventory;//0xBD4
	CPedWeaponManager* cPedWeaponManager;//0xBD8

	//0xE9C is the vehicle they are in when a ped

};
#pragma pack(4)

struct CSectorPositionDataNode {//This one ended up not bbeing used 
	ClassData* classData;//0x0
	PAD(0, 0x6C - 0x4);//0x4
	float x;//0x6C
	float y;//0x70
	float z;//0x74
	ClassData* CProjectBaseSyncParentNode;
};

struct CPlayerSectorPosNode {
	ClassData* classData;//0x0
	PAD(0, 0x6C - 0x4);//0x4
	float x;//0x6C
	float y;//0x70
	float z;//0x74
};

#pragma pack(1)
struct CSectorDataNode {
	ClassData* classData;//0x0
	CPlayerSectorPosNode* posNode;//0x4
	PAD(0, 0x6C - 0x8);//0x8
	//During testing with breakpoints, this data seemed to be stable for player ped cnet. I could even see it increment slowly as I moved
	short x;//0x6C
	short y;//0x6E
	short z;//0x70
	short unk;//0x72
	ClassData* CSectorPositionDataNode;//0x74
};
#pragma pack(4)


//size if 0x50 -- confirmed because there is a static one
struct CFingerOfGodPresenceEvent {
	ClassData* classData;//0x0
	unsigned int hashValue;//0x4
	int unk1;//0x8
	char vehicleName[0x20];//0xC -- 0x20 is a guess
	char unk2[0x24];//0x2C
	void receiveFunc2(int value) {
		//classData->call<int>(0x14)(this,value);
		//0x14
	}
	void receiveFunc() {
		//0x18
	}
	void sendFunc() {
		//0x10 - it returns 0
	}
};


struct CBountyPresenceEvent {
	ClassData* classData;//0x0
	char playerName[0x20];//0x4
	char unk[0x50];//0x24  I don't actually know the size
	void startFunc(int value) {
		classData->call<int>(0x14)(this, value);
	}
	void receiveFunc() {
		//0x18
	}
	void sendFunc() {
		//bountyEventFunction_send
		//0x10 - not in this!!!! it returns 0
	}
};

union IntFloat {
	int i;
	unsigned int ui;
	float f;
};
struct CStatUpdatePresenceEvent {//max size of 0x98, probably 0x94
	ClassData* classData;//0x0
	unsigned int statHash;//0x4
	int statType;//0x8 - 1 for float, else for int (and others?)
	IntFloat value;//0xC
	unsigned int v2;//0x10 - unknown value necessary
	char from[0x40];//0x14
	char sv[0x40];//0x54
	unsigned int unk;//0x94 - I don't think this is actually ever used. probably size 0x94
};

struct N4rage10scrProgramE {
	ClassData* classData;//0x0
	unsigned int unk1;//0x4
	unsigned int goodPointer1;//0x8
	PAD(0, 0x38 - 0xC);//0xC
	unsigned int nameHash;//0x38
	unsigned int status;//0x3C  - 1 is not running, 2 is running, 0 maybe not loaded
};

struct JsonArg {
	char* jsonString;//0x0
	int jsonStringBufferSize;//0x4  max length  0x100
	int unk1;//0x8 -1
	int unk2;//0xC -1
	int unk3;//0x10 0
	int jsonStringLength;//0x14 strlen
};

struct EventDataStruct {//size = 0x1C
	int unk1;
	int unk2;
	int unk3;
	int unk4;
	int unk5;
	int unk6;
	int unk7;
	char unk8[200];
};

int Read_Global_Address(int a_uiGlobalID)
{
	int Ptr = *(volatile int*)((GLOBALS_PTR) + (((a_uiGlobalID / 0x40000) & 0x3F) * 4)         );
	if (Ptr != 0)
		return (Ptr + ((a_uiGlobalID % 0x40000) * 4));
	return 0;
}

class Global {
private:
	unsigned int index;
	unsigned int getAddress() {
		return Read_Global_Address(index);
	}
public:
	Global(unsigned int globalID)
		: index(globalID/*Read_Global_Address(globalID)*/)
	{}
	Global imm(int i) {
		return Global(index + i);
	}
	Global arr(int index, int size) {
		return imm(index * size);
	}
	template <typename T>
	void set(T value) {
		*(T*)getAddress() = value;
	}
	template <typename T>
	T get() {
		return *(T*)getAddress();
	}
	unsigned int* at() {
		return (unsigned int*)getAddress();
	}
};

class Struct {
public:
	unsigned int address;
	Struct(unsigned int* data)
		: address((unsigned int)data)
	{}
	Struct imm(int i) {
		return Struct((unsigned int*)(((unsigned int)address) + (i * 4)));
	}
	Struct arr(int index, int size) {
		return imm(index * size);
	}
	template <typename T>
	void set(T value) {
		*(T*)address = value;
	}
	template <typename T>
	T get() {
		return *(T*)address;
	}
	unsigned int* at() {
		return (unsigned int*)address;
	}
};

class StackFrame {
private:
	void* stack;
	unsigned int size;
public:
	StackFrame(void* _stack, unsigned int _size) :
		stack(_stack), size(_size)
	{

	}
	void* get(unsigned int offset) {
		return (void*)((unsigned int)stack + (size - offset));
	}
	template <typename T>
	void setArg(unsigned int offset, T value) {
		*(T*)get(offset) = value;
	}
	template <typename T>
	T getArg(unsigned int offset) {
		return *(T*)get(offset);
	}
};

//signature checks and scrProgram struct by roulette
bool DataCompare(const uint8_t* pbData, const uint8_t* pbMask, const char* szMask)
{
	for (; *szMask; ++szMask, ++pbData, ++pbMask)
	{
		if (*szMask == 'x' && *pbData != *pbMask)
		{
			return false;
		}
	}
	return (*szMask == NULL);
}

uintptr_t FindPattern(uintptr_t startAddress, uintptr_t endAddress, uint8_t* pbPattern, const char* szMask)
{
	for (uintptr_t i = 0; i < endAddress; i++)
		if (DataCompare((uint8_t*)(startAddress + i), pbPattern, szMask))
			return (uintptr_t)(startAddress + i);

	return NULL;
}

uintptr_t FindPattern(uintptr_t startAddress, uintptr_t endAddress, uint8_t* pbPattern, const char* szMask, uintptr_t* occurrences, uint32_t maxOccurances)
{
	uint32_t numFound = 0;
	for (uintptr_t i = 0; i < endAddress; i++)
		if (DataCompare((uint8_t*)(startAddress + i), pbPattern, szMask) && numFound < maxOccurances)
			return occurrences[numFound++] = (uintptr_t)(startAddress + i);

	return NULL;
}

struct scrProgram
{
	void* m_vmt;
	uintptr_t** m_unknown;           // 0x0004
	uint8_t** m_code_blocks;         // 0x0008
	uint32_t m_hash;                 // 0x0000
	uint32_t m_code_size;            // 0x0000
	uint32_t m_arg_count;            // 0x0000
	uint32_t m_local_count;          // 0x0000
	uint32_t m_global_count;         // 0x0000
	uint32_t m_native_count;         // 0x0000
	int32_t* m_local_data;           // 0x0024
	int64_t** m_global_data;         // 0x0000
	void** m_native_entrypoints;     // 0x0000
	char m_padding6[0x08];           // 0x0000
	uint32_t m_name_hash;            // 0x0000
	char m_padding7[0x04];           // 0x0000
	const char* m_name;              // 0x0000
	const char** m_strings_data;     // 0x0000
	uint32_t m_strings_count;        // 0x0000
	char m_padding8[0x0C];           // 0x0000


	bool IsValid() const
	{
		return m_code_size > 0;
	}

	uint32_t GetNumCodePages() const
	{
		return (m_code_size + 0x3FFF) >> 14;
	}

	uint32_t GetCodePageSize(uint32_t page) const
	{
		return (page < 0 || page >= GetNumCodePages() ? 0 : (page == GetNumCodePages() - 1) ? m_code_size & 0x3FFF : 0x4000);
	}

	uint8_t* GetCodePage(uint32_t page) const
	{
		return m_code_blocks[page];
	}

	uint8_t* GetCodeAddress(uint32_t codePosition) const
	{
		return codePosition < 0 || codePosition >= m_code_size ? NULL : &m_code_blocks[codePosition >> 14][codePosition & 0x3FFF];
	}

	const char* GetString(uint32_t stringPosition) const
	{
		return stringPosition < 0 || stringPosition >= m_strings_count ? NULL : &m_strings_data[stringPosition >> 14][stringPosition & 0x3FFF];
	}

	int32_t GetLocal(uint32_t localPosition) const
	{
		return m_local_data[localPosition];
	}

	void SetLocal(uint32_t localPosition, int32_t value)
	{
		m_local_data[localPosition] = value;
	}
};

struct scrProgramTableEntry
{
	scrProgram* m_program;
	char padding[4];
	uint32_t hash;

	bool IsLoaded() const
	{
		return m_program != NULL;
	}
};

struct scrProgramTable
{
	scrProgramTableEntry* m_data;
	uintptr_t* m_bitmap;
	uint32_t m_size;
	scrProgramTableEntry* FindScript(uint32_t hash)
	{
		if (m_data == NULL)
			return NULL; // table initialization hasn't happened yet

		for (uint32_t i = 0; i < m_size; i++)
		{
			if (m_data[i].hash == hash)
			{
				return &m_data[i];
			}
		}
		return NULL;
	}
};

scrProgramTable* scriptTable = (scrProgramTable*)0x1DE8EC0;


//some global function definitions based on these...
CNetGamePlayer* getCNetGamePlayer(int player);//0x47E00C
CObj* getCObjFromIndex(int entity);//getEntityFromIndex 0xA552E0
int entityToIndex(CObj* entityAddress);//0x474054 aka 0xA551A4
CObjModelInfo* getCModelInfo(unsigned int hash);//0x9D9830 actual function r4 is unused int pointer (stores return index or some shit)
GtaThread* getCurrentGtaThread() { return call<GtaThread*>(0x9B82DC)();/*return ((GtaThread*(*)())0x9B82DC)();*/ }
GtaThread* getGtaThreadById(int id) { return call<GtaThread*>(0x9B80DC)(id);/*return ((GtaThread*(*)(int))0x9B80DC)(id);*/ };
CFingerOfGodPresenceEvent* globalCFingerOfGodPresenceEvent = (CFingerOfGodPresenceEvent*)0x225A3E8;
unsigned int getG_CGameArrayMgr() { return call<unsigned int>(0x9F8E20)();/*return ((unsigned int(*)())0x9F8E20)();*/ }
CGameScriptHandlerNetwork* getCurrentCGameScriptHandlerNetwork() { return call<CGameScriptHandlerNetwork*>(0x47DEC4)();/*return ((CGameScriptHandlerNetwork *(*)())0x47DEC4)();*/ };
unsigned int joaat(char* text, int r4 = 0) { return call<unsigned int>(0xAE580C)(text, r4); };
unsigned int getMyPlayerNetworkData() { return call<unsigned int>(0x12EF35C)(); };
CObj* getMyCPed() { return call<CObj*>(0x1117978)(); };
CObj* getPlayerCPedByIndex(int index) { return call<CObj*>(0x47DFA0)(index); };
void calculatePlayerPosition(float realCoords[3], float* sectorX, float* sectorY, float* sectorZ, float segmentPos[3]) { call<int>(0x1344108)(realCoords, sectorX, sectorY, sectorZ, segmentPos); };
N4rage10scrProgramE* getN4rage10scrProgramE(unsigned int scriptHash) { return call<N4rage10scrProgramE*>(0x9B45E4)(scriptHash); }
scrProgram* getScrProgram(unsigned int scriptHash) { return call<scrProgram*>(0x9B45E4)(scriptHash); }
CPlayerInfo* getMyCPlayerInfo() { return call<CPlayerInfo*>(0x36CBE4)(); };
char* getMyPSNName() { return call<char*>(0x12ED734)(); };

//example would be 0x1CF7374 for pool_NetworkEntityAreas_32CNetworkEntityAreaWorldStateData
bool doesPoolHaveOpenSlot(unsigned int poolPointer) {
	unsigned int poolAddr = *(unsigned int*)poolPointer;
	int neg1check = *(int*)(poolAddr + 0x10);
	if (neg1check == -1) {
		if (*(int*)(poolAddr + 0x1C) == 0)
			return false;
		if (*(int*)(poolAddr + 0x24) == 0)
			return false;
		//probably an issue.... but we'll stick to this
	}
	return true;
}

//some enums
enum EVENTS {
	OBJECT_ID_FREED_EVENT,
	OBJECT_ID_REQUEST_EVENT,
	ARRAY_DATA_VERIFY_EVENT,
	SCRIPT_ARRAY_DATA_VERIFY_EVENT,
	REQUEST_CONTROL_EVENT,
	GIVE_CONTROL_EVENT,
	WEAPON_DAMAGE_EVENT,
	REQUEST_PICKUP_EVENT,
	REQUEST_MAP_PICKUP_EVENT,
	GAME_CLOCK_EVENT,
	GAME_WEATHER_EVENT,
	RESPAWN_PLAYER_PED_EVENT,
	GIVE_WEAPON_EVENT,
	REMOVE_WEAPON_EVENT,
	REMOVE_ALL_WEAPONS_EVENT,
	VEHICLE_COMPONENT_CONTROL_EVENT,
	FIRE_EVENT,
	EXPLOSION_EVENT,
	START_PROJECTILE_EVENT,
	ALTER_WANTED_LEVEL_EVENT,
	CHANGE_RADIO_STATION_EVENT,
	RAGDOLL_REQUEST_EVENT,
	PLAYER_TAUNT_EVENT,
	PLAYER_CARD_STAT_EVENT,
	DOOR_BREAK_EVENT,
	SCRIPTED_GAME_EVENT,
	REMOTE_SCRIPT_INFO_EVENT,
	REMOTE_SCRIPT_LEAVE_EVENT,
	MARK_AS_NO_LONGER_NEEDED_EVENT,
	CONVERT_TO_SCRIPT_ENTITY_EVENT,
	SCRIPT_WORLD_STATE_EVENT,
	INCIDENT_ENTITY_EVENT,
	CLEAR_AREA_EVENT,
	NETWORK_REQUEST_SYNCED_SCENE_EVENT,
	NETWORK_START_SYNCED_SCENE_EVENT,
	NETWORK_UPDATE_SYNCED_SCENE_EVENT,
	NETWORK_STOP_SYNCED_SCENE_EVENT,
	GIVE_PED_SCRIPTED_TASK_EVENT,
	GIVE_PED_SEQUENCE_TASK_EVENT,
	NETWORK_CLEAR_PED_TASKS_EVENT,
	NETWORK_START_PED_ARREST_EVENT,
	NETWORK_START_PED_UNCUFF_EVENT,
	NETWORK_SOUND_CAR_HORN_EVENT,
	NETWORK_ENTITY_AREA_STATUS_EVENT,
	NETWORK_GARAGE_OCCUPIED_STATUS_EVENT,
	PED_CONVERSATION_LINE_EVENT,
	SCRIPT_ENTITY_STATE_CHANGE_EVENT,
	NETWORK_PLAY_SOUND_EVENT,
	NETWORK_STOP_SOUND_EVENT,
	NETWORK_BANK_REQUEST_EVENT,
	REQUEST_DOOR_EVENT,
	NETWORK_TRAIN_REQUEST_EVENT,
	NETWORK_TRAIN_REPORT_EVENT,
	NETWORK_INCREMENT_STAT_EVENT,
	MODIFY_VEHICLE_LOCK_WORD_STATE_DATA,
	REQUEST_PHONE_EXPLOSION_EVENT,
	REQUEST_DETACHMENT_EVENT,
	KICK_VOTES_EVENT,
	GIVE_PICKUP_REWARDS_EVENT,
	NETWORK_CRC_HASH_CHECK_EVENT,
	BLOW_UP_VEHICLE_EVENT,
	NETWORK_SPECIAL_FIRE_EQUIPPED_WEAPON,
	NETWORK_RESPONDED_TO_THREAT_EVENT,
	NETWORK_SHOUT_TARGET_POSITION,
	VOICE_DRIVEN_MOUTH_MOVEMENT_FINISHED_EVENT,
	PICKUP_DESTROYED_EVENT,
	UPDATE_PLAYER_SCARS_EVENT,
	NETWORK_CHECK_EXE_SIZE_EVENT,
	NETWORK_PTFX_EVENT,
	NETWORK_PED_SEEN_DEAD_PED_EVENT,
	REMOVE_STICKY_BOMB_EVENT,
	NETWORK_CHECK_CODE_CRCS_EVENT,
	INFORM_SILENCED_GUNSHOT_EVENT,
	PED_PLAY_PAIN_EVENT,
	CACHE_PLAYER_HEAD_BLEND_DATA_EVENT,
	REMOVE_PED_FROM_PEDGROUP_EVENT,
	NO_EVENT,
	REPORT_CASH_SPAWN_EVENT,
	EVENT_LIST_SIZE//always keep this one last
};
char* EVENT_NAMES[] = { "OBJECT_ID_FREED_EVENT","OBJECT_ID_REQUEST_EVENT","ARRAY_DATA_VERIFY_EVENT","SCRIPT_ARRAY_DATA_VERIFY_EVENT","REQUEST_CONTROL_EVENT","GIVE_CONTROL_EVENT","WEAPON_DAMAGE_EVENT","REQUEST_PICKUP_EVENT","REQUEST_MAP_PICKUP_EVENT","GAME_CLOCK_EVENT","GAME_WEATHER_EVENT","RESPAWN_PLAYER_PED_EVENT","GIVE_WEAPON_EVENT","REMOVE_WEAPON_EVENT","REMOVE_ALL_WEAPONS_EVENT","VEHICLE_COMPONENT_CONTROL_EVENT","FIRE_EVENT","EXPLOSION_EVENT","START_PROJECTILE_EVENT","ALTER_WANTED_LEVEL_EVENT","CHANGE_RADIO_STATION_EVENT","RAGDOLL_REQUEST_EVENT","PLAYER_TAUNT_EVENT","PLAYER_CARD_STAT_EVENT","DOOR_BREAK_EVENT","SCRIPTED_GAME_EVENT","REMOTE_SCRIPT_INFO_EVENT","REMOTE_SCRIPT_LEAVE_EVENT","MARK_AS_NO_LONGER_NEEDED_EVENT","CONVERT_TO_SCRIPT_ENTITY_EVENT","SCRIPT_WORLD_STATE_EVENT","INCIDENT_ENTITY_EVENT","CLEAR_AREA_EVENT","NETWORK_REQUEST_SYNCED_SCENE_EVENT","NETWORK_START_SYNCED_SCENE_EVENT","NETWORK_UPDATE_SYNCED_SCENE_EVENT","NETWORK_STOP_SYNCED_SCENE_EVENT","GIVE_PED_SCRIPTED_TASK_EVENT","GIVE_PED_SEQUENCE_TASK_EVENT","NETWORK_CLEAR_PED_TASKS_EVENT","NETWORK_START_PED_ARREST_EVENT","NETWORK_START_PED_UNCUFF_EVENT","NETWORK_SOUND_CAR_HORN_EVENT","NETWORK_ENTITY_AREA_STATUS_EVENT","NETWORK_GARAGE_OCCUPIED_STATUS_EVENT","PED_CONVERSATION_LINE_EVENT","SCRIPT_ENTITY_STATE_CHANGE_EVENT","NETWORK_PLAY_SOUND_EVENT","NETWORK_STOP_SOUND_EVENT","NETWORK_BANK_REQUEST_EVENT","REQUEST_DOOR_EVENT","NETWORK_TRAIN_REQUEST_EVENT","NETWORK_TRAIN_REPORT_EVENT","NETWORK_INCREMENT_STAT_EVENT","MODIFY_VEHICLE_LOCK_WORD_STATE_DATA","REQUEST_PHONE_EXPLOSION_EVENT","REQUEST_DETACHMENT_EVENT","KICK_VOTES_EVENT","GIVE_PICKUP_REWARDS_EVENT","NETWORK_CRC_HASH_CHECK_EVENT","BLOW_UP_VEHICLE_EVENT","NETWORK_SPECIAL_FIRE_EQUIPPED_WEAPON","NETWORK_RESPONDED_TO_THREAT_EVENT","NETWORK_SHOUT_TARGET_POSITION","VOICE_DRIVEN_MOUTH_MOVEMENT_FINISHED_EVENT","PICKUP_DESTROYED_EVENT","UPDATE_PLAYER_SCARS_EVENT","NETWORK_CHECK_EXE_SIZE_EVENT","NETWORK_PTFX_EVENT","NETWORK_PED_SEEN_DEAD_PED_EVENT","REMOVE_STICKY_BOMB_EVENT","NETWORK_CHECK_CODE_CRCS_EVENT","INFORM_SILENCED_GUNSHOT_EVENT","PED_PLAY_PAIN_EVENT","CACHE_PLAYER_HEAD_BLEND_DATA_EVENT","REMOVE_PED_FROM_PEDGROUP_EVENT","NO_EVENT","REPORT_CASH_SPAWN_EVENT" };
namespace TYPES {
	enum vehicleType {
		AUTOMOBILE,
		BIKE,
		BOAT,
		DOOR,
		HELI,
		OBJECT,
		PED,
		PICKUP,
		PICKUP_PLACEMENT,
		PLANE,
		SUBMARINE,
		PLAYER,
		TRAILER,
		TRAIN,
		GLASS_PANE
	};
};

//So these RPFTYPES are like weird because I don't quite understand why they are different than TYPES but they are, so not all are correct
namespace RPFTYPES {
	enum vehicleType {
		AUTOMOBILE,
		PLANE,//correct
		BOAT,
		DOOR,
		PED,
		OBJECT,
		HELI,//correct
		PICKUP,
		PICKUP_PLACEMENT,
		BIKE,
		SUBMARINE,
		PLAYER,
		TRAILER,
		TRAIN,
		GLASS_PANE
	};
};


/*
I wrote this so keeping it here anyways... would have to reformat other code though and ain't bothering
struct PlayerDataSend;
class SCEvent {
private:
	char *jsonData;                       //ptr to json data
	const int amtPlayers = 0x20;          //amount in player array
	PlayerDataSend players[amtPlayers];   //player array
	int spot;                             //current player count
public:
	SCEvent(char *_jsonData);             //create it
	void setJsonData(char *_jsonData);    //set the data
	void resetPlayers();                  //reset player list
	void addPlayer(char *name);           //add player by name
	unsigned int getPlayersAddress();     //private, used in send func
	void send();                          //send type 1 (to all the players said via addPlayer)
	void sendType2();                     //send type 2 (to all the players said via addPlayer)
	void pushEventOnPlayerInLobby(int player);      //set to send to player by player id
	void pushEventOnPlayerName(char *playerName);   //set to send to player by player name
	void pushEventOnLobby();              //set to send to everyone in lobby
};
*/







//some quick mappings (xbox womp womp)
/*enum Buttons
{
	Button_A = 0xC1,
	Button_B = 0xC3,
	Button_X = 0xC2,
	Button_Y = 0xC0,
	Button_Back = 0xBF,
	Button_L1 = 0xC4,
	Button_L2 = 0xC6,
	Button_L3 = 0xC8,
	Button_R1 = 0xC5,
	Button_R2 = 0xC7,
	Button_R3 = 0xC9,
	Dpad_Up = 0xCA, //202 actually dpad right
	Dpad_Down = 0xCB,
	Dpad_Left = 0xCC,
	Dpad_Right = 0xCD   //PS3 X
};*/

enum InputButtons
{
	BUTTON_X = 0x15, //PS3 1.12
	//BUTTON_X = 0xCD, //PS3 1.12 duplicate of 0x15
	BUTTON_R1 = 0xC2, //PS3 1.12
	BUTTON_TRIANGLE = 0xB1, //PS3 1.12
	BUTTON_Y = 0xC0,
	BUTTON_LB = 0x13,
	BUTTON_LT = 0x19,
	BUTTON_LS = 0x56,
	DPAD_RIGHT = 0x2C, //PS3 1.12
	BUTTON_RT = 0x47,
	BUTTON_RS = 0x1A,
	BUTTON_START = 0xAD, //PS3 1.12
	DPAD_LEFT = 0xAE,
	BUTTON_L2 = 0xC3, //PS3 1.12
	BUTTON_CIRCLE = 0xAF,  //PS3 1.12
	DPAD_DOWN = 20, //PS3 1.12
	DPAD_UP = 27 //PS3 1.12
};


enum eControl
{
	ControlNextCamera = 0,
	ControlLookLeftRight = 1,
	ControlLookUpDown = 2,
	ControlLookUpOnly = 3,
	ControlLookDownOnly = 4,
	ControlLookLeftOnly = 5,
	ControlLookRightOnly = 6,
	ControlCinematicSlowMo = 7,
	ControlFlyUpDown = 8,
	ControlFlyLeftRight = 9,
	ControlScriptedFlyZUp = 10,
	ControlScriptedFlyZDown = 11,
	ControlWeaponWheelUpDown = 12,
	ControlWeaponWheelLeftRight = 13,
	ControlWeaponWheelNext = 14,
	ControlWeaponWheelPrev = 15,
	ControlSelectNextWeapon = 16,
	ControlSelectPrevWeapon = 17,
	ControlSkipCutscene = 18,
	ControlCharacterWheel = 19,
	ControlMultiplayerInfo = 20,
	ControlSprint = 21,
	ControlJump = 22,
	ControlEnter = 23,
	ControlAttack = 24,
	ControlAim = 25,
	ControlLookBehind = 26,
	ControlPhone = 27,
	ControlSpecialAbility = 28,
	ControlSpecialAbilitySecondary = 29,
	ControlMoveLeftRight = 30,
	ControlMoveUpDown = 31,
	ControlMoveUpOnly = 32,
	ControlMoveDownOnly = 33,
	ControlMoveLeftOnly = 34,
	ControlMoveRightOnly = 35,
	ControlDuck = 36,
	ControlSelectWeapon = 37,
	ControlPickup = 38,
	ControlSniperZoom = 39,
	ControlSniperZoomInOnly = 40,
	ControlSniperZoomOutOnly = 41,
	ControlSniperZoomInSecondary = 42,
	ControlSniperZoomOutSecondary = 43,
	ControlCover = 44,
	ControlReload = 45,
	ControlTalk = 46,
	ControlDetonate = 47,
	ControlHUDSpecial = 48,
	ControlArrest = 49,
	ControlAccurateAim = 50,
	ControlContext = 51,
	ControlContextSecondary = 52,
	ControlWeaponSpecial = 53,
	ControlWeaponSpecial2 = 54,
	ControlDive = 55,
	ControlDropWeapon = 56,
	ControlDropAmmo = 57,
	ControlThrowGrenade = 58,
	ControlVehicleMoveLeftRight = 59,
	ControlVehicleMoveUpDown = 60,
	ControlVehicleMoveUpOnly = 61,
	ControlVehicleMoveDownOnly = 62,
	ControlVehicleMoveLeftOnly = 63,
	ControlVehicleMoveRightOnly = 64,
	ControlVehicleSpecial = 65,
	ControlVehicleGunLeftRight = 66,
	ControlVehicleGunUpDown = 67,
	ControlVehicleAim = 68,
	ControlVehicleAttack = 69,
	ControlVehicleAttack2 = 70,
	ControlVehicleAccelerate = 71,
	ControlVehicleBrake = 72,
	ControlVehicleDuck = 73,
	ControlVehicleHeadlight = 74,
	ControlVehicleExit = 75,
	ControlVehicleHandbrake = 76,
	ControlVehicleHotwireLeft = 77,
	ControlVehicleHotwireRight = 78,
	ControlVehicleLookBehind = 79,
	ControlVehicleCinCam = 80,
	ControlVehicleNextRadio = 81,
	ControlVehiclePrevRadio = 82,
	ControlVehicleNextRadioTrack = 83,
	ControlVehiclePrevRadioTrack = 84,
	ControlVehicleRadioWheel = 85,
	ControlVehicleHorn = 86,
	ControlVehicleFlyThrottleUp = 87,
	ControlVehicleFlyThrottleDown = 88,
	ControlVehicleFlyYawLeft = 89,
	ControlVehicleFlyYawRight = 90,
	ControlVehiclePassengerAim = 91,
	ControlVehiclePassengerAttack = 92,
	ControlVehicleSpecialAbilityFranklin = 93,
	ControlVehicleStuntUpDown = 94,
	ControlVehicleCinematicUpDown = 95,
	ControlVehicleCinematicUpOnly = 96,
	ControlVehicleCinematicDownOnly = 97,
	ControlVehicleCinematicLeftRight = 98,
	ControlVehicleSelectNextWeapon = 99,
	ControlVehicleSelectPrevWeapon = 100,
	ControlVehicleRoof = 101,
	ControlVehicleJump = 102,
	ControlVehicleGrapplingHook = 103,
	ControlVehicleShuffle = 104,
	ControlVehicleDropProjectile = 105,
	ControlVehicleMouseControlOverride = 106,
	ControlVehicleFlyRollLeftRight = 107,
	ControlVehicleFlyRollLeftOnly = 108,
	ControlVehicleFlyRollRightOnly = 109,
	ControlVehicleFlyPitchUpDown = 110,
	ControlVehicleFlyPitchUpOnly = 111,
	ControlVehicleFlyPitchDownOnly = 112,
	ControlVehicleFlyUnderCarriage = 113,
	ControlVehicleFlyAttack = 114,
	ControlVehicleFlySelectNextWeapon = 115,
	ControlVehicleFlySelectPrevWeapon = 116,
	ControlVehicleFlySelectTargetLeft = 117,
	ControlVehicleFlySelectTargetRight = 118,
	ControlVehicleFlyVerticalFlightMode = 119,
	ControlVehicleFlyDuck = 120,
	ControlVehicleFlyAttackCamera = 121,
	ControlVehicleFlyMouseControlOverride = 122,
	ControlVehicleSubTurnLeftRight = 123,
	ControlVehicleSubTurnLeftOnly = 124,
	ControlVehicleSubTurnRightOnly = 125,
	ControlVehicleSubPitchUpDown = 126,
	ControlVehicleSubPitchUpOnly = 127,
	ControlVehicleSubPitchDownOnly = 128,
	ControlVehicleSubThrottleUp = 129,
	ControlVehicleSubThrottleDown = 130,
	ControlVehicleSubAscend = 131,
	ControlVehicleSubDescend = 132,
	ControlVehicleSubTurnHardLeft = 133,
	ControlVehicleSubTurnHardRight = 134,
	ControlVehicleSubMouseControlOverride = 135,
	ControlVehiclePushbikePedal = 136,
	ControlVehiclePushbikeSprint = 137,
	ControlVehiclePushbikeFrontBrake = 138,
	ControlVehiclePushbikeRearBrake = 139,
	ControlMeleeAttackLight = 140,
	ControlMeleeAttackHeavy = 141,
	ControlMeleeAttackAlternate = 142,
	ControlMeleeBlock = 143,
	ControlParachuteDeploy = 144,
	ControlParachuteDetach = 145,
	ControlParachuteTurnLeftRight = 146,
	ControlParachuteTurnLeftOnly = 147,
	ControlParachuteTurnRightOnly = 148,
	ControlParachutePitchUpDown = 149,
	ControlParachutePitchUpOnly = 150,
	ControlParachutePitchDownOnly = 151,
	ControlParachuteBrakeLeft = 152,
	ControlParachuteBrakeRight = 153,
	ControlParachuteSmoke = 154,
	ControlParachutePrecisionLanding = 155,
	ControlMap = 156,
	ControlSelectWeaponUnarmed = 157,
	ControlSelectWeaponMelee = 158,
	ControlSelectWeaponHandgun = 159,
	ControlSelectWeaponShotgun = 160,
	ControlSelectWeaponSmg = 161,
	ControlSelectWeaponAutoRifle = 162,
	ControlSelectWeaponSniper = 163,
	ControlSelectWeaponHeavy = 164,
	ControlSelectWeaponSpecial = 165,
	ControlSelectCharacterMichael = 166,
	ControlSelectCharacterFranklin = 167,
	ControlSelectCharacterTrevor = 168,
	ControlSelectCharacterMultiplayer = 169,
	ControlSaveReplayClip = 170,
	ControlSpecialAbilityPC = 171,
	ControlPhoneUp = 172,
	ControlPhoneDown = 173,
	ControlPhoneLeft = 174,
	ControlPhoneRight = 175,
	ControlPhoneSelect = 176,
	ControlPhoneCancel = 177,
	ControlPhoneOption = 178,
	ControlPhoneExtraOption = 179,
	ControlPhoneScrollForward = 180,
	ControlPhoneScrollBackward = 181,
	ControlPhoneCameraFocusLock = 182,
	ControlPhoneCameraGrid = 183,
	ControlPhoneCameraSelfie = 184,
	ControlPhoneCameraDOF = 185,
	ControlPhoneCameraExpression = 186,
	ControlFrontendDown = 187,
	ControlFrontendUp = 188,
	ControlFrontendLeft = 189,
	ControlFrontendRight = 190,
	ControlFrontendRdown = 191,
	ControlFrontendRup = 192,
	ControlFrontendRleft = 193,
	ControlFrontendRright = 194,
	ControlFrontendAxisX = 195,
	ControlFrontendAxisY = 196,
	ControlFrontendRightAxisX = 197,
	ControlFrontendRightAxisY = 198,
	ControlFrontendPause = 199,
	ControlFrontendPauseAlternate = 200,
	ControlFrontendAccept = 201,
	ControlFrontendCancel = 202, //dpad right
	ControlFrontendX = 203,
	ControlFrontendY = 204,
	ControlFrontendLb = 205,
	ControlFrontendRb = 206,
	ControlFrontendLt = 207,
	ControlFrontendRt = 208,
	ControlFrontendLs = 209,
	ControlFrontendRs = 210,
	ControlFrontendLeaderboard = 211,
	ControlFrontendSocialClub = 212,
	ControlFrontendSocialClubSecondary = 213,
	ControlFrontendDelete = 214,
	ControlFrontendEndscreenAccept = 215,
	ControlFrontendEndscreenExpand = 216,
	ControlFrontendSelect = 217,
	ControlScriptLeftAxisX = 218,
	ControlScriptLeftAxisY = 219,
	ControlScriptRightAxisX = 220,
	ControlScriptRightAxisY = 221,
	ControlScriptRUp = 222,
	ControlScriptRDown = 223,
	ControlScriptRLeft = 224,
	ControlScriptRRight = 225,
	ControlScriptLB = 226,
	ControlScriptRB = 227,
	ControlScriptLT = 228,
	ControlScriptRT = 229,
	ControlScriptLS = 230,
	ControlScriptRS = 231,
	ControlScriptPadUp = 232,
	ControlScriptPadDown = 233,
	ControlScriptPadLeft = 234,
	ControlScriptPadRight = 235,
	ControlScriptSelect = 236,
	ControlCursorAccept = 237,
	ControlCursorCancel = 238,
	ControlCursorX = 239,
	ControlCursorY = 240,
	ControlCursorScrollUp = 241,
	ControlCursorScrollDown = 242,
	ControlEnterCheatCode = 243,
	ControlInteractionMenu = 244,
	ControlMpTextChatAll = 245,
	ControlMpTextChatTeam = 246,
	ControlMpTextChatFriends = 247,
	ControlMpTextChatCrew = 248,
	ControlPushToTalk = 249,
	ControlCreatorLS = 250,
	ControlCreatorRS = 251,
	ControlCreatorLT = 252,
	ControlCreatorRT = 253,
	ControlCreatorMenuToggle = 254,
	ControlCreatorAccept = 255,
	ControlCreatorDelete = 256,
	ControlAttack2 = 257,
	ControlRappelJump = 258,
	ControlRappelLongJump = 259,
	ControlRappelSmashWindow = 260,
	ControlPrevWeapon = 261,
	ControlNextWeapon = 262,
	ControlMeleeAttack1 = 263,
	ControlMeleeAttack2 = 264,
	ControlWhistle = 265,
	ControlMoveLeft = 266,
	ControlMoveRight = 267,
	ControlMoveUp = 268,
	ControlMoveDown = 269,
	ControlLookLeft = 270,
	ControlLookRight = 271,
	ControlLookUp = 272,
	ControlLookDown = 273,
	ControlSniperZoomIn = 274,
	ControlSniperZoomOut = 275,
	ControlSniperZoomInAlternate = 276,
	ControlSniperZoomOutAlternate = 277,
	ControlVehicleMoveLeft = 278,
	ControlVehicleMoveRight = 279,
	ControlVehicleMoveUp = 280,
	ControlVehicleMoveDown = 281,
	ControlVehicleGunLeft = 282,
	ControlVehicleGunRight = 283,
	ControlVehicleGunUp = 284,
	ControlVehicleGunDown = 285,
	ControlVehicleLookLeft = 286,
	ControlVehicleLookRight = 287,
	ControlReplayStartStopRecording = 288,
	ControlReplayStartStopRecordingSecondary = 289,
	ControlScaledLookLeftRight = 290,
	ControlScaledLookUpDown = 291,
	ControlScaledLookUpOnly = 292,
	ControlScaledLookDownOnly = 293,
	ControlScaledLookLeftOnly = 294,
	ControlScaledLookRightOnly = 295,
	ControlReplayMarkerDelete = 296,
	ControlReplayClipDelete = 297,
	ControlReplayPause = 298,
	ControlReplayRewind = 299,
	ControlReplayFfwd = 300,
	ControlReplayNewmarker = 301,
	ControlReplayRecord = 302,
	ControlReplayScreenshot = 303,
	ControlReplayHidehud = 304,
	ControlReplayStartpoint = 305,
	ControlReplayEndpoint = 306,
	ControlReplayAdvance = 307,
	ControlReplayBack = 308,
	ControlReplayTools = 309,
	ControlReplayRestart = 310,
	ControlReplayShowhotkey = 311,
	ControlReplayCycleMarkerLeft = 312,
	ControlReplayCycleMarkerRight = 313,
	ControlReplayFOVIncrease = 314,
	ControlReplayFOVDecrease = 315,
	ControlReplayCameraUp = 316,
	ControlReplayCameraDown = 317,
	ControlReplaySave = 318,
	ControlReplayToggletime = 319,
	ControlReplayToggletips = 320,
	ControlReplayPreview = 321,
	ControlReplayToggleTimeline = 322,
	ControlReplayTimelinePickupClip = 323,
	ControlReplayTimelineDuplicateClip = 324,
	ControlReplayTimelinePlaceClip = 325,
	ControlReplayCtrl = 326,
	ControlReplayTimelineSave = 327,
	ControlReplayPreviewAudio = 328,
	ControlVehicleDriveLook = 329,
	ControlVehicleDriveLook2 = 330,
	ControlVehicleFlyAttack2 = 331,
	ControlRadioWheelUpDown = 332,
	ControlRadioWheelLeftRight = 333,
	ControlVehicleSlowMoUpDown = 334,
	ControlVehicleSlowMoUpOnly = 335,
	ControlVehicleSlowMoDownOnly = 336,
	ControlMapPointOfInterest = 337,
};
