#lang racket
(require "main.rkt")

#;(comp (begin
        (prim gc-debug-mode)
            (letrec ([f (λ (times)
                          (prim make-vector 1024)
                          (prim display "*")
                          (if (prim zero? times) #t
                              (f (prim - times 1))))])
              (prim display (f 10000)))))

#;(comp (let ((yin ((λ (foo) (prim display "@") foo) (call/cc (λ (bar) bar)))))
             (let ((yang ((λ (foo) (prim display "*") foo) (call/cc (λ (bar) bar)))))
             (yin yang))))

