CC = g++
CFLAGS = -Wall -std=c++17 -fstack-protector -fcheck-new -fsized-deallocation -fstrict-overflow              \
		 -flto-odr-type-merging -fno-omit-frame-pointer -Wlarger-than=8192 -Wstack-usage=8192               \
		 -pie -fPIE -Werror=vla -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat             \
		 -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported        \
		 -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security  \
		 -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd              \
		 -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow               \
		 -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2                          \
		 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override      \
		 -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast      \
		 -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing               \
		  -Wno-old-style-cast -Wno-varargs -Wstack-protector

LDFLAGS =

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
debug: LDFLAGS += -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero       \
	-fsanitize=integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute \
	-fsanitize=shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr
debug: $(EXECUTABLE)

release: CFLAGS += -O2 -g -DNDEBUG
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
