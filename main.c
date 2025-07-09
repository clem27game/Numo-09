
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#define MAX_CODE_SIZE 10000
#define MAX_VARIABLES 1000
#define MAX_STRING_LEN 1000
#define MAX_FILENAME_LEN 256
#define MAX_STACK_SIZE 100

// Console colors
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define BOLD "\033[1m"

typedef struct {
    int type; // 3=int, 4=string, 5=bool, 6=float, 7=array
    char name[50];
    union {
        int int_val;
        char str_val[MAX_STRING_LEN];
        bool bool_val;
        double float_val;
        int array_val[100];
    } value;
    int array_size;
} Variable;

typedef struct {
    int position;
    int condition_result;
} StackFrame;

typedef struct {
    Variable vars[MAX_VARIABLES];
    int var_count;
    char code[MAX_CODE_SIZE];
    int position;
    int code_length;
    bool debug_mode;
    StackFrame stack[MAX_STACK_SIZE];
    int stack_pointer;
    int loop_depth;
    char current_color[20];
} NumoInterpreter;

// Initialize interpreter
void init_interpreter(NumoInterpreter *interp) {
    interp->var_count = 0;
    interp->position = 0;
    interp->code_length = 0;
    interp->debug_mode = false;
    interp->stack_pointer = 0;
    interp->loop_depth = 0;
    strcpy(interp->current_color, RESET);
    memset(interp->code, 0, MAX_CODE_SIZE);
    srand(time(NULL));
}

// Load .num file
bool load_numo_file(NumoInterpreter *interp, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf(RED "Error: Cannot open file %s\n" RESET, filename);
        return false;
    }
    
    char ch;
    int i = 0;
    while ((ch = fgetc(file)) != EOF && i < MAX_CODE_SIZE - 1) {
        if (isdigit(ch)) {
            interp->code[i++] = ch;
        }
    }
    interp->code[i] = '\0';
    interp->code_length = i;
    fclose(file);
    
    printf(GREEN "Loaded Numo 0-9 program: %d digits\n" RESET, interp->code_length);
    return true;
}

// Set console color
void set_color(NumoInterpreter *interp, int color_code) {
    switch (color_code) {
        case 0: strcpy(interp->current_color, RESET); break;
        case 1: strcpy(interp->current_color, RED); break;
        case 2: strcpy(interp->current_color, GREEN); break;
        case 3: strcpy(interp->current_color, YELLOW); break;
        case 4: strcpy(interp->current_color, BLUE); break;
        case 5: strcpy(interp->current_color, MAGENTA); break;
        case 6: strcpy(interp->current_color, CYAN); break;
        case 7: strcpy(interp->current_color, WHITE); break;
        case 8: strcpy(interp->current_color, BOLD); break;
        case 9: strcpy(interp->current_color, RESET); break;
    }
}

// Execute binary code (0s and 1s)
void execute_binary(NumoInterpreter *interp, int start, int end) {
    printf(CYAN "Executing binary sequence from position %d to %d\n" RESET, start, end);
    
    printf("%s", interp->current_color);
    for (int i = start; i < end; i += 8) {
        if (i + 7 < end) {
            int ascii_val = 0;
            for (int j = 0; j < 8; j++) {
                if (interp->code[i + j] == '1') {
                    ascii_val |= (1 << (7 - j));
                }
            }
            if (ascii_val >= 32 && ascii_val <= 126) {
                printf("%c", ascii_val);
            }
        }
    }
    printf(RESET "\n");
}

// Create variable (type 3, 4, 5, 6, 7)
void create_variable(NumoInterpreter *interp, int type, int position) {
    if (interp->var_count >= MAX_VARIABLES) return;
    
    Variable *var = &interp->vars[interp->var_count];
    var->type = type;
    sprintf(var->name, "var_%d_%d", type, position);
    
    switch (type) {
        case 3: // Numeric variable (integers)
            var->value.int_val = position % 100;
            printf(YELLOW "Created numeric variable %s = %d\n" RESET, var->name, var->value.int_val);
            break;
        case 4: // Text string variable
            sprintf(var->value.str_val, "text_%d", position);
            printf(MAGENTA "Created text variable %s = \"%s\"\n" RESET, var->name, var->value.str_val);
            break;
        case 5: // Boolean variable
            var->value.bool_val = (position % 2 == 0);
            printf(BLUE "Created boolean variable %s = %s\n" RESET, var->name, 
                   var->value.bool_val ? "true" : "false");
            break;
        case 6: // Float variable
            var->value.float_val = (double)(position % 100) / 10.0;
            printf(GREEN "Created float variable %s = %.2f\n" RESET, var->name, var->value.float_val);
            break;
        case 7: // Array variable
            var->array_size = 5;
            for (int i = 0; i < var->array_size; i++) {
                var->value.array_val[i] = (position + i) % 10;
            }
            printf(CYAN "Created array variable %s with %d elements\n" RESET, var->name, var->array_size);
            break;
    }
    interp->var_count++;
}

// Advanced mathematical operations
void handle_advanced_math(NumoInterpreter *interp, int operation, int position) {
    if (interp->var_count < 2) return;
    
    double val1 = 0, val2 = 0;
    int count = 0;
    
    // Get last two numeric values
    for (int i = interp->var_count - 1; i >= 0 && count < 2; i--) {
        if (interp->vars[i].type == 3 || interp->vars[i].type == 6) {
            double value = (interp->vars[i].type == 3) ? 
                          interp->vars[i].value.int_val : 
                          interp->vars[i].value.float_val;
            if (count == 0) val1 = value;
            else val2 = value;
            count++;
        }
    }
    
    double result = 0;
    char op_name[20];
    
    switch (operation) {
        case 0: // Addition
            result = val2 + val1;
            strcpy(op_name, "Addition");
            break;
        case 1: // Subtraction
            result = val2 - val1;
            strcpy(op_name, "Subtraction");
            break;
        case 2: // Multiplication
            result = val2 * val1;
            strcpy(op_name, "Multiplication");
            break;
        case 3: // Division
            result = (val1 != 0) ? val2 / val1 : 0;
            strcpy(op_name, "Division");
            break;
        case 4: // Power
            result = pow(val2, val1);
            strcpy(op_name, "Power");
            break;
        case 5: // Square root
            result = sqrt(val1);
            strcpy(op_name, "Square root");
            break;
        case 6: // Sine
            result = sin(val1);
            strcpy(op_name, "Sine");
            break;
        case 7: // Cosine
            result = cos(val1);
            strcpy(op_name, "Cosine");
            break;
        case 8: // Logarithm
            result = (val1 > 0) ? log(val1) : 0;
            strcpy(op_name, "Logarithm");
            break;
        case 9: // Modulo
            result = (val1 != 0) ? fmod(val2, val1) : 0;
            strcpy(op_name, "Modulo");
            break;
    }
    
    printf(GREEN "%s: %.2f %s %.2f = %.2f\n" RESET, op_name, val2, 
           (operation <= 3) ? "+-*/"[operation] : '?', val1, result);
    
    // Store result
    if (interp->var_count < MAX_VARIABLES) {
        Variable *var = &interp->vars[interp->var_count];
        var->type = 6;
        sprintf(var->name, "math_result_%d", position);
        var->value.float_val = result;
        interp->var_count++;
    }
}

// Advanced loops and iterations
void handle_advanced_loops(NumoInterpreter *interp, int loop_type, int position) {
    printf(BLUE "Advanced loop type %d at position %d\n" RESET, loop_type, position);
    
    int loop_count = 0;
    bool condition = true;
    
    switch (loop_type) {
        case 0: // For loop (fixed iterations)
            loop_count = (position % 10) + 1;
            printf(YELLOW "FOR loop: %d iterations\n" RESET, loop_count);
            break;
        case 1: // While loop (condition-based)
            loop_count = 5; // Max iterations for safety
            printf(YELLOW "WHILE loop: condition-based (max %d iterations)\n" RESET, loop_count);
            break;
        case 2: // Do-while loop
            loop_count = 3;
            printf(YELLOW "DO-WHILE loop: %d iterations\n" RESET, loop_count);
            break;
        case 3: // Repeat loop
            loop_count = (position % 5) + 1;
            printf(YELLOW "REPEAT loop: %d times\n" RESET, loop_count);
            break;
    }
    
    // Execute loop
    for (int i = 0; i < loop_count && condition; i++) {
        printf(CYAN "  Loop iteration %d/%d\n" RESET, i+1, loop_count);
        
        // Simple condition for while loops
        if (loop_type == 1) {
            condition = (i < 3); // Stop after 3 iterations
        }
        
        // Execute next few operations
        int start_pos = interp->position + 1;
        for (int j = 0; j < 2 && start_pos + j < interp->code_length; j++) {
            char next_op = interp->code[start_pos + j];
            if (next_op >= '3' && next_op <= '7') {
                create_variable(interp, next_op - '0', start_pos + j);
            }
        }
    }
    
    interp->position += 2; // Skip processed operations
}

// Robust conditional structures
void handle_robust_conditions(NumoInterpreter *interp, int condition_type, int position) {
    printf(MAGENTA "Conditional structure type %d at position %d\n" RESET, condition_type, position);
    
    bool condition_result = false;
    int compare_val = position % 10;
    
    switch (condition_type) {
        case 0: // IF condition
            condition_result = (compare_val % 2 == 0);
            printf(CYAN "IF condition: %s (value=%d)\n" RESET, 
                   condition_result ? "TRUE" : "FALSE", compare_val);
            break;
        case 1: // IF-ELSE condition
            condition_result = (compare_val > 5);
            printf(CYAN "IF-ELSE condition: %s (value=%d)\n" RESET, 
                   condition_result ? "TRUE (executing IF branch)" : "FALSE (executing ELSE branch)", compare_val);
            break;
        case 2: // SWITCH-CASE
            printf(CYAN "SWITCH-CASE: value=%d\n" RESET, compare_val);
            condition_result = true;
            break;
        case 3: // WHILE condition
            condition_result = (compare_val < 8);
            printf(CYAN "WHILE condition: %s (value=%d)\n" RESET, 
                   condition_result ? "TRUE" : "FALSE", compare_val);
            break;
        case 4: // UNTIL condition
            condition_result = (compare_val >= 7);
            printf(CYAN "UNTIL condition: %s (value=%d)\n" RESET, 
                   condition_result ? "TRUE" : "FALSE", compare_val);
            break;
    }
    
    // Store condition result on stack
    if (interp->stack_pointer < MAX_STACK_SIZE) {
        interp->stack[interp->stack_pointer].position = position;
        interp->stack[interp->stack_pointer].condition_result = condition_result;
        interp->stack_pointer++;
    }
    
    // Execute based on condition
    if (condition_result) {
        printf(GREEN "  Executing TRUE branch\n" RESET);
        // Execute next operation
        if (interp->position + 1 < interp->code_length) {
            char next_op = interp->code[interp->position + 1];
            if (next_op >= '3' && next_op <= '7') {
                create_variable(interp, next_op - '0', interp->position + 1);
            }
        }
    } else {
        printf(RED "  Skipping FALSE branch\n" RESET);
        interp->position++; // Skip next operation
    }
}

// Enhanced input/output operations
void handle_enhanced_io(NumoInterpreter *interp, int io_type, int position) {
    printf(BLUE "Enhanced I/O operation type %d at position %d\n" RESET, io_type, position);
    
    switch (io_type) {
        case 0: // Input number
            printf(YELLOW "Enter a number: " RESET);
            int input_val;
            if (scanf("%d", &input_val) == 1) {
                if (interp->var_count < MAX_VARIABLES) {
                    Variable *var = &interp->vars[interp->var_count];
                    var->type = 3;
                    sprintf(var->name, "input_num_%d", position);
                    var->value.int_val = input_val;
                    printf(GREEN "Stored input %d in variable %s\n" RESET, input_val, var->name);
                    interp->var_count++;
                }
            }
            break;
        case 1: // Input string
            printf(YELLOW "Enter a string: " RESET);
            char input_str[MAX_STRING_LEN];
            if (scanf("%s", input_str) == 1) {
                if (interp->var_count < MAX_VARIABLES) {
                    Variable *var = &interp->vars[interp->var_count];
                    var->type = 4;
                    sprintf(var->name, "input_str_%d", position);
                    strcpy(var->value.str_val, input_str);
                    printf(GREEN "Stored input \"%s\" in variable %s\n" RESET, input_str, var->name);
                    interp->var_count++;
                }
            }
            break;
        case 2: // Display variable
            if (interp->var_count > 0) {
                Variable *var = &interp->vars[interp->var_count - 1];
                printf(CYAN "Display variable %s: " RESET, var->name);
                switch (var->type) {
                    case 3: printf("%d\n", var->value.int_val); break;
                    case 4: printf("\"%s\"\n", var->value.str_val); break;
                    case 5: printf("%s\n", var->value.bool_val ? "true" : "false"); break;
                    case 6: printf("%.2f\n", var->value.float_val); break;
                }
            }
            break;
        case 3: // Clear screen
            system("clear");
            printf(GREEN "Screen cleared\n" RESET);
            break;
        case 4: // Pause/Wait
            printf(YELLOW "Press Enter to continue..." RESET);
            getchar();
            break;
        case 5: // Random number
            if (interp->var_count < MAX_VARIABLES) {
                Variable *var = &interp->vars[interp->var_count];
                var->type = 3;
                sprintf(var->name, "random_%d", position);
                var->value.int_val = rand() % 100;
                printf(GREEN "Generated random number %d\n" RESET, var->value.int_val);
                interp->var_count++;
            }
            break;
        case 6: // Print with color
            set_color(interp, position % 10);
            printf("%sColored output at position %d\n" RESET, interp->current_color, position);
            break;
        case 7: // Formatted output
            printf(BOLD "=== Formatted Output ===" RESET "\n");
            printf(CYAN "Position: %d\n" RESET, position);
            printf(YELLOW "Variables: %d\n" RESET, interp->var_count);
            break;
        case 8: // Sound/Bell
            printf("\a"); // Bell sound
            printf(MAGENTA "Sound alert at position %d\n" RESET, position);
            break;
        case 9: // Time/Date
            time_t now = time(NULL);
            printf(GREEN "Current time: %s" RESET, ctime(&now));
            break;
    }
}

// String operations
void handle_string_operations(NumoInterpreter *interp, int operation, int position) {
    printf(MAGENTA "String operation %d at position %d\n" RESET, operation, position);
    
    if (interp->var_count < 2) return;
    
    char str1[MAX_STRING_LEN] = "", str2[MAX_STRING_LEN] = "";
    int str_count = 0;
    
    // Get last two string variables
    for (int i = interp->var_count - 1; i >= 0 && str_count < 2; i--) {
        if (interp->vars[i].type == 4) {
            if (str_count == 0) strcpy(str1, interp->vars[i].value.str_val);
            else strcpy(str2, interp->vars[i].value.str_val);
            str_count++;
        }
    }
    
    char result[MAX_STRING_LEN] = "";
    
    switch (operation) {
        case 0: // Concatenate
            strcpy(result, str2);
            strcat(result, str1);
            printf(GREEN "Concatenation: \"%s\" + \"%s\" = \"%s\"\n" RESET, str2, str1, result);
            break;
        case 1: // Length
            sprintf(result, "%d", (int)strlen(str1));
            printf(GREEN "Length of \"%s\" = %s\n" RESET, str1, result);
            break;
        case 2: // Uppercase
            strcpy(result, str1);
            for (int i = 0; result[i]; i++) {
                result[i] = toupper(result[i]);
            }
            printf(GREEN "Uppercase: \"%s\" = \"%s\"\n" RESET, str1, result);
            break;
        case 3: // Lowercase
            strcpy(result, str1);
            for (int i = 0; result[i]; i++) {
                result[i] = tolower(result[i]);
            }
            printf(GREEN "Lowercase: \"%s\" = \"%s\"\n" RESET, str1, result);
            break;
        case 4: // Reverse
            strcpy(result, str1);
            int len = strlen(result);
            for (int i = 0; i < len / 2; i++) {
                char temp = result[i];
                result[i] = result[len - 1 - i];
                result[len - 1 - i] = temp;
            }
            printf(GREEN "Reverse: \"%s\" = \"%s\"\n" RESET, str1, result);
            break;
    }
    
    // Store result
    if (interp->var_count < MAX_VARIABLES) {
        Variable *var = &interp->vars[interp->var_count];
        var->type = 4;
        sprintf(var->name, "str_result_%d", position);
        strcpy(var->value.str_val, result);
        interp->var_count++;
    }
}

// Handle conditions (digit 6) - Enhanced
void handle_condition(NumoInterpreter *interp, int position) {
    int condition_type = position % 10;
    handle_robust_conditions(interp, condition_type % 5, position);
}

// Handle input (digit 7) - Enhanced
void handle_input(NumoInterpreter *interp, int position) {
    int io_type = position % 10;
    handle_enhanced_io(interp, io_type, position);
}

// Handle math operations (digit 8) - Enhanced
void handle_math(NumoInterpreter *interp, int position) {
    int operation = position % 10;
    handle_advanced_math(interp, operation, position);
}

// Handle file operations (digit 9) - Enhanced
void handle_file_ops(NumoInterpreter *interp, int position) {
    printf(CYAN "Advanced file operation at position %d\n" RESET, position);
    
    char filename[50];
    sprintf(filename, "numo_output_%d.txt", position);
    
    FILE *file = fopen(filename, "w");
    if (file) {
        fprintf(file, "=== Numo 0-9 Advanced Output Report ===\n");
        fprintf(file, "Position: %d\n", position);
        fprintf(file, "Variables created: %d\n", interp->var_count);
        fprintf(file, "Stack pointer: %d\n", interp->stack_pointer);
        fprintf(file, "Loop depth: %d\n", interp->loop_depth);
        fprintf(file, "\n--- Variables ---\n");
        
        for (int i = 0; i < interp->var_count; i++) {
            Variable *var = &interp->vars[i];
            switch (var->type) {
                case 3:
                    fprintf(file, "%s (int) = %d\n", var->name, var->value.int_val);
                    break;
                case 4:
                    fprintf(file, "%s (string) = \"%s\"\n", var->name, var->value.str_val);
                    break;
                case 5:
                    fprintf(file, "%s (bool) = %s\n", var->name, 
                           var->value.bool_val ? "true" : "false");
                    break;
                case 6:
                    fprintf(file, "%s (float) = %.2f\n", var->name, var->value.float_val);
                    break;
                case 7:
                    fprintf(file, "%s (array) = [", var->name);
                    for (int j = 0; j < var->array_size; j++) {
                        fprintf(file, "%d%s", var->value.array_val[j], 
                               (j < var->array_size - 1) ? ", " : "");
                    }
                    fprintf(file, "]\n");
                    break;
            }
        }
        
        fprintf(file, "\n--- Execution Statistics ---\n");
        fprintf(file, "Total code length: %d\n", interp->code_length);
        fprintf(file, "Debug mode: %s\n", interp->debug_mode ? "enabled" : "disabled");
        
        fclose(file);
        printf(GREEN "Created advanced report: %s\n" RESET, filename);
    }
}

// Handle advanced loops (digit 2) - Enhanced
void handle_loops(NumoInterpreter *interp, int position) {
    int loop_type = position % 4;
    handle_advanced_loops(interp, loop_type, position);
}

// Main interpreter loop
void interpret(NumoInterpreter *interp) {
    printf(BOLD GREEN "Starting Numo 0-9 Advanced Interpretation...\n" RESET);
    printf(CYAN "Code: %s\n" RESET, interp->code);
    printf(YELLOW "==================================================\n" RESET);
    
    int binary_start = -1;
    
    while (interp->position < interp->code_length) {
        char current = interp->code[interp->position];
        
        if (interp->debug_mode) {
            printf(MAGENTA "Position %d: Processing digit '%c'\n" RESET, interp->position, current);
        }
        
        switch (current) {
            case '0':
            case '1':
                if (binary_start == -1) {
                    binary_start = interp->position;
                }
                break;
                
            case '2': // End of binary program / Advanced loops
                if (binary_start != -1) {
                    execute_binary(interp, binary_start, interp->position);
                    binary_start = -1;
                } else {
                    handle_loops(interp, interp->position);
                }
                break;
                
            case '3': // Create numeric variable (integers)
                create_variable(interp, 3, interp->position);
                break;
                
            case '4': // Create text string variable
                create_variable(interp, 4, interp->position);
                break;
                
            case '5': // Create boolean variable
                create_variable(interp, 5, interp->position);
                break;
                
            case '6': // Advanced conditional execution and control flow
                handle_condition(interp, interp->position);
                break;
                
            case '7': // Enhanced Input/Output operations
                handle_input(interp, interp->position);
                break;
                
            case '8': // Advanced mathematical operations and calculations
                handle_math(interp, interp->position);
                break;
                
            case '9': // Advanced file operations and data persistence
                handle_file_ops(interp, interp->position);
                break;
                
            default:
                printf(RED "Unknown digit: %c at position %d\n" RESET, current, interp->position);
                break;
        }
        
        interp->position++;
    }
    
    // Execute any remaining binary code
    if (binary_start != -1) {
        execute_binary(interp, binary_start, interp->position);
    }
    
    printf(BOLD GREEN "\nProgram execution completed successfully!\n" RESET);
    printf(YELLOW "Variables created: %d\n" RESET, interp->var_count);
    printf(CYAN "Stack operations: %d\n" RESET, interp->stack_pointer);
    printf(MAGENTA "Final position: %d\n" RESET, interp->position);
}

// Print help
void print_help() {
    printf(BOLD CYAN "Numo 0-9 Advanced Programming Language Interpreter\n" RESET);
    printf(YELLOW "=========================================\n" RESET);
    printf("Usage: ./main <file.num> [options]\n\n");
    printf(BOLD "Enhanced Numo 0-9 Syntax:\n" RESET);
    printf(GREEN "0,1 - " RESET "Binary code (machine language)\n");
    printf(GREEN "2   - " RESET "End binary program / Advanced loops (FOR, WHILE, DO-WHILE, REPEAT)\n");
    printf(GREEN "3   - " RESET "Create numeric variable (integers)\n");
    printf(GREEN "4   - " RESET "Create text string variable\n");
    printf(GREEN "5   - " RESET "Create boolean variable\n");
    printf(GREEN "6   - " RESET "Advanced conditional execution (IF, IF-ELSE, SWITCH, WHILE, UNTIL)\n");
    printf(GREEN "7   - " RESET "Enhanced Input/Output operations (colored output, formatted I/O)\n");
    printf(GREEN "8   - " RESET "Advanced mathematical operations (+, -, *, /, ^, √, sin, cos, log, %)\n");
    printf(GREEN "9   - " RESET "Advanced file operations and data persistence\n\n");
    
    printf(BOLD "New Features:\n" RESET);
    printf(CYAN "• " RESET "Colored console output\n");
    printf(CYAN "• " RESET "Advanced mathematical operations (trigonometry, logarithms)\n");
    printf(CYAN "• " RESET "Robust conditional structures (IF-ELSE, SWITCH-CASE)\n");
    printf(CYAN "• " RESET "Enhanced loops (FOR, WHILE, DO-WHILE, REPEAT)\n");
    printf(CYAN "• " RESET "String operations (concatenation, length, case conversion)\n");
    printf(CYAN "• " RESET "Float and array variable types\n");
    printf(CYAN "• " RESET "Stack-based execution model\n");
    printf(CYAN "• " RESET "Random number generation\n");
    printf(CYAN "• " RESET "Time/date functions\n");
    printf(CYAN "• " RESET "Sound alerts\n");
    printf(CYAN "• " RESET "Screen clearing\n");
    printf(CYAN "• " RESET "Enhanced debugging\n\n");
    
    printf(BOLD "Options:\n" RESET);
    printf(YELLOW "-d, --debug   " RESET "Enable debug mode\n");
    printf(YELLOW "-h, --help    " RESET "Show this help\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_help();
        return 1;
    }
    
    NumoInterpreter interp;
    init_interpreter(&interp);
    
    // Parse command line arguments
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--debug") == 0) {
            interp.debug_mode = true;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_help();
            return 0;
        }
    }
    
    // Load and execute the Numo file
    if (load_numo_file(&interp, argv[1])) {
        interpret(&interp);
    }
    
    return 0;
}
