
#define __ALWAYS_INLINE static inline __attribute__( ( always_inline ) )
#define __NO_INLINE __attribute__((noinline))
#define __NAKED __attribute__ ((naked)) __attribute__((noinline))


#define COBRA 1

int write_process(void* destination, const void* source, size_t size)
{
#if COBRA == 1
	memcpy(destination,source,size);
	return 0;
#else
	system_call_4(905, (uint64_t)sys_process_getpid(), (uint64_t)destination, size, (uint64_t)source);//dex only
	return_to_user_prog(int);
#endif
}
template <typename T>
__ALWAYS_INLINE T(*call(unsigned int address, unsigned int toc = TOC))(...) {
	volatile opd_s opd = { address,toc };
	T(*func)(...) = (T(*)(...)) & opd;
	return func;
}

int *getPatchInJump(int Destination, bool Linked)
{
		int FuncBytes[4];													// Use this data to copy over the address.
		Destination = *(volatile int*)Destination;									// Get the actual destination address (pointer to void).
		FuncBytes[0] = (0x3D600000) + ((Destination >> 16) & 0xFFFF);			// lis %r11, dest>>16
		if (Destination & 0x8000)											// if bit 16 is 1...
			FuncBytes[0] += 1;
		FuncBytes[1] = (0x396B0000) + (Destination & 0xFFFF);					// addi %r11, %r11, dest&OxFFFF
		FuncBytes[2] = (0x7D6903A6);											// mtctr %r11
		FuncBytes[3] = (0x4E800420);											// bctr
		if (Linked)
			FuncBytes[3] += 1;												// bctrl
		return FuncBytes;
}

void PatchInJump(uint64_t Address, int Destination, bool Linked) {
	int *FuncBytes = getPatchInJump(Destination, Linked);
	write_process((void*)Address, FuncBytes, 4 * 4);
}

void createFile(char *loc) {
	int fd;
	int ret;
	ret = cellFsOpen(loc,CELL_FS_O_CREAT|CELL_FS_O_RDWR|CELL_FS_O_TRUNC, &fd, NULL, 0);
    cellFsClose(fd);
	if (!ret) {
		cellFsClose(fd);
	}
}
void writeToFile(char *file, char buf[] = NULL, int size = 0) {
	CellFsErrno err;
	int fd;
	uint64_t nrw;
	int ret;
	err = cellFsOpen(file,CELL_FS_O_RDWR|CELL_FS_O_CREAT, &fd, NULL, 0);
	cellFsTruncate(file,0);
	if (size != 0)
		err = cellFsWrite(fd, (const void *)buf, (uint64_t)size, &nrw);
	err = cellFsClose(fd); 
}
void appendToFile(char *file, char buf[] = NULL, int size = 0) {
	int err;
	int fd;
	uint64_t nrw;
	int ret;
	err = cellFsOpen(file,CELL_FS_O_RDWR|CELL_FS_O_APPEND|CELL_FS_O_CREAT, &fd, NULL, 0);
	if (size != 0)
		err = cellFsWrite(fd, (const void *)buf, (uint64_t)size, &nrw);
	err = cellFsClose(fd);
}
void debug_write(char *str) {
	writeToFile("/dev_hdd0/tmp/debugfile.txt",str,strlen(str));
}
void debug_append(char *str) {
	appendToFile("/dev_hdd0/tmp/debugfile.txt",str,strlen(str));
}

void reverse(char arr[], int count)
{
   char temp;
   for (int i = 0; i < count/2; ++i)
   {
      temp = arr[i];
      arr[i] = arr[count-i-1];
      arr[count-i-1] = temp;
   }
}

// Implementation of itoa()
char* itoa(unsigned int num, char* str, int base)
{
    int i = 0;
    bool isNegative = false;

    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    // In standard itoa(), negative numbers are handled only with
    // base 10. Otherwise numbers are considered unsigned.
    if (num < 0 && base == 10)
    {
        //isNegative = true;
        //num = -num;
    }

    // Process individual digits
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        num = num/base;
    }

    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';

    str[i] = '\0'; // Append string terminator

    // Reverse the string
    reverse(str, i);

    return str;
}

char *ItoS(int num)
{
	static char buf[30];
	return itoa(num,buf,10);
}
using namespace std;
char *ItoSH(int num)
{
	static char buf[30];
	return itoa(num,buf,10);
}

void sleep(usecond_t time)  //1 second = 1000
{
	sys_timer_usleep(time * 1000);
}