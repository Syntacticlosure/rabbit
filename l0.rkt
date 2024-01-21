#lang racket
(require "adt.rkt" "utils.rkt" (prefix-in l1. "l1.rkt"))

(adt expr #:public
     [lit (c any/c)]
     [var (n symbol?)]
     [λ (vars (listof symbol?)) (body (listof expr?))] ;; sugar body
     [app (f expr?) (args (listof expr?))]
     [prim (op symbol?) (args (listof expr?))]
     [if (test expr?) (then  expr?) (else expr?)]
     [begin (body (listof expr?))]
     ;; sugars
     [set! (var symbol?) (val expr?)]
     [let (vars (listof symbol?)) (vals (listof expr?)) (body (listof expr?))]
     [letrec (vars (listof symbol?)) (vals (listof expr?)) (body (listof expr?))])

;; Parsers & Unparsers
(: parse (-> any/c expr?))
(define (parse sexp)
  (match sexp
    [(? symbol? n) (expr.var n)]
    [(and c (or (? boolean?) (? integer?) (? string?))) (expr.lit c)]
    [`(quote ,(? symbol? s)) (expr.lit s)] ;; symbol constant
    [`(λ ,vars ,body ...) (expr.λ vars (map parse body))]
    [`(prim ,op ,args ...) (expr.prim op (map parse args))]
    [`(begin ,body ...) (expr.begin (map parse body))]
    [`(if ,test ,then ,else) (expr.if (parse test) (parse then) (parse else))]
    [`(set! ,var ,val) (expr.set! var (parse val))]
    [`(let ,bindings ,body ...) (expr.let (map first bindings)
                                          (map (λ (x) (parse (second x))) bindings)
                                          (map parse body))]
    [`(letrec ,bindings ,body ...) (expr.letrec (map first bindings)
                                                (map (λ (x) (parse (second x))) bindings)
                                                (map parse body))]
    [`(,f ,args ...) (expr.app (parse f)
                               (map parse args))]))

(: unparse (-> expr? any/c))
(define (unparse expr)
  (match expr
    [(expr.lit c) (if (symbol? c) `(quote ,c) c)]
    [(expr.var n) n]
    [(expr.λ vars body) `(λ ,vars ,@(map unparse body))]
    [(expr.app f args) `(,(unparse f) ,@(map unparse args))]
    [(expr.prim op args) `(prim ,op ,@(map unparse args))]
    [(expr.begin body) `(begin ,@(map unparse body))]
    [(expr.if test then else) `(if ,(unparse test) ,(unparse then) ,(unparse else))]
    [(expr.set! var val) `(set! ,var ,(unparse val))]
    [(expr.let vars vals body) `(let ,(map list vars (map unparse vals)) ,@(map unparse body))]
    [(expr.letrec vars vals body) `(letrec ,(map list vars (map unparse vals)) ,@(map unparse body))]))

;; desugarer

(define/contract (expr.map f expr)
  (-> (-> expr? any/c) expr? any/c)
  (define ? f)
  (define (?@ x) (map f x))
  (match expr
    [(expr.lit c) expr]
    [(expr.var n) expr]
    [(expr.λ vars body) (expr.λ vars (?@ body))]
    [(expr.app f args) (expr.app (? f) (?@ args))]
    [(expr.prim op args) (expr.prim op (?@ args))]
    [(expr.begin body) (expr.begin (?@ body))]
    [(expr.if test then else) (expr.if (? test) (? then) (? else))]
    [(expr.set! var val) (expr.set! var (? val))]
    [(expr.let vars vals body) (expr.let vars (?@ vals) (?@ body))]
    [(expr.letrec vars vals body) (expr.letrec vars (?@ vals) (?@ body))]))

;; 
(define/contract (desugar-let+letrec expr)
  (-> expr? expr?)
  (define ? desugar-let+letrec)
  (define (?@ x) (map ? x))
  (match expr
    [(expr.let vars vals body) (expr.app (expr.λ vars (?@ body)) (?@ vals))]
    [(expr.letrec vars vals body) (define uninit-vals (map (λ (_) (expr.lit #f)) vals))
                                  (define var-assignments (map (λ (v val) (expr.set! v (? val)))
                                                               vars vals))
                                  (expr.app (expr.λ vars (append var-assignments (?@ body))) uninit-vals)]
    [_ (expr.map ? expr)]))
                                             
;; turns multi-body into single-body
(define/contract (desugar-body expr)
  (-> expr? expr?)
  (define ? desugar-body)
  (define (?@ x) (map desugar-body x))
  (define (one-element-list? l) (null? (cdr l))) ;; assume l couldn't be empty
  (match expr
    [(expr.lit c) expr]
    [(expr.var n) expr]
    [(expr.λ vars body) (if (one-element-list? body)
                            (expr.λ vars (?@ body))
                            (expr.λ vars (list (expr.begin (?@ body)))))]
    [(expr.let vars vals body) (if (one-element-list? body)
                                   (expr.let vars vals (?@ body))
                                   (expr.let vars vals (list (expr.begin (?@ body)))))]
    [(expr.letrec vars vals body) (if (one-element-list? body)
                                      (expr.letrec vars vals (?@ body))
                                      (expr.letrec vars vals (list (expr.begin (?@ body)))))]
    [_ (expr.map desugar-body expr)]))

;; alpha renaming
;; assume desguar-let+letrec,desugar-body
(define/contract (alpha expr)
  (-> expr? expr?)
  (define (loop expr env)
    (define (? x) (loop x env))
    (define (?@ x) (map ? x))
    (define (ref n) (hash-ref env n (λ () (error (format "variable not found: ~a" n)))))
    (match expr
      [(expr.var n) (expr.var (ref n))]
      [(expr.set! var val) (expr.set! (ref var) (? val))]
      [(expr.λ vars body) (define fresh-vars (map (λ (_) (gensym 'x)) vars))
                          (define env* (for/fold ([env env])
                                                 ([key vars]
                                                  [val fresh-vars])
                                         (hash-set env key val)))
                          (expr.λ fresh-vars (map (λ (e) (loop e env*)) body))]
      [_ (expr.map ? expr)]))
  (loop expr (hasheq 'call/cc 'call/cc ;; keep track global env
                     )))


;; set-eliminiation: assume alpha renamed
(define/contract (desugar-set! expr)
  (-> expr? expr?)
  (define (mutable-vars expr)
    (define mvars (seteq))
    (define (loop expr)
      (match expr
        [(expr.set! var val) (set! mvars (set-add mvars var))
                             (loop val)
                             expr]
        [_ (expr.map loop expr)]))
    (loop expr)
    mvars)
  (define mvars (mutable-vars expr))
  (define (loop expr)
    (define ? loop)
    (define (?@ x) (map ? x))
    (match expr
      [(expr.var n) (if (set-member? mvars n)
                        (expr.prim 'unbox (list expr))
                        expr)]
      [(expr.λ vars body) (define margs (filter (curry set-member? mvars) vars))
                          (define empty-boxes (map (λ (var) (expr.prim 'box (list (expr.var var)))) margs))
                          (if (null? margs)
                              (expr.λ vars (?@ body))
                              (expr.λ vars (list (expr.app (expr.λ margs (?@ body))
                                                           empty-boxes))))]
      [(expr.set! var val) (expr.prim 'set-box! (list (expr.var var) (? val)))]
      [expr (expr.map ? expr)]))
  (loop expr))

;; cpsify
;; cps it, and transform it into L1
;; assume all passes above
(define/contract (cpsify expr)
  (-> expr? l1.expr?)
  ;; cps code from matt might blog
  (define (T-k/list exprlist k)
    (define (loop exprlist acc)
      (if (null? exprlist)
          (k (reverse acc))
          (T-k (car exprlist) (λ (r)
                                (loop (cdr exprlist) (cons r acc))))))
    (loop exprlist '()))
  ;; M : expr? -> l1.atom?
  (define (M expr)
    (match expr
      [(expr.lit c) (l1.atom.lit c)]
      [(expr.var n) (l1.atom.var n)]
      [(expr.λ vars (list body)) (define k (gensym 'k))
                                 (l1.atom.λ (append vars (list k))
                                            (T-c body (l1.atom.var k)))]))
  ;; T-c: expr? l1.atom.var? -> l1.expr?
  (define (T-c expr c)
    (match expr
      [(or (? expr.lit?) (? expr.var?) (? expr.λ?)) ;;atoms
       (l1.expr.app c (list (M expr)))]
      [(expr.app f args) (T-k f (λ (f_)
                                  (T-k/list args (λ (args_)
                                                   (l1.expr.app f_ (append args_ (list c)))))))]
      [(expr.prim op args)
       (define bindvar (gensym 'b))
       (T-k/list args (λ (args_)
                        (l1.expr.prim op args_ bindvar (l1.expr.app c (list (l1.atom.var bindvar))))))]
      [(expr.if test then else)
       (T-k test (λ (test_)
                   (l1.expr.if test_ 
                               (T-c then c)
                               (T-c else c))))]
      [(expr.begin body)
       (define-values (tail nontail) (let ([rbody (reverse body)])
                                       (values (car rbody)
                                               (reverse (cdr rbody)))))
       (T-k/list nontail (λ (nontail_)
                           ;; ignore the value of nontail position
                           (T-c tail c)))]))

  ;; T-k : expr? (-> l1.atom? l1.expr?) -> l1.expr?
  (define (T-k expr k)
    (match expr
      [(or (? expr.lit?) (? expr.var?) (? expr.λ?))
       (k (M expr))]
      [(expr.app f args)
       (define bindvar (gensym 'b))
       (T-k f (λ (f_)
                (T-k/list args (λ (args_)
                                 (l1.expr.app f_ (append args_ (list (l1.atom.λ (list bindvar)
                                                                                (k (l1.atom.var bindvar))))))))))]
      [(expr.prim op args)
       (define bindvar (gensym 'b))
       (T-k/list args (λ (args_)
                        (l1.expr.prim op args_ bindvar (k (l1.atom.var bindvar)))))]
      [(expr.if test then else)
       (T-k test (λ (test_)
                   (l1.expr.if test_ (T-k then k)
                               (T-k else k))))]
      [(expr.begin body)
       (T-k/list body (λ (body_)
                        (k (last body_))))]))
  (T-c expr (l1.atom.halt)))

(: passes (-> any/c l1.expr?))
(define (passes sexp)
  (cpsify (desugar-set! (alpha (desugar-body (desugar-let+letrec (parse sexp)))))))
(: debug-passes (-> any/c any/c))
(define (debug-passes sexp)
  (with-gensym (l1.expr.unparse (passes sexp))))


