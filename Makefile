UNAME_S := $(shell uname -s)

CC := clang++ -target x86_64-pc-linux-gnu

SRCDIR := src
BUILDDIR := build
TARGETDIR := bin

EXECUTABLE := nand2tetris
TARGET := $(TARGETDIR)/$(EXECUTABLE)

INSTALLBINDIR := /usr/local/bin

SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

# uncomment once we have some includes
# INCDIRS := $(shell find include/**/* -name '*.h' -exec dirname {} \; | sort | uniq)
# INCLIST := $(patsubst include/%,-I include/%,$(INCDIRS))
INCLIST :=
BUILDLIST := $(patsubst include/%,$(BUILDDIR)/%,$(INCDIRS))

CFLAGS := -c -std=c++17 -O2
INC := -I include $(INCLIST) -I /usr/local/include
LIB := -L /usr/local/lib

ifneq ($(UNAME_S),Linux)
	CFLAGS += -stdlib=libc++
endif

$(TARGET): $(OBJECTS)
ifdef TARGETDIR
	@mkdir -p $(TARGETDIR)
endif
	@echo "Linking..."
	@echo "  Linking $(TARGET)"; $(CC) $^ -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
ifdef BUILDLIST
	@mkdir -p $(BUILDLIST)
endif
	@echo "Compiling $<..."; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo "Cleaning $(TARGET)..."; $(RM) -r $(BUILDDIR) $(TARGET)

install:
	@echo "Installing $(EXECUTABLE)..."; cp $(TARGET) $(INSTALLBINDIR)

distclean:
	@echo "Removing $(EXECUTABLE)"; rm $(INSTALLBINDIR)/$(EXECUTABLE)

.PHONY: clean
