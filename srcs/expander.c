/* ************************************************************************** */
/* */
/* :::      ::::::::   */
/* expander.c                                         :+:      :+:    :+:   */
/* +:+ +:+         +:+     */
/* By: rhiguita <rhiguita@student.42madrid.com>   +#+  +:+       +#+        */
/* +#+#+#+#+#+   +#+           */
/* Created: 2025/06/24 16:34:07 by rhiguita          #+#    #+#             */
/* Updated: 2025/08/19 17:30:00 by rhiguita         ###   ########.fr       */
/* */
/* ************************************************************************** */

#include "../inc/minishell.h"
#include "../inc/expander.h"
#include "../inc/env_utils.h" // Asegúrate de que esta cabecera exista y tenga get_env_value

// --- Funciones de String Builder ---

// Inicializa el string builder
static void ft_sb_init(t_string_builder *sb)
{
	sb->length = 0;
	sb->capacity = INITIAL_SB_CAPACITY;
	sb->buffer = (char *)ft_calloc(sb->capacity, sizeof(char));
	if (!sb->buffer)
	{
		sb->capacity = 0;
	}
}

// Asegura que hay suficiente espacio en el buffer.
static bool ft_sb_grow(t_string_builder *sb, size_t needed)
{
	char	*new_buffer;
	size_t	new_capacity;

	if (!sb->buffer)
		return (false);
	if (sb->length + needed < sb->capacity)
		return (true);
	new_capacity = sb->capacity;
	while (sb->length + needed >= new_capacity)
		new_capacity *= 2;
	new_buffer = (char *)ft_calloc(new_capacity, sizeof(char));
	if (!new_buffer)
	{
		free(sb->buffer);
		sb->buffer = NULL;
		sb->capacity = 0;
		sb->length = 0;
		return (false);
	}
	ft_memcpy(new_buffer, sb->buffer, sb->length);
	free(sb->buffer);
	sb->buffer = new_buffer;
	sb->capacity = new_capacity;
	return (true);
}

// Añade un caracter al string builder
static void ft_sb_append_char(t_string_builder *sb, char c)
{
	if (!ft_sb_grow(sb, 1))
		return ;
	sb->buffer[sb->length] = c;
	sb->length++;
	sb->buffer[sb->length] = '\0';
}

// Añade una cadena al string builder
static void ft_sb_append_str(t_string_builder *sb, const char *str)
{
	size_t	str_len;

	if (!str || !sb->buffer)
		return ;
	str_len = ft_strlen(str);
	if (!ft_sb_grow(sb, str_len))
		return ;
	ft_memcpy(sb->buffer + sb->length, str, str_len);
	sb->length += str_len;
	sb->buffer[sb->length] = '\0';
}

// Finaliza y retorna la cadena construida.
static char *ft_sb_build(t_string_builder *sb)
{
	char	*final_str;

	if (!sb->buffer)
		return (NULL);
	final_str = ft_strdup(sb->buffer);
	free(sb->buffer);
	sb->buffer = NULL;
	sb->length = 0;
	sb->capacity = 0;
	return (final_str);
}

// --- Funciones de Expansión ---

static char *extract_and_get_value(const char *s, int *start_of_var_idx,
	t_struct *mini)
{
	int		temp_i;
	char	*var_name;
	char	*var_value;

	temp_i = *start_of_var_idx;
	while (s[temp_i] && (ft_isalnum(s[temp_i]) || s[temp_i] == '_'))
		temp_i++;
	var_name = ft_substr(s, *start_of_var_idx, temp_i - *start_of_var_idx);
	if (!var_name)
		return (NULL);
	var_value = get_env_value(var_name, mini);
	free(var_name);
	*start_of_var_idx = temp_i;
	return (var_value);
}

static void handle_special_dollar(t_string_builder *sb, const char *s, int *i,
	t_struct *mini)
{
	char	*var_value;

	if (s[*i + 1] == '?')
	{
		var_value = get_env_value("?", mini);
		if (var_value)
		{
			ft_sb_append_str(sb, var_value);
			free(var_value);
		}
		(*i) += 2;
	}
	else if (ft_isdigit(s[*i + 1]))
	{
		(*i) += 2; // En Minishell, $ seguido de un dígito no se expande, se ignora.
	}
	else
	{
		ft_sb_append_char(sb, s[*i]);
		(*i)++;
	}
}

static void handle_alphanum_dollar(t_string_builder *sb, const char *s, int *i,
	t_struct *mini)
{
	char	*var_value;
	int		start_of_var;

	start_of_var = *i + 1;
	var_value = extract_and_get_value(s, &start_of_var, mini);
	if (var_value)
	{
		ft_sb_append_str(sb, var_value);
		free(var_value);
	}
	*i = start_of_var;
}

static void process_dollar_expansion(t_string_builder *sb, const char *s,
	int *i, t_struct *mini, char current_quote_char)
{
	if (current_quote_char == '\'')
	{
		ft_sb_append_char(sb, s[*i]);
		(*i)++;
		return ;
	}
	if (s[*i + 1] == '?' || ft_isdigit(s[*i + 1]))
		handle_special_dollar(sb, s, i, mini);
	else if (ft_isalpha(s[*i + 1]) || s[*i + 1] == '_')
		handle_alphanum_dollar(sb, s, i, mini);
	else
	{
		ft_sb_append_char(sb, s[*i]);
		(*i)++;
	}
}

char	*expand_string(char *original_str, t_struct *mini)
{
	t_string_builder	sb;
	int					i;
	char				current_quote_char;

	if (!original_str)
		return (ft_strdup(""));
	ft_sb_init(&sb);
	if (!sb.buffer)
		return (NULL);
	i = 0;
	current_quote_char = 0;
	while (original_str[i])
	{
		if (original_str[i] == '\'' || original_str[i] == '\"')
		{
			if (current_quote_char == 0)
				current_quote_char = original_str[i];
			else if (current_quote_char == original_str[i])
				current_quote_char = 0;
			i++;
			continue ;
		}
		if (original_str[i] == '$')
			process_dollar_expansion(&sb, original_str, &i, mini,
				current_quote_char);
		else
		{
			ft_sb_append_char(&sb, original_str[i]);
			i++;
		}
	}
	return (ft_sb_build(&sb));
}

void	expand_variables(t_command *cmd_list, t_struct *mini)
{
	t_command		*current_cmd;
	t_redirection	*current_redir;
	char			*expanded_str;
	int				i;

	current_cmd = cmd_list;
	while (current_cmd)
	{
		i = 0;
		while (current_cmd->args && current_cmd->args[i])
		{
			expanded_str = expand_string(current_cmd->args[i], mini);
			if (!expanded_str)
			{
				mini->last_exit_status = 1;
				return ;
			}
			free(current_cmd->args[i]);
			current_cmd->args[i] = expanded_str;
			i++;
		}
		current_redir = current_cmd->redirections;
		while (current_redir)
		{
			// El delimitador de Heredoc NO se expande.
			if (current_redir->type != REDIR_HEREDOC)
			{
				// CORRECCIÓN: Usar current_redir->file en lugar de args[i]
				expanded_str = expand_string(current_redir->file, mini);
				if (!expanded_str)
				{
					mini->last_exit_status = 1;
					return ;
				}
				free(current_redir->file);
				current_redir->file = expanded_str;
			}
			current_redir = current_redir->next;
		}
		current_cmd = current_cmd->next;
	}
}