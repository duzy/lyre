;;
;;      Duzy Chan <code@duzy.info>
;; 
(require 'perl-mode)

(defvar lyre-keyword-face 'lyre-keyword-face)
(defface lyre-keyword-face
  '((t (:inherit 'font-lock-keyword-face))) "lyre: keyword face"
  :group 'lyre)

(defvar lyre-builtin-face 'lyre-builtin-face)
(defface lyre-builtin-face
  '((t (:inherit 'font-lock-builtin-face))) "lyre: builtin face"
  :group 'lyre)

(defvar lyre-warning-face 'lyre-warning-face)
(defface lyre-warning-face
  '((t (:inherit 'font-lock-warning-face))) "lyre: warning face"
  :group 'lyre)

(defvar lyre-string-face 'lyre-string-face)
(defface lyre-string-face
  '((t (:inherit 'font-lock-string-face))) "lyre: string face"
  :group 'lyre)

(defvar lyre-string-bold-face 'lyre-string-bold-face)
(defface lyre-string-bold-face
  '((t (:inherit 'lyre-string-face :weight bold))) "lyre: bold string face"
  :group 'lyre)

(defvar lyre-type-face 'lyre-type-face)
(defface lyre-type-face
  '((t (:inherit 'font-lock-type-face))) "lyre: type face"
  :group 'lyre)

(defvar lyre-type-bold-face 'lyre-type-bold-face)
(defface lyre-type-bold-face
  '((t (:inherit 'lyre-type-face :weight bold))) "lyre: bold type face"
  :group 'lyre)

(defvar lyre-light-gray-background-face 'lyre-light-gray-background-face)
(defface lyre-light-gray-background-face
  '((t (:background "LightGray"))) "lyre: LightGray background face"
  :group 'lyre)

(defvar lyre-variable-name-face 'lyre-variable-name-face)
(defface lyre-variable-name-face
  '((t (:inherit 'font-lock-variable-name-face))) "lyre: variable face"
  :group 'lyre)

(defconst vc (face-foreground font-lock-variable-name-face))
(defvar lyre-variable-name-bold-face 'lyre-variable-name-bold-face)
(defface lyre-variable-name-bold-face
  '((t (:inherit 'lyre-variable-name-face :weight bold))) "lyre: bold variable face"
  :group 'lyre)

(defvar lyre-function-name-face 'lyre-function-name-face)
(defface lyre-function-name-face
  '((t (:inherit 'font-lock-function-name-face))) "lyre: function face"
  :group 'lyre)

(defvar lyre-function-name-bold-face 'lyre-function-name-bold-face)
(defface lyre-function-name-bold-face
  '((t (:inherit 'lyre-function-name-face :weight bold))) "lyre: bold function face"
  :group 'lyre)

(defvar lyre-reference-face 'lyre-reference-face)
(defface lyre-reference-face
  '((t (:inherit 'font-lock-reference-face :foreground "DimGray"))) ;;DarkGray, DimGray, gray
  "lyre: reference face"
  :group 'lyre)

(defvar lyre-reference-bold-face 'lyre-reference-bold-face)
(defface lyre-reference-bold-face
  '((t (:inherit 'lyre-reference-bold-face :weight bold))) "lyre: bold reference face"
  :group 'lyre)

(defvar lyre-constant-face 'lyre-constant-face)
(defface lyre-constant-face
  '((t (:inherit 'font-lock-constant-face))) "lyre: constant face"
  :group 'lyre)

(defvar lyre-constant-bold-face 'lyre-constant-bold-face)
(defface lyre-constant-bold-face
  '((t (:inherit 'lyre-constant-face :weight bold))) "lyre: bold constant face"
  :group 'lyre)

;; font-lock-builtin-face 	font-lock-comment-delimiter-face
;; font-lock-comment-face 	font-lock-constant-face
;; font-lock-doc-face 	font-lock-function-name-face
;; font-lock-keyword-face 	font-lock-negation-char-face
;; font-lock-preprocessor-face 	font-lock-reference-face
;; font-lock-string-face 	font-lock-syntactic-face-function
;; font-lock-type-face 	font-lock-variable-name-face
;; font-lock-warning-face

(defvar lyre-keyword-list
  '("decl"
    "language"
    "speak"
    "type"
    "proc"
    "is"
    "see"
    "with"
    "per" ; "in" is a special case
    "return"
    ))

(defvar lyre-builtin-list
  '())

(defvar lyre-re-local-variable-name     "\\([$@%]\\)\\([.]?\\)\\([a-z_][A-Za-z_0-9:]*\\)")
(defvar lyre-re-export-variable-name    "\\([$@%]\\)\\([.]?\\)\\([A-Z][A-Za-z_0-9:]*\\)")
(defvar lyre-re-variable-keyed          "[$@%][.]?[A-Za-z_][A-Za-z_0-9:]*\\(<\\)\\(.*?\\)\\(>\\)")
(defvar lyre-re-colon-keyword           "\s\\(:[A-Za-z_][A-Za-z_0-9]*\\)\\>")
(defvar lyre-re-export-function-name    "\\([A-Z][A-Za-z_0-9]*\\)(")
(defvar lyre-re-local-function-name     "\\([a-z_][A-Za-z_0-9]*\\)(")
(defvar lyre-re-lang-name               "lang\s+\\([a-z_][A-Za-z_0-9]*\\)")
(defvar lyre-re-method-name             "method\s+\\([A-Za-z_][A-Za-z_0-9]*\\)\s*:")
(defvar lyre-re-export-reference-name   "\\([A-Z][A-Za-z_0-9]*\\)")
(defvar lyre-re-local-reference-name    "\\([a-z_][A-Za-z_0-9]*\\)")

(defun lyre-ppre (re) (format "\\<\\(%s\\)\\>[^_]" (regexp-opt re)))
(defun lyre-idre () nil)

(defvar lyre-font-lock-defaults
  (let ()
    (list
     (list lyre-re-local-variable-name            '(1 lyre-variable-name-face)      '(2 lyre-variable-name-bold-face) '(3 lyre-variable-name-face))
     (list lyre-re-export-variable-name           '(1 lyre-variable-name-bold-face) '(2 lyre-variable-name-bold-face) '(3 lyre-variable-name-bold-face))
     (list lyre-re-variable-keyed                 '(1 lyre-constant-face)           '(2 lyre-string-face)             '(3 lyre-constant-face))

     ;; FIXME: '(2 lyre-light-gray-background-face) is not working
     (list "[^A-Za-z_0-9]\\(<\\)\\(.*?\\)\\(>\\)" '(1 lyre-string-bold-face) '(2 lyre-light-gray-background-face) '(3 lyre-string-bold-face))
     
     (cons "\\(?:class\\|template\\)\s+\\([A-Z][A-Za-z_0-9]*\\)"  '(1 lyre-type-bold-face))
     (cons "\\(?:class\\|template\\)\s+\\([a-z_][A-Za-z_0-9]*\\)" '(1 lyre-type-face))

     (cons lyre-re-method-name               '(1 lyre-function-name-face))
     (cons lyre-re-lang-name                 '(1 lyre-reference-bold-face))

     (cons "\\(?:speak\\|per\\).*?\\(in\\)"  '(1 lyre-keyword-face))

     (cons (lyre-ppre lyre-keyword-list)     '(1 lyre-keyword-face))
     (cons (lyre-ppre lyre-builtin-list)     '(1 lyre-builtin-face))
     
     (cons lyre-re-colon-keyword             '(1 lyre-constant-face)) ;;  :keyword
     (cons lyre-re-export-function-name      '(1 lyre-function-name-bold-face))
     (cons lyre-re-local-function-name       '(1 lyre-function-name-face))
     (cons lyre-re-export-reference-name     '(1 lyre-reference-bold-face))
     (cons lyre-re-local-reference-name      '(1 lyre-reference-face))
     ))
  "Minimal highlighting expressions for LYRE mode")

(defvar lyre-syntax-table
  (let ((lyre-syntax-table (make-syntax-table)))
    (modify-syntax-entry ?\" "\"" lyre-syntax-table) ;; double-quote used as string quote (also prog-mode defaults)
    (modify-syntax-entry ?\' "\"" lyre-syntax-table) ;; single-quote used as string quote
    (modify-syntax-entry ?_ "_" lyre-syntax-table)   ;; name constituents
    (modify-syntax-entry ?\\ "\\" lyre-syntax-table) ;; name constituents
    (modify-syntax-entry ?# "<" lyre-syntax-table)   ;; '#' starts a comment
    (modify-syntax-entry ?\n ">" lyre-syntax-table)  ;; per-line comments
    ;;(modify-syntax-entry ?< "(" lyre-syntax-table)  ;; treat '<' as open-parenthesis
    ;;(modify-syntax-entry ?> ")" lyre-syntax-table)  ;; treat '>' as close-parenthesis
    ;;(modify-syntax-entry ?$ "|" lyre-syntax-table)
    lyre-syntax-table)
  "Syntax table for LYRE mode. See `Table of Syntax Classes'")

(defvar lyre-other-file-alist
  '(("\\.LY$" (".LY"))
    ("\\.ly$" (".ly"))
    ("\\.lyre$" (".lyre")))
  "Alist of extensions to find given the current file's extension")

(defvar lyre-mode-hook nil
  "Normal hook to run when entering LYRE mode.")

(defvar lyre-mode-map
  (let ((lyre-mode-map (make-sparse-keymap)))
    (define-key lyre-mode-map [S-iso-lefttab] 'ff-find-other-file)
    (define-key lyre-mode-map [tab] 'lyre-indent-command)
    (define-key lyre-mode-map "\C-i" 'lyre-indent-command)
    lyre-mode-map)
  "Keymap for LYRE major mode")

(defun lyre-editing-template-p ()
  (or (looking-at "^---+")
      (looking-at "^---+[:space:]*end")
      (looking-back "^---+\n")
      (let ((tmp nil) (beg nil) (end nil) (lang nil))
        (save-excursion
          (when (set 'beg (re-search-backward "^---+\n" nil t))
            (when (looking-back "lang[:space:]+\\([A-Za-z_0-9]+\\)[:space:]+.*?\n")
              (set 'lang (match-data 1)) (message lang))
            (set 'tmp (re-search-forward "^---+[:space:]*end" nil t))))
        (set 'end (save-excursion (re-search-forward "^---+[:space:]*end" nil t)))
        (and beg (or (and tmp end (= tmp end)) (and (not tmp) (not end)))))
      nil))

(defun lyre-indent-command (&optional arg)
  "Indent LYRE code in the active region or current line."
  (interactive)
  (if (lyre-editing-template-p)
      (progn
       (message "template"))
    (perl-indent-command arg)))

(define-derived-mode lyre-mode prog-mode "LYRE"
  "Major mode for editing .lyre files."
  (set-syntax-table lyre-syntax-table)
  (set (make-local-variable 'font-lock-defaults) '(lyre-font-lock-defaults))
  (set (make-local-variable 'ff-other-file-alist) 'lyre-other-file-alist)
  (set (make-local-variable 'comment-start) "#*")
  (set (make-local-variable 'comment-end) "*#")
  (set (make-local-variable 'comment-padding) "")
  ;(add-to-list 'align-c++-modes 'lyre-mode)
  )

(progn
  (add-to-list 'auto-mode-alist '("\\.LY\\'" . lyre-mode))
  (add-to-list 'auto-mode-alist '("\\.ly\\'" . lyre-mode))
  (add-to-list 'auto-mode-alist '("\\.lyre\\'" . lyre-mode)))

(provide 'lyre-mode)

;; (add-hook 'find-file-hook
;;           (lambda ()
;;             (unless (functionp 'mo-mode)
;;                 (let ((file "~/tools/a/work/mo/ll/lyre/mode.el"))
;;                   (when (file-exists-p file) (load-file file))))))
