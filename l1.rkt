#lang racket
(require "adt.rkt" "utils.rkt" racket/control)

(adt.together
 (adt atom #:public
      ;; atoms
      [lit (c any/c)]
      [var (n symbol?)]
      [λ (vars (listof symbol?)) (body expr?)]
      [halt])
 (adt expr #:public
      [app (f atom?) (args (listof atom?))]
      [prim (op symbol?) (args (listof atom?)) (bind symbol?) (cont expr?)]
      [if (test atom?) (then expr?) (else expr?)]))

(: atom.unparse (-> atom? any/c))
(define (atom.unparse atom)
  (match atom
    [(atom.lit c) (if (symbol? c)
                      `(quote ,c)
                      c)]
    [(atom.var n) n]
    [(atom.λ vars body) `(λ ,vars ,(expr.unparse body))]
    [(atom.halt) `halt]))

(: expr.unparse (-> expr? any/c))
(define (expr.unparse expr)
  (match expr
    [(expr.app f args) `(,(atom.unparse f) ,@(map atom.unparse args))]
    [(expr.prim op args bind cont) `(prim ,op ,(map atom.unparse args) ,bind ,(expr.unparse cont))]
    [(expr.if test then else) `(if ,(atom.unparse test) ,(expr.unparse then)
                                   ,(expr.unparse else))]))


;; Closure conversion
(: clo-conv (-> expr? (list/c expr? (hash/c symbol? atom.λ?))))
(define (clo-conv expr)
  ;; tasks of closure conversion
  ;; eliminate lambda atoms by prim `make-closure`, modify lam and app 's arguments to pass activation record
  ;; lambda lifting
  ;; generate bindings for shallow free variables
  ;; create a new activation record extends the old one
  (define global-env (hasheq))
  
  ;; get shallow free variables
  (define (varrefs expr)
    (define (loop/a atom)
      (match atom
        [(or (? atom.lit?) (? atom.λ?)
             (? atom.halt?))
         (seteq)]
        [(atom.var n) (seteq n)]))
    (define (loop/a/list atom-list)
      (foldr set-union (seteq) (map loop/a atom-list)))
    (define (loop/e expr)
      (match expr
        [(expr.app f args) (loop/a/list (cons f args))]
        [(expr.if test then else) (set-union (loop/a test)
                                             (loop/e then)
                                             (loop/e else))]
        [(expr.prim op args bind cont) (set-union (loop/a/list args)
                                                  (set-remove (loop/e cont) bind))]))
    (loop/e expr))
  ;; build activiation record
  ;; activation record is a vector of length n+1
  ;; 0 -> parent stack frame
  ;; build-activation-record: atom.var? (listof symbol?) -> atom.var?
  (define (build-activation-record parent vars)
    (define len (add1 (length vars)))
    (define env-sym (gensym 'env))
    (define env (atom.var env-sym))
    (shift k
           (define body (for/fold ([body (k env)])
                                  ([(var i) (in-indexed vars)])
                          (define effect (gensym '_))
                          (expr.prim 'vector-set! (list env (atom.lit (add1 i)) (atom.var var)) effect body)))
           (expr.prim 'make-vector (list (atom.lit len) (atom.lit #f)) env-sym
                      (expr.prim 'vector-set! (list env (atom.lit 0) parent) (gensym '_)
                                 body))))

  ;; get the index of variable in stackframe
  (define (find-in-stackframe var frame)
    (call/ec (λ (bk)
               (for ([(v i) (in-indexed frame)])
                 (when (eqv? var v)
                   (bk (add1 i))))
               #f)))
  ;; generate-freevar-bindings: (listof symbol?) atom.var? stack? -> void?
  (define (generate-freevar-bindings vars env stack)
    ;; returns a path to the free variable
    ;; `(n i) , n times access parent stack frame, i variable index
    (define (path var)
      (define (loop stack n)
        (define i (find-in-stackframe var (car stack)))
        (if i (list n i)
            (loop (cdr stack) (add1 n))))
      (loop stack 0))
    (define paths (map path vars))
    (define max-parent-depth (apply max (map first paths)))
    ;; generate necessary parent-env access
    ;; generate-envs : -> (listof atom.var?)
    (define (generate-envs)
      (shift k
             (define env-syms (for/list ([i (in-range 1 (add1 max-parent-depth))])
                                (gensym 'env)))
             (define body (k (cons env (map atom.var env-syms))))
             (define klist (for/fold ([klist '()]
                        [env env]
                        #:result klist)
                       ([env-sym env-syms])
               (values (cons (λ (body) (expr.prim 'vector-ref (list env (atom.lit 0)) env-sym body)) klist)
                       (atom.var env-sym))))
             (foldr (λ (k body)
                      (k body)) body (reverse klist))))
    ;; generate bindings
    (define (generate-bindings envs)
      (shift k
             (for/fold ([body (k (void))])
                       ([v vars]
                        [path paths])
               (match-define (list n i) path)
               (expr.prim 'vector-ref (list (list-ref envs n) (atom.lit i)) v body))))
    (define envs (generate-envs))
    (generate-bindings envs))
    
  (define (build-closure fn env)
    (define fn-sym (gensym 'scheme_fun))
    (set! global-env (hash-set global-env fn-sym fn))
    (define clo (gensym 'clo))
    (shift k
           (expr.prim 'make-closure (list (atom.var fn-sym) env) clo (k (atom.var clo)))))

  (define (conv/a atom env stack)
    (match atom
      [(atom.λ vars body)
       (define freevars (set->list (set-subtract (varrefs body) (list->seteq vars))))
       (define env-sym (gensym 'env))
       (build-closure
        (atom.λ (cons env-sym vars)
                (reset (define env (build-activation-record (atom.var env-sym) vars))
                       (when (not (null? freevars))
                         (generate-freevar-bindings freevars (atom.var env-sym) stack))
                       (conv/e body env (cons vars stack)))) env)]
      [_ atom]))
  (define (conv/e expr env stack)
    (match expr
      [(expr.if test then else) (expr.if (conv/a test env stack)
                                         (conv/e then env stack)
                                         (conv/e else env stack))]
      [(expr.prim op args bind cont) (expr.prim op (map (λ (x) (conv/a x env stack)) args)
                                                bind (conv/e cont env stack))]
      [(expr.app f args) (expr.app (conv/a f env stack)
                                   (map (λ (x) (conv/a x env stack)) args))]))
  (define env-sym (gensym 'env))
  (list (expr.prim 'make-vector (list (atom.lit 0) (atom.lit #f))
                   env-sym (reset 
                            (conv/e expr (atom.var env-sym) '()))) global-env))


(: codegen/a (-> atom? string?))
(define (codegen/a atom)
  (match atom
    [(atom.lit (? integer? i)) (format "make_SInteger(~a)" i)]
    [(atom.lit (? boolean? b)) (format "make_SBoolean(~a)" (if b 1 0))]
    [(atom.lit (? string? s)) (format "make_SString((unsigend char[]){~a})"
                                      (string-join (map number->string (append (bytes->list (string->bytes/locale s))
                                                                               (list 0))) ","))]
    [(atom.halt) "proc_halt"]
    [(atom.var n) (symbol->string n)];; λ is eliminated by clo conv
    ))

(define (prim->c-symbol p)
  (match p
    ['+ 'add]
    ['add1 'add1]
    ['- 'sub]
    ['sub1 'sub1]
    ['* 'mul]
    ['/ 'div]
    ['displayln 'displayln]
    ['make-vector 'make_vector]
    ['vector-ref 'vector_ref]
    ['vector-set! 'vector_set]
    ['make-closure 'make_closure]
    ['zero? 'zero]
    ['box 'box]
    ['unbox 'unbox]
    ['set-box! 'set_box]))

(: codegen/e (-> expr? string?))
(define (codegen/e expr)
  (match expr
    [(expr.if test then else)
     (string-append (format "if(~a.ptr) {\n" (codegen/a test))
                    (codegen/e then)
                    "}else{\n"
                    (codegen/e else)
                    "}\n")]
    [(expr.prim op args bind cont)
     (string-append (format "struct SValue ~a=prim_~a(~a);\n" bind (prim->c-symbol op)
                            (string-join (map codegen/a args) ","))
                    (codegen/e cont))]
    [(expr.app (app codegen/a f) args)
     (define proc (gensym 'proc))
     (define (fun-sig arity)
       (format "void (*)(~a)" (string-join (build-list arity (λ (_)
                                                               "struct SValue")) ",")))
     (string-append (format "if(!(~a.type == PROCEDURE)) { raise_error(\"application\",\"not a procedure\"); }\n" f)
                    (format "struct SProcedure ~a=*(struct SProcedure*)~a.ptr;\n" proc f)
                    (format "(*(~a)~a.fptr)(~a);\n" 
                            (fun-sig (add1 (length args)))
                            proc
                            (string-join (cons (format "~a.env" proc)
                                                         (map codegen/a args)) ",")))]))

(: codegen (-> (list/c expr? (hash/c symbol? atom.λ?)) string?))
(define (codegen repr)
  (match-define (list e funs) repr)
  (define (codegen/λ name lam)
    (match lam
      [(atom.λ vars body) (string-append (format "void ~a(~a){\n" name
                                                 (string-join (map (λ (v)
                                                                     (format "struct SValue ~a" v)) vars)
                                                              ","))
                                         (codegen/e body)
                                         "}\n")]))
  (define lams (for/fold ([str ""])
                         ([(k v) (in-hash funs)])
                 (string-append str (codegen/λ k v))))
  (string-append "#include \"runtime.h\"\n"
                 lams
                 "int main(){\n"
                 "setup();\n"
                 (codegen/e e)
                 "}\n"))
                         
  

