
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
;; ; If you want to use proxy server, set ‘url-proxy-services’. For example:
;; ;(setq url-proxy-services '(("http" . "localhost:8339")))
(setq url-proxy-services '(("http" . "proxy.osk.jp.omron.net:8080")))

;; ;;auto-install.el
(require 'auto-install)
(setq auto-install-directory (expand-file-name "~/.emacs.d/auto-install/"))
(auto-install-update-emacswiki-package-name t)
(auto-install-compatibility-setup)             ; 互換性確保

;;package.el
(when (require 'package nil t)
	;;パッケージリポジトリにMarmaladeと開発者運営のELPAを追加
	(add-to-list 'package-archives
		     '("marmalade" . "http://marmalade-repo.org/packages/"))
	(add-to-list 'package-archives '("ELPA" . "http://tromey.com/elpa/"))
	;;インストールするディレクトリを指定
	(setq package-user-dir (expand-file-name "~/.emacs.d/elpa/"))
	;;インストールしたパッケージにロードパスを通して読み込む
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
;; find-fileの強化
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

;; 同一バッファ名にディレクトリ付与
(require 'uniquify)
(setq uniquify-buffer-name-style 'forward)
(setq uniquify-buffer-name-style 'post-forward-angle-brackets)
;; (setq uniquify-ignore-buffers-re "*[^*]+*")

;; バッファ中の行番号表示
(global-linum-mode t)

;; 行番号のフォーマット
(set-face-attribute 'linum nil :height 0.9)
(setq linum-format "%4d")


;;IME
(setq default-input-method "W32-IME") ;標準IMEの設定
(w32-ime-initialize)   ;IMEの初期化
(set-cursor-color "orange")  ;IME OFF時の初期カーソルカラー
(setq w32-ime-buffer-switch-p nil) ;バッファ切り替え時にIME状態を引き継ぐ

;IME ON/OFF時のカーソルカラー
(add-hook 'input-method-inactivate-hook
   (lambda() (set-cursor-color "orange")))
(add-hook 'input-method-activate-hook
   (lambda() (set-cursor-color "lightblue")))

;; IME状態のモードライン表示
(setq-default w32-ime-mode-line-state-indicator "[Aa]")
(setq w32-ime-mode-line-state-indicator-list '("[Aa]" "[あ]" "[Aa]"))

;; スクロール行数（一行ごとのスクロール）
(setq vertical-centering-font-regexp ".*")
(setq scroll-conservatively 35)
(setq scroll-margin 0)
(setq scroll-step 1)

;;wdired-mode
(require 'wdired)
(define-key dired-mode-map "r" 'wdired-change-to-wdired-mode)

;; iswtchb
(iswitchb-mode 1) ;;iswitchbモードON
;;; C-f, C-b, C-n, C-p で候補を切り替えることができるように。
(add-hook 'iswitchb-define-mode-map-hook
      (lambda ()
				(define-key iswitchb-mode-map "[tab]" 'iswitchb-next-match)
        (define-key iswitchb-mode-map "\C-n" 'iswitchb-next-match)
        (define-key iswitchb-mode-map "\C-p" 'iswitchb-prev-match)
        (define-key iswitchb-mode-map "\C-f" 'iswitchb-next-match)
        (define-key iswitchb-mode-map "\C-b" 'iswitchb-prev-match)))

;; iswitchbでバッファ切り替え中にバッファ内容を表示
(defadvice iswitchb-exhibit
  (after
   iswitchb-exhibit-with-display-buffer
   activate)
  "選択している buffer を window に表示してみる。"
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
;; grepから検索結果を直接編集
;; C-c C-p
;;   編集を開始する
;; C-x C-s/C-c C-c/C-c C-e
;;   編集内容を確定
;; C-c C-k
;;   編集した内容を破棄してwgrepを終了する
;; C-x C-q
;;   wgrepを終了する
(require 'wgrep nil t)

;; 矩形選択
(cua-mode t)
(setq cua-enable-cua-keys nil) ; そのままだと C-x が切り取りになってしまったりするので無効化

;; 行末スペース削除
(add-hook 'before-save-hook 'delete-trailing-whitespace)

;general
;;; スタートアップ非表示
(setq inhibit-startup-screen t)

;; スタートアップ時のエコー領域メッセージの非表示
(setq inhibit-startup-echo-area-message -1)

;;; ツールバー非表示
(tool-bar-mode 0)

;;; ファイルのフルパスをタイトルバーに表示
(setq frame-title-format
      (format "%%f - Emacs@%s" (system-name)))

;;; 最近使ったファイルをメニューに表示
(recentf-mode 1)
(setq recentf-max-menu-items 10)
(setq recentf-max-saved-items 10)

(show-paren-mode 1)
(setq-default make-backup-files nil)  ;BackUp File
(setq auto-save-default t)   ;自動保存
(setq line-number-mode t)   ;行番号表示
(setq column-number-mode 1)   ;列番号表示
(put 'downcase-region 'disabled nil)  ;小文字化実行時の確認を無効に
(global-set-key "\C-x\C-b" 'bs-show)  ;C-xC-bをM-x bs-showに変更
(global-set-key [M-kanji] 'ignore) ;M-kanjiとうるさいので				.				..
(put 'upcase-region 'disabled nil)

;;キーバインド
(global-set-key "\C-h" 'backward-delete-char) ;C-h でBackSpace
(global-set-key "\M-r" 'query-replace);M-rで置換
(global-set-key "\C-o" 'other-window) ;C-oで別のウィンドウへ移動
(global-set-key "\M-g" 'goto-line)    ;M-gで

;;c-modeのコーディングスタイル
;(setq c-default-style "linux")
;;インデントはタブにする
(setq indent-tabs-mode t)
;;タブ幅
(setq default-tab-width 2)

;; フレーム位置
(setq default-frame-alist
      (append (list
        '(width . 80)
        '(height . 50)
        '(top . 0)
        '(left . 0))
       default-frame-alist))
(setq resize-mini-windows t)   ;ミニバッファ拡大防止

;; ;;Windows で フォントをゆたぽんに
;; (when (eq window-system 'w32)
;;   (set-face-attribute 'default nil
;;                       :family "Yutapon coding regular"
;;                       :height 100)
;;   (set-fontset-font nil 'japanese-jisx0208 (font-spec :family "Yutapon coding regular")))


;; テーマ
(require 'color-theme)
(color-theme-calm-forest)
