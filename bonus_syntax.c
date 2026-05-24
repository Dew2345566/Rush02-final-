/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bonus_syntax.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cthongsa <cthongsa@student.42bangkok.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/24 15:29:56 by cthongsa          #+#    #+#             */
/*   Updated: 2026/05/24 16:08:50 by cthongsa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rush02.h"

static int	word_in_list(char *w, char **list, int n)
{
	int	i;

	i = 0;
	while (i < n)
	{
		if (!ft_strcmp(w, list[i]))
			return (1);
		i++;
	}
	return (0);
}

int	is_scale(char *w)
{
	char	*s[12];

	s[0] = "thousand";
	s[1] = "million";
	s[2] = "billion";
	s[3] = "trillion";
	s[4] = "quadrillion";
	s[5] = "quintillion";
	s[6] = "sextillion";
	s[7] = "septillion";
	s[8] = "octillion";
	s[9] = "nonillion";
	s[10] = "decillion";
	s[11] = "undecillion";
	return (word_in_list(w, s, 12));
}

int	is_tens_word(char *w)
{
	char	*t[8];

	t[0] = "twenty";
	t[1] = "thirty";
	t[2] = "forty";
	t[3] = "fifty";
	t[4] = "sixty";
	t[5] = "seventy";
	t[6] = "eighty";
	t[7] = "ninety";
	return (word_in_list(w, t, 8));
}

int	is_ones_word(char *w)
{
	char	*o[9];

	o[0] = "one";
	o[1] = "two";
	o[2] = "three";
	o[3] = "four";
	o[4] = "five";
	o[5] = "six";
	o[6] = "seven";
	o[7] = "eight";
	o[8] = "nine";
	return (word_in_list(w, o, 9));
}

char	*get_sep(char *prev, char *curr)
{
	if (is_scale(prev))
		return (", ");
	if (!ft_strcmp(prev, "hundred") && !is_scale(curr))
		return (" and ");
	if (is_tens_word(prev) && is_ones_word(curr))
		return ("-");
	return (" ");
}
