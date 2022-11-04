/*
thumb16 thumb32 arm32 inlineHook
author: ele7enxxh
mail: ele7enxxh@qq.com
website: ele7enxxh.com
modified time: 2015-01-23
created time: 2015-11-30
*/

#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/ptrace.h>

#include "relocate.h"
#include "inlineHook.h"

#include "../const.h"

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

#define PAGE_START(addr)	(~(PAGE_SIZE - 1) & (addr))
#define SET_BIT0(addr)		(addr | 1)
#define CLEAR_BIT0(addr)	(addr & 0xFFFFFFFE)
#define TEST_BIT0(addr)		(addr & 1)

#define ACTION_ENABLE	0
#define ACTION_DISABLE	1

#define bool    int
#define true    1
#define false   0

#if defined(__arm__)
	
enum hook_status {
	REGISTERED,
	HOOKED,
};

struct inlineHookItem {
	uint32_t target_addr; //// 该指针会在hook阶段替换成  new_addr
	uint32_t new_addr; ////  是需要hook 函数的的实现的地址
	uint32_t **proto_addr; //// 给外层原函数的调用地址。
	void *orig_instructions; //// target 原函数的指令
	int orig_boundaries[4]; ////  原来函数指令的个数
	int trampoline_boundaries[20]; ////  新的trampoline 的指令个数
	int count; //// 指令的个数
	void *trampoline_instructions; //// mmaps 一个新的内存来存放trampoline 的指令内容
	int length; //// 判断是thumb 或者 arm 指令 arm 存放8 长度， thumb 放 12 字节码
	int status; //// 当前函数是否是被hook
	int mode;
};

struct inlineHookInfo {
	struct inlineHookItem item[1024];
	int size;
};

static struct inlineHookInfo info = {0};

static int getAllTids(pid_t exclude_tid, pid_t *tids)
{
	char dir_path[32];
	DIR *dir;
	int i;
	struct dirent *entry;
	pid_t tid;

	if (exclude_tid < 0) {
		snprintf(dir_path, sizeof(dir_path), "/proc/self/task");
	}else {
		snprintf(dir_path, sizeof(dir_path), "/proc/%d/task", exclude_tid);
	}

	dir = opendir(dir_path);
    if (dir == NULL) {
    	return 0;
    }

    i = 0;
    while((entry = readdir(dir)) != NULL) {
    	tid = atoi(entry->d_name);
    	if (tid != 0 && tid != exclude_tid) {
    		tids[i++] = tid;
    	}
    }
    closedir(dir);
    return i;
}

static bool doProcessThreadPC(struct inlineHookItem *item, struct pt_regs *regs, int action)
{
	int offset;
	int i;

	switch (action)
	{
		case ACTION_ENABLE:
			offset = regs->ARM_pc - CLEAR_BIT0(item->target_addr); //
			for (i = 0; i < item->count; ++i) {
				LOGE("doProcessThreadPC:%d, %d, %d", offset, item->count, item->orig_boundaries[i]);
				if (offset == item->orig_boundaries[i]) {

					LOGE("doProcessThreadPC in:%d, %d ", offset,item->orig_boundaries[i]);

					regs->ARM_pc = (uint32_t) item->trampoline_instructions + item->trampoline_boundaries[i];
					return true;
				}
			}
			break;
		case ACTION_DISABLE:
			offset = regs->ARM_pc - (int) item->trampoline_instructions;
			for (i = 0; i < item->count; ++i) {
				if (offset == item->trampoline_boundaries[i]) {
					regs->ARM_pc = CLEAR_BIT0(item->target_addr) + item->orig_boundaries[i];
					return true;
				}
			}
			break;
	}
	return false;
}

static void processThreadPC(pid_t tid, struct inlineHookItem *item, int action)
{
	struct pt_regs regs;

	if (ptrace(PTRACE_GETREGS, tid, NULL, &regs) == 0) {
		if (item == NULL) {
			int pos;

			for (pos = 0; pos < info.size; ++pos) {
				if (doProcessThreadPC(&info.item[pos], &regs, action) == true) {
					break;
				}
			}
		}
		else {
			doProcessThreadPC(item, &regs, action);
		}

		ptrace(PTRACE_SETREGS, tid, NULL, &regs);
	}
}

static pid_t freeze(struct inlineHookItem *item, int action)
{
	int count;
	pid_t tids[1024]; // 记录多线程的id
	pid_t pid;

	pid = -1;
	count = getAllTids(gettid(), tids);
	if (count > 0) {
		pid = fork();

		if (pid == 0) { // 子进程中
			int i;

			LOGE("darren 子进程 %d", getpid());
			for (i = 0; i < count; ++i) {
				if (ptrace(PTRACE_ATTACH, tids[i], NULL, NULL) == 0) { // ptrace 多个线程
					waitpid(tids[i], NULL, WUNTRACED);
					processThreadPC(tids[i], item, action);
				}
			}
			
			raise(SIGSTOP);

			for (i = 0; i < count; ++i) {
				ptrace(PTRACE_DETACH, tids[i], NULL, NULL);
			}

			raise(SIGKILL);
		}else if (pid > 0) {
			waitpid(pid, NULL, WUNTRACED);
		}
	}

	return pid;
}

static void unFreeze(pid_t pid)
{
	if (pid < 0) {
		return;
	}

	kill(pid, SIGCONT);
	wait(NULL);
}

static bool isExecutableAddr(uint32_t addr)
{
	FILE *fp;
	char line[1024];
	uint32_t start;
	uint32_t end;

	fp = fopen("/proc/self/maps", "r");
	if (fp == NULL) {
		return false;
	}

	while (fgets(line, sizeof(line), fp)) {
		if (strstr(line, "r-xp") || strstr(line, "rwxp")) {
			start = strtoul(strtok(line, "-"), NULL, 16);
			end = strtoul(strtok(NULL, " "), NULL, 16);
			if (addr >= start && addr <= end) {
				fclose(fp);
				return true;
			}
		}
	}

	fclose(fp);

	return false;
}

static struct inlineHookItem *findInlineHookItem(uint32_t target_addr)
{
	int i;

	for (i = 0; i < info.size; ++i) {
		if (info.item[i].target_addr == target_addr) {
			return &info.item[i];
		}
	}

	return NULL;
}

static struct inlineHookItem *addInlineHookItem() {
	struct inlineHookItem *item;

	if (info.size >= 1024) {
		return NULL;
	}

	item = &info.item[info.size];
	++info.size;

	return item;
}

static void deleteInlineHookItem(int pos)
{
	info.item[pos] = info.item[info.size - 1];
	--info.size;
}

enum ele7en_status registerInlineHook(uint32_t target_addr, uint32_t new_addr, uint32_t **proto_addr)
{
	struct inlineHookItem *item;

	if (!isExecutableAddr(target_addr) || !isExecutableAddr(new_addr)) {
		return ELE7EN_ERROR_NOT_EXECUTABLE;
	}

	item = findInlineHookItem(target_addr);
	if (item != NULL) {
		if (item->status == REGISTERED) {
			return ELE7EN_ERROR_ALREADY_REGISTERED;
		}
		else if (item->status == HOOKED) {
			return ELE7EN_ERROR_ALREADY_HOOKED;
		}else {
			return ELE7EN_ERROR_UNKNOWN;
		}
	}

	item = addInlineHookItem();

	item->target_addr = target_addr;
	item->new_addr = new_addr;
	item->proto_addr = proto_addr;


	item->length = TEST_BIT0(item->target_addr) ? 12 : 8; // 判断是thumb 或者 arm 指令 arm 存放8 长度， thumb 放 12 字节码

	LOGE("inlinehook32:%d, %x,%x", item->length, item->target_addr, (void *) CLEAR_BIT0(item->target_addr));

	item->orig_instructions = malloc(item->length);
	memcpy(item->orig_instructions, (void *) CLEAR_BIT0(item->target_addr), item->length);

	item->trampoline_instructions = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
	relocateInstruction(item->target_addr, item->orig_instructions, item->length, item->trampoline_instructions, item->orig_boundaries, item->trampoline_boundaries, &item->count);

	item->status = REGISTERED;

	return ELE7EN_OK;
}

static void doInlineUnHook(struct inlineHookItem *item, int pos)
{
	mprotect((void *) PAGE_START(CLEAR_BIT0(item->target_addr)), PAGE_SIZE * 2, PROT_READ | PROT_WRITE | PROT_EXEC);
	memcpy((void *) CLEAR_BIT0(item->target_addr), item->orig_instructions, item->length);
	mprotect((void *) PAGE_START(CLEAR_BIT0(item->target_addr)), PAGE_SIZE * 2, PROT_READ | PROT_EXEC);
	munmap(item->trampoline_instructions, PAGE_SIZE);
	free(item->orig_instructions);

	deleteInlineHookItem(pos);

	cacheflush(CLEAR_BIT0(item->target_addr), CLEAR_BIT0(item->target_addr) + item->length, 0);
}

enum ele7en_status inlineUnHook(uint32_t target_addr)
{
	int i;

	for (i = 0; i < info.size; ++i) {
		if (info.item[i].target_addr == target_addr && info.item[i].status == HOOKED) {
			pid_t pid;

			pid = freeze(&info.item[i], ACTION_DISABLE);

			doInlineUnHook(&info.item[i], i);

			unFreeze(pid);

			return ELE7EN_OK;
		}
	}

	return ELE7EN_ERROR_NOT_HOOKED;
}

void inlineUnHookAll()
{
	pid_t pid;
	int i;

	pid = freeze(NULL, ACTION_DISABLE);

	for (i = 0; i < info.size; ++i) {
		if (info.item[i].status == HOOKED) {
			doInlineUnHook(&info.item[i], i);
			--i;
		}
	}

	unFreeze(pid);
}

static void doInlineHook(struct inlineHookItem *item)
{
	mprotect((void *) PAGE_START(CLEAR_BIT0(item->target_addr)), PAGE_SIZE * 2, PROT_READ | PROT_WRITE | PROT_EXEC);

	if (item->proto_addr != NULL) {
		*(item->proto_addr) = TEST_BIT0(item->target_addr) ? (uint32_t *) SET_BIT0((uint32_t) item->trampoline_instructions) : item->trampoline_instructions;
	}
	
	if (TEST_BIT0(item->target_addr)) { // thumb mode
		int i;

		i = 0;
		if (CLEAR_BIT0(item->target_addr) % 4 != 0) {
			((uint16_t *) CLEAR_BIT0(item->target_addr))[i++] = 0xBF00;  // NOP
		}
		((uint16_t *) CLEAR_BIT0(item->target_addr))[i++] = 0xF8DF;
		((uint16_t *) CLEAR_BIT0(item->target_addr))[i++] = 0xF000;	// LDR.W PC, [PC]
		((uint16_t *) CLEAR_BIT0(item->target_addr))[i++] = item->new_addr & 0xFFFF;
		((uint16_t *) CLEAR_BIT0(item->target_addr))[i++] = item->new_addr >> 16;
	}
	else {
		((uint32_t *) (item->target_addr))[0] = 0xe51ff004;	// LDR PC, [PC, #-4]   （pc+8）获取到当前的PC， pc-8 获取到指向当前的地址，加4 表示将下一条指令的值放入当前的PC
		((uint32_t *) (item->target_addr))[1] = item->new_addr;
	}

	/**
	 * 寄存器间接寻址
注意到一个寄存器的容量也是32位，刚好能装下一个绝对地址，所以可以把目标地址先存到某个寄存器（Rm）中，然后执行LDR PC, Rm就实现了绝对地址的跳转，
	 这种以某个寄存器作为基准的寻址方式叫做寄存器间接寻址。再进一步，在实际开发中我们发现PC寄存器就是一个天然的铆点，
	 并且想要跳转的目标地址往往离程序当前执行到的地址不远，所以索性用PC加上一个偏移量来表达一个绝对地址，
	 格式为：LDR PC, [PC, offset]，这种寻址方式又叫PC相对寻址。使用PC相对寻址，我们可以用8个字节（即2条指令的长度）来完成一个绝对地址的跳转操作：
	 虚拟地址	内容
0x00006000	LDR PC, [PC, 4]
0x00006004	destination address
0x00006000位置的指令含义为当CPU执行到此时，将该地址加4字节-即0x00006004地址内的内容写入到PC中，而内容就是我们事先写入的目标地址。

 在学习ARM裸机实时，对于程序的链接地址和程序的运行地址是很重要的，通常在程序的前几k的代码中使用的都是相对跳转指令b,bl，
 前面几k的代码的主要任务就是将整个程序复制到对应的链接地址上去的。这个过程就是代码的重定位，在重定位之前的代码与位置无关(是位置无关码)；
 重定位完成以后就可以跳转到新的地址中去运行了，通常使用 ldr pc, xxx，来实现跳转。这里先来看看三段汇编代码，来理解uboot中重定位后，跳转的实现。und_addr，do_und都是标记，也就是地址。
ldr pc, und_addr（这是uboot中的用法）
ldr pc, do_und
ldr pc, =do_und（这是一条伪指令）


	  ldr pc, und_addr（这是uboot中的用法）

30000000:	ea00000e 	b	30000040 <reset>
30000004:	e51ff004 	ldr	pc, [pc, #-4]	; 30000008 <und_addr>

30000008 <und_addr>:
30000008:	3000000c 	.word	0x3000000c

3000000c <do_und>:
3000000c:	e3a0d30d 	mov	sp, #872415232	; 0x34000000
30000010:	e92d5fff 	push	{r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip, lr}

ldr pc, do_und

0000000 <_start>:
30000000:	ea00000e 	b	30000040 <reset>
30000004:	e59ff000 	ldr	pc, [pc, #0]	; 3000000c <do_und>

30000008 <und_addr>:
30000008:	3000000c 	.word	0x3000000c

3000000c <do_und>:
3000000c:	e3a0d30d 	mov	sp, #872415232	; 0x34000000
30000010:	e92d5fff 	push	{r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip, lr}

ldr pc, =do_und（这是一条伪指令）


3000 0004:	e59ff0b0 	ldr	pc, [pc, #176]	; 300000bc <halt+0x4>

30000008 <und_addr>:
30000008:	3000000c 	.word	0x3000000c

3000000c <do_und>:
3000000c:	e3a0d30d 	mov	sp, #872415232	; 0x34000000
30000010:	e92d5fff 	push	{r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip, lr}
30000014:	e10f0000 	mrs	r0, CPSR
30000018:	e59f10a0 	ldr	r1, [pc, #160]	; 300000c0 <halt+0x8>
3000001c:	eb0000dd 	bl	30000398 <printException>
30000020:	e14f0000 	mrs	r0, SPSR
...
...
...
300000b8 <halt>:
300000b8:	eafffffe 	b	300000b8 <halt>
300000bc:	3000000c 	.word	0x3000000c
300000c0:	3000002c 	.word	0x3000002c


	 */

	mprotect((void *) PAGE_START(CLEAR_BIT0(item->target_addr)), PAGE_SIZE * 2, PROT_READ | PROT_EXEC);
	item->status = HOOKED;
	cacheflush(CLEAR_BIT0(item->target_addr), CLEAR_BIT0(item->target_addr) + item->length, 0);
}

enum ele7en_status inlineHook(uint32_t target_addr)
{
	int i;
	struct inlineHookItem *item;

	item = NULL;
	for (i = 0; i < info.size; ++i) {
		if (info.item[i].target_addr == target_addr) {
			item = &info.item[i];
			break;
		}
	}

	if (item == NULL) {
		return ELE7EN_ERROR_NOT_REGISTERED;
	}

	if (item->status == REGISTERED) {
		pid_t pid;

		pid = freeze(item, ACTION_ENABLE);
		doInlineHook(item);
		unFreeze(pid);
		return ELE7EN_OK;
	}else if (item->status == HOOKED) {
		return ELE7EN_ERROR_ALREADY_HOOKED;
	}else {
		return ELE7EN_ERROR_UNKNOWN;
	}
}

void inlineHookAll()
{
	pid_t pid;
	int i;

	pid = freeze(NULL, ACTION_ENABLE);

	for (i = 0; i < info.size; ++i) {
		if (info.item[i].status == REGISTERED) {
			doInlineHook(&info.item[i]);
		}
	}

	unFreeze(pid);
}

#endif  //#if defined(__arm__)