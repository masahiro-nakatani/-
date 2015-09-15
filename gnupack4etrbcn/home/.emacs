
;; ; dot.emacs for shimizu

;;load-path
(expand-file-name "~/.emacs.d/")
(setq load-path (append (list
                         (expand-file-name "~/.emacs")
                         (expand-file-name "~/.emacs.d/")
												 (expand-file-name "~/.emacs.d/auto-install/")
												 (expand-file-name "~/.emacs.d/elpa/")
                         )
                        load-path))

;;for-cygwin
;; @ setup-cygwin
   (setq cygwin-mount-cygwin-bin-directory
         (concat (getenv "CYGWIN_DIR") "\\bin"))
   (require 'setup-cygwin)

;; ;; network proxy
;; ; If you want to use proxy server, set �eurl-proxy-services�f. For example:
;; ;(setq url-proxy-services '(("http" . "localhost:8339")))
(setq url-proxy-services '(("http" . "proxy.osk.jp.omron.net:8080")))

;; ;;auto-install.el
(require 'auto-install)
(setq auto-install-directory (expand-file-name "~/.emacs.d/auto-install/"))
(auto-install-update-emacswiki-package-name t)
(auto-install-compatibility-setup)             ; �݊����m��

;;package.el
(when (require 'package nil t)
	;;�p�b�P�[�W���|�W�g����Marmalade�ƊJ���҉^�c��ELPA��ǉ�
	(add-to-list 'package-archives
		     '("marmalade" . "http://marmalade-repo.org/packages/"))
	(add-to-list 'package-archives '("ELPA" . "http://tromey.com/elpa/"))
	;;�C���X�g�[������f�B���N�g�����w��
	(setq package-user-dir (expand-file-name "~/.emacs.d/elpa/"))
	;;�C���X�g�[�������p�b�P�[�W�Ƀ��[�h�p�X��ʂ��ēǂݍ���
	(package-initialize))

;; redo+.el
;; http://www.emacswiki.org/emacs/download/redo+.el
(require 'redo+)
(global-unset-key "\C-\\")
(global-set-key (kbd "\C-\\") 'redo)
(setq undo-limit 600000)
(setq undo-strong-limit 900000)

;; auto-complete.el
(require 'auto-complete)
(global-auto-complete-mode t)
(define-key ac-complete-mode-map "\C-n" 'ac-next)
(define-key ac-complete-mode-map "\C-p" 'ac-previous)


;; @ migemo/cmigemo
(setq migemo-command (concat (getenv "INST_DIR")
														 "\\app\\cmigemo\\cmigemo"))
(setq migemo-options '("-q" "--emacs"))
(setq migemo-dictionary (concat (getenv "INST_DIR")
																"\\app\\cmigemo\\dict\\utf-8\\migemo-dict"))
(setq migemo-user-dictionary nil)
(setq migemo-regex-dictionary nil)
(setq migemo-use-pattern-alist t)
(setq migemo-use-frequent-pattern-alist t)
(setq migemo-pattern-alist-length 1024)
(setq migemo-coding-system 'utf-8-unix)
(load-library "migemo")
(migemo-init)

;; kogiku
;; find-file�̋���
(require 'kogiku)
(setq kogiku-mode-change-key "\C-e")

;; org-mode
(setq org-startup-truncated nil)
(defun change-truncation()
  (interactive)
  (cond ((eq truncate-lines nil)
         (setq truncate-lines t))
        (t
         (setq truncate-lines nil))))

;; ����o�b�t�@���Ƀf�B���N�g���t�^
(require 'uniquify)
(setq uniquify-buffer-name-style 'forward)
(setq uniquify-buffer-name-style 'post-forward-angle-brackets)
;; (setq uniquify-ignore-buffers-re "*[^*]+*")

;; �o�b�t�@���̍s�ԍ��\��
(global-linum-mode t)

;; �s�ԍ��̃t�H�[�}�b�g
(set-face-attribute 'linum nil :height 0.9)
(setq linum-format "%4d")


;;IME
(setq default-input-method "W32-IME") ;�W��IME�̐ݒ�
(w32-ime-initialize)   ;IME�̏�����
(set-cursor-color "orange")  ;IME OFF���̏����J�[�\���J���[
(setq w32-ime-buffer-switch-p nil) ;�o�b�t�@�؂�ւ�����IME��Ԃ������p��

;IME ON/OFF���̃J�[�\���J���[
(add-hook 'input-method-inactivate-hook
   (lambda() (set-cursor-color "orange")))
(add-hook 'input-method-activate-hook
   (lambda() (set-cursor-color "lightblue")))

;; IME��Ԃ̃��[�h���C���\��
(setq-default w32-ime-mode-line-state-indicator "[Aa]")
(setq w32-ime-mode-line-state-indicator-list '("[Aa]" "[��]" "[Aa]"))

;; �X�N���[���s���i��s���Ƃ̃X�N���[���j
(setq vertical-centering-font-regexp ".*")
(setq scroll-conservatively 35)
(setq scroll-margin 0)
(setq scroll-step 1)

;;wdired-mode
(require 'wdired)
(define-key dired-mode-map "r" 'wdired-change-to-wdired-mode)

;; iswtchb
(iswitchb-mode 1) ;;iswitchb���[�hON
;;; C-f, C-b, C-n, C-p �Ō���؂�ւ��邱�Ƃ��ł���悤�ɁB
(add-hook 'iswitchb-define-mode-map-hook
      (lambda ()
				(define-key iswitchb-mode-map "[tab]" 'iswitchb-next-match)
        (define-key iswitchb-mode-map "\C-n" 'iswitchb-next-match)
        (define-key iswitchb-mode-map "\C-p" 'iswitchb-prev-match)
        (define-key iswitchb-mode-map "\C-f" 'iswitchb-next-match)
        (define-key iswitchb-mode-map "\C-b" 'iswitchb-prev-match)))

;; iswitchb�Ńo�b�t�@�؂�ւ����Ƀo�b�t�@���e��\��
(defadvice iswitchb-exhibit
  (after
   iswitchb-exhibit-with-display-buffer
   activate)
  "�I�����Ă��� buffer �� window �ɕ\�����Ă݂�B"
  (when (and
         (eq iswitchb-method iswitchb-default-method)
         iswitchb-matches)
    (select-window
     (get-buffer-window (cadr (buffer-list))))
    (let ((iswitchb-method 'samewindow))
      (iswitchb-visit-buffer
       (get-buffer (car iswitchb-matches))))
    (select-window (minibuffer-window))))

;; wgrep
;; M-x install-elisp-from-emacswiki wgrep.el
;; ------------------------------------------------------------------------
;; @ wgrep.el
;; grep���猟�����ʂ𒼐ڕҏW
;; C-c C-p
;;   �ҏW���J�n����
;; C-x C-s/C-c C-c/C-c C-e
;;   �ҏW���e���m��
;; C-c C-k
;;   �ҏW�������e��j������wgrep���I������
;; C-x C-q
;;   wgrep���I������
(require 'wgrep nil t)

;; ��`�I��
(cua-mode t)
(setq cua-enable-cua-keys nil) ; ���̂܂܂��� C-x ���؂���ɂȂ��Ă��܂����肷��̂Ŗ�����

;; �s���X�y�[�X�폜
(add-hook 'before-save-hook 'delete-trailing-whitespace)

;general
;;; �X�^�[�g�A�b�v��\��
(setq inhibit-startup-screen t)

;; �X�^�[�g�A�b�v���̃G�R�[�̈惁�b�Z�[�W�̔�\��
(setq inhibit-startup-echo-area-message -1)

;;; �c�[���o�[��\��
(tool-bar-mode 0)

;;; �t�@�C���̃t���p�X���^�C�g���o�[�ɕ\��
(setq frame-title-format
      (format "%%f - Emacs@%s" (system-name)))

;;; �ŋߎg�����t�@�C�������j���[�ɕ\��
(recentf-mode 1)
(setq recentf-max-menu-items 10)
(setq recentf-max-saved-items 10)

(show-paren-mode 1)
(setq-default make-backup-files nil)  ;BackUp File
(setq auto-save-default t)   ;�����ۑ�
(setq line-number-mode t)   ;�s�ԍ��\��
(setq column-number-mode 1)   ;��ԍ��\��
(put 'downcase-region 'disabled nil)  ;�����������s���̊m�F�𖳌���
(global-set-key "\C-x\C-b" 'bs-show)  ;C-xC-b��M-x bs-show�ɕύX
(global-set-key [M-kanji] 'ignore) ;M-kanji�Ƃ��邳���̂�				.				..
(put 'upcase-region 'disabled nil)

;;�L�[�o�C���h
(global-set-key "\C-h" 'backward-delete-char) ;C-h ��BackSpace
(global-set-key "\M-r" 'query-replace);M-r�Œu��
(global-set-key "\C-o" 'other-window) ;C-o�ŕʂ̃E�B���h�E�ֈړ�
(global-set-key "\M-g" 'goto-line)    ;M-g��

;;c-mode�̃R�[�f�B���O�X�^�C��
;(setq c-default-style "linux")
;;�C���f���g�̓^�u�ɂ���
(setq indent-tabs-mode t)
;;�^�u��
(setq default-tab-width 2)

;; �t���[���ʒu
(setq default-frame-alist
      (append (list
        '(width . 80)
        '(height . 50)
        '(top . 0)
        '(left . 0))
       default-frame-alist))
(setq resize-mini-windows t)   ;�~�j�o�b�t�@�g��h�~

;; ;;Windows �� �t�H���g���䂽�ۂ��
;; (when (eq window-system 'w32)
;;   (set-face-attribute 'default nil
;;                       :family "Yutapon coding regular"
;;                       :height 100)
;;   (set-fontset-font nil 'japanese-jisx0208 (font-spec :family "Yutapon coding regular")))


;; �e�[�}
(require 'color-theme)
(color-theme-calm-forest)
