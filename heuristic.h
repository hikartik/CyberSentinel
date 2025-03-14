// heuristic.h
#ifndef HEURISTIC_H
#define HEURISTIC_H

// Analyzes the file's entropy.
// Returns 1 if the file is flagged as suspicious (high entropy), 0 otherwise.
int analyze_file_entropy(const char *filepath);

#endif // HEURISTIC_H

