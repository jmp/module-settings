#include <string.h>
#include "test.h"
#include "settings.h"

/*
 * Creation tests
 */

static int test_settings_create(void) {
	Settings *settings = settings_create();
	test_assert(settings != NULL);
	settings_free(settings);

	return TEST_PASS;
}

static int test_settings_create_no_memory(void) {
	test_malloc_disable();
	test_assert(settings_create() == NULL);

	return TEST_PASS;
}

/*
 * String tests
 */

static int test_settings_string_add(void) {
	Settings *settings = settings_create();
	test_assert(settings_set_string(settings, "foo", "abc"));
	test_assert(settings_set_string(settings, "bar", "def"));
	test_assert(settings_set_string(settings, "baz", "ghi"));
	test_assert(strncmp("abc", settings_get_string(settings, "foo", "ERROR"), 64) == 0);
	test_assert(strncmp("def", settings_get_string(settings, "bar", "ERROR"), 64) == 0);
	test_assert(strncmp("ghi", settings_get_string(settings, "baz", "ERROR"), 64) == 0);
	settings_free(settings);

	return TEST_PASS;
}

static int test_settings_string_add_no_memory(void) {
	Settings *settings = settings_create();
	test_assert(settings != NULL);
	test_malloc_disable();
	test_assert(!settings_set_string(settings, "foo", "abc"));
	settings_free(settings);

	return TEST_PASS;
}

static int test_settings_string_replace(void) {
	Settings *settings = settings_create();
	test_assert(settings_set_string(settings, "foo", "abc"));
	test_assert(strncmp("abc", settings_get_string(settings, "foo", "ERROR"), 64) == 0);
	test_assert(settings_set_string(settings, "foo", "def"));
	test_assert(strncmp("def", settings_get_string(settings, "foo", "ERROR"), 64) == 0);
	settings_free(settings);

	return TEST_PASS;
}

static int test_settings_string_null_key(void) {
	Settings *settings = settings_create();
	test_assert(!settings_set_string(settings, NULL, "abc"));
	settings_free(settings);

	return TEST_PASS;
}

static int test_settings_string_null_value(void) {
	Settings *settings = settings_create();
	test_assert(!settings_set_string(settings, "foo", NULL));
	settings_free(settings);

	return TEST_PASS;
}

static int test_settings_string_null_settings(void) {
	test_assert(!settings_set_string(NULL, "foo", "123"));

	return TEST_PASS;
}

static int test_settings_string_empty_key(void) {
	Settings *settings = settings_create();
	test_assert(settings_set_string(settings, "", "abc"));
	test_assert(strncmp("abc", settings_get_string(settings, "", "ERROR"), 64) == 0);
	settings_free(settings);

	return TEST_PASS;
}

static int test_settings_string_empty_value(void) {
	Settings *settings = settings_create();
	test_assert(settings_set_string(settings, "foo", ""));
	test_assert(strncmp("", settings_get_string(settings, "foo", "ERROR"), 64) == 0);
	settings_free(settings);

	return TEST_PASS;
}

static int test_settings_string_too_long_key(void) {
	Settings *settings = settings_create();
	char too_long_key[500 + 1];
	memset(too_long_key, 'X', 500);
	too_long_key[500] = '\0';
	test_assert(strlen(too_long_key) == 500);
	test_assert(!settings_set_string(settings, too_long_key, "abc"));
	settings_free(settings);

	return TEST_PASS;
}

static int test_settings_string_too_long_value(void) {
	Settings *settings = settings_create();
	char too_long_value[500 + 1] = { 'X' };
	memset(too_long_value, 'X', 500);
	too_long_value[500] = '\0';
	test_assert(strlen(too_long_value) == 500);
	test_assert(!settings_set_string(settings, "foo", too_long_value));
	settings_free(settings);

	return TEST_PASS;
}

static int test_settings_string_exists(void) {
	Settings *settings = settings_create();
	test_assert(settings_set_string(settings, "foo", "abc"));
	test_assert(strncmp("abc", settings_get_string(settings, "foo", "ERROR"), 64) == 0);
	settings_free(settings);

	return TEST_PASS;
}

static int test_settings_string_missing(void) {
	Settings *settings = settings_create();
	test_assert(strncmp("abc", settings_get_string(settings, "foo", "abc"), 64) == 0);
	settings_free(settings);

	return TEST_PASS;
}

static int test_settings_string_missing_null(void) {
	Settings *settings = settings_create();
	test_assert(settings_set_string(settings, "foo", "abc"));
	test_assert(strncmp("ERROR", settings_get_string(settings, NULL, "ERROR"), 64) == 0);
	settings_free(settings);

	return TEST_PASS;
}

/*
 * Integer tests
 */

static int test_settings_int_add(void) {
	Settings *settings = settings_create();
	test_assert(settings_set_int(settings, "foo", 1264));
	test_assert(settings_set_int(settings, "bar", 456));
	test_assert(settings_set_int(settings, "baz", 789));
	test_assert(settings_get_int(settings, "foo", 9999) == 1264);
	test_assert(settings_get_int(settings, "bar", 9999) == 456);
	test_assert(settings_get_int(settings, "baz", 9999) == 789);
	settings_free(settings);

	return TEST_PASS;
}

static int test_settings_int_negative(void) {
	Settings *settings = settings_create();
	test_assert(settings_set_int(settings, "foo", -1264));
	test_assert(settings_set_int(settings, "bar", -456));
	test_assert(settings_set_int(settings, "baz", -789));
	test_assert(settings_get_int(settings, "foo", 9999) == -1264);
	test_assert(settings_get_int(settings, "bar", 9999) == -456);
	test_assert(settings_get_int(settings, "baz", 9999) == -789);
	settings_free(settings);

	return TEST_PASS;
}

static int test_settings_int_replace(void) {
	Settings *settings = settings_create();
	test_assert(settings_set_int(settings, "foo", 1264));
	test_assert(settings_get_int(settings, "foo", 9999) == 1264);
	test_assert(settings_set_int(settings, "foo", 456));
	test_assert(settings_get_int(settings, "foo", 9999) == 456);
	settings_free(settings);

	return TEST_PASS;
}

static int test_settings_int_null_key(void) {
	Settings *settings = settings_create();
	test_assert(!settings_set_int(settings, NULL, 1234));
	settings_free(settings);

	return TEST_PASS;
}

static int test_settings_int_empty_key(void) {
	Settings *settings = settings_create();
	test_assert(settings_set_int(settings, "", 1264));
	test_assert(settings_get_int(settings, "", 9999) == 1264);
	settings_free(settings);

	return TEST_PASS;
}

static int test_settings_int_exists(void) {
	Settings *settings = settings_create();
	test_assert(settings_set_int(settings, "foo", 1264));
	test_assert(settings_get_int(settings, "foo", 9999) == 1264);
	settings_free(settings);

	return TEST_PASS;
}

static int test_settings_int_missing(void) {
	Settings *settings = settings_create();
	test_assert(settings_get_int(settings, "foo", 1264) == 1264);
	settings_free(settings);

	return TEST_PASS;
}

/*
 * Float tests
 */

static int test_settings_float_add(void) {
	Settings *settings = settings_create();
	test_assert(settings_set_float(settings, "foo", 123.1f));
	test_assert(settings_set_float(settings, "bar", 456.2f));
	test_assert(settings_set_float(settings, "baz", 789.3f));
	test_assert(settings_get_float(settings, "foo", 9999.0f) == 123.1f);
	test_assert(settings_get_float(settings, "bar", 9999.0f) == 456.2f);
	test_assert(settings_get_float(settings, "baz", 9999.0f) == 789.3f);
	settings_free(settings);

	return TEST_PASS;
}

static int test_settings_float_negative(void) {
	Settings *settings = settings_create();
	test_assert(settings_set_float(settings, "foo", -123.1f));
	test_assert(settings_set_float(settings, "bar", -456.2f));
	test_assert(settings_set_float(settings, "baz", -789.3f));
	test_assert(settings_get_float(settings, "foo", 9999.0f) == -123.1f);
	test_assert(settings_get_float(settings, "bar", 9999.0f) == -456.2f);
	test_assert(settings_get_float(settings, "baz", 9999.0f) == -789.3f);
	settings_free(settings);

	return TEST_PASS;
}

static int test_settings_float_replace(void) {
	Settings *settings = settings_create();
	test_assert(settings_set_float(settings, "foo", 123.1f));
	test_assert(settings_get_float(settings, "foo", 9999.0f) == 123.1f);
	test_assert(settings_set_float(settings, "foo", 456.2f));
	test_assert(settings_get_float(settings, "foo", 9999.0f) == 456.2f);
	settings_free(settings);

	return TEST_PASS;
}

static int test_settings_float_null_key(void) {
	Settings *settings = settings_create();
	test_assert(!settings_set_float(settings, NULL, 123.1f));
	settings_free(settings);

	return TEST_PASS;
}

static int test_settings_float_empty_key(void) {
	Settings *settings = settings_create();
	test_assert(settings_set_float(settings, "", 123.1f));
	test_assert(settings_get_float(settings, "", 9999.0f) == 123.1f);
	settings_free(settings);

	return TEST_PASS;
}

static int test_settings_float_exists(void) {
	Settings *settings = settings_create();
	test_assert(settings_set_float(settings, "foo", 123.1f));
	test_assert(settings_get_float(settings, "foo", 9999.0f) == 123.1f);
	settings_free(settings);

	return TEST_PASS;
}

static int test_settings_float_missing(void) {
	Settings *settings = settings_create();
	test_assert(settings_get_float(settings, "foo", 123.1f) == 123.1f);
	settings_free(settings);

	return TEST_PASS;
}

/*
 * Test loading from file
 */

static int test_settings_load(void) {
	Settings *settings = settings_create();
	char config_path[L_tmpnam];
	FILE *f;

	test_assert(tmpnam(config_path) != NULL);
	test_assert(config_path != NULL);
	f = fopen(config_path, "wt");
	test_assert(f != NULL);
	test_assert(fprintf(f, "foo  bar  = abc def =   ghi   \n") > 0);
	test_assert(fprintf(f, "  bar =   54321 \n") > 0);
	test_assert(fprintf(f, "baz =  123.1\n") > 0);
	test_assert(fclose(f) == 0);

	test_assert(settings_load(settings, config_path));
	test_assert(strncmp("abc def =   ghi", settings_get_string(settings, "foo  bar", "ERROR"), 64) == 0);
	test_assert(settings_get_int(settings, "bar", 9999) == 54321);
	test_assert(settings_get_float(settings, "baz", 9999.0f) == 123.1f);
	settings_free(settings);
	test_assert(remove(config_path) == 0);

	return TEST_PASS;
}

static int test_settings_load_missing_file(void) {
	FILE *f = fopen("missing_file.txt", "rt");
	Settings *settings = settings_create();
	test_assert(f == NULL);
	test_assert(!settings_load(settings, "missing_file.txt"));
	settings_free(settings);

	return TEST_PASS;
}

static int test_settings_load_null_settings(void) {
	test_assert(!settings_load(NULL, "test_config_load.txt"));

	return TEST_PASS;
}

static int test_settings_load_null_path(void) {
	Settings *settings = settings_create();

	test_assert(!settings_load(settings, NULL));
	settings_free(settings);

	return TEST_PASS;
}


/*
 * Test saving to file
 */

static int test_settings_save(void) {
	Settings *settings = settings_create();
	char config_path[L_tmpnam];
	char *expected_contents = "foo = abc def ghi\nbar = 54321\nbaz = 123.1\n";
	char buf[1000] = {'\0'};
	FILE *f;

	test_assert(tmpnam(config_path) != NULL);
	settings_set_string(settings, "foo", "abc def ghi");
	settings_set_string(settings, "bar", "54321");
	settings_set_string(settings, "baz", "123.1");
	test_assert(settings_save(settings, config_path));
	settings_free(settings);

	/* Check file */
	test_assert((f = fopen(config_path, "rt")) != NULL);
	test_assert(fread(buf, 1, 1000, f) == strlen(expected_contents));
	test_assert(strncmp(buf, expected_contents, 1000) == 0);
	test_assert(fclose(f) == 0);
	test_assert(remove(config_path) == 0);

	return TEST_PASS;
}

static int test_settings_save_empty(void) {
	Settings *settings = settings_create();
	char config_path[L_tmpnam];
	char buf[1000];
	FILE *f;

	test_assert(tmpnam(config_path) != NULL);
	test_assert(settings_save(settings, config_path));
	settings_free(settings);

	/* Check file */
	test_assert((f = fopen(config_path, "rt")) != NULL);
	test_assert(fread(buf, 1, 1000, f) == 0);
	test_assert(fclose(f) == 0);
	test_assert(remove(config_path) == 0);

	return TEST_PASS;
}

static int test_settings_save_file_fails_to_open(void) {
	Settings *settings = settings_create();
	test_assert(!settings_save(settings, "<>:?*|\"/ ")); /* Make fopen fail */
	settings_free(settings);

	return TEST_PASS;
}

static int test_settings_save_null_settings(void) {
	test_assert(!settings_save(NULL, "test_config_save.txt"));

	return TEST_PASS;
}

static int test_settings_save_null_path(void) {
	Settings *settings = settings_create();
	settings_set_string(settings, "foo", "abc def ghi");
	settings_set_string(settings, "bar", "54321");
	settings_set_string(settings, "bar", "123.1");
	test_assert(!settings_save(settings, NULL));
	settings_free(settings);

	return TEST_PASS;
}

/*
 * Remove tests
 */

static int test_settings_remove(void) {
	Settings *settings = settings_create();
	test_assert(settings_set_int(settings, "foo", 123));
	test_assert(settings_get_int(settings, "foo", 99999) == 123);
	test_assert(settings_remove(settings, "foo"));
	test_assert(settings_get_int(settings, "foo", 99999) == 99999);
	settings_free(settings);
	return TEST_PASS;
}

static int test_settings_remove_null_settings(void) {
	test_assert(!settings_remove(NULL, "something"));
	return TEST_PASS;
}

static int test_settings_remove_null_key(void) {
	Settings *settings = settings_create();
	test_assert(settings_set_string(settings, "foo", "bar"));
	test_assert(!settings_remove(settings, NULL));
	settings_free(settings);
	return TEST_PASS;
}

static int test_settings_remove_missing_key(void) {
	Settings *settings = settings_create();
	test_assert(settings_set_string(settings, "foo", "bar"));
	test_assert(!settings_remove(settings, "something"));
	settings_free(settings);
	return TEST_PASS;
}

int main(void) {
	setbuf(stdout, NULL);

	test_run(test_settings_create);
	test_run(test_settings_create_no_memory);

	test_run(test_settings_string_add);
	test_run(test_settings_string_add_no_memory);
	test_run(test_settings_string_replace);
	test_run(test_settings_string_null_key);
	test_run(test_settings_string_null_value);
	test_run(test_settings_string_null_settings);
	test_run(test_settings_string_empty_key);
	test_run(test_settings_string_empty_value);
	test_run(test_settings_string_exists);
	test_run(test_settings_string_missing);
	test_run(test_settings_string_missing_null);
	
	test_run(test_settings_int_add);
	test_run(test_settings_int_negative);
	test_run(test_settings_int_replace);
	test_run(test_settings_int_null_key);
	test_run(test_settings_int_empty_key);
	test_run(test_settings_int_exists);
	test_run(test_settings_int_missing);

	test_run(test_settings_float_add);
	test_run(test_settings_float_negative);
	test_run(test_settings_float_replace);
	test_run(test_settings_float_null_key);
	test_run(test_settings_float_empty_key);
	test_run(test_settings_float_exists);
	test_run(test_settings_float_missing);

	test_run(test_settings_load);
	test_run(test_settings_load_missing_file);
	test_run(test_settings_load_null_settings);
	test_run(test_settings_load_null_path);

	test_run(test_settings_save);
	test_run(test_settings_save_empty);
	test_run(test_settings_save_file_fails_to_open);
	test_run(test_settings_save_null_settings);
	test_run(test_settings_save_null_path);

	test_run(test_settings_remove);
	test_run(test_settings_remove_null_settings);
	test_run(test_settings_remove_null_key);
	test_run(test_settings_remove_missing_key);

	test_print_stats();

	return test_get_fail_count();
}