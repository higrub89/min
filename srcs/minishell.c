#include "../inc/minishell.h"
#include "../inc/lexer.h"

// Una función de ayuda para imprimir el tipo de token como una cadena.
// Facilita la lectura de la salida de prueba.
static const char *token_type_to_str(t_token_type type)
{
    if (type == WORD) return "WORD";
    if (type == PIPE) return "PIPE";
    if (type == IN) return "IN";
    if (type == OUT) return "OUT";
    if (type == APPE_OUT) return "APPE_OUT";
    if (type == HEREDOC) return "HEREDOC";
    return "UNKNOWN";
}

// Función principal para probar el lexer.
int main(void)
{
    char        *input_line;
    t_token     *token_list;
    t_token     *current_token;
    t_struct    mini; // El lexer necesita una referencia, aunque no la usemos mucho aquí.

    // Inicialización básica de la estructura mini.
    // Para las pruebas del lexer, solo necesitamos que no sea basura.
    ft_bzero(&mini, sizeof(t_struct));

    while (1)
    {
        input_line = readline("lexer_test> ");
        if (!input_line) // Manejo de CTRL+D (EOF)
        {
            printf("exit\n");
            break;
        }

        if (input_line[0])
            add_history(input_line);

        printf("--- Input: \"%s\" ---\n", input_line);

        // 1. Llama al Lexer
        token_list = lexer(input_line, &mini);

        // 2. Imprime la lista de tokens generada
        if (!token_list)
        {
            printf("-> Lexer returned NULL (syntax error or empty line)\n");
        }
        else
        {
            printf("-> Tokens: ");
            current_token = token_list;
            while (current_token)
            {
                printf("[%s: \"%s\"]",
                    token_type_to_str(current_token->type), current_token->value);
                if (current_token->next)
                    printf(" -> ");
                current_token = current_token->next;
            }
            printf("\n");
        }

        // 3. Libera la memoria
        if (token_list)
            free_tokens(token_list);
        free(input_line);

        printf("-------------------------\n\n");
    }
    // system("leaks lexer_test"); // Descomenta esto si estás en macOS para comprobar fugas
    return (0);
}