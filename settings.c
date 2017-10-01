#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include "settings.h"

/* Optionally, use the memory module */
#ifdef USE_MEMORY
	#include "memory.h"
#else
	#define memory_malloc malloc
	#define memory_realloc realloc
	#define memory_free free
#endif

/* Utility macros for calculating the number of digits for integers */
#define STRINGIFY(x) #x
#define NUM_DIGITS(x) (sizeof(STRINGIFY(x)) - 1)

/* Maximum number of digits for ints and doubles */
#define INT_DIGITS (NUM_DIGITS(INT_MAX) + 1)
#define DBL_DIGITS (3 + DBL_MANT_DIG - DBL_MIN_EXP)

/* A single key-value pair */
struct Pair {
	char *key;
	char *value;
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

		/* Make sure it's NUL-terminated */
		str[i - begin] = '\0';
	}
}

/*
 * (Re)allocates space for the given array.
 */
static char *resize(char *array, size_t size) {
	size += 1; /* Leave room for NUL */

	if (array == NULL || strlen(array) <= size) {
		array = memory_realloc(array, size);
		if (array) {
			/* Make sure it terminates */
			array[size - 1] = '\0';
		}
	}

	return array;
}

/*
 * Copy key and value from the source to destination.
 */
static int copy_pair(char *dst_key, char *dst_val, const char *src_key, const char *src_val) {
	size_t key_len;
	size_t val_len;

	/* Cannot insert NULL */
	if (dst_key == NULL || dst_val == NULL) {
		return 0;
	}

	/* Leave room for NUL */
	key_len = strlen(src_key) + 1;
	val_len = strlen(src_val) + 1;

	/* Replace key and value */
	strncpy(dst_key, src_key, key_len);
	strncpy(dst_val, src_val, val_len);

	/* Make sure they terminate at least at the very end */
	dst_key[key_len - 1] = '\0';
	dst_val[val_len - 1] = '\0';

	return 1;
}

/*
 * Free the memory allocated for the given pair.
 */
static void free_pair(struct Pair *pair) {
	if (pair != NULL) {
		memory_free(pair->key);
		pair->key = NULL;
		memory_free(pair->value);
		pair->value = NULL;
		memory_free(pair);
	}
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
			/* By default this is the last pair with empty key and value */
			pair->key = resize(NULL, strlen(key));
			pair->value = resize(NULL, strlen(value));
			pair->next = NULL;
			pair->prev = NULL;
			/* Fill pair with the new values */
			if (!copy_pair(pair->key, pair->value, key, value)) {
				free_pair(pair);
				pair = NULL;
			}
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
	return strcmp(key1, key2) == 0;
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

/*
 * Read a line from the given file.
 */
char *get_line(FILE *f) {
	size_t size = BUFSIZ;
	size_t len;
	char *buf = memory_malloc(size);

	if (buf == NULL || fgets(buf, size, f) == NULL) {
		return NULL;
	}

	len = strlen(buf);
	while (strchr(buf, '\n') == NULL) {
		size += BUFSIZ;
		buf = memory_realloc(buf, size);
		if (fgets(buf + len, size - len, f) == NULL) {
			return buf;
		}
		len += strlen(buf + len);
	}

	if (buf) {
		buf[size - 1] = '\0';
	}

	return buf;
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
			free_pair(pair);
			pair = next;
		}
	}
}

int settings_load(Settings *settings, const char *path) {
	FILE *f;
	char *line;

	/* Settings and path are required */
	if (settings == NULL || path == NULL) {
		return 0;
	}

	/* Attempt to open file as text */
	if (!(f = fopen(path, "rt"))) {
		return 0;
	}

	/* Successfully opened, so read any settings */
	while ((line = get_line(f)) != NULL) {
		/* Look for an equals sign */
		char *p = strchr(line, '=');
		if (p != NULL) {
			/* Lengths of each part */
			const size_t key_len = p - line;      /* [####=....] */
			const size_t val_len = strlen(p + 1); /* [....=####] */
			/* Buffers of appropriate length*/
			char key[key_len];
			char val[val_len];
			/* Split at the equals sign */
			strncpy(key, line, key_len);
			strncpy(val, p + 1, val_len);
			/* Terminate the parts */
			key[key_len - 1] = '\0';
			val[val_len - 1] = '\0';
			/* Remove extraneous spaces */
			trim(key);
			trim(val);
			/* Add to settings */
			settings_set_string(settings, key, val);
		}
		memory_free(line);
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
		/* Settings, key, and value are mandatory */
		return 0;
	}

	pair = find_pair(settings, key);
	if (pair != NULL) {
		pair->key = resize(pair->key, strlen(key));
		pair->value = resize(pair->value, strlen(value));
		result = copy_pair(pair->key, pair->value, key, value);
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
	/* Convert int to string */
	char value_str[INT_DIGITS + 1] = { '\0' };
	snprintf(value_str, INT_DIGITS, "%d", value);

	/* Save the string */
	return settings_set_string(settings, key, value_str);
}

int settings_set_float(Settings *settings, const char *key, float value) {
	/* Convert float to string */
	char value_str[DBL_DIGITS + 1] = { '\0' };
	snprintf(value_str, DBL_DIGITS, "%f", value);

	/* Save the string */
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
