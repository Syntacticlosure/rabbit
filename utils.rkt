#lang racket
(provide (all-defined-out))
(define appendlist%
  (class object%
    (super-new)
    (define content '())
    (define/public (push x)
      (set! content (cons x content)))
    (define/public (push* x)
      (for ([x_ (in-list x)])
        (push x_)))
    (define/public (pop-last)
      (begin0 (car content)
              (set! content (cdr content))))
    (define/public (pop*)
      (begin0 (reverse content)
              (set! content '()))) ))

(define gensym-table%
  (class object%
    (super-new)
    (define table (hash))
    (define (replace-invalid-characters s)
      ;; replace invalid characters in identifier(c)
      (regexp-replace* #rx"[-\\?!/+*]" s "_"))
      
    (define/public (gensym x)
      (define str (replace-invalid-characters (symbol->string x)))
      (define count (hash-ref table str 0))
      (begin0 (string->symbol (format "~a~a" str count))
              (set! table (hash-set table str (add1 count)))))))

(define (gensym s)
  (send (gensym-table) gensym s))

(define gensym-table (make-parameter #f))

(define-syntax-rule (with-gensym expr)
  (parameterize ([gensym-table (new gensym-table%)])
    expr))
