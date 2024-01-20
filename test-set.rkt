#lang racket
(require (prefix-in l0. "l0.rkt")
         (prefix-in l1. "l1.rkt")
         "utils.rkt")
(define (dbg e)
  (match-define (list x y) (with-gensym (l1.clo-conv (l0.passes e))))
  (list (l1.expr.unparse x) (for/hasheq ([(k v) (in-hash y)])
                              (values k (l1.atom.unparse v)))))
#;
(dbg `(λ (x) x))
#;
(dbg `(λ (x) (λ (y) x)))
#;
(dbg `(λ (f x) (λ (y) (f x y))))
(dbg `(letrec ([even? (λ (x) (if (prim zero? x) #t
                                          (odd? (prim sub1 x))))]
                        [odd? (λ (x) (if (prim zero? x) #f
                                         (even? (prim sub1 x))))])
                 (even? 9)))
(dbg  `(let ([pos 0])
                 (let ([move (λ (x)
                               (set! pos (prim + x pos)))])
                   (move 10)
                   (move 100)
                   (move 9)
                   (prim displayln pos))))