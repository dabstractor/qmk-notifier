#include "notifier.h"
#include <string.h>
#include <ctype.h>

// Forward declarations for helper functions
static bool starts_with_pattern(const char *pattern, const char *str, bool case_sensitive);
static bool ends_with_pattern(const char *pattern, const char *str, bool case_sensitive);


bool pattern_match(const char *pattern, const char *str, bool case_sensitive) {
    // Always do an exact string comparison first
    bool exact_match = case_sensitive ?
        (strcmp(pattern, str) == 0) :
        (strcasecmp(pattern, str) == 0);

    if (exact_match) {
        return true;
    }

    // Special characters check is now skipped - proceed with pattern matching

    // Handle start-of-string anchor (^)
    bool anchor_start = (pattern[0] == '^');
    if (anchor_start) pattern++;

    // Check for end-of-string anchor ($)
    size_t pattern_len = strlen(pattern);
    bool anchor_end = (pattern_len > 0 && pattern[pattern_len - 1] == '$');

    // Remove the $ for processing
    char pattern_copy[64]; // Adjust size as needed
    if (anchor_end) {
        strncpy(pattern_copy, pattern, pattern_len - 1);
        pattern_copy[pattern_len - 1] = '\0';
        pattern = pattern_copy;
    }

    // If we have a start anchor, we must match from the beginning
    if (anchor_start) {
        return starts_with_pattern(pattern, str, case_sensitive);
    }

    // If we have an end anchor, we must match at the end
    if (anchor_end) {
        return ends_with_pattern(pattern, str, case_sensitive);
    }

    // No anchors, so we can match the pattern anywhere in the string
    size_t str_len = strlen(str);
    for (size_t i = 0; i <= str_len; i++) {
        if (starts_with_pattern(pattern, str + i, case_sensitive)) {
            return true;
        }
    }

    return false;
}


// Helper function to check if string starts with pattern
static bool starts_with_pattern(const char *pattern, const char *str, bool case_sensitive) {
    while (*pattern && *str) {
        if (*pattern == '*') {
            // Wildcard: skip ahead in pattern and try to match the rest
            pattern++;

            // If * is at the end of pattern, it matches anything
            if (*pattern == '\0') return true;

            // Try to match the rest of the pattern at different positions
            for (const char *s = str; *s; s++) {
                if (starts_with_pattern(pattern, s, case_sensitive)) {
                    return true;
                }
            }
            return false;
        } else if (case_sensitive ? (*pattern != *str) : (tolower((unsigned char)*pattern) != tolower((unsigned char)*str))) {
            // Characters don't match
            return false;
        }

        // Characters match, move to next
        pattern++;
        str++;
    }

    // If we've reached the end of both strings, it's a match
    // If there's only * left in pattern, also a match
    while (*pattern == '*') pattern++;
    return *pattern == '\0' && *str == '\0';
}

// Helper function to check if string ends with pattern
static bool ends_with_pattern(const char *pattern, const char *str, bool case_sensitive) {
    size_t pattern_len = strlen(pattern);
    size_t str_len = strlen(str);

    // If pattern contains no wildcards, just compare the end of string
    if (strchr(pattern, '*') == NULL) {
        if (pattern_len > str_len) return false;
        return case_sensitive ? strcmp(str + (str_len - pattern_len), pattern) == 0
                              : strcasecmp(str + (str_len - pattern_len), pattern) == 0;
    }

    // For wildcard patterns, try a direct pattern match from each potential end position
    for (size_t i = str_len; i > 0; i--) {
        if (starts_with_pattern(pattern, str + (i - 1), case_sensitive)) {
            // Verify it reaches the end of the string
            return true;
        }
    }

    return false;
}



