let Component = "Common" in {
    
    // Basic.

    def fatal_too_many_errors
        : Error<"too many errors emitted, stopping now">, DefaultFatal; 
    
    
    let CategoryName = "Lexical Issue" in 
    {
        def err_aaa : Error<"aaa">;
    }

    // Targets

    def err_target_unknown_triple : Error<
      "unknown target triple '%0', please use -triple or -arch">;
    def err_target_unknown_cpu : Error<"unknown target CPU '%0'">;
    def err_target_unknown_abi : Error<"unknown target ABI '%0'">;
    def err_target_unknown_fpmath : Error<"unknown FP unit '%0'">;
    def err_target_unsupported_fpmath : Error<
      "the '%0' unit is not supported with this instruction set">;
    def err_target_unsupported_unaligned : Error<
      "the %0 sub-architecture does not support unaligned accesses">;

    // Source manager
    def err_cannot_open_file : Error<"cannot open file '%0': %1">, DefaultFatal;
    def err_file_modified : Error<
      "file '%0' modified since it was first processed">, DefaultFatal;
    def err_unsupported_bom : Error<"%0 byte order mark detected in '%1', but "
      "encoding is not supported">, DefaultFatal;
    def err_unable_to_rename_temp : Error<
      "unable to rename temporary '%0' to output file '%1': '%2'">;
    def err_unable_to_make_temp : Error<
      "unable to make temporary file: %0">;
}
