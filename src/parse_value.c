/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_value.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpatrici <gpatrici@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 16:42:36 by gpatrici          #+#    #+#             */
/*   Updated: 2026/10/20 15:51:20 by gpatrici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

// Parses an optional sign at the start of a numeric argument.
static int parse_sign(const char *str, int *index, int *sign)
{
	*sign = 1;
	*index = 0;
	if (!str || !str[*index])
		return (0);
	if (str[*index] == '+' || str[*index] == '-')
	{
		if (str[*index] == '-')
			*sign = -1;
		(*index)++;
	}
	if (!str[*index])
		return (0);
	return (1);
}

// Consumes a sequence of digits and converts it to a long long value.
static int parse_digits(const char *str, int index, long long *result)
{
	long long value;

	value = 0;
	while (str[index])
	{
		if (str[index] < '0' || str[index] > '9')
			return (0);
		value = value * 10 + (str[index] - '0');
		index++;
	}
	*result = value;
	return (1);
}

// Parses a signed long long value from a string and rejects malformed input.
int parse_ll_arg(const char *str, long long *value)
{
	int index;
	int sign;
	long long result;

	if (!parse_sign(str, &index, &sign))
		return (0);
	if (!parse_digits(str, index, &result))
		return (0);
	*value = result * sign;
	return (1);
}

// Parses an int-sized value and ensures it stays within the C integer range.
int parse_i_arg(const char *str, int *value)
{
	long long parsed;

	if (!parse_ll_arg(str, &parsed))
		return (0);
	if (parsed < -2147483648LL || parsed > 2147483647LL)
		return (0);
	*value = (int)parsed;
	return (1);
}

// Validates that the configured simulation values are allowed.
int valid_values(const t_simulation *sim)
{
	if (sim->coder_count <= 0 || sim->burnout_ms <= 0)
		return (0);
	if (sim->compile_ms < 0 || sim->debug_ms < 0 || sim->refactor_ms < 0)
		return (0);
	if (sim->required_compiles <= 0 || sim->cooldown_ms < 0)
		return (0);
	if (sim->scheduler != FIFO && sim->scheduler != EDF)
		return (0);
	return (1);
}
