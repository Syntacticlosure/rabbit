#lang racket
(require "../l0.rkt" "../codegen.rkt")
(provide run comp)

(define-syntax-rule (run e)
  (let ()
    (compile "test.c" e)
    (gcc "test.c" "test")
    (match-define (list stdout stdin pid stderr ctl) (process "test.exe"))
    (ctl 'wait)
    (close-output-port stdin)
    (close-input-port stderr)
    (port->string stdout #:close? #t)))

(define-syntax-rule (comp e)
  (let ()
    (compile "test.c" e)
    (gcc "test.c" "test")))

