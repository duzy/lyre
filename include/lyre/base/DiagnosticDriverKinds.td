let Component = "Driver" in {
    
    def err_drv_unknown_argument : Error<"unknown argument: '%0'">;
    def err_drv_missing_argument : Error<"argument to '%0' is missing (expected %1 value%s1)">;

    def err_drv_no_such_file : Error<"no such file or directory: '%0'">;

}
