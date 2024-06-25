#lang racket
(require racket/runtime-path)
(define-runtime-path runtime-libs-path "*.c")
(define-runtime-path runtime-headers-path ".")

(define runtime-libs (path->string runtime-libs-path))
(define runtime-headers (path->string runtime-headers-path))

(provide runtime-libs runtime-headers)