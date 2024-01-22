#lang racket
(require (prefix-in l0. "l0.rkt")
         (prefix-in l1. "l1.rkt")
         "codegen.rkt"
         "utils.rkt")
(define (compile f e)
  (call-with-output-file f (λ (out)
                             (with-gensym (displayln (codegen (l1.clo-conv (l0.passes e))) out))
                             )
    #:exists 'replace))
#;
(compile  "test.c" `(prim display 2))
#;
(compile "test.c" `(prim display ((λ (x) x) 1)))

#;
(dbg `(λ (x) (λ (y) x)))
#;
(compile `(λ (f x) (λ (y) (f x y))))

#;(compile "test.c" `(letrec ([even? (λ (x) (if (prim zero? x) #t
                                              (odd? (prim - x 1))))]
                            [odd? (λ (x) (if (prim zero? x) #f
                                             (even? (prim - x 1))))])
                     (prim display (even? 9))))

#;(compile "test.c"  `(let ([pos 0])
                        (let ([move (λ (x)
                                      (set! pos (prim + x pos)))])
                          (move 10)
                          (move 100)
                          (move 9)
                          (prim display pos))))
#;
(compile `(prim displayln "hello world"))

(compile "test.c" `(let ([yin ((λ (cc) (prim display "@") cc) (call/cc (λ (cc) cc)))]
                        [yang ((λ (cc) (prim display "*") cc) (call/cc (λ (cc) cc)))])
                    (yin yang)))
#;
(compile "test.c"
         `(letrec ([f (λ (x)
                        (prim display "@") (f x))])
            (f 1)))