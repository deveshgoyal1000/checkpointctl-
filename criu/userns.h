#ifndef __CR_USERNS_H__
#define __CR_USERNS_H__

#include <stdbool.h>
#include "common/compiler.h"

extern int userns_call(void *arg, int flags, void *data, size_t size_data,
		      int fd_usernsd);

/* Check if the process is running in its own user namespace */
extern bool is_in_userns(void);

/* Check if the process is using host's mount namespace */
extern bool is_using_host_mntns(void);

#endif /* __CR_USERNS_H__ */
