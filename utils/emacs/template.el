(defvar lyre-template-text-face 'lyre-template-text-face)
(defface lyre-template-text-face
  '((t (:inherit 'lyre-string-face))) "lyre: template text face"
  :group 'lyre)

(defun lyre-template-expresion-re (re) (format "\\$(.*?%s.*?)" re))
(defun lyre-template-codeblock-re (re) (format "\\${.*?%s.*?}" re))
(defun lyre-template-statement-re (re) (format "^\\..*?%s.*?[\n;]?" re))
(defun lyre-template-expresion-ppre (re) (lyre-template-expresion-re (format "\\<\\(%s\\)\\>" re)))
(defun lyre-template-codeblock-ppre (re) (lyre-template-codeblock-re (format "\\<\\(%s\\)\\>" re)))
(defun lyre-template-statement-ppre (re) (lyre-template-statement-re (format "\\<\\(%s\\)\\>" re)))

(defvar lyre-template-font-lock-defaults
  (let ()
    (list
     (list (lyre-template-expresion-re lyre-re-local-variable-name)         '(1 lyre-variable-name-face)      '(2 lyre-variable-name-bold-face) '(3 lyre-variable-name-face))
     (list (lyre-template-codeblock-re lyre-re-local-variable-name)         '(1 lyre-variable-name-face)      '(2 lyre-variable-name-bold-face) '(3 lyre-variable-name-face))
     (list (lyre-template-statement-re lyre-re-local-variable-name)         '(1 lyre-variable-name-face)      '(2 lyre-variable-name-bold-face) '(3 lyre-variable-name-face))

     (list (lyre-template-expresion-re lyre-re-export-variable-name)        '(1 lyre-variable-name-bold-face) '(2 lyre-variable-name-bold-face) '(3 lyre-variable-name-bold-face))
     (list (lyre-template-codeblock-re lyre-re-export-variable-name)        '(1 lyre-variable-name-bold-face) '(2 lyre-variable-name-bold-face) '(3 lyre-variable-name-bold-face))
     (list (lyre-template-statement-re lyre-re-export-variable-name)        '(1 lyre-variable-name-bold-face) '(2 lyre-variable-name-bold-face) '(3 lyre-variable-name-bold-face))

     (list (lyre-template-expresion-re lyre-re-variable-keyed)              '(1 lyre-constant-face)           '(2 lyre-string-face)             '(3 lyre-constant-face))
     (list (lyre-template-codeblock-re lyre-re-variable-keyed)              '(1 lyre-constant-face)           '(2 lyre-string-face)             '(3 lyre-constant-face))
     (list (lyre-template-statement-re lyre-re-variable-keyed)              '(1 lyre-constant-face)           '(2 lyre-string-face)             '(3 lyre-constant-face))

     (cons (lyre-template-expresion-re (lyre-ppre lyre-keyword-list))         '(1 lyre-keyword-face))
     (cons (lyre-template-codeblock-re (lyre-ppre lyre-keyword-list))         '(1 lyre-keyword-face))
     (cons (lyre-template-statement-re (lyre-ppre lyre-keyword-list))         '(1 lyre-keyword-face))

     (cons (lyre-template-expresion-re (lyre-ppre lyre-builtin-list))         '(1 lyre-builtin-face))
     (cons (lyre-template-codeblock-re (lyre-ppre lyre-builtin-list))         '(1 lyre-builtin-face))
     (cons (lyre-template-statement-re (lyre-ppre lyre-builtin-list))         '(1 lyre-builtin-face))

     (cons (lyre-template-expresion-re lyre-re-colon-keyword)               '(1 lyre-constant-face)) ;;  :keyword
     (cons (lyre-template-codeblock-re lyre-re-colon-keyword)               '(1 lyre-constant-face)) ;;  :keyword
     (cons (lyre-template-statement-re lyre-re-colon-keyword)               '(1 lyre-constant-face)) ;;  :keyword

     (cons (lyre-template-expresion-re lyre-re-export-function-name)        '(1 lyre-function-name-bold-face))
     (cons (lyre-template-codeblock-re lyre-re-export-function-name)        '(1 lyre-function-name-bold-face))
     (cons (lyre-template-statement-re lyre-re-export-function-name)        '(1 lyre-function-name-bold-face))

     (cons (lyre-template-expresion-re lyre-re-local-function-name)         '(1 lyre-function-name-face))
     (cons (lyre-template-codeblock-re lyre-re-local-function-name)         '(1 lyre-function-name-face))
     (cons (lyre-template-statement-re lyre-re-local-function-name)         '(1 lyre-function-name-face))

     (cons (lyre-template-expresion-re lyre-re-export-reference-name)       '(1 lyre-reference-bold-face))
     (cons (lyre-template-codeblock-re lyre-re-export-reference-name)       '(1 lyre-reference-bold-face))
     (cons (lyre-template-statement-re lyre-re-export-reference-name)       '(1 lyre-reference-bold-face))

     (cons (lyre-template-expresion-re lyre-re-local-reference-name)        '(1 lyre-reference-face))
     (cons (lyre-template-codeblock-re lyre-re-local-reference-name)        '(1 lyre-reference-face))
     (cons (lyre-template-statement-re lyre-re-local-reference-name)        '(1 lyre-reference-face))

     (list "^\\(\\.\\)[^;\n]*\\([\n;]?\\)"      '(1 lyre-constant-bold-face) '(2 lyre-constant-face))
     (list "\\(\\$(\\).*?\\()\\)"               '(1 lyre-constant-face)      '(2 lyre-constant-face))
     (list "\\(\\${\\).*?\\(}\\)"               '(1 lyre-constant-face)      '(2 lyre-constant-face))

     (cons "\\(.*?\\)\\$(.*?)"                  '(1 lyre-template-text-face)) ;; the text before $()
     (cons "\\(.*?\\)\\${.*?}"                  '(1 lyre-template-text-face)) ;; the text before ${}
     (cons "\\(?:.*?\\$(.*?)\\)+\\(.+?\\)\n"    '(1 lyre-template-text-face)) ;; the text after $()
     (cons "\\(?:.*?\\${.*?}\\)+\\(.+?\\)\n"    '(1 lyre-template-text-face)) ;; the text after ${}
     (cons "^\\([^\\.][^\n]*\\)"                '(1 lyre-template-text-face)) ;; the text without $() or ${}
     ))
  "Minimal highlighting expressions for LYRE template")

(defvar lyre-template-syntax-table
  (let ((lyre-template-syntax-table (make-syntax-table)))
    (modify-syntax-entry ?\" "." lyre-syntax-table) ;; disable string quote
    ;;(modify-syntax-entry ?\' "." lyre-syntax-table) ;; disable string quote
    lyre-template-syntax-table)
  "Syntax table for Lyre mode. See `Table of Syntax Classes'")

(define-derived-mode lyre-template-mode prog-mode "LyT"
  "Major mode for editing Lyre template files."
  (set-syntax-table lyre-template-syntax-table)
  (set (make-local-variable 'font-lock-defaults) '(lyre-template-font-lock-defaults))
  )

(provide 'lyre-template-mode)
