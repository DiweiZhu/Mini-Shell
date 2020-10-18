CC = gcc

NAME = minishell
CFLAGS = -I$(IDIR)
IDIR = includes
SDIR = src
ODIR = src/obj

_DEPS = minishell.h
_OBJ = minishell.o

DEPS = $(patsubst %, $(IDIR)/%, $(_DEPS))
OBJ = $(patsubst %, $(ODIR)/%, $(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(NAME): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o

fclean: clean
	rm -f $(NAME)
