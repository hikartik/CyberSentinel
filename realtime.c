// realtime.c
#include "utils.h"
#include "realtime.h"
#include <sys/inotify.h>
#include <unistd.h>

#define EVENT_SIZE  (sizeof(struct inotify_event))
#define BUF_LEN     (1024 * (EVENT_SIZE + 16))

void monitor_directory(const char *directory) {
    int fd = inotify_init();
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
    // For demo, stop after 5 events.
    int event_count = 0;
    while (event_count < 5) {
        int length = read(fd, buffer, BUF_LEN);
        if (length < 0) {
            perror("read");
            break;
        }
        int i = 0;
        while (i < length) {
            struct inotify_event *event = (struct inotify_event *) &buffer[i];
            if (event->len) {
                if (event->mask & IN_CREATE)
                    printf("[Realtime] File %s created.\n", event->name);
                else if (event->mask & IN_MODIFY)
                    printf("[Realtime] File %s modified.\n", event->name);
                else if (event->mask & IN_DELETE)
                    printf("[Realtime] File %s deleted.\n", event->name);
                event_count++;
            }
            i += EVENT_SIZE + event->len;
        }
    }
    inotify_rm_watch(fd, wd);
    close(fd);
}

