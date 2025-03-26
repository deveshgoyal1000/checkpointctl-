#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mount.h>
#include "userns.h"
#include "log.h"
#include "common/bug.h"

bool is_in_userns(void)
{
    uid_t uid_inside, uid_outside;
    gid_t gid_inside, gid_outside;

    uid_inside = geteuid();
    gid_inside = getegid();

    uid_outside = uid_inside;
    gid_outside = gid_inside;

    /* Get the uid/gid from /proc/self/uid_map and /proc/self/gid_map */
    if (uid_inside == 0 || gid_inside == 0) {
        int fd;
        char buf[32];

        fd = open("/proc/self/uid_map", O_RDONLY);
        if (fd >= 0) {
            if (read(fd, buf, sizeof(buf)) > 0) {
                sscanf(buf, "%u %u", &uid_inside, &uid_outside);
            }
            close(fd);
        }

        fd = open("/proc/self/gid_map", O_RDONLY);
        if (fd >= 0) {
            if (read(fd, buf, sizeof(buf)) > 0) {
                sscanf(buf, "%u %u", &gid_inside, &gid_outside);
            }
            close(fd);
        }
    }

    return uid_inside != uid_outside || gid_inside != gid_outside;
}

bool is_using_host_mntns(void)
{
    struct stat st1, st2;

    if (stat("/proc/1/ns/mnt", &st1) < 0)
        return false;

    if (stat("/proc/self/ns/mnt", &st2) < 0)
        return false;

    return st1.st_ino == st2.st_ino;
}

int userns_call(int (*fn)(void *), int flags, void *arg, size_t arg_size,
                int fd_usernsd)
{
    int ret;
    void *stack;
    pid_t pid;

    stack = malloc(4096);
    if (!stack) {
        pr_err("Failed to allocate stack for usernsd call\n");
        return -1;
    }

    pid = clone(fn, stack + 4096, flags | CLONE_VM | CLONE_FILES | SIGCHLD, arg);
    if (pid < 0) {
        pr_err("Failed to clone process for usernsd call\n");
        free(stack);
        return -1;
    }

    if (waitpid(pid, &ret, 0) < 0) {
        pr_err("Failed to wait for usernsd call completion\n");
        free(stack);
        return -1;
    }

    free(stack);
    return WEXITSTATUS(ret);
}
