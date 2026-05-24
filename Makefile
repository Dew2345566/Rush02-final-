# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: cthongsa <cthongsa@student.42bangkok.co    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/05/23 13:36:02 by cthongsa          #+#    #+#              #
#    Updated: 2026/05/24 17:37:21 by cthongsa         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = rush-02
 
CC = cc
CFLAGS = -Wall -Wextra -Werror
 
SRCS = main.c utils.c str_utils.c dict.c dict_parse.c dict_load.c \
	convert.c bignum.c bonus.c bonus_syntax.c bonus_build.c convert_help.c
OBJS = $(SRCS:.c=.o)
 
all: $(NAME)
 
$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)
 
%.o: %.c rush02.h
	$(CC) $(CFLAGS) -c $< -o $@
 
clean:
	rm -f $(OBJS)
 
fclean: clean
	rm -f $(NAME)
 
re: fclean all
 
.PHONY: all clean fclean re
