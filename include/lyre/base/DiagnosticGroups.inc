
#ifdef GET_DIAG_ARRAYS
static const int16_t DiagArrays[] = {
  /* Empty */ -1,
};

static const int16_t DiagSubGroups[] = {
  /* Empty */ -1,
  /* DiagSubGroup0 */ 1, -1,
};

static const char DiagGroupNames[] = {
    "\010absolute\016absolute-value\020division-by-zero\015literal-range\004"
    "main\020main-return-type\013newline-eof"
};

#endif // GET_DIAG_ARRAYS


#ifdef GET_DIAG_TABLE
  { /* absolute */         0, /* Empty */     0, /* DiagSubGroup0 */ 1 },
  { /* absolute-value */   9, /* Empty */     0, /* Empty */         0 },
  { /* division-by-zero */ 24, /* Empty */     0, /* Empty */         0 },
  { /* literal-range */    41, /* Empty */     0, /* Empty */         0 },
  { /* main */             55, /* Empty */     0, /* Empty */         0 },
  { /* main-return-type */ 60, /* Empty */     0, /* Empty */         0 },
  { /* newline-eof */      77, /* Empty */     0, /* Empty */         0 },
#endif // GET_DIAG_TABLE


#ifdef GET_CATEGORY_TABLE
CATEGORY("", DiagCat_None)
CATEGORY("Lexical Issue", DiagCat_Lexical_Issue)
CATEGORY("Semantic Issue", DiagCat_Semantic_Issue)
#endif // GET_CATEGORY_TABLE

