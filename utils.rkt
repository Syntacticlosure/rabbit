#lang racket
(provide with-gensym gensym :)
(define-syntax-rule (: name contract)
  (provide (contract-out [name contract])))

(define gensym-table (make-parameter #f))

(define-syntax-rule (with-gensym expr)
  (parameterize ([gensym-table (hasheq)])
    expr))

(define (gensym sym)
  (define count (hash-ref (gensym-table) sym 0))
  (begin0 (string->symbol (format "~a~a" sym count))
          (gensym-table (hash-set (gensym-table) sym (add1 count)))))

