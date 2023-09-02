PROGRAM = traceroute
SOURCES = main.c icmp.c report.c udp.c common.c
SRCDIR = src
BUILDDIR = build
BINDIR = /usr/local/bin
OBJECTS = $(SOURCES:%.c=$(BUILDDIR)/%.o)
DEPENDENCY-FILES = $(SOURCES:%.c=$(BUILDDIR)/%.d)
CFLAGS = -std=c17 -Wall -Wextra
CPPFLAGS = -D_GNU_SOURCE


$(PROGRAM): $(OBJECTS)
	@echo "[LD] $(OBJECTS) -> $@"
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $(OBJECTS)

ifeq ($(words $(findstring $(MAKECMDGOALS), clean distclean)), 0)
include $(DEPENDENCY-FILES)
endif

%.o: %.d

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@echo "[CC] $< -> $@"
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

$(BUILDDIR)/%.d: $(SRCDIR)/%.c
	@echo "[DEP] $< -> $@"
	mkdir -p $(dir $@)
	$(CC) -MT $(dir $@)$*.o -MM $< -MF $@

install: $(PROGRAM)
	@echo "[INSTALL] $< -> $(BINDIR)"
	install -m 755 $(PROGRAM) $(BINDIR)

uninstall:
	@echo "Removed: $(BINDIR)/$(PROGRAM)"
	rm -f $(BINDIR)/$(PROGRAM)

clean:
	@echo "Removed: $(BUILDDIR)"
	rm -rf $(BUILDDIR)

distclean: clean
	@echo "Removed: $(PROGRAM)"
	rm -f $(PROGRAM)

.PHONY: clean distclean
.SILENT:
