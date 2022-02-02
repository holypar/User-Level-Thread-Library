# Target library
target := libuthread.a
objs:= queue.o 
# Flags
CC  := gcc
CFLAGS := -Wall -Wextra -Werror -MMD
CFLAGS += -g 
AR  := ar rcs 


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

