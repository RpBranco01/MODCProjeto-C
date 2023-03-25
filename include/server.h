/**
 * Dada uma string do com formato username:password
 * guarda a original e devolve-a alterando a string
*/
char* handle_operation(char *operation);

/**
 * Função do Servidor que dado os argumentos
 * preenche as variáveis globais
*/
void get_params(char const *argv[]);

/**
 * Dado o nome do ficheiro e o número da linha
 * apaga a linha desse ficheiro
*/
int delete_line(char *filename, int line_number);

/**
 * Dado a string verifica se esta possui um \\n
*/
int has_newline(char *str);