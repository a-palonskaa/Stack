CC = g++
CFLAGS = -Wall -std=c++17 -Wall -Wextra -Weffc++ -Wc++14-compat -Wmissing-declarations \
		 -Wcast-align -Wcast-qual -Wchar-subscripts -Wconversion -Wctor-dtor-privacy           \
		 -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat=2           \
		 -Winline -Wnon-virtual-dtor -Woverloaded-virtual -Wpacked -Wpointer-arith             \
		 -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo                 \
		 -Wstrict-overflow=2 -Wsuggest-override -Wswitch-default -Wswitch-enum -Wundef         \
		 -Wunreachable-code -Wunused -Wvariadic-macros -Wno-literal-range 			           \
		 -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast 			       \
		 -Wno-varargs -Wstack-protector -Wsuggest-override -Wbounds-attributes-redundant       \
		 -Wlong-long -Wopenmp -fcheck-new -fsized-deallocation -fstack-protector 		       \
		 -fstrict-overflow -fno-omit-frame-pointer -Wlarger-than=8192 -Wstack-protector        \
		 -fPIE -Werror=vla

SOURCES_HASH = hash.cpp
SOURCES_STACK = main.cpp stack.cpp verify.cpp pop_push.cpp

BUILD_DIR = build
HASH_DIR = hash
STACK_DIR = stack

SRCS_STACK = $(addprefix $(STACK_DIR)/src/, $(SOURCES_STACK))
OBJECTS_STACK = $(addprefix $(BUILD_DIR)/, $(SRCS_STACK:%.cpp=%.o))
DEPS_STACK = $(OBJECTS_STACK:%.o=%.d)

SRCS_HASH = $(addprefix $(HASH_DIR)/, $(SOURCES_HASH))
OBJECTS_HASH = $(addprefix $(BUILD_DIR)/, $(SRCS_HASH:%.cpp=%.o))
DEPS_HASH = $(OBJECTS_HASH:%.o=%.d)

OBJECTS = $(OBJECTS_HASH) $(OBJECTS_STACK)
DEPS = $(DEPS_HASH) $(DEPS_STACK)
SOURCES = $(SRCS_HASH) $(SRCS_STACK)

EXECUTABLE = build/meow

CFLAGS += -I$(HASH_DIR) -I$(addprefix $(STACK_DIR)/, include)

GUARD_L0 =
GUARD_L1 = -D  CANARY_PROTECT
GUARD_L2 = -D  CANARY_PROTECT -D  HASH_PROTECT

.PHONY: all debug release

all: release

debug: CFLAGS += -O0 -D DEBUG $(GUARD_L2)
debug: $(EXECUTABLE)

release: CFLAGS += -O2 -DNDEBUG
release: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

$(OBJECTS): $(BUILD_DIR)/%.o:%.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MP -MMD -c $< -o $@

clean:
	rm -f $(OBJECTS) $(DEPS) $(EXECUTABLE)

NODEPS = clean

ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
-include $(DEPS)
endif
