// realtime.h
#ifndef REALTIME_H
#define REALTIME_H

// Monitors the specified directory for file changes.
// New and modified files are scanned and quarantined if suspicious.
void monitor_directory(const char *directory);

#endif // REALTIME_H

