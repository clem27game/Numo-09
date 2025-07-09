
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_CODE_SIZE 10000
#define MAX_VARIABLES 1000
#define MAX_STRING_LEN 1000
#define MAX_FILENAME_LEN 256

typedef struct {
    int type; // 3=int, 4=string, 5=bool
    char name[50];
    union {
        int int_val;
        char str_val[MAX_STRING_LEN];
        bool bool_val;
    } value;
} Variable;

typedef struct {
    Variable vars[MAX_VARIABLES];
    int var_count;
    char code[MAX_CODE_SIZE];
    int position;
    int code_length;
    bool debug_mode;
} NumoInterpreter;

// Initialize interpreter
void init_interpreter(NumoInterpreter *interp) {
    interp->var_count = 0;
    interp->position = 0;
    interp->code_length = 0;
    interp->debug_mode = false;
    memset(interp->code, 0, MAX_CODE_SIZE);
}

// Load .num file
bool load_numo_file(NumoInterpreter *interp, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Cannot open file %s\n", filename);
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
    
    printf("Loaded Numo 0-9 program: %d digits\n", interp->code_length);
    return true;
}

// Execute binary code (0s and 1s)
void execute_binary(NumoInterpreter *interp, int start, int end) {
    printf("Executing binary sequence from position %d to %d\n", start, end);
    
    // Simple binary interpreter - convert to ASCII and print
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
    printf("\n");
}

// Create variable (type 3, 4, or 5)
void create_variable(NumoInterpreter *interp, int type, int position) {
    if (interp->var_count >= MAX_VARIABLES) return;
    
    Variable *var = &interp->vars[interp->var_count];
    var->type = type;
    sprintf(var->name, "var_%d_%d", type, position);
    
    switch (type) {
        case 3: // Integer variable
            var->value.int_val = position % 100; // Use position as initial value
            printf("Created integer variable %s = %d\n", var->name, var->value.int_val);
            break;
        case 4: // String variable
            sprintf(var->value.str_val, "string_%d", position);
            printf("Created string variable %s = \"%s\"\n", var->name, var->value.str_val);
            break;
        case 5: // Boolean variable
            var->value.bool_val = (position % 2 == 0);
            printf("Created boolean variable %s = %s\n", var->name, 
                   var->value.bool_val ? "true" : "false");
            break;
    }
    interp->var_count++;
}

// Handle conditions (digit 6)
void handle_condition(NumoInterpreter *interp, int position) {
    printf("Processing condition at position %d\n", position);
    
    // Simple condition: if position is even, execute next instruction
    if (position % 2 == 0) {
        printf("Condition TRUE: Executing next instruction\n");
    } else {
        printf("Condition FALSE: Skipping next instruction\n");
        interp->position++; // Skip next digit
    }
}

// Handle input (digit 7)
void handle_input(NumoInterpreter *interp, int position) {
    printf("Input requested at position %d\n", position);
    printf("Enter a number: ");
    
    int input_val;
    if (scanf("%d", &input_val) == 1) {
        // Store input in a new variable
        if (interp->var_count < MAX_VARIABLES) {
            Variable *var = &interp->vars[interp->var_count];
            var->type = 3;
            sprintf(var->name, "input_%d", position);
            var->value.int_val = input_val;
            printf("Stored input %d in variable %s\n", input_val, var->name);
            interp->var_count++;
        }
    }
}

// Handle math operations (digit 8)
void handle_math(NumoInterpreter *interp, int position) {
    printf("Math operation at position %d\n", position);
    
    // Simple math: add two previous integer variables if they exist
    if (interp->var_count >= 2) {
        int val1 = 0, val2 = 0;
        for (int i = interp->var_count - 1; i >= 0 && (val1 == 0 || val2 == 0); i--) {
            if (interp->vars[i].type == 3) {
                if (val2 == 0) val2 = interp->vars[i].value.int_val;
                else if (val1 == 0) val1 = interp->vars[i].value.int_val;
            }
        }
        int result = val1 + val2;
        printf("Math result: %d + %d = %d\n", val1, val2, result);
        
        // Store result
        if (interp->var_count < MAX_VARIABLES) {
            Variable *var = &interp->vars[interp->var_count];
            var->type = 3;
            sprintf(var->name, "math_result_%d", position);
            var->value.int_val = result;
            interp->var_count++;
        }
    }
}

// Handle file operations (digit 9)
void handle_file_ops(NumoInterpreter *interp, int position) {
    printf("File operation at position %d\n", position);
    
    char filename[50];
    sprintf(filename, "numo_output_%d.txt", position);
    
    FILE *file = fopen(filename, "w");
    if (file) {
        fprintf(file, "Numo 0-9 file operation at position %d\n", position);
        fprintf(file, "Variables created: %d\n", interp->var_count);
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
            }
        }
        fclose(file);
        printf("Created file: %s\n", filename);
    }
}

// Main interpreter loop
void interpret(NumoInterpreter *interp) {
    printf("Starting Numo 0-9 interpretation...\n");
    printf("Code: %s\n", interp->code);
    printf("=" * 50);
    printf("\n");
    
    int binary_start = -1;
    
    while (interp->position < interp->code_length) {
        char current = interp->code[interp->position];
        
        if (interp->debug_mode) {
            printf("Position %d: Processing digit '%c'\n", interp->position, current);
        }
        
        switch (current) {
            case '0':
            case '1':
                if (binary_start == -1) {
                    binary_start = interp->position;
                }
                break;
                
            case '2': // End of binary program
                if (binary_start != -1) {
                    execute_binary(interp, binary_start, interp->position);
                    binary_start = -1;
                }
                break;
                
            case '3': // Create integer variable
                create_variable(interp, 3, interp->position);
                break;
                
            case '4': // Create string variable
                create_variable(interp, 4, interp->position);
                break;
                
            case '5': // Create boolean variable
                create_variable(interp, 5, interp->position);
                break;
                
            case '6': // Condition
                handle_condition(interp, interp->position);
                break;
                
            case '7': // Input
                handle_input(interp, interp->position);
                break;
                
            case '8': // Math operation
                handle_math(interp, interp->position);
                break;
                
            case '9': // File operation
                handle_file_ops(interp, interp->position);
                break;
                
            default:
                printf("Unknown digit: %c at position %d\n", current, interp->position);
                break;
        }
        
        interp->position++;
    }
    
    // Execute any remaining binary code
    if (binary_start != -1) {
        execute_binary(interp, binary_start, interp->position);
    }
    
    printf("\nProgram execution completed.\n");
    printf("Variables created: %d\n", interp->var_count);
}

// Print help
void print_help() {
    printf("Numo 0-9 Programming Language Interpreter\n");
    printf("=========================================\n");
    printf("Usage: ./main <file.num> [options]\n\n");
    printf("Numo 0-9 Syntax:\n");
    printf("0,1 - Binary code (machine language)\n");
    printf("2   - End binary program marker\n");
    printf("3   - Create integer variable\n");
    printf("4   - Create string variable\n");
    printf("5   - Create boolean variable\n");
    printf("6   - Conditional execution\n");
    printf("7   - Input from console\n");
    printf("8   - Mathematical operations\n");
    printf("9   - File operations\n\n");
    printf("Options:\n");
    printf("-d, --debug   Enable debug mode\n");
    printf("-h, --help    Show this help\n");
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
