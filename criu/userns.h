#ifndef __CR_USERNS_H__
#define __CR_USERNS_H__

#include <stdbool.h>
#include "common/compiler.h"

extern int userns_call(int (*fn)(void *), void *arg, size_t arg_size,
		      int fd_usernsd);

/* Check if the process is running in its own user namespace */
extern bool is_in_userns(void);

/* Check if the process is using host's mount namespace */
extern bool is_using_host_mntns(void);

#endif /* __CR_USERNS_H__ */
