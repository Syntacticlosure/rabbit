#lang racket
(require "main.rkt" "../l1.rkt" "../l0.rkt" rackunit)

(check-equal? (run (prim display 2)) "2")
(check-equal? (run (prim display ((λ (x) x) 1))) "1")

(check-equal? (run (let ([f (λ (x) (prim + x 1))])
  (prim display (f 3)))) "4")

(check-equal? (run (letrec ([even? (λ (x) (if (prim zero? x) #t
                                                (odd? (prim - x 1))))]
                              [odd? (λ (x) (if (prim zero? x) #f
                                               (even? (prim - x 1))))])
                       (prim display (even? 9)))) "#f")

(check-equal? (run (let ([pos 0])
                       (let ([move (λ (x)
                                     (set! pos (prim + x pos)))])
                         (move 10)
                         (move 100)
                         (move 9)
                         (prim display pos)))) "119")