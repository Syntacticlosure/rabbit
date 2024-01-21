#lang racket
(require (prefix-in l0. "l0.rkt")
         (prefix-in l1. "l1.rkt")
         "utils.rkt")
(define (compile e)
  (with-gensym (displayln (l1.codegen (l1.clo-conv (l0.passes e)))))
  )
#;
(compile `(λ (x) x))
#;
(dbg `(λ (x) (λ (y) x)))
#;
(dbg `(λ (f x) (λ (y) (f x y))))
#;
(compile `(letrec ([even? (λ (x) (if (prim zero? x) #t
                                     (odd? (prim sub1 x))))]
                   [odd? (λ (x) (if (prim zero? x) #f
                                    (even? (prim sub1 x))))])
            (prim displayln (even? 9))))
#;
(compile  `(let ([pos 0])
             (let ([move (λ (x)
                           (set! pos (prim + x pos)))])
               (move 10)
               (move 100)
               (move 9)
               (prim displayln pos))))
#;
(compile `(prim displayln "hello world"))

(compile `(let ((yin
                 ((λ (cc) (prim display "@") cc) (call/cc (λ (c) c))))
                (yang
                 ((λ (cc) (prim display "*") cc) (call/cc (λ (c) c)))))
            (yin yang)))