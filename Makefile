# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: gpatrici <gpatrici@student.42lisboa.com    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/10/20 16:01:23 by gpatrici          #+#    #+#              #
#    Updated: 2026/10/20 16:01:24 by gpatrici         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = codexion

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread
CPPFLAGS = -I./src

SRCS = src/codexion.c src/main_support.c src/parse.c src/parse_value.c \
	src/heap.c src/heap_support.c src/dongle.c src/dongle_acquire.c \
	src/dongle_wait.c src/sim.c src/sim_monitor.c src/coder.c \
	src/setup.c src/setup_prepare.c
OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

src/%.o: src/%.c src/codexion.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
