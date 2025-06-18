// realtime.c
#include "utils.h"
#include "realtime.h"
#include "scanner.h"
#include "heuristic.h"
#include "quarantine.h"
#include <sys/inotify.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>

#define EVENT_SIZE  (sizeof(struct inotify_event))
#define BUF_LEN     (1024 * (EVENT_SIZE + 16))

void monitor_directory(const char *directory) {
    int fd = inotify_init1(IN_NONBLOCK);
    if (fd < 0) {
        perror("inotify_init");
        return;
    }
    int wd = inotify_add_watch(fd, directory, IN_CREATE | IN_MODIFY | IN_DELETE);
    if (wd < 0) {
        perror("inotify_add_watch");
        close(fd);
        return;
    }
    printf("[Realtime] Monitoring directory: %s\n", directory);
    char buffer[BUF_LEN];
    // Stop after processing a number of events or periods of inactivity.
    int event_count = 0;
    int idle_cycles = 0;
    while (event_count < 10 && idle_cycles < 5) {
        int length = read(fd, buffer, BUF_LEN);
        if (length < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                idle_cycles++;
                sleep(1);
                continue;
            }
            perror("read");
            break;
        }
        int i = 0;
        while (i < length) {
            struct inotify_event *event = (struct inotify_event *)&buffer[i];
            if (event->len) {
                if (event->mask & IN_CREATE)
                    printf("[Realtime] File %s created.\n", event->name);
                else if (event->mask & IN_MODIFY)
                    printf("[Realtime] File %s modified.\n", event->name);
                else if (event->mask & IN_DELETE)
                    printf("[Realtime] File %s deleted.\n", event->name);

                if (!(event->mask & IN_ISDIR) && (event->mask & (IN_CREATE | IN_MODIFY))) {
                    char filepath[PATH_MAX];
                    snprintf(filepath, sizeof(filepath), "%s/%s", directory, event->name);
                    int infected = scan_file_signature(filepath);
                    infected |= analyze_file_entropy(filepath);
                    if (infected) {
                        quarantine_file(filepath, "./quarantine");
                    }
                }
                event_count++;
            }
            i += EVENT_SIZE + event->len;
        }
    }
    inotify_rm_watch(fd, wd);
    close(fd);
}

