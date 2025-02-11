.PHONY: all clean ensureDirs

# Disable implicit rules
.SUFFIXES:

CC = gcc
CFLAGS = -Wall -g
CPPFLAGS = -I$(HEADERSDIR)
LDFLAGS = -lpthread
VPATH = src/

# Directories
SRCDIR = src
HEADERSDIR = headers
OBJDIR = obj
EXECDIR = .

# Files
SRCS = $(wildcard $(SRCDIR)/*.c)
HEADERS = $(wildcard $(HEADERSDIR)/*.h)
OBJS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))
MAIN_OBJ = $(OBJDIR)/shell.o
EXEC = $(MAIN_OBJ:$(OBJDIR)%.o=$(EXECDIR)/%)

# Compile and build all
all: ensureDirs $(EXEC)

# Ensure directories exist
ensureDirs:
	@mkdir -p $(OBJDIR) $(EXECDIR) $(HEADERSDIR)

# Compile source files into object files
$(OBJDIR)/%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# Link object files into a single executable
$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Clean up generated files
clean:
	-rm -f $(EXEC) $(OBJS)
	-rm -rf $(OBJDIR)/*.o 
	-rm -r $(OBJDIR)


