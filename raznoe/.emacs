
(global-display-line-numbers-mode)

(defun fontify-frame (frame)
  (set-frame-parameter frame 'font "Monospace-14"))
;; Fontify current frame
(fontify-frame nil)
;; Fontify any future frames
(push 'fontify-frame after-make-frame-functions)

(set-default 'truncate-lines t)

(setq show-paren-delay 0)
(show-paren-mode 1)


