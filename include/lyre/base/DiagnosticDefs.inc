#ifdef DIAG

#ifdef DIAGS_FOR_ALL_COMPONENTS
# ifndef DIAGS_FOR_COMMON
# define DIAGS_FOR_COMMON()
# endif
# ifndef DIAGS_FOR_DRIVER
# define DIAGS_FOR_DRIVER()
# endif
# ifndef DIAGS_FOR_SEMA
# define DIAGS_FOR_SEMA()
# endif
# undef DIAGS_FOR_ALL_COMPONENTS
#endif

#ifndef DIAGS_BEG
# define DIAGS_BEG(UC,LC)
#endif
#ifndef DIAGS_END
# define DIAGS_END(UC,LC)
#endif

#ifdef DIAGS_FOR_COMMON
DIAGS_BEG(COMMON, Common)
DIAG(err_aaa, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "aaa", 0, false, 1)
DIAG(err_cannot_open_file, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Fatal), "cannot open file '%0': %1", 0, false, 0)
DIAG(err_file_modified, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Fatal), "file '%0' modified since it was first processed", 0, false, 0)
DIAG(err_unable_to_make_temp, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "unable to make temporary file: %0", 0, false, 0)
DIAG(err_unable_to_rename_temp, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "unable to rename temporary '%0' to output file '%1': '%2'", 0, false, 0)
DIAG(err_unsupported_bom, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Fatal), "%0 byte order mark detected in '%1', but encoding is not supported", 0, false, 0)
DIAG(fatal_too_many_errors, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Fatal), "too many errors emitted, stopping now", 0, false, 0)
DIAGS_END(COMMON, Common)
#undef DIAGS_FOR_COMMON
#endif

#ifdef DIAGS_FOR_DRIVER
DIAGS_BEG(DRIVER, Driver)
DIAG(err_drv_missing_argument, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "argument to '%0' is missing (expected %1 value%s1)", 0, false, 0)
DIAG(err_drv_no_such_file, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "no such file or directory: '%0'", 0, false, 0)
DIAG(err_drv_unknown_argument, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "unknown argument: '%0'", 0, false, 0)
DIAGS_END(DRIVER, Driver)
#undef DIAGS_FOR_DRIVER
#endif

#ifdef DIAGS_FOR_SEMA
DIAGS_BEG(SEMA, Sema)
DIAG(err_unavailable, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "%0 is unavailable", 0, false, 2)
DIAG(err_unavailable_message, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "%0 is unavailable: %1", 0, false, 2)
DIAGS_END(SEMA, Sema)
#undef DIAGS_FOR_SEMA
#endif

#endif

#undef DIAGS_BEG
#undef DIAGS_END
#undef DIAG
