#lang racket
(require "utils.rkt" "l1.rkt")

(: codegen/a (-> atom? string?))
(define (codegen/a atom)
  (match atom
    [(atom.lit (? integer? i)) (format "S_INTEGER(~a)" i)]
    [(atom.lit (? boolean? b)) (if b "constant_true" "constant_false")]
    [(atom.lit (? string? s)) (format "S_STRING(((unsigned char[]){~a}))"
                                      (string-join (map number->string (append (bytes->list (string->bytes/locale s))
                                                                               (list 0))) ","))]
    [(atom.halt) "proc_halt"]
    [(atom.var n) (if (eq? n 'call/cc)
                      "proc_callcc"
                      (symbol->string n))] ;; λ is eliminated by clo conv
    ))


(: codegen/e (-> expr? string?))

(define (codegen/e expr)
  (define (codegen/prim op args bind* cont)
    (define args* (map codegen/a args))
    (define bind (symbol->string bind*))
    (define (: . args)
      (string-append (apply string-append args) (codegen/e cont)))
    (define (:= lhs e)
      (format "struct SValue ~a = ~a;\n" lhs e))
    (define (:call fun . args)
      (format "~a(~a)" fun (string-join args ",")))
    (define (:call/effect fun . args)
      (format "~a(~a);\n" fun (string-join args ",")))
    (match* (op args*)
      [('void (list))
       (: (:= bind "constant_void"))]
      [('exit (list sig))
       (:call/effect "exit" (format "(int)~a.ptr" sig))]
      [('make-vector (list len init))
       (:
        (:= bind (:call "S_UNINIT_VECTOR" len))
        (:call/effect "init_SVector" bind init))]
      [('vector-length (list vec))
       (: (:= bind (:call "SVector_len" vec)))]
      [('vector-set! (list vec i v))
       (: (:= bind (:call "SVector_set" vec i v)))]
      [('vector-ref (list vec i))
       (: (:= bind (:call "SVector_ref" vec i)))]
      [('box (list v)) ;; current implementation of boxes uses vectors
       (:
        (:= bind (:call "S_UNINIT_VECTOR" "S_INTEGER(1)"))
        (:call/effect "init_SVector" bind v))]
      [('unbox (list b))
       (: (:= bind (:call "SVector_ref" b "S_INTEGER(0)")))]
      [('set-box! (list b v))
       (: (:= bind (:call "SVector_set" b "S_INTEGER(0)" v)))]
      [('make-closure (list fun env))
       (: (:= bind (:call "S_PROCEDURE" fun env)))]
      [('zero? (list i))
       (: (:= bind (format "~a.ptr? constant_true: constant_false" i)))]
      [((or '+ '- '* '/) (list i1 i2))
       (: (:= bind (format "S_INTEGER((long long) ~a.ptr ~a (long long) ~a.ptr)" i1 op i2)))]
      [('display (list v))
       (: (:call/effect "print_SValue" v)
          (:= bind "constant_void"))]
      [('newline (list))
       (: "printf(\"\\n\");\n"
          (:= bind "constant_void"))]
      [((or 'void? 'integer? 'boolean? 'string? 'vector? 'procedure?) (list x))
       (define c-type (match op
                        ['void? "VOID"]
                        ['integer? "INTEGER"]
                        ['boolean? "BOOLEAN"]
                        ['string? "STRING"]
                        ['vector? "VECTOR"]
                        ['procedure? "PROCEDURE"]))
       (: (:= bind (format "~a.type == ~a ? constant_true: constant_false" x c-type)))]))
      
  (match expr
    [(expr.if test then else)
     (string-append (format "if(~a.ptr) {\n" (codegen/a test))
                    (codegen/e then)
                    "}else{\n"
                    (codegen/e else)
                    "}\n")]
    [(expr.prim op args bind cont)
     (codegen/prim op args bind cont)]
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
                 "SETUP\n"
                 (codegen/e e)
                 "}\n"))