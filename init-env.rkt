#lang racket
(provide init-env)

(define (:not x) `(if ,x #f #t))
(define (:when x . bodies) `(if ,x (begin . ,bodies) (prim void)))

(define (init-env expr)
  `(let ([+ (λ (a b)
              ,(:when (:not `(prim integer? ,a))
                      '(display "+: contract violation")
                      '(new)
                      `(display "expected: ")
                  