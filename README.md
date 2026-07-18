#include -> ?load
printf("") -> place ("")
void -> void_func

example:

Print Hello World:

?load stdio.h
!flags = "set_print_flag_multible = true" //the flag so it can print multible place("")
         "allow_printf_to_be_place = true"
         "set_return = 0" //so you don't have to place under "place("")" the return 0;

class main() {
    place("Hello World");
}

class file_name() {
    save_file_as = "filename.sc"
    path = "path/to/save/file"
    executeable = true //or false if non executable
}