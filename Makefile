PROGRAM = traceroute
SOURCES = main.c icmp.c report.c udp.c
BUILDDIR = build
OBJECTS = $(SOURCES:%.c=$(BUILDDIR)/%.o)
DEPENDENCY-FILES = $(SOURCES:%.c=$(BUILDDIR)/%.d)
CFLAGS = -std=c17 -Wall -Wextra
CPPFLAGS = -D_GNU_SOURCE


$(PROGRAM): $(OBJECTS)
	@echo "[LD] $(OBJECTS) -> $@"
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $(OBJECTS)

%.o: %.d

ifeq ($(words $(findstring $(MAKECMDGOALS), clean distclean)), 0)
include $(DEPENDENCY-FILES)
endif

$(BUILDDIR)/%.o: %.c
	@echo "[CC] $< -> $@"
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

$(BUILDDIR)/%.d: %.c
	@echo "[DEP] $< -> $@"
	mkdir -p $(dir $@)
	$(CC) -MT $(dir $@)$*.o -MM $< -MF $@

clean:
	rm -rf build

distclean: clean
	rm -f $(PROGRAM)

.PHONY: clean distclean
.SILENT:
