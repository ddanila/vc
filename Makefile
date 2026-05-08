# End-to-end test runner for VC.COM builds.
#
# The kvikdos suite is a set of C test binaries that link against the
# kvikdos 8086 emulator (vendor/kvikdos), boot a VC.COM via the test
# harness, push keystrokes, and read back the text screen. Tests are
# selected per VC version: 4.99.09 ships without the editor, so the
# editor-specific tests are excluded from its group.
#
# QEMU e2e runs the same VC.COM under real DOS in qemu-system-i386 and
# checks a small set of screen assertions via QMP — same toolchain as
# build.sh.
#
# Usage:
#   make test-kvikdos-4.05         # full suite against build/4.05/VC.COM
#   make test-kvikdos-4.99.09      # editor tests excluded
#   make test-qemu-4.05            # smoke-test under real DOS
#   make test-qemu-4.99.09
#   make test-4.05 / test-4.99.09  # both suites, per version
#   make test                      # everything

.PHONY: build-tests clean \
        test test-4.05 test-4.99.09 \
        test-kvikdos test-kvikdos-4.05 test-kvikdos-4.99.09 \
        test-qemu-4.05 test-qemu-4.99.09

KVIKDOS_DIR = vendor/kvikdos

# Tests that work against original VC.COM for both 4.05 and 4.99.
TEST_GROUPS_BASE = \
    test_basic test_viewer test_viewer_hex \
    test_fileops_copy test_fileops_adv test_fileops_sel test_fileops_nav \
    test_menu test_menu_features test_menu_nav \
    test_dialogs test_event_loop test_event_keys \
    test_panel_adv test_panel_keys \
    test_coverage_deep test_coverage_gaps test_coverage_gaps2 \
    test_error_inject test_error_access \
    test_render_panel test_cmdline_edit test_select_filter \
    test_nav_edges test_recovery

# Editor (F4) is not present in 4.99.09; these only run against 4.05.
TEST_GROUPS_EDITOR = test_editor test_deep_editors test_gaps_editors

TEST_GROUPS_4_05    = $(TEST_GROUPS_BASE) $(TEST_GROUPS_EDITOR)
TEST_GROUPS_4_99_09 = $(TEST_GROUPS_BASE)

TEST_BINS_4_05    = $(addprefix tests/, $(TEST_GROUPS_4_05))
TEST_BINS_4_99_09 = $(addprefix tests/, $(TEST_GROUPS_4_99_09))

# All test binaries (union — same C files, build once)
TEST_BINS_ALL = $(addprefix tests/, $(sort $(TEST_GROUPS_4_05) $(TEST_GROUPS_4_99_09)))

KVIKDOS_CPU_DEPS = $(KVIKDOS_DIR)/cpu8086.c $(KVIKDOS_DIR)/cpu8086.h \
                   $(KVIKDOS_DIR)/cpu8086_xt.h $(KVIKDOS_DIR)/mini_kvm.h \
                   $(KVIKDOS_DIR)/XTulator/XTulator/cpu/cpu.c
KVIKDOS_SRC_DEPS = $(KVIKDOS_DIR)/kvikdos.c $(KVIKDOS_DIR)/mini_kvm.h

CFLAGS     = -O2 -W -Wall -Wextra -Werror=implicit-function-declaration \
             -fno-strict-aliasing -Wno-overlength-strings
CPU_CFLAGS = -O2 -W -Wall -Wextra -Werror=implicit-function-declaration \
             -fno-strict-aliasing

# Shared objects (built once, reused by all test binaries).
test_harness.o: $(KVIKDOS_DIR)/test_harness.c $(KVIKDOS_DIR)/test_harness.h \
                $(KVIKDOS_SRC_DEPS) $(KVIKDOS_CPU_DEPS)
	$(CC) $(CFLAGS) -c -o $@ $(KVIKDOS_DIR)/test_harness.c

cpu8086.o: $(KVIKDOS_DIR)/cpu8086.c $(KVIKDOS_DIR)/cpu8086.h
	$(CC) $(CPU_CFLAGS) -c -o $@ $(KVIKDOS_DIR)/cpu8086.c

# Per-binary build rule.
tests/test_%: tests/test_%.c tests/test_common.h test_harness.o cpu8086.o
	$(CC) $(CFLAGS) -I$(KVIKDOS_DIR) -c -o tests/test_$*.o tests/test_$*.c
	$(CC) -o $@ tests/test_$*.o test_harness.o cpu8086.o -lpthread
	@rm -f tests/test_$*.o

build-tests: $(TEST_BINS_ALL)

# Parallelism for kvikdos test runs.
PARALLEL_JOBS ?= 8

# Pattern rule body for "run a list of test binaries against VC_BINARY".
# $(1) = friendly version label, $(2) = path to VC.COM, $(3) = test bins,
# $(4) = path to VC.OVL (optional, for versions that need an overlay).
define run_kvikdos
	@echo "--- kvikdos: $(1) ($(words $(3)) tests, max $(PARALLEL_JOBS) parallel) ---"
	@if [ ! -f "$(2)" ]; then \
	  echo "ERROR: $(2) not found — run ./build.sh $(1) first" >&2; exit 1; \
	fi
	@if [ -n "$(4)" ] && [ ! -f "$(4)" ]; then \
	  echo "ERROR: $(4) not found — run ./build.sh $(1) first" >&2; exit 1; \
	fi
	@echo $(3) | tr ' ' '\n' | \
	  VC_BINARY=$(2) VC_OVL_PATH="$(4)" \
	  xargs -P $(PARALLEL_JOBS) -I{} sh -c \
	    'VC_BINARY=$(2) VC_OVL_PATH="$(4)" ./{} || exit 1'
endef

test-kvikdos-4.05: $(TEST_BINS_4_05)
	$(call run_kvikdos,4.05,build/4.05/VC.COM,$(TEST_BINS_4_05),)

test-kvikdos-4.99.09: $(TEST_BINS_4_99_09)
	$(call run_kvikdos,4.99.09,build/4.99.09/VC.COM,$(TEST_BINS_4_99_09),build/4.99.09/VC.OVL)

test-kvikdos: test-kvikdos-4.05 test-kvikdos-4.99.09

# QEMU e2e: boots real DOS with the built VC.COM.
test-qemu-4.05:
	bash tests/test_volkov_e2e.sh 4.05

test-qemu-4.99.09:
	bash tests/test_volkov_e2e.sh 4.99.09

# Combined per-version targets and the catch-all.
test-4.05: test-kvikdos-4.05 test-qemu-4.05
test-4.99.09: test-kvikdos-4.99.09 test-qemu-4.99.09
test: test-4.05 test-4.99.09

clean:
	rm -f $(TEST_BINS_ALL) test_harness.o cpu8086.o
	rm -f tests/test_*.o
	rm -f coverage_*.bin coverage.bin
