// SPDX-License-Identifier: GPL-2.0
/*
 * L2 lock-in for the kernel layout of struct cmd_obj.
 *
 * The host L2 header (tests/host/include/host_cmd_queue_types.h) used to
 * declare "u8 res, no_io;", i.e. no_io packed right after res, and the Rust
 * CmdObj mirror matched it. The real kernel struct (include/rtw_cmd.h) puts
 * no_io *after* sctx:
 *
 *   padapter@0 cmdcode@8 res@10 parmbuf@16 cmdsz@24 rsp@32 rspsz@40
 *   sctx@48 no_io@56 list@64   (sizeof == 80)
 *
 * So on the kernel path the mirror was wrong twice over:
 *   1) no_io was read from offset 11 (padding, always 0 after rtw_zmalloc),
 *      so rtw_cmd_filter never honored no_io and dropped those commands
 *      before hw_init completed;
 *   2) the list node was written at offset 56, clobbering the kernel's
 *      no_io byte, and container_of on dequeue used 56 instead of 64.
 *      core/rtw_cmd.c still reads pcmd->no_io at offset 56 under
 *      CONFIG_LPS_LCLK, so it saw a pointer byte instead of the flag.
 *
 * This test compiles rust/rtw_cmd_rest.rs with the kernel cfgs
 * (rust_cmd_queue, no host_cmd_queue_test) and drives it against a real
 * 80-byte cmd_obj fixture with the host-mirror slots poisoned, so a
 * regression back to direct mirror access cannot pass.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int32_t s32;

#define _SUCCESS 1
#define _FAIL 0

struct klist {
	struct klist *next;
	struct klist *prev;
};

/* Real kernel struct cmd_obj field order (include/rtw_cmd.h). */
struct kernel_cmd_obj {
	void *padapter;
	u16 cmdcode;
	u8 res;
	u8 *parmbuf;
	u32 cmdsz;
	u8 *rsp;
	u32 rspsz;
	void *sctx;
	u8 no_io;
	struct klist list;
};

/* Kernel _queue: list first, lock after. */
struct kernel_queue {
	struct klist queue;
	int lock;
};

_Static_assert(sizeof(struct kernel_cmd_obj) == 80, "kernel cmd_obj is 80 bytes");
_Static_assert(__builtin_offsetof(struct kernel_cmd_obj, sctx) == 48, "sctx@48");
_Static_assert(__builtin_offsetof(struct kernel_cmd_obj, no_io) == 56, "no_io@56");
_Static_assert(__builtin_offsetof(struct kernel_cmd_obj, list) == 64, "list@64");
_Static_assert(__builtin_offsetof(struct kernel_queue, lock) == 16, "queue lock@16");

/* The offset the old Rust mirror used for no_io / the list node. */
#define MIRROR_NO_IO_OFF 11
#define MIRROR_LIST_OFF 56

s32 rtw_cmd_filter(void *pcmdpriv, void *cmd_obj);
s32 _rtw_enqueue_cmd(void *queue, void *obj, _Bool to_head);
void *_rtw_dequeue_cmd(void *queue);
void *rtw_dequeue_cmd(void *pcmdpriv);

/* --- fixture state driven by the C side --- */
static struct kernel_queue g_queue;
static int g_hw_init_completed;
static int g_cmdthd_running;
static u8 g_adapter;
static u32 g_freed_sz;

/* --- C accessor shims: the kernel build gets these from
 * core/rtw_cmd_queue.c, compiled against the real struct. --- */
u8 rtw_rust_cmd_obj_no_io(void *pcmd)
{
	return ((struct kernel_cmd_obj *)pcmd)->no_io;
}

struct klist *rtw_rust_cmd_obj_list(void *pcmd)
{
	return &((struct kernel_cmd_obj *)pcmd)->list;
}

void *rtw_rust_cmd_obj_from_list(struct klist *plist)
{
	return (void *)((char *)plist -
			__builtin_offsetof(struct kernel_cmd_obj, list));
}

u32 rtw_rust_cmd_obj_size(void)
{
	return (u32)sizeof(struct kernel_cmd_obj);
}

/* --- remaining externs the Rust kernel path needs --- */
u8 rtw_rust_hw_init_completed(void *adapter)
{
	(void)adapter;
	return g_hw_init_completed ? 1 : 0;
}

void rtw_rust_queue_enter_critical(int *lock, unsigned long *irql)
{
	(void)lock;
	(void)irql;
}

void rtw_rust_queue_exit_critical(int *lock, unsigned long *irql)
{
	(void)lock;
	(void)irql;
}

void *rtw_rust_cmd_priv_padapter(void *p)
{
	(void)p;
	return &g_adapter;
}

int rtw_rust_cmd_priv_cmdthd_running(void *p)
{
	(void)p;
	return g_cmdthd_running;
}

void *rtw_rust_cmd_priv_cmd_queue(void *p)
{
	(void)p;
	return &g_queue;
}

void *rtw_rust_cmd_priv_cmd_queue_sema(void *p)
{
	(void)p;
	return &g_queue.lock;
}

void *rtw_rust_cmd_priv_for_enqueue(void *p)
{
	return p;
}

void _rtw_up_sema(int *sema)
{
	(void)sema;
}

void _rtw_mfree(void *p, u32 sz)
{
	(void)p;
	g_freed_sz = sz;
}

static int fail(const char *msg)
{
	fprintf(stderr, "FAIL: %s\n", msg);
	return 1;
}

static void queue_init(void)
{
	g_queue.queue.next = &g_queue.queue;
	g_queue.queue.prev = &g_queue.queue;
	g_queue.lock = 0;
}

/*
 * Fresh 80-byte cmd_obj as rtw_zmalloc would hand it over, with the
 * host-mirror no_io slot poisoned to the value that would flip the
 * pre-fix behaviour, so reading the wrong offset is always detected.
 */
static void cmd_obj_init(struct kernel_cmd_obj *obj, u8 no_io, u8 mirror_poison)
{
	memset(obj, 0, sizeof(*obj));
	obj->cmdcode = 1;	/* not CMD_SET_CHANPLAN */
	obj->cmdsz = 4;
	obj->no_io = no_io;
	((u8 *)obj)[MIRROR_NO_IO_OFF] = mirror_poison;
}

int main(void)
{
	struct kernel_cmd_obj obj;
	void *got;

	/* 1) no_io must be read from offset 56, not the mirror's offset 11.
	 * hw_init not completed + no_io set => command must be allowed. */
	queue_init();
	cmd_obj_init(&obj, 1, 0);
	g_hw_init_completed = 0;
	g_cmdthd_running = 1;
	if (rtw_cmd_filter(&g_queue, &obj) != _SUCCESS)
		return fail("rtw_cmd_filter dropped a no_io command (read no_io@11)");

	/* 2) The mirror slot must not be able to fake no_io. */
	queue_init();
	cmd_obj_init(&obj, 0, 0xff);
	g_hw_init_completed = 0;
	g_cmdthd_running = 1;
	if (rtw_cmd_filter(&g_queue, &obj) != _FAIL)
		return fail("rtw_cmd_filter honored poisoned no_io@11");

	/* 3) Enqueue must link the kernel list node at offset 64 and must
	 * not write over no_io@56. core/rtw_cmd.c reads that byte. */
	queue_init();
	cmd_obj_init(&obj, 1, 0);
	if (_rtw_enqueue_cmd(&g_queue, &obj, 0) != _SUCCESS)
		return fail("_rtw_enqueue_cmd failed");
	if (obj.no_io != 1)
		return fail("enqueue clobbered no_io@56 with the list node");
	if (g_queue.queue.next != &obj.list)
		return fail("enqueue linked the wrong offset (not list@64)");
	if (g_queue.queue.prev != &obj.list)
		return fail("enqueue tail pointer is not list@64");

	/* 4) Dequeue must recover the object base from list@64. */
	got = _rtw_dequeue_cmd(&g_queue);
	if (got != (void *)&obj)
		return fail("dequeue container_of used the wrong list offset");
	if (obj.no_io != 1)
		return fail("dequeue clobbered no_io@56");

	/* 5) Same for an object linked by C at list@64 (LIST_CONTAINOR). */
	queue_init();
	cmd_obj_init(&obj, 0, 0);
	obj.list.next = &g_queue.queue;
	obj.list.prev = &g_queue.queue;
	g_queue.queue.next = &obj.list;
	g_queue.queue.prev = &obj.list;
	got = rtw_dequeue_cmd(&g_queue);
	if (got != (void *)&obj)
		return fail("rtw_dequeue_cmd did not resolve a C-linked list@64");

	/* 6) insert-to-head path (rtw_cmd.c ENQ_HEAD retry) uses list@64 too. */
	queue_init();
	cmd_obj_init(&obj, 1, 0);
	if (_rtw_enqueue_cmd(&g_queue, &obj, 1) != _SUCCESS)
		return fail("_rtw_enqueue_cmd(to_head) failed");
	if (obj.no_io != 1)
		return fail("enqueue-to-head clobbered no_io@56");
	if (g_queue.queue.next != &obj.list)
		return fail("enqueue-to-head linked the wrong offset");

	printf("PASS: kernel cmd_obj layout (no_io@%zu, list@%zu, size %zu)\n",
	       (size_t)__builtin_offsetof(struct kernel_cmd_obj, no_io),
	       (size_t)__builtin_offsetof(struct kernel_cmd_obj, list),
	       sizeof(struct kernel_cmd_obj));
	return 0;
}
