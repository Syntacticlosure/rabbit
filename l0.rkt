#lang racket
(require nanopass/base racket/hash (except-in racket/control set)
         "utils.rkt")
(define-language L0
  (terminals
   (variable (x))
   (primitive (pr))
   (constant (c)))
  (Expr (e body)
        x
        c
        (λ (x* ...) body* ... body)
        (prim pr e* ...)
        (e1 e2 ...)
        (begin e* ... e)
        (set! x e)
        (if e0 e1 e2)
        (let ([x* e*] ...) body* ... body)
        (let* ([x* e*] ...) body* ... body)
        (letrec ([x* e*] ...) body* ... body))
  (entry Expr))


(define (variable? x) (and (symbol? x)
                           (not (eq? x 'null))))

(define (primitive? x) (memq x '(init-cstring display newline exit
                                              vector? procedure? pair? null? fixnum? char?
                                              void? boolean? cons car cdr
                                 make-closure box unbox set-box! make-vector vector-ref vector-set!
                                              closure-env
                                              = > < <= >= + - * / zero? gc gc-debug-mode)))

(define (constant? x) (or (eq? x 'null)
                          (void? x)
                          (boolean? x)
                          (fixnum? x)
                          (char? x)
                          (string? x)))

(define-language L1
  (extends L0)
  (Expr (e body)
        (- (let ([x* e*] ...) body* ... body)
           (let* ([x* e*] ...) body* ... body)
           (letrec ([x* e*] ...) body* ... body)
           (λ (x* ...) body* ... body))
        (+ (λ (x* ...) body))))

(define-pass desugar : L0 (ir) -> L1 ()
  (definitions)
  (Expr : Expr (ir) -> Expr ()
        [(λ (,x* ...) ,[body*] ... ,[body]) `(λ (,x* ...) (begin ,body* ... ,body))]
        [(let ([,x* ,[e*]] ...) ,[body*] ... ,[body])
         `((λ (,x* ...) (begin ,body* ... ,body))
           ,e* ...)]
        [(let* () ,[body*] ... ,[body]) `(begin ,body* ... ,body)]
        [(let* ([,x* ,e*] ...) ,body* ... ,body)
         (match* (x* e*)
           [(`(,x ,x^ ...) `(,e ,e^ ...))
            (desugar (with-output-language (L0 Expr)
                       `((λ (,x) (let* ([,x^ ,e^] ...) ,body* ... ,body)) ,(desugar e))))])]
        [(letrec ([,x* ,[e*]] ...) ,[body*] ... ,[body])
         (define e^ (map (λ (_) 'null) x*))
         `((λ (,x* ...) (begin
                          (set! ,x* ,e*) ...
                          ,body* ... ,body)) ,e^ ...)]))
        

;; alpha-renaming

(define-syntax-rule (genvar sym v ...)
  (begin (define v (gensym 'sym)) ...))

(define-pass alpha-rename : L1 (ir) -> L1 ()
  (definitions
    (define env (make-parameter (hash 'call/cc
                                      'primfun_call_cc)))
    (define-syntax-rule (extend-env (keys vals) body ...)
      (parameterize ([env (for/fold ([env (env)])
                                    ([k (in-list keys)]
                                     [v (in-list vals)])
                            (hash-set env k v))])
        body ...)))
  (Expr : Expr (ir) -> Expr ()
        [,x (hash-ref (env) x)]
        [(set! ,x ,[e]) `(set! ,(hash-ref (env) x) ,e)]
        [(λ (,x* ...) ,e) (define x** (map gensym x*))
                          `(λ (,x** ...) ,(extend-env (x* x**)
                                                      (Expr e)))]))

(define-language L2
  (extends L1)
  (Expr (e body)
        (- (set! x e))))

(define-pass eliminate-set! : L1 (ir) -> L2 ()
  (definitions
    (define mvars (set))
    (define (mvar? x) (set-member? mvars x))
    (define-pass mark-mvars : L1 (ir) -> L1 ()
      (Expr : Expr (ir) -> Expr ()
            [(set! ,x ,[e]) (set! mvars (set-add mvars x))
                            `(set ,x ,e)]))
    (mark-mvars ir))
  (Expr : Expr (ir) -> Expr ()
        [,x (if (mvar? x)
                `(prim unbox ,x)
                x)]
        [(set! ,x ,[e]) `(prim set-box! ,x ,e)]
        [(λ (,x* ...) ,[body]) (define x** (filter mvar? x*))
                               (define boxes (map (λ (x) `(prim box ,x)) x**))
                               (if (null? x**)
                                   `(λ (,x* ...) ,body)
                                   `(λ (,x* ...) ((λ (,x** ...) ,body) ,boxes ...)))]))
    
(define-language LCPS
  (terminals
   (variable (x))
   (primitive (pr))
   (constant (c)))
  (Atom (a)
        x
        c
        (λ (x* ...) e))
  (PrimCall (pcall)
        (prim pr a ...))
  (Expr (e body)
        a
        (a1 a2 ...)
        pcall
        (let-atom x a)
        (let x pcall)
        (begin body* ... body)
        (if a e1 e2))
  (entry Expr))

(define-pass cpsify : L2 (ir) -> LCPS ()
  (definitions
    (define (eta f)
      (nanopass-case (LCPS Atom) f
                     [(λ (,x0) (,x1 ,x2)) (if (equal? x0 x2)
                                              x1
                                              f)]
                     [else f])))
  (Expr : Expr (ir) -> Expr ()
        [,x x]
        [,c c]
        [(λ (,x* ...) ,e) (define k (gensym 'k))
                          `(λ (,x* ... ,k) ,(reset
                                             (in-context Expr `(,k ,(Expr e)))))]
        [(,e1 ,[e2] ...)
         (define x (gensym 'x))
         (shift k `(,(Expr e1) ,e2 ... ,(eta `(λ (,x) ,(k x)))))]
        [(prim ,pr ,[e*] ...)
         (define x (gensym 'x))
         (shift k `(begin (let ,x (prim ,pr ,e* ...))
                          ,(k x)))]
        [(begin ,[body*] ... ,[body])
         body]
        [(if ,[e0] ,e1 ,e2)
         (shift cont
                `(if ,e0
                       ,(reset (cont (Expr e1)))
                       ,(reset (cont (Expr e2)))))])
  `((λ (primfun_call_cc)
            ,(reset (Expr ir)))
           (λ (f k)
             (f (λ (x k2) (k x)) k))))

(define-pass flatten-begin : LCPS (ir) -> LCPS ()
  (Expr : Expr (ir) -> Expr ()
        [(begin ,[e]) e]
        [(begin ,[body*] ... ,[body])
         (define insts (new appendlist%))
         (define (expand e)
           (nanopass-case (LCPS Expr) e
                          [(begin ,e* ... ,e)
                           (send insts push* e*)
                           (send insts push e)]
                          [else (send insts push e)]))
         (for ([body body*])
           (expand body))
         (expand body)
         (define last (send insts pop-last))
         `(begin
            ,(send insts pop*) ... ,last)]))
                            
(define-pass clo-conv : LCPS (ir) -> LCPS ()
  (definitions
    (define (get-freevars ir)
      (define fvars (set))
      (define letvars (set))
      (define-pass pass : LCPS (ir) -> LCPS ()
        (definitions)
        (Atom : Atom (ir) -> Atom ()
              [,x (set! fvars (set-add fvars x)) x]
              [(λ (,x* ...) ,e) `(λ (,x* ...) ,e)])
        (Expr : Expr (ir) -> Expr ()
              [(let ,x ,[e])
               (set! letvars (set-add letvars x))
               `(let ,x ,e)]
              [(let-atom ,x ,a)
               (set! letvars (set-add letvars x))
               ir])
        (Expr ir))
      (pass ir)
      (set-subtract fvars letvars))

    (with-output-language (LCPS Expr)
      (define insts (make-parameter (new appendlist%)))
      (define current-env (make-parameter 'null))
      (define current-stack (make-parameter '()))
      (define activation-record-usage (make-parameter (box #f)))
      (define-syntax-rule (with-param (current-env* current-stack* activation-record-usage*)
                            e ...)
        (parameterize ([current-env current-env*]
                       [current-stack current-stack*]
                       [activation-record-usage activation-record-usage*])
          e ...))

      
      (define (commit inst)
        (send (insts) push inst))
      (define (final-insts) (send (insts) pop*))
    
      (define (build-activation-record vars old-env env)
        (define len (add1 (length vars)))
        (commit `(let ,env (prim make-vector ,len)))
        (commit `(prim vector-set! ,env 0 ,old-env))
        (for ([(var i) (in-indexed vars)])
          (commit `(prim vector-set! ,env ,(add1 i) ,var))))
    
   
      (define (generate-freevar-bindings x* e env)
        (define vars (set->list (set-subtract (get-freevars e)
                                              (list->set x*))))
        (define (path var)
          (call/ec (λ (break)
                     (for* ([(frame i1) (in-indexed (current-stack))]
                            [(v i2) (in-indexed frame)])
                       (when (eq? v var) (break (list i1 (add1 i2)))))
                     (error (format "compilation error: cannot find variable ~a"
                                    var)))))

        (define (generate)
          (define paths (map path vars))
        (define max-parent-depth (apply max (map first paths)))

        (define envs (cons env (map (λ (x) (gensym 'env)) (range max-parent-depth))))

        (let ([before (first envs)])
          (for ([env (rest envs)])
            (commit `(let ,env (prim vector-ref ,before 0)))
            (set! before env)))
          
        (for ([v vars]
              [path paths])
          (match-define (list n i) path)
          (commit `(let ,v (prim vector-ref ,(list-ref envs n) ,i)))))
        
        (when (not (null? vars)) (generate)))))
  
  (Expr : Expr (ir) -> Expr ())
  (Atom : Atom (ir) -> Atom ()
        [(λ (,x* ...) ,e) (shift k
                                 (parameterize ([insts (new appendlist%)])
                                   (genvar env old-env new-env)
                                   (genvar self self)
                                   (genvar clo clo)
                                   
                                   (commit (in-context Expr `(let ,old-env (prim closure-env ,self))))
                                   (generate-freevar-bindings x* e old-env)

                                   (define new-stack (cons x* (current-stack)))
                                   (define actrecord-used? (box #f))
                                   (define e* (with-param (new-env new-stack actrecord-used?)
                                                (reset (Expr e))))
                                   (when (unbox actrecord-used?)
                                       (build-activation-record x* old-env new-env))
                                   
                                   (define lam `(λ (,self ,x* ...)
                                                    (begin ,(final-insts) ...
                                                           ,e*)))
                                   (set-box! (activation-record-usage) #t)
                                   
                                   (in-context Expr `(begin
                                                       (let ,clo (prim make-closure ,lam ,(current-env)))
                                                       ,(k clo)))))])
  
  (reset (Expr ir)))



(define-syntax (passes stx)
  (syntax-case stx ()
    [(_ pass ...)
     #`(λ (x)
         #,(foldr (λ (p r)
                    #`(#,p #,r))
                  #`x
                  (reverse (syntax->list #`(pass ...)))))]))
(define-syntax-rule (l0-debug e)
  (display (with-gensym (l0-passes (with-output-language (L0 Expr)
                `e)))))


(provide L0 LCPS l0-passes l0-debug passes with-gensym gensym)
(define l0-passes (passes desugar alpha-rename eliminate-set! cpsify clo-conv flatten-begin ))




