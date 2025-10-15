# ==== Root Makefile (mygame/Makefile) ====

APP         := ${NAME}
SRC_DIR     := src
INC_DIR     ?= include
BUILD_DIR   := build
BIN         := $(BUILD_DIR)/$(APP)
LIB         := $(BUILD_DIR)/lib$(APP).a

CFLAGS      ?= -std=c23 -O2 -Wall -Wextra -Wpedantic
LDLIBS      ?=
CPPFLAGS    ?=

PKGS := ${PKGS} # a comma separated list of pkg-config names
NIXMODE ?= ${NIXMODE} # can be nix-build, nix-shell, ''
MODE := ${MODE}   # use 'make MODE=lib' to build library

define _filter_pkgs
$(strip $(foreach p,$(1),$(if $(shell pkg-config --exists $(p) && echo 1),$(p),)))
endef
AVAILABLE_PKGS := $(call _filter_pkgs,$(PKGS))

CPPFLAGS += -I$(INC_DIR) $(shell pkg-config --cflags $(AVAILABLE_PKGS) 2>/dev/null)
LDLIBS   += $(shell pkg-config --libs $(AVAILABLE_PKGS) 2>/dev/null)

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

MODULE_DIRS := $(sort $(dir $(wildcard modules/*/)))
MODULE_INCLUDES = $(foreach M,$(MODULE_DIRS),-I$(M)/include)
ifeq ($(NIXMODE),nix-build)
  MODULE_LIBS = $(foreach M,$(MODULE_DIRS),$(wildcard $(M)/lib/*.a) $(wildcard $(M)/lib/*.so))
else
  MODULE_LIBS = $(foreach M,$(MODULE_DIRS),$(wildcard $(M)/build/*.a) $(wildcard $(M)/build/*.so))
endif

.PHONY: all run clean modules test format lib bin install uninstall

all: $(MODE)

modules:
	@for m in $(MODULE_DIRS); do \
	  if ! $(MAKE) -C $$m -q >/dev/null 2>&1; then \
	    echo "==> Building $$m"; \
	    if [ "$(NIXMODE)" = "nix-shell" ]; then \
	      echo "==> NIXMODE=nix-shell, building $$m"; \
	      nix-shell $$m/default.nix --argstr mode lib --run "make -C $$m"; \
	    elif [ "$(NIXMODE)" = "nix-build" ]; then \
	      echo "==> NIXMODE=nix-build, skipping $$m"; \
	    else \
	      echo "==> NIXMODE=none, building $$m"; \
	      $(MAKE) -C $$m; \
	    fi; \
	  else \
	    echo "==> $$m up to date"; \
	  fi; \
	done

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(MODULE_INCLUDES) -c $< -o $@

bin: modules $(OBJS)
	$(CC) $(OBJS) $(MODULE_LIBS) $(LDLIBS) -o $(BIN)
	@echo "✅ Built executable: $(BIN)"

lib: modules $(OBJS)
	ar rcs $(LIB) $(OBJS)
	@echo "✅ Built static library: $(LIB)"

install: $(MODE)
ifeq ($(MODE),lib)
	install -Dm644 $(LIB) $(DESTDIR)$(PREFIX)/lib/$(notdir $(LIB))
	install -d $(DESTDIR)$(PREFIX)/include
	cp -r $(INC_DIR)/* $(DESTDIR)$(PREFIX)/include/
	@echo "📦 Installed library to $(PREFIX)/lib and headers to $(PREFIX)/include/"
else
	install -Dm755 $(BIN) $(DESTDIR)$(PREFIX)/bin
	@echo "📦 Installed binary to $(PREFIX)/bin"
endif

uninstall:
ifeq ($(MODE),lib)
	rm -f $(DESTDIR)$(PREFIX)/lib/$(notdir $(LIB))
	rm -rf $(DESTDIR)$(PREFIX)/include
else
	rm -f $(DESTDIR)$(PREFIX)/bin
endif
	@echo "🧹 Uninstalled $(MODE) files"

run: bin
	./$(BIN)

test:
	@for m in $(MODULE_DIRS); do \
	  if [ -f $$m/Makefile ] && grep -q "^test:" $$m/Makefile; then \
	    echo "==> Testing $$m"; \
	    if [ "$(NIXMODE)" = "shell" ]; then \
	      nix-shell $$m/default.nix --run "$(MAKE) -C $$m test"; \
	    elif [ "$(NIXMODE)" = "nix-build" ]; then \
	      echo "==> NIXMODE=nix-build, skipping tests for $$m"; \
	    else \
	      $(MAKE) -C $$m test; \
	    fi; \
	  fi; \
	done

clean:
	rm -rf $(BUILD_DIR)
	@for m in $(MODULE_DIRS); do \
	  if [ "$(NIXMODE)" = "shell" ]; then \
	    nix-shell $$m/default.nix --run "$(MAKE) -C $$m clean || true"; \
	  elif [ "$(NIXMODE)" = "nix-build" ]; then \
	    echo "==> NIXMODE=nix-build, skipping clean for $$m"; \
	  else \
	    $(MAKE) -C $$m clean || true; \
	  fi; \
	done
