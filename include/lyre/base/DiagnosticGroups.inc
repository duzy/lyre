
#ifdef GET_DIAG_ARRAYS
static const int16_t DiagArrays[] = {
  /* Empty */ -1,
  /* DiagArray2 */ diag::warn_incompatible_analyzer_plugin_api, -1,
  /* DiagArray3 */ diag::warn_fe_backend_plugin, -1,
  /* DiagArray5 */ diag::warn_fe_backend_frame_larger_than, diag::warn_fe_frame_larger_than, -1,
  /* DiagArray6 */ diag::warn_missing_submodule, -1,
  /* DiagArray7 */ diag::warn_fe_inline_asm, -1,
  /* DiagArray11 */ diag::remark_module_build, diag::remark_module_build_done, -1,
  /* DiagArray14 */ diag::warn_fe_override_module, -1,
  /* DiagArray15 */ diag::remark_fe_backend_optimization_remark, -1,
  /* DiagArray16 */ diag::remark_fe_backend_optimization_remark_analysis, -1,
  /* DiagArray17 */ diag::warn_fe_backend_optimization_failure, -1,
  /* DiagArray18 */ diag::remark_fe_backend_optimization_remark_missed, -1,
  /* DiagArray19 */ diag::remark_fe_backend_plugin, -1,
  /* DiagArray20 */ diag::warn_fe_serialized_diag_failure, diag::warn_fe_serialized_diag_merge_failure, -1,
  /* DiagArray21 */ diag::warn_unknown_diag_option, diag::warn_unknown_warning_specifier, -1,
};

static const int16_t DiagSubGroups[] = {
  /* Empty */ -1,
  /* DiagSubGroup0 */ 1, -1,
};

static const char DiagGroupNames[] = {
    "\010absolute\016absolute-value\034analyzer-incompatible-plugin\016backe"
    "nd-plugin\020division-by-zero\022frame-larger-than=\023incomplete-umbre"
    "lla\ninline-asm\015literal-range\004main\020main-return-type\014module-"
    "build\017module-conflict\013newline-eof\017override-module\004pass\015p"
    "ass-analysis\013pass-failed\013pass-missed\025remark-backend-plugin\026"
    "serialized-diagnostics\026unknown-warning-option"
};

#endif // GET_DIAG_ARRAYS


#ifdef GET_DIAG_TABLE
  { /* absolute */                     0, /* Empty */     0, /* DiagSubGroup0 */ 1 },
  { /* absolute-value */               9, /* Empty */     0, /* Empty */         0 },
  { /* analyzer-incompatible-plugin */ 24, /* DiagArray2 */ 1, /* Empty */         0 },
  { /* backend-plugin */               53, /* DiagArray3 */ 3, /* Empty */         0 },
  { /* division-by-zero */             68, /* Empty */     0, /* Empty */         0 },
  { /* frame-larger-than= */           85, /* DiagArray5 */ 5, /* Empty */         0 },
  { /* incomplete-umbrella */          104, /* DiagArray6 */ 8, /* Empty */         0 },
  { /* inline-asm */                   124, /* DiagArray7 */ 10, /* Empty */         0 },
  { /* literal-range */                135, /* Empty */     0, /* Empty */         0 },
  { /* main */                         149, /* Empty */     0, /* Empty */         0 },
  { /* main-return-type */             154, /* Empty */     0, /* Empty */         0 },
  { /* module-build */                 171, /* DiagArray11 */ 12, /* Empty */         0 },
  { /* module-conflict */              184, /* Empty */     0, /* Empty */         0 },
  { /* newline-eof */                  200, /* Empty */     0, /* Empty */         0 },
  { /* override-module */              212, /* DiagArray14 */ 15, /* Empty */         0 },
  { /* pass */                         228, /* DiagArray15 */ 17, /* Empty */         0 },
  { /* pass-analysis */                233, /* DiagArray16 */ 19, /* Empty */         0 },
  { /* pass-failed */                  247, /* DiagArray17 */ 21, /* Empty */         0 },
  { /* pass-missed */                  259, /* DiagArray18 */ 23, /* Empty */         0 },
  { /* remark-backend-plugin */        271, /* DiagArray19 */ 25, /* Empty */         0 },
  { /* serialized-diagnostics */       293, /* DiagArray20 */ 27, /* Empty */         0 },
  { /* unknown-warning-option */       316, /* DiagArray21 */ 30, /* Empty */         0 },
#endif // GET_DIAG_TABLE


#ifdef GET_CATEGORY_TABLE
CATEGORY("", DiagCat_None)
CATEGORY("Lexical Issue", DiagCat_Lexical_Issue)
CATEGORY("Backend Issue", DiagCat_Backend_Issue)
CATEGORY("Inline Assembly Issue", DiagCat_Inline_Assembly_Issue)
CATEGORY("Semantic Issue", DiagCat_Semantic_Issue)
#endif // GET_CATEGORY_TABLE

