#lang racket
(require "l0.rkt" nanopass/base "utils.rkt")

;; lambda lifted LCPS
(define-language L1
  (extends LCPS)
  (Atom (a)
        (- (λ (x* ...) e)))
  (Decl (decl)
        (+ (λ x (x* ...) e))
        (+ (global x))
        (+ (string-constant x c)))
  (Program (p)
           (+ (decl ...)))
  (entry Program))

;; lift lambdas and constant strings
(define-pass lift-everything : LCPS (ir) -> L1 ()
  (definitions
    (define lam-decls (new appendlist%))
    (define cstrings (new appendlist%))
    (define string-constants (new appendlist%))
    (define globals (new appendlist%)))
  (Expr : Expr (ir) -> Expr ())
  (Atom : Atom (ir) -> Atom ()
        [(λ (,x* ...) ,[e])
         (define x (gensym 'lam))
         (send lam-decls push (in-context Decl `(λ ,x (,x* ...) (begin
                                                                  (prim gc ,x* ...)
                                                                  ,e))))
         x]
        [,c (if (string? c)
                (let ([x (gensym 'cstring)]
                      [y (gensym 'string-constant)])
                  (send globals push (in-context Decl `(global ,x)))
                  (send string-constants push (in-context Decl `(string-constant ,y ,c)))
                  (send cstrings push (in-context Expr `(prim init-cstring ,x ,y)))
                  x)
                c)])
  (let ([e (Expr ir)])
    (in-context Program `(,(send globals pop*) ...
                          ,(send string-constants pop*) ...
                          ,(send lam-decls pop*) ...
                          (λ rabbit_toplevel (self)
                            (begin ,(send cstrings pop*) ...
                                   ,e))))))


(define l1-passes (passes l0-passes lift-everything))
(define-syntax-rule (l1-debug e)
  (with-gensym (l1-passes (with-output-language (L0 Expr)
                            `e))))
(provide l1-passes L1 l1-debug)

