
# Declaration of variables
CXXFLAGS = -c -std=c++17 -Ofast -flto -fPIC -I . -Wall
LDFLAGS = -shared -Ofast -flto -lpcap
LIBRARY = libTinyCAP

ifdef OS
	STAEX = lib
	DYNEX = dll
	OBJEX = obj
else
	STAEX = a
	DYNEX = so
	OBJEX = o
endif

# File names
HEADERS = $(shell find . -name "*.h")
SOURCES = $(shell find . -name "*.cpp")
OBJECTS = $(SOURCES:.cpp=.$(OBJEX))

STATIC = $(LIBRARY).$(STAEX)
DYNAMIC = $(LIBRARY).$(DYNEX)

# Main target
all: $(STATIC) $(DYNAMIC)

# Targets
$(STATIC): $(OBJECTS)
	$(AR) rc $@ $^

$(DYNAMIC): $(OBJECTS)
	$(CXX) $^ $(LDFLAGS) -o $@

# Compile source files
%.$(OBJEX): %.cpp %.h
	$(CXX) $(CXXFLAGS) $< -o $@

# Format code
format:
	clang-format -style=file -i $(HEADERS) $(SOURCES)

# Clean generated files
clean:
	$(RM) $(STATIC) $(DYNAMIC) $(OBJECTS)
