#lang racket
(require "main.rkt" "../l0.rkt")


#;(run (letrec ([range (λ (start end)
                       (if (prim = start end)
                           (cons end null)
                           (cons start (range (prim + start 1) end))))])
       (prim display (range 1 10))))

(run (letrec ([range (λ (start end)
                       (if (prim = start end)
                           (cons end null)
                           (cons start (range (prim + start 1) end))))]
              [sum (λ (lst)
                     (if (null? lst)
                         0
                         (prim + (sum (cdr lst)) (car lst))))]
                     )
       (prim display (sum (range 1 10)))))