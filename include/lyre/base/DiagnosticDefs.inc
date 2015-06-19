#ifdef DIAG

#ifdef DIAGS_FOR_ALL_COMPONENTS
# ifndef DIAGS_FOR_COMMON
# define DIAGS_FOR_COMMON()
# endif
# ifndef DIAGS_FOR_DRIVER
# define DIAGS_FOR_DRIVER()
# endif
# ifndef DIAGS_FOR_DRIVER
# define DIAGS_FOR_DRIVER()
# endif
# ifndef DIAGS_FOR_DRIVER
# define DIAGS_FOR_DRIVER()
# endif
# undef DIAGS_FOR_ALL_COMPONENTS
#endif

#ifdef DIAGS_BEG
# define DIAGS_BEG(UC,LC)
#endif
#ifdef DIAGS_END
# define DIAGS_END(UC,LC)
#endif

#ifdef DIAGS_FOR_COMMON
DIAGS_BEG(COMMON, Common)
DIAG(err_aaa, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "aaa", 0, false, 1)
DIAGS_END(COMMON, Common)
#endif

#ifdef DIAGS_FOR_DRIVER
DIAGS_BEG(DRIVER, Driver)
DIAG(err_drv_missing_argument, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "argument to '%0' is missing (expected %1 value%s1)", 0, false, 0)
DIAG(err_drv_no_such_file, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "no such file or directory: '%0'", 0, false, 0)
DIAG(err_drv_unknown_argument, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "unknown argument: '%0'", 0, false, 0)
DIAGS_END(DRIVER, Driver)
#endif

#ifdef DIAGS_FOR_DRIVER
DIAGS_BEG(DRIVER, Driver)
DIAG(err_drv_missing_argument, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "argument to '%0' is missing (expected %1 value%s1)", 0, false, 0)
DIAG(err_drv_no_such_file, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "no such file or directory: '%0'", 0, false, 0)
DIAG(err_drv_unknown_argument, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "unknown argument: '%0'", 0, false, 0)
DIAGS_END(DRIVER, Driver)
#endif

#ifdef DIAGS_FOR_DRIVER
DIAGS_BEG(DRIVER, Driver)
DIAG(err_drv_missing_argument, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "argument to '%0' is missing (expected %1 value%s1)", 0, false, 0)
DIAG(err_drv_no_such_file, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "no such file or directory: '%0'", 0, false, 0)
DIAG(err_drv_unknown_argument, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "unknown argument: '%0'", 0, false, 0)
DIAGS_END(DRIVER, Driver)
#endif

#endif

#undef DIAGS_BEG
#undef DIAGS_END
#undef DIAG
