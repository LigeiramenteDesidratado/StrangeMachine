CC=clang
CV = #-std=c11

SRCS=$(shell find . -type f -name '*.c')

FLAGS = $(CV) -Wall -Wextra -pedantic -g3 -o3 -flto -pipe \
		-fvisibility=hidden -fcf-protection=full \
		-Werror=format-security -Wconversion \
		-Werror=implicit-function-declaration -Wbad-function-cast

MACROS = -DLOG_USE_COLOR #-DNDEBUG

LIBS = -lm -lSDL2

OBJDIR = .build
OBJS := $(SRCS:%.c=$(OBJDIR)/%.o)

DEPDIR = .deps
DEPFILES := $(SRCS:%.c=$(DEPDIR)/%.d)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

$(shell mkdir -p $(dir $(OBJS)))
$(shell mkdir -p $(dir $(DEPFILES)))

INCLUDE= -I ./engine/include/ -I ./engine/ -I ./engine/src/ -I ./examples/terror-em-sl/

TARGET = main

$(OBJDIR)/%.o: %.c | $(OBJDIR) $(DEPDIR)
	@echo -e "[CC] $@"
	@$(CC) $(DEPFLAGS) $(INCLUDE) $(FLAGS) $(MACROS) -c -o $@ $<

$(TARGET): $(OBJS)
	@echo [LD] $@
	@$(CC) $^ $(FLAGS) $(LIBS) -o $@

.PHONY: clean
clean:
	@rm -rf $(OBJDIR) $(DEPDIR) $(TARGET)

$(DEPFILES):

include $(wildcard $(DEPFILES))
