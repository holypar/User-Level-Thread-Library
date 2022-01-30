# Target library
target := libuthread.a
objs:= prempt.o queue.o uthread.o 
# Flags
CC  := gcc
CFLAGS := -Wall -Wextra -Werror -MMD
CFLAGS += -g 

ifneq ($(V),1)
Q = @
V = 0
endif

# Include dependencies 
deps := $(patsubst %.o,%.d,$(objs))
-include $(deps)

# Compiling the program 
all: $(target)

$(target): $(objs) 
	@echo "CC	$@"
	$(Q)$(CC) -o $@ $< 

%.o: %.c
	@echo "CC	$@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

clean: 
	@echo "clean"
	$(Q)rm -f $(target) $(objs) $(deps)

