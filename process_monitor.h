// process_monitor.h
#ifndef PROCESS_MONITOR_H
#define PROCESS_MONITOR_H

// Lists running processes (Linux /proc based).
void list_processes(void);

// Checks a process name for suspicious keywords.
// Returns 1 if suspicious, 0 otherwise.
int check_suspicious_process(const char *process_name);

#endif // PROCESS_MONITOR_H

