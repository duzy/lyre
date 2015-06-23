let Component = "Common" in {
    
    // Basic.

    def fatal_too_many_errors
        : Error<"too many errors emitted, stopping now">, DefaultFatal; 
    
    
    let CategoryName = "Lexical Issue" in 
    {
        def err_aaa : Error<"aaa">;
    }

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
