#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "settings.h"

/* Optionally, use the memory module */
#ifdef USE_MEMORY
	#include "memory.h"
#else
	#define memory_malloc malloc
	#define memory_free free
#endif

/*
 * Configurable limits that can be set on compile-time.
 */

/* Maximum length for keys, including the NULL-terminator */
#ifndef CONFIG_KEY_LENGTH
#define CONFIG_KEY_LENGTH 128
#endif

/* Maximum length for values, including the NULL-terminator */
#ifndef CONFIG_VALUE_LENGTH
#define CONFIG_VALUE_LENGTH 128
#endif

/* Buffer size for reading lines in files */
#ifndef CONFIG_LINE_LENGTH
#define CONFIG_LINE_LENGTH 1024
#endif

/* A single key-value pair */
struct Pair {
	char key[CONFIG_KEY_LENGTH];
	char value[CONFIG_VALUE_LENGTH];
	struct Pair *next;
	struct Pair *prev;
};

/* The main settings structure */
struct Settings {
	struct Pair *first;
	struct Pair *last;
};

/*
 * Remove extra whitespace from around the string.
 * Modifies the given string by removing all leading
 * and trailing whitespace characters.
 */
static void trim(char *str) {
	if (str != NULL) {
		size_t i;
		size_t begin = 0;
		size_t end = strlen(str) - 1;

		/* Shift begin to the first non-space */
		while (isspace((unsigned char) str[begin])) {
			++begin;
		}

		/* Shift end to the first non-space */
		while ((end >= begin) && isspace((unsigned char) str[end])) {
			--end;
		}

		/* Shift all characters back to the start of the string array */
		for (i = begin; i <= end; ++i) {
			str[i - begin] = str[i];
		}

		/* Make sure it's NULL-terminated */
		str[i - begin] = '\0';
	}
}

/*
 * Copy key and value from the source to destination.
 */
static void copy_pair(char *dst_key, char *dst_value, const char *src_key, const char *src_value) {
	/* Replace key and value */
	strncpy(dst_key, src_key, CONFIG_KEY_LENGTH);
	strncpy(dst_value, src_value, CONFIG_VALUE_LENGTH);
	/* Make sure they terminate at least at the very end */
	dst_key[CONFIG_KEY_LENGTH - 1] = '\0';
	dst_value[CONFIG_VALUE_LENGTH - 1] = '\0';
}

/*
 * Create a new key/value pair from the given key and value.
 * Returns a newly allocated pair, or NULL on failure.
 */
static struct Pair *create_pair(const char *key, const char *value) {
	struct Pair *pair = NULL;

	/* NULL keys and values are not accepted */
	if (key != NULL && value != NULL) {
		pair = memory_malloc(sizeof(struct Pair));
		if (pair) {
			copy_pair(pair->key, pair->value, key, value);
			/* By default this is the last pair */
			pair->next = NULL;
			pair->prev = NULL;
		}
	}

	return pair;
}

/*
 * Check if the given keys match.
 * Both keys should be non-NULL.
 * Returns 1 if the keys are the same, 0 otherwise.
 */
static int keys_match(const char *key1, const char *key2) {
	if (key1 == NULL || key2 == NULL) {
		return 0;
	}
	return strncmp(key1, key2, CONFIG_KEY_LENGTH) == 0;
}

/*
 * Find the key/value pair corresponding to the given key.
 * Performs a linear search on the linked list.
 * Returns the pair if it exists, NULL otherwise.
 */
static struct Pair *find_pair(Settings *settings, const char *key) {
	if (settings != NULL) {
		struct Pair *pair = settings->first;
		while (pair != NULL) {
			if (keys_match(pair->key, key)) {
				return pair;
			}
			pair = pair->next;
		}
	}
	return NULL;
}

Settings *settings_create(void) {
	Settings *settings = memory_malloc(sizeof(Settings));
	if (settings) {
		settings->first = NULL;
		settings->last = NULL;
	}
	return settings;
}

void settings_free(Settings *settings) {
	if (settings != NULL) {
		struct Pair *pair = settings->first;
		while (pair != NULL) {
			struct Pair *next = pair->next;
			memory_free(pair);
			pair = next;
		}
	}
}

int settings_load(Settings *settings, const char *path) {
	FILE *f;
	char line[CONFIG_LINE_LENGTH];

	/* Settings and path are required */
	if (settings == NULL || path == NULL) {
		return 0;
	}

	/* Attempt to open file as text */
	if (!(f = fopen(path, "rt"))) {
		return 0;
	}

	/* Successfully opened, so read any settings */
	while (fgets(line, CONFIG_LINE_LENGTH, f) != NULL) {
		/* Look for an equals sign */
		char *p = strchr(line, '=');
		if (p != NULL) {
			/*
			 * At worst, one part will contain the entire line
			 * minus the equals sign (if the equals sign happens
			 * to be at the very beginning or end of the line.
			 */
			char key[CONFIG_LINE_LENGTH];
			char val[CONFIG_LINE_LENGTH];
			/* Lengths of each part */
			const size_t key_len = p - line;
			const size_t val_len = strlen(p + 1);
			/* Split at the equals sign */
			strncpy(key, line, p - line);
			strncpy(val, p + 1, strlen(p + 1));
			/* Terminate the parts */
			key[key_len - 1] = '\0';
			val[val_len - 1] = '\0';
			/* Remove extraneous spaces */
			trim(key);
			trim(val);
			/* Add to settings */
			settings_set_string(settings, key, val);
		}
	}

	fclose(f);
	return 1;
}

int settings_save(Settings *settings, const char *path) {
	FILE *f;
	struct Pair *pair;

	/* Settings and path are mandatory */
	if (settings == NULL || path == NULL) {
		return 0;
	}

	/* Open for writing */
	if (!(f = fopen(path, "wt"))) {
		return 0;
	}

	/* Write one key and value per line */
	pair = settings->first;
	while (pair != NULL) {
		fprintf(f, "%s = %s\n", pair->key, pair->value);
		pair = pair->next;
	}

	fclose(f);
	return 1;
}

const char *settings_get_string(Settings *settings, const char *key, const char *default_value) {
	struct Pair *pair = find_pair(settings, key);
	if (pair != NULL) {
		return pair->value;
	}
	return default_value;
}

int settings_get_int(Settings *settings, const char *key, int default_value) {
	struct Pair *pair = find_pair(settings, key);
	if (pair != NULL) {
		return atoi(pair->value);
	}
	return default_value;
}

float settings_get_float(Settings *settings, const char *key, float default_value) {
	struct Pair *pair = find_pair(settings, key);
	if (pair != NULL) {
		return atof(pair->value);
	}
	return default_value;
}

int settings_set_string(Settings *settings, const char *key, const char *value) {
	int result = 0;
	struct Pair *pair;

	if (settings == NULL || key == NULL || value == NULL) {
		/* Settings, key and pair are mandatory */
		return 0;
	}

	if (strlen(key) >= CONFIG_KEY_LENGTH || strlen(value) >= CONFIG_VALUE_LENGTH) {
		/* Key or value is too long */
		return 0;
	}

	pair = find_pair(settings, key);
	if (pair != NULL) {
		copy_pair(pair->key, pair->value, key, value);
		result = 1;
	} else {
		/* We have to create a new pair */
		pair = create_pair(key, value);
		if (pair != NULL) {
			if (settings->first == NULL) {
				settings->first = pair;
			}
			if (settings->last == NULL) {
				settings->last = pair;
			} else {
				/* Append to last */
				settings->last->next = pair;
				pair->prev = settings->last;
				settings->last = pair;
			}
			result = 1;
		}
	}
	return result; /* Failed to set value */
}

int settings_set_int(Settings *settings, const char *key, int value) {
	char value_str[CONFIG_VALUE_LENGTH] = { '\0' };
	snprintf(value_str, CONFIG_VALUE_LENGTH, "%d", value);
	return settings_set_string(settings, key, value_str);
}

int settings_set_float(Settings *settings, const char *key, float value) {
	char value_str[CONFIG_VALUE_LENGTH] = { '\0' };
	snprintf(value_str, CONFIG_VALUE_LENGTH, "%f", value);
	return settings_set_string(settings, key, value_str);
}

int settings_remove(Settings *settings, const char *key) {
	struct Pair *pair = find_pair(settings, key);

	if (pair != NULL) {
		/* Update prev of the next pair */
		if (pair->next != NULL) {
			pair->next->prev = pair->prev;
		} else {
			/* No next pair, so this must be the last */
			settings->last = pair->prev;
		}
		/* Update next of the prev pair */
		if (pair->prev != NULL) {
			pair->prev->next = pair->next;
		} else {
			/* No previous pair, so this must be the first one */
			settings->first = pair->next;
		}
		memory_free(pair);
		return 1;
	}

	return 0;
}
