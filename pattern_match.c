#include "notifier.h"
#include <string.h>
#include <ctype.h>

// Forward declarations for helper functions
static bool match_string(const char *pattern, const char *str, bool case_sensitive);


bool pattern_match(const char *pattern, const char *str, bool case_sensitive) {
    // Always do an exact string comparison first
    bool exact_match = case_sensitive ?
        (strcmp(pattern, str) == 0) :
        (strcasecmp(pattern, str) == 0);

    if (exact_match) {
        return true;
    }

    // No anchors, so we can match the pattern anywhere in the string
    size_t str_len = strlen(str);
    for (size_t i = 0; i <= str_len; i++) {
        if (match_string(pattern, str + i, case_sensitive)) {
            return true;
        }
    }

    return false;
}


// Helper function to check if string starts with pattern
static bool match_string(const char *pattern, const char *str, bool case_sensitive) {
    while (*pattern && *str) {
        if (*pattern == '*') {
            // Wildcard: skip ahead in pattern and try to match the rest
            pattern++;

            // If * is at the end of pattern, it matches anything
            if (*pattern == '\0') return true;

            // Try to match the rest of the pattern at different positions
            for (const char *s = str; *s; s++) {
                if (match_string(pattern, s, case_sensitive)) {
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



