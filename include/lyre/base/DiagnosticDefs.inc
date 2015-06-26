#ifdef DIAG

#ifdef DIAGS_FOR_ALL_COMPONENTS
# ifndef DIAGS_FOR_COMMON
# define DIAGS_FOR_COMMON()
# endif
# ifndef DIAGS_FOR_DRIVER
# define DIAGS_FOR_DRIVER()
# endif
# ifndef DIAGS_FOR_FRONTEND
# define DIAGS_FOR_FRONTEND()
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
DIAG(err_target_unknown_abi, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "unknown target ABI '%0'", 0, false, 0)
DIAG(err_target_unknown_cpu, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "unknown target CPU '%0'", 0, false, 0)
DIAG(err_target_unknown_fpmath, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "unknown FP unit '%0'", 0, false, 0)
DIAG(err_target_unknown_triple, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "unknown target triple '%0', please use -triple or -arch", 0, false, 0)
DIAG(err_target_unsupported_fpmath, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "the '%0' unit is not supported with this instruction set", 0, false, 0)
DIAG(err_target_unsupported_unaligned, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "the %0 sub-architecture does not support unaligned accesses", 0, false, 0)
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

#ifdef DIAGS_FOR_FRONTEND
DIAGS_BEG(FRONTEND, Frontend)
DIAG(err_conflicting_module_files, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "module '%0' is defined in both '%1' and '%2'", 0, false, 0)
DIAG(err_conflicting_module_names, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "conflicting module names specified: '-fmodule-name=%0' and '-fmodule-implementation-of %1'", 0, false, 0)
DIAG(err_fe_action_not_available, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "action %0 not compiled in", 0, false, 0)
DIAG(err_fe_backend_frame_larger_than, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "%0", 0, false, 2)
DIAG(err_fe_backend_plugin, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "%0", 0, false, 2)
DIAG(err_fe_cannot_link_module, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Fatal), "cannot link module '%0': %1", 0, false, 0)
DIAG(err_fe_dependency_file_requires_MT, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "-dependency-file requires at least one -MT or -MQ option", 0, false, 0)
DIAG(err_fe_error_backend, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Fatal), "error in backend: %0", 0, false, 0)
DIAG(err_fe_error_opening, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "error opening '%0': %1", 0, false, 0)
DIAG(err_fe_error_reading, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "error reading '%0'", 0, false, 0)
DIAG(err_fe_error_reading_stdin, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "error reading stdin: %0", 0, false, 0)
DIAG(err_fe_expected_clang_command, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "expected a clang compiler command", 0, false, 0)
DIAG(err_fe_expected_compiler_job, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "unable to handle compilation, expected exactly one compiler job in '%0'", 0, false, 0)
DIAG(err_fe_inline_asm, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "%0", 0, false, 3)
DIAG(err_fe_invalid_code_complete_file, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Fatal), "cannot locate code-completion file %0", 0, false, 0)
DIAG(err_fe_invalid_plugin_name, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "unable to find plugin '%0'", 0, false, 0)
DIAG(err_fe_no_pch_in_dir, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "no suitable precompiled header file found in directory '%0'", 0, false, 0)
DIAG(err_fe_pth_file_has_no_source_header, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "PTH file '%0' does not designate an original source header file for -include-pth", 0, false, 0)
DIAG(err_fe_remap_missing_from_file, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Fatal), "could not remap from missing file '%0'", 0, false, 0)
DIAG(err_fe_remap_missing_to_file, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Fatal), "could not remap file '%0' to the contents of file '%1'", 0, false, 0)
DIAG(err_fe_stdout_binary, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Fatal), "unable to change standard output to binary", 0, false, 0)
DIAG(err_fe_unable_to_create_target, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "unable to create target: '%0'", 0, false, 0)
DIAG(err_fe_unable_to_interface_with_target, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "unable to interface with target machine", 0, false, 0)
DIAG(err_fe_unable_to_load_pch, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "unable to load PCH file", 0, false, 0)
DIAG(err_fe_unable_to_load_plugin, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "unable to load plugin '%0': '%1'", 0, false, 0)
DIAG(err_fe_unable_to_open_output, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "unable to open output file '%0': '%1'", 0, false, 0)
DIAG(err_invalid_vfs_overlay, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Fatal), "invalid virtual filesystem overlay file '%0'", 0, false, 0)
DIAG(err_missing_module, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Fatal), "no module named '%0' declared in module map file '%1'", 0, false, 0)
DIAG(err_missing_module_name, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Fatal), "no module name provided; specify one with -fmodule-name=", 0, false, 0)
DIAG(err_missing_vfs_overlay_file, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Fatal), "virtual filesystem overlay file '%0' not found", 0, false, 0)
DIAG(err_module_cannot_create_includes, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "cannot create includes file for module %0: %1", 0, false, 0)
DIAG(err_module_file_invalid, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Fatal), "file '%0' is not a valid precompiled module file", 0, false, 0)
DIAG(err_module_file_not_found, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Fatal), "module file '%0' not found", 0, false, 0)
DIAG(err_module_file_not_module, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Fatal), "AST file '%0' was not built as a module", 0, false, 0)
DIAG(err_module_header_missing, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "%select{|umbrella }0header '%1' not found", 0, false, 0)
DIAG(err_module_map_not_found, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Fatal), "module map file '%0' not found", 0, false, 0)
DIAG(err_module_unavailable, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "module '%0' %select{is incompatible with|requires}1 feature '%2'", 0, false, 0)
DIAG(err_no_submodule, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "no submodule named %0 in module '%1'", 0, false, 0)
DIAG(err_no_submodule_suggest, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "no submodule named %0 in module '%1'; did you mean '%2'?", 0, false, 0)
DIAG(err_relocatable_without_isysroot, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "must specify system root with -isysroot when building a relocatable PCH file", 0, false, 0)
DIAG(err_unknown_analyzer_checker, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "no analyzer checkers are associated with '%0'", 0, false, 0)
DIAG(err_verify_inconsistent_diags, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "'%0' diagnostics %select{expected|seen}1 but not %select{seen|expected}1: %2", 0, false, 0)
DIAG(err_verify_invalid_content, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "invalid expected %0: %1", 0, false, 0)
DIAG(err_verify_invalid_no_diags, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "%select{expected|'expected-no-diagnostics'}0 directive cannot follow %select{'expected-no-diagnostics' directive|other expected directives}0", 0, false, 0)
DIAG(err_verify_invalid_range, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "invalid range following '-' in expected %0", 0, false, 0)
DIAG(err_verify_missing_end, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "cannot find end ('}}') of expected %0", 0, false, 0)
DIAG(err_verify_missing_file, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "file '%0' could not be located in expected %1", 0, false, 0)
DIAG(err_verify_missing_line, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "missing or invalid line number following '@' in expected %0", 0, false, 0)
DIAG(err_verify_missing_regex, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "cannot find start of regex ('{{') in %0", 0, false, 0)
DIAG(err_verify_missing_start, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "cannot find start ('{{') of expected %0", 0, false, 0)
DIAG(err_verify_no_directives, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "no expected directives found: consider use of 'expected-no-diagnostics'", 0, false, 0)
DIAG(note_fe_backend_frame_larger_than, CLASS_NOTE, static_cast<unsigned>(diag::Severity::Fatal), "%0", 0, false, 2)
DIAG(note_fe_backend_optimization_remark_invalid_loc, CLASS_NOTE, static_cast<unsigned>(diag::Severity::Fatal), "could not determine the original source location for %0:%1:%2", 0, false, 0)
DIAG(note_fe_backend_plugin, CLASS_NOTE, static_cast<unsigned>(diag::Severity::Fatal), "%0", 0, false, 2)
DIAG(note_fe_inline_asm, CLASS_NOTE, static_cast<unsigned>(diag::Severity::Fatal), "%0", 0, false, 3)
DIAG(note_fe_inline_asm_here, CLASS_NOTE, static_cast<unsigned>(diag::Severity::Fatal), "instantiated into assembly here", 0, false, 0)
DIAG(note_fixit_applied, CLASS_NOTE, static_cast<unsigned>(diag::Severity::Fatal), "FIX-IT applied suggested code changes", 0, false, 0)
DIAG(note_fixit_failed, CLASS_NOTE, static_cast<unsigned>(diag::Severity::Fatal), "FIX-IT unable to apply suggested code changes", 0, false, 0)
DIAG(note_fixit_in_macro, CLASS_NOTE, static_cast<unsigned>(diag::Severity::Fatal), "FIX-IT unable to apply suggested code changes in a macro", 0, false, 0)
DIAG(note_fixit_unfixed_error, CLASS_NOTE, static_cast<unsigned>(diag::Severity::Fatal), "FIX-IT detected an error it cannot fix", 0, false, 0)
DIAG(note_incompatible_analyzer_plugin_api, CLASS_NOTE, static_cast<unsigned>(diag::Severity::Fatal), "current API version is '%0', but plugin was compiled with version '%1'", 0, false, 0)
DIAG(note_module_def_undef_here, CLASS_NOTE, static_cast<unsigned>(diag::Severity::Fatal), "macro was %select{defined|#undef'd}0 here", 0, false, 0)
DIAG(note_module_file_imported_by, CLASS_NOTE, static_cast<unsigned>(diag::Severity::Fatal), "imported by %select{|module '%2' in }1'%0'", 0, false, 0)
DIAG(note_suggest_disabling_all_checkers, CLASS_NOTE, static_cast<unsigned>(diag::Severity::Fatal), "use -analyzer-disable-all-checks to disable all static analyzer checkers", 0, false, 0)
DIAG(remark_fe_backend_optimization_remark, CLASS_REMARK, static_cast<unsigned>(diag::Severity::Ignored), "%0", 15, false, 2)
DIAG(remark_fe_backend_optimization_remark_analysis, CLASS_REMARK, static_cast<unsigned>(diag::Severity::Ignored), "%0", 16, false, 2)
DIAG(remark_fe_backend_optimization_remark_missed, CLASS_REMARK, static_cast<unsigned>(diag::Severity::Ignored), "%0", 18, false, 2)
DIAG(remark_fe_backend_plugin, CLASS_REMARK, static_cast<unsigned>(diag::Severity::Ignored), "%0", 19, false, 2)
DIAG(remark_module_build, CLASS_REMARK, static_cast<unsigned>(diag::Severity::Ignored), "building module '%0' as '%1'", 11, false, 0)
DIAG(remark_module_build_done, CLASS_REMARK, static_cast<unsigned>(diag::Severity::Ignored), "finished building module '%0'", 11, false, 0)
DIAG(warn_fe_backend_frame_larger_than, CLASS_WARNING, static_cast<unsigned>(diag::Severity::Warning), "%0", 5, false, 2)
DIAG(warn_fe_backend_optimization_failure, CLASS_WARNING, static_cast<unsigned>(diag::Severity::Warning), "%0", 17, false, 2)
DIAG(warn_fe_backend_plugin, CLASS_WARNING, static_cast<unsigned>(diag::Severity::Warning), "%0", 3, false, 2)
DIAG(warn_fe_cc_log_diagnostics_failure, CLASS_WARNING, static_cast<unsigned>(diag::Severity::Warning), "unable to open CC_LOG_DIAGNOSTICS file: %0 (using stderr)", 0, false, 0)
DIAG(warn_fe_cc_print_header_failure, CLASS_WARNING, static_cast<unsigned>(diag::Severity::Warning), "unable to open CC_PRINT_HEADERS file: %0 (using stderr)", 0, false, 0)
DIAG(warn_fe_frame_larger_than, CLASS_WARNING, static_cast<unsigned>(diag::Severity::Warning), "stack frame size of %0 bytes in %q1", 5, false, 2)
DIAG(warn_fe_inline_asm, CLASS_WARNING, static_cast<unsigned>(diag::Severity::Warning), "%0", 7, false, 3)
DIAG(warn_fe_macro_contains_embedded_newline, CLASS_WARNING, static_cast<unsigned>(diag::Severity::Warning), "macro '%0' contains embedded newline; text after the newline is ignored", 0, false, 0)
DIAG(warn_fe_override_module, CLASS_WARNING, static_cast<unsigned>(diag::Severity::Warning), "overriding the module target triple with %0", 14, false, 0)
DIAG(warn_fe_serialized_diag_failure, CLASS_WARNING, static_cast<unsigned>(diag::Severity::Warning), "unable to open file %0 for serializing diagnostics (%1)", 20, false, 0)
DIAG(warn_fe_serialized_diag_merge_failure, CLASS_WARNING, static_cast<unsigned>(diag::Severity::Warning), "unable to merge a subprocess's serialized diagnostics", 20, false, 0)
DIAG(warn_fixit_no_changes, CLASS_NOTE, static_cast<unsigned>(diag::Severity::Fatal), "FIX-IT detected errors it could not fix; no output will be generated", 0, false, 0)
DIAG(warn_incompatible_analyzer_plugin_api, CLASS_WARNING, static_cast<unsigned>(diag::Severity::Warning), "checker plugin '%0' is not compatible with this version of the analyzer", 2, false, 0)
DIAG(warn_missing_submodule, CLASS_WARNING, static_cast<unsigned>(diag::Severity::Warning), "missing submodule '%0'", 6, false, 0)
DIAG(warn_unknown_diag_option, CLASS_WARNING, static_cast<unsigned>(diag::Severity::Warning), "unknown %select{warning|remark}0 option '%1'%select{|; did you mean '%3'?}2", 21, false, 0)
DIAG(warn_unknown_warning_specifier, CLASS_WARNING, static_cast<unsigned>(diag::Severity::Warning), "unknown %0 warning specifier: '%1'", 21, false, 0)
DIAGS_END(FRONTEND, Frontend)
#undef DIAGS_FOR_FRONTEND
#endif

#ifdef DIAGS_FOR_SEMA
DIAGS_BEG(SEMA, Sema)
DIAG(err_unavailable, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "%0 is unavailable", 0, false, 4)
DIAG(err_unavailable_message, CLASS_ERROR, static_cast<unsigned>(diag::Severity::Error), "%0 is unavailable: %1", 0, false, 4)
DIAGS_END(SEMA, Sema)
#undef DIAGS_FOR_SEMA
#endif

#endif

#undef DIAGS_BEG
#undef DIAGS_END
#undef DIAG
