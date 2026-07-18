#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define VERSION "1.0.0"
#define MAX_LINE 512

int main(int argc, char *argv[]) {
    // 1. Check for --version flag
    if (argc == 2 && strcmp(argv[1], "--version") == 0) {
        printf("Simple C Compiler (scc) version %s\n", VERSION);
        return 0;
    }

    if (argc < 2) {
        printf("Usage:\n  %s <filename.sc>\n  %s <filename.sc> -o <output_name>\n  %s --version\n", argv[0], argv[0], argv[0]);
        return 1;
    }

    char *source_path = argv[1];
    char binary_output_name[128] = "";
    bool compile_to_binary = false;

    // 2. Check for -o flag
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            strcpy(binary_output_name, argv[i + 1]);
            compile_to_binary = true;
            break;
        }
    }

    FILE *source = fopen(source_path, "r");
    if (!source) {
        printf("Error: Could not open source file %s\n", source_path);
        return 1;
    }

    char output_filename[128] = "output.c";
    bool set_return_zero = false;
    bool in_file_config = false;

    // First Pass: Scan for internal config flags
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), source)) {
        if (strstr(line, "\"set_return = 0\"")) {
            set_return_zero = true;
        }
        if (strstr(line, "class file_name()")) {
            in_file_config = true;
        }
        if (in_file_config) {
            char *file_as = strstr(line, "save_file_as =");
            if (file_as && !compile_to_binary) { // Only use fallback if -o wasn't passed
                char quote_content[64];
                if (sscanf(file_as, "save_file_as = \"%[^\"]\"", quote_content) == 1) {
                    char *dot = strrchr(quote_content, '.');
                    if (dot && strcmp(dot, ".sc") == 0) {
                        strcpy(dot, ".c");
                    }
                    strcpy(output_filename, quote_content);
                }
            }
            if (strstr(line, "}")) {
                in_file_config = false;
            }
        }
    }

    // If -o flag was used, override the intermediate C filename match
    if (compile_to_binary) {
        snprintf(output_filename, sizeof(output_filename), "%s_temp.c", binary_output_name);
    }

    rewind(source);

    FILE *out = fopen(output_filename, "w");
    if (!out) {
        printf("Error: Could not create intermediate file %s\n", output_filename);
        fclose(source);
        return 1;
    }

    in_file_config = false;
    bool in_main = false;

    // Second Pass: Transpile Simple C code to Standard C
    while (fgets(line, sizeof(line), source)) {
        if (strstr(line, "!flags") || strstr(line, "\"set_print_flag") || strstr(line, "\"allow_printf") || strstr(line, "\"set_return")) {
            continue;
        }
        if (strstr(line, "class file_name()")) {
            in_file_config = true;
            continue;
        }
        if (in_file_config) {
            if (strstr(line, "}")) {
                in_file_config = false;
            }
            continue;
        }

        char *load_pos = strstr(line, "?load");
        if (load_pos) {
            char library[64];
            if (sscanf(load_pos, "?load %s", library) == 1) {
                fprintf(out, "#include <%s>\n", library);
                continue;
            }
        }

        if (strstr(line, "class main()")) {
            fprintf(out, "int main()");
            char *brace = strstr(line, "{");
            if (brace) fprintf(out, " {\n");
            else fprintf(out, "\n");
            in_main = true;
            continue;
        }

        char *place_pos = strstr(line, "place");
        if (place_pos) {
            char content[256];
            char *start_quote = strchr(place_pos, '"');
            if (start_quote) {
                char *end_quote = strchr(start_quote + 1, '"');
                if (end_quote) {
                    size_t len = end_quote - (start_quote + 1);
                    strncpy(content, start_quote + 1, len);
                    content[len] = '\0';
                    
                    int spaces = place_pos - line;
                    for (int i = 0; i < spaces; i++) fprintf(out, " ");
                    
                    fprintf(out, "printf(\"%s\\n\");\n", content);
                    continue;
                }
            }
        }

        if (in_main && strstr(line, "}")) {
            if (set_return_zero) {
                fprintf(out, "    return 0;\n");
            }
            fprintf(out, "}\n");
            in_main = false;
            continue;
        }

        fprintf(out, "%s", line);
    }

    fclose(source);
    fclose(out);

    // 3. Perform Compilation if -o was supplied
    if (compile_to_binary) {
        char command[512];
        // Build standard gcc compile string
        snprintf(command, sizeof(command), "gcc %s -o %s", output_filename, binary_output_name);
        
        int ret = system(command);
        
        // Clean up intermediate .c file
        remove(output_filename);

        if (ret == 0) {
            printf("[Success] Compiled directly into executable binary: %s\n", binary_output_name);
        } else {
            printf("[Error] GCC compilation failed.\n");
            return 1;
        }
    } else {
        printf("[Success] Transpiled intermediate file: %s\n", output_filename);
    }

    return 0;
}