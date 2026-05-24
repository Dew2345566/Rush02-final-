/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rush02.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cthongsa <cthongsa@student.42bangkok.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/24 12:38:52 by cthongsa          #+#    #+#             */
/*   Updated: 2026/05/24 17:38:31 by cthongsa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RUSH02_H
# define RUSH02_H

# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>

# define DICT_FILE "numbers.dict"
# define MAX_ENTRIES 1024
# define BUF_SIZE 65536
# define MAX_DIGITS 64

typedef unsigned long long	t_ull;

typedef struct s_entry
{
	char	*key;
	char	*value;
}	t_entry;

typedef struct s_dict
{
	t_entry	entries[MAX_ENTRIES];
	int		size;
}	t_dict;

int		ft_strlen(char *s);
char	*ft_strdup(char *s);
char	*ft_strdup_n(char *s, int n);
char	*ft_strjoin(char *a, char *b);
char	*ft_strjoin_sp(char *a, char *b);
void	ft_putstr(char *s);
char	*ft_trim(char *s);
int		ft_isdigit(char c);
int		ft_str_isnum(char *s);
int		ft_strcmp(char *a, char *b);

char	*strip_zeros(char *s);
int		bn_is_zero(char *n);
int		bn_cmp(char *a, char *b);
char	*bn_div(char *n, char *divisor, char **remainder);

void	dict_init(t_dict *d);
int		dict_add(t_dict *d, char *key, char *val);
char	*dict_value_get(t_dict *d, char *key);
char	*dict_floor(t_dict *d, char *n);
void	dict_free(t_dict *d);
char	*read_file_buf(char *filename);
int		process_lines(char *buf, t_dict *d);
int		dict_load(char *filename, t_dict *d);

char	*large_remainder(char *result, char *rem, t_dict *d);
char	*convert(char *n, t_dict *d);
int		is_scale(char *w);
int		is_tens_word(char *w);
int		is_ones_word(char *w);
char	*get_sep(char *prev, char *curr);
char	*proper_syntax(char *raw);
int		run_stdin(char *dict_file);
int		error_exit(char *msg);

#endif
