/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpatrici <gpatrici@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 16:42:38 by gpatrici          #+#    #+#             */
/*   Updated: 2026/10/20 15:51:26 by gpatrici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

// Maps the scheduler string to FIFO or EDF.
int scheduler_from_string(const char *str)
{
	if (strcmp(str, "fifo") == 0)
		return (FIFO);
	if (strcmp(str, "edf") == 0)
		return (EDF);
	return (-1);
}

// Reads every command-line option and validates the simulation values.
int parse_args(t_simulation *sim, char **argv)
{
	if (!parse_i_arg(argv[1], &sim->coder_count))
		return (0);
	if (!parse_ll_arg(argv[2], &sim->burnout_ms))
		return (0);
	if (!parse_i_arg(argv[3], &sim->compile_ms))
		return (0);
	if (!parse_i_arg(argv[4], &sim->debug_ms))
		return (0);
	if (!parse_i_arg(argv[5], &sim->refactor_ms))
		return (0);
	if (!parse_i_arg(argv[6], &sim->required_compiles))
		return (0);
	if (!parse_i_arg(argv[7], &sim->cooldown_ms))
		return (0);
	sim->scheduler = (t_scheduler)scheduler_from_string(argv[8]);
	return (valid_values(sim));
}
