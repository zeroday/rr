#ifndef REP_PROCESS_EVENT_H_
#define REP_PROCESS_EVENT_H_

#include "../share/types.h"
#include "../share/trace.h"

void rep_process_syscall(struct context* context);

/*
 * The 'num' parameter in these macros usually corresponds to the
 * number of buffers that had to be saved in the record phase,
 * e.g.:
 * SYS_REC1(stat64, sizeof(struct stat64), regs.ecx)
 * will need to be replayed with num = 1.
 */

#define EMU_FD			0

/*********************** All system calls that are emulated are handled here *****************************/


#define SYS_EMU_ARG(syscall,num) \
	case SYS_##syscall: { \
		if (state == STATE_SYSCALL_ENTRY) { \
	       goto_next_syscall_emu(context); \
		   validate_args(context);\
		} else {\
			int i; for (i=0;i<(num);i++) {set_child_data(context);}\
			set_return_value(context); \
			validate_args(context); \
			finish_syscall_emu(context);} \
	break; }



/*********************** All system calls that are executed are handled here *****************************/


#define SYS_EXEC_ARG(syscall,num) \
	case SYS_##syscall: { \
		if (state == STATE_SYSCALL_ENTRY) { __ptrace_cont(context);}\
		else {\
			__ptrace_cont(context);\
			int i; for (i = 0; i < num; i++) {set_child_data(context);}\
			set_return_value(context); \
			validate_args(context);}\
	break; }


#define SYS_EXEC_ARG_RET(ctx,syscall,num) \
	case SYS_##syscall: { \
		if (state == STATE_SYSCALL_ENTRY) { __ptrace_cont(ctx);}\
		else {\
			__ptrace_cont(ctx);\
			int i; for (i = 0; i < num; i++) {set_child_data(ctx);}\
			validate_args(ctx);}\
	break; }




/*********************** All system calls that include file descriptors are handled here *****************************/


#define SYS_FD_ARG(syscall,num) \
	SYS_EMU_ARG(syscall,num)
#define SYS_FD_USER_DEF(syscall,fixes,code) \
	case SYS_##syscall: { \
		if (state == STATE_SYSCALL_ENTRY) {goto_next_syscall_emu(context);}\
		else {code \
			validate_args(context);\
			finish_syscall_emu(context);}\
	break; }

#if (EMU_FD) /* all output operations on fd are emulated */
#define SYS_FD_OUT_ARG(syscall,num)\
	SYS_FD_ARG(syscall,num)
#else /* all output operations on fd are executed */
#define SYS_FD_OUT_ARG(syscall,num)\
	SYS_EXEC_ARG(syscall,num)
#endif

/*
#define SYS_FD_ARG0(syscall)\
	case SYS_##syscall: { \
		if (state == STATE_SYSCALL_ENTRY) {__ptrace_cont(context);}\
		else {__ptrace_cont(context); \
		validate_args(context);} \
	break; }
#define SYS_FD_REG_ARG1(syscall, reg) \
	case SYS_##syscall: { \
		if (state == STATE_SYSCALL_ENTRY) {__ptrace_cont(context);}\
		else {__ptrace_cont(context); \
		fix_raw_data_ptr(context);\
		validate_args(context);} \
	break; }
#define SYS_FD_ADR_ARG2(syscall, addr1, addr2) \
	case SYS_##syscall: { \
		if (state == STATE_SYSCALL_ENTRY) {__ptrace_cont(context, trace);}\
		else {__ptrace_cont(context); \
		fix_raw_data_ptr(context);\
		fix_raw_data_ptr(context);\
		validate_args(context);} \
	break; }
#define SYS_FD_USER_DEF(syscall,fixes,code) \
	case SYS_##syscall: { \
		if (state == STATE_SYSCALL_ENTRY) {__ptrace_cont(context);}\
		else {code \
		__ptrace_cont(context); \
		set_return_value(context); \
		validate_args(context);} \
	break; }
*/


#endif /* REP_PROCESS_EVENT_H_ */
