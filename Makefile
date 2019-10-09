UNAME_S := $(shell uname -s)

CC := clang++ -target x86_64-pc-linux-gnu

SRCDIR := src
BUILDDIR := build
TARGETDIR := bin

EXECUTABLE := nand
TARGET := $(TARGETDIR)/$(EXECUTABLE)

INSTALLBINDIR := /usr/local/bin

SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

# INCDIRS := $(shell find include/**/* -name '*.h' -exec dirname {} \; | sort | uniq)
# INCLIST := $(patsubst include/%,-I include/%,$(INCDIRS))
INCLIST := ""
BUILDLIST := $(patsubst include/%,$(BUILDDIR)/%,$(INCDIRS))

CFLAGS := -c -std=c++17 -O0 -Wall
INC := -I include $(INCLIST) -I /usr/local/include
LIB := -L /usr/local/lib

ifneq ($(UNAME_S),Linux)
	CFLAGS += -stdlib=libc++
endif

$(TARGET): $(OBJECTS)
	@mkdir -p $(TARGETDIR)
	@echo "Linking..."
	@echo "  Linking $(TARGET)";
	$(CC) $^ -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
ifdef BUILDLIST
	@mkdir -p $(BUILDLIST)
endif
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo "Cleaning $(TARGET)..."; $(RM) -r $(BUILDDIR) $(TARGET)

install:
	@echo "Installing $(EXECUTABLE)..."; cp $(TARGET) $(INSTALLBINDIR)

distclean:
	@echo "Removing $(EXECUTABLE)"; rm $(INSTALLBINDIR)/$(EXECUTABLE)

run: $(TARGET)
	@bin/nand

.PHONY: clean
