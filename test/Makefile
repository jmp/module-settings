CC=gcc
TARGET=test
CFLAGS=-I.. -std=c99 -pedantic -Wall -Werror -Wextra \
	-DWRAP_MALLOC -Wl,--wrap,malloc \
	-DWRAP_REALLOC -Wl,--wrap,realloc
SOURCES=test.c ../settings.c

ifdef ComSpec
	# Windows systems
	TARGET := $(TARGET).exe
	mkdir = mkdir $(subst /,\,$(1)) > nul 2>&1 || (exit 0)
	rm = $(wordlist 2,65535,$(foreach FILE,$(subst /,\,$(1)),& del $(FILE) > nul 2>&1)) || (exit 0)
	rmdir = rmdir /s /q $(subst /,\,$(1)) > nul 2>&1 || (exit 0)
else
	# Unix-like systems
	mkdir = mkdir -p $(1)
	rm = rm $(1) > /dev/null 2>&1 || true
	rmdir = rm -r $(1) > /dev/null 2>&1 || true
endif

# For generating coverage reports with gcov
ifdef COVERAGE
	CFLAGS := $(CFLAGS) -fprofile-arcs -ftest-coverage
endif

$(TARGET):
	@$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

clean:
	@$(call rm,$(TARGET))
	@$(call rm,*.gc*)

.PHONY: clean
