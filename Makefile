CC = gcc

NAME = minishell
CFLAGS = -I$(IDIR) -g
IDIR = includes
SDIR = src
ODIR = src/obj

_DEPS = minishell.h
_SRC = minishell.c builtins.c builtin_exit.c builtin_cd.c
_OBJ = $(patsubst %.c, %.o, $(_SRC))

DEPS = $(patsubst %, $(IDIR)/%, $(_DEPS))
OBJ = $(patsubst %, $(ODIR)/%, $(_OBJ))

$(NAME): $(DEPS) $(OBJ)
	$(CC) -o $@ $(OBJ) $(CFLAGS)

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o

fclean: clean
	rm -f $(NAME)
