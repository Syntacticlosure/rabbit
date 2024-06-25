#lang racket
(require "l0.rkt" nanopass/base)
(provide with-base-lib)

(define with-base-lib
  (λ (e)
    (with-output-language (L0 Expr)
      `(letrec ([null? (λ (x) (prim null? x))]
                [raise-error (λ (info x)
                               (prim display info)
                               (prim display ":")
                               (prim display x)
                               (prim newline)
                               (prim exit -1))]
                [pair? (λ (x) (prim pair? x))]
                [car (λ (x) (if (prim pair? x)
                                (prim car x)
                                (raise-error "car: not a pair" x)))]
                [cdr (λ (x) (if (prim pair? x)
                                (prim cdr x)
                                (raise-error "cdr: not a pair" x)))]
                [cons (λ (x y) (prim cons x y))]
                [list? (λ (x) (if (prim null? x)
                                  #t
                                  (if (prim pair? x)
                                      (list? (prim cdr x))
                                      #f)))]
                [zero? (λ (x) (if (prim fixnum? x)
                                  (prim zero? x)
                                  (raise-error "zero?: not a pair" x)))])
         ,e))))
                       
