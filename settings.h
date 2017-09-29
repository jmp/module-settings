#ifndef SETTINGS_H
#define SETTINGS_H

typedef struct Settings Settings;

/*
 * Create a new settings object.
 *
 * Returns a pointer to the allocated settings object, or NULL if out of memory.
 */
extern Settings *settings_create(void);

/*
 * Free the given settings object.
 *
 * This goes through all the key/value pairs in the settings
 * and frees them as well. 
 */
extern void settings_free(Settings *settings);

/*
 * Load settings from the given path.
 *
 * The text file in the given path will be opened, and any
 * settings in it will be loaded as new key/value pairs into
 * the given settings object. Existing keys and values will
 * be preserved, but may be overwritten if the key already
 * exists in the settings.
 *
 * The key/value pairs in the file should be in the following format:
 *
 *     key name = some value
 *     another key = another value
 *     ...
 *
 * Returns 1 on success, or 0 on failure (e.g. if the path does not exist).
 */
extern int settings_load(Settings *settings, const char *path);

/*
 * Save the given settings into the given path.
 *
 * The given path will be opened as a text file, and the key/value pairs
 * in the given settings object will be saved into it. There will be one
 * key/value pair per line, separated by an equals sign.
 *
 * Returns 1 on success, or 0 on failure.
 */
extern int settings_save(Settings *settings, const char *path);

/*
 * Get a string from the settings.
 *
 * Finds the value in the settings corresponding to the given key and returns it.
 * If the key does not exist, then the given default value is returned.
 */
extern const char *settings_get_string(Settings *settings, const char *key, const char *default_value);

/*
 * Get an integer from the settings.
 *
 * Finds the value in the settings corresponding to the given key and returns it.
 * If the key does not exist, then the given default value is returned.
 */
extern int settings_get_int(Settings *settings, const char *key, int default_value);

/*
 * Get a float from the settings.
 *
 * Finds the value in the settings corresponding to the given key and returns it.
 * If the key does not exist, then the given default value is returned.
 */
extern float settings_get_float(Settings *settings, const char *key, float default_value);

/*
 * Add a string value to the settings.
 *
 * If the given key already exists, it will be replaced.
 * Both the key and value must be non-NULL.
 * Returns 1 if the value was added successfully, 0 otherwise.
 */
extern int settings_set_string(Settings *settings, const char *key, const char *value);

/*
 * Add an integer value to the settings.
 *
 * If the given key already exists, it will be replaced.
 * The key must be non-NULL.
 * Returns 1 if the value was added successfully, 0 otherwise.
 */
extern int settings_set_int(Settings *settings, const char *key, int value);

/*
 * Add a float value to the settings.
 *
 * If the given key already exists, it will be replaced.
 * The key must be non-NULL.
 * Returns 1 if the value was added successfully, 0 otherwise.
 */
extern int settings_set_float(Settings *settings, const char *key, float value);

/*
 * Remove the key from the settings.
 *
 * Performs a linear search and removes the given key
 * (and the value associated with it), if it exists.
 * Returns 1 if the key was removed successfully, 0 otherwise.
 */
extern int settings_remove(Settings *settings, const char *key);

#endif /* SETTINGS_H */
