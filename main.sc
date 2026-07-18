?load stdio.h
!flags = "set_return = 0"
	 "allow_printf_to_be_place = false"
	 "set_print_flag_multible = false"

class main() {
    place("Test");
}

class file_name() {
    save_file_as = "testing.sc"
    path = "./"
    executeable = true
}
