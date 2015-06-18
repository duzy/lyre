#ifdef COMMON_START
__COMMON_START = DIAG_START_COMMON,
#undef COMMON_START
#endif

#ifdef DIAG
DIAG(err_aaa, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "aaa", 0, false, 1)
#undef DIAG
#endif
