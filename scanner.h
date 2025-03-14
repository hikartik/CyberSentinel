// scanner.h
#ifndef SCANNER_H
#define SCANNER_H

// Scans the given file using signature-based detection.
// Returns 1 if the file is detected as malware, 0 otherwise.
int scan_file_signature(const char *filepath);

#endif // SCANNER_H

