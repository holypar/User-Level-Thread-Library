# Target library
target := libuthread.a
objs:= context.o preempt.o uthread.o queue.o 
# Flags
CC  := gcc
CFLAGS := -Wall -Wextra -Werror -MMD
AR  := ar rcs 

ifneq ($(D),1)
CFLAGS	+= -O2
else
CFLAGS	+= -g
endif

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
	@echo "AR	$@"
	$(Q)$(AR) $@ $(objs)

%.o: %.c
	@echo "CC	$@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

clean: 
	@echo "clean"
	$(Q)rm -f $(target) $(objs) $(deps)

