(let ((dir (file-name-directory load-file-name)) (mmm nil) (ly nil) (lyt nil))
  (set 'ly  (format "%s/mode.el" dir))
  (set 'lyt (format "%s/template.el" dir))
  (set 'mmm (format "%s/mmm-mode" dir))
  (when (file-exists-p ly)  (load-file ly))
  (when (file-exists-p lyt) (load-file lyt))
  (when (file-exists-p mmm)
    (add-to-list 'load-path mmm)
    (when (load-file (format "%s/mmm-auto.el" mmm))
      (setq mmm-global-mode 'maybe)
      (mmm-add-classes
       '((lyre-embedded-template
          :submode lyre-template-mode
          :face mmm-default-submode-face
          :front "\\<speak\s+template\s+.*?\n---+\n"
          :front-offset 0
          :back "^---+\\>"
          :back-offset 0)
         (lyre-embedded-xml
          :submode sgml-mode ;nxml-mode
          :face mmm-code-submode-face ;mmm-default-submode-face
          :front "\\<speak\s+\\(?XML\\|xml\\)\\>.*?\n---+\n"
          :front-offset 0
          :back "^---+\\>"
          :back-offset 0)
         (lyre-embedded-shell
          :submode shell-script-mode
          :face mmm-code-submode-face ;mmm-default-submode-face
          :front "\\<speak\s+shell\\>.*?\n---+\n"
          :front-offset 0
          :back "^---+\\>"
          :back-offset 0)
         (lyre-embedded-bash
          :submode shell-script-mode
          :face mmm-code-submode-face ;mmm-default-submode-face
          :front "\\<speak\s+bash\\>.*?\n---+\n"
          :front-offset 0
          :back "^---+\\>"
          :back-offset 0)
         (lyre-embedded-perl5
          :submode perl-mode
          :face mmm-code-submode-face
          :front "\\<speak\s+Perl5\\>.*?\n---+\n"
          :front-offset 0
          :back "^---+\\>"
          :back-offset 0)
         (lyre-embedded-perl6
          :submode perl-mode
          :face mmm-code-submode-face
          :front "\\<speak\s+Perl6\\>.*?\n---+\n"
          :front-offset 0
          :back "^---+\\>"
          :back-offset 0)
         (lyre-embedded-python
          :submode python-mode
          :face mmm-code-submode-face
          :front "\\<speak\s+python\\>.*?\n---+\n"
          :front-offset 0
          :back "^---+\\>"
          :back-offset 0)))
      (mmm-add-mode-ext-class 'lyre-mode nil 'lyre-embedded-template)
      (mmm-add-mode-ext-class 'lyre-mode nil 'lyre-embedded-xml)
      (mmm-add-mode-ext-class 'lyre-mode nil 'lyre-embedded-shell)
      (mmm-add-mode-ext-class 'lyre-mode nil 'lyre-embedded-bash)
      (mmm-add-mode-ext-class 'lyre-mode nil 'lyre-embedded-perl5)
      (mmm-add-mode-ext-class 'lyre-mode nil 'lyre-embedded-perl6)
      (mmm-add-mode-ext-class 'lyre-mode nil 'lyre-embedded-python)
      t)))
