#lang at-exp racket
(require nanopass/base "l0.rkt" "l1.rkt" "stypes.rkt" "utils.rkt"
         "runtime/main.rkt" "base-lib.rkt")
(define text string-append)
(provide compile gcc)

(define (cstr s)
  (define (align l)
    (define size (length l))
    (define over (remainder size 8))
    (if (= over 0) l
        (append l (build-list (- 8 over) (λ (_) 0)))))
  (string-join (map number->string (align (append (bytes->list (string->bytes/locale s))
                                                  (list 0)))) ","))


(define-pass codegen : L1 (ir) -> * (any)
  (definitions
    (define bindto (make-parameter #f))
    (define-syntax-rule (cvar x ...)
      (begin (define x (~a (gensym 'x))) ...))
    
    (define (funargs lst)
      (string-join
       (map (λ (x) @text{SVALUE @(~a x)}) lst) ","))
    (define fprototypes (new appendlist%))
    (define (compile-prim prim args bindto)
      (define (indent texts)
        (map (λ (x) (string-append x "  ")) texts))
      (define (text . texts)
        (apply string-append texts))
      (define (bind . texts)
        (if bindto (apply string-append (indent texts)) ""))
      (match* (prim args)
        [('vector? `(,x))
         @bind{SVALUE @bindto = SVALUE_TYPE(@x) == STYPE_POINTER &&
          ((SHEADER*)@x) -> type == STYPE_VECTOR? @(cdata #t) : @(cdata #f);}]
        [('make-vector `(,len))
         (cvar vec_space vec_len)
         @text{size_t @|vec_len| = sizeof(SVECTOR) + 8* SVALUE_FIXNUM(@len);
          pSVECTOR @vec_space = alloca(@vec_len);
          @|vec_space|->header = (SHEADER){STYPE_VECTOR, @|vec_len|, NULL};
          @|vec_space|->len= SVALUE_FIXNUM(@len);
          @bind{SVALUE @bindto = (SVALUE)@vec_space;}}]
        [('vector-ref `(,vec ,idx))
         @bind{SVALUE @bindto = ((SVALUE*)((void*)@|vec| + sizeof(SVECTOR)))[SVALUE_FIXNUM(@|idx|)];}]
        [('vector-set! `(,vec ,idx ,val))
         @text{((SVALUE*)((void*)@|vec| + sizeof(SVECTOR)))[SVALUE_FIXNUM(@|idx|)] = @|val|;
          @bind{SVALUE @bindto = SVOID;}}]

        [('box `(,val))
         @text{@compile-prim['make-vector (list (cdata 1)) bindto]
          @(compile-prim 'vector-set! (list bindto (cdata 0) val) #f)}]
        [('unbox `(,bx))
         (compile-prim 'vector-ref (list bx (cdata 0)) bindto)]
        [('set-box! `(,bx ,val))
         (compile-prim 'vector-set! (list bx (cdata 0) val) bindto)]

        [('procedure? `(,x))
         @bind{SVALUE @bindto = SVALUE_TYPE(@x) == STYPE_POINTER  &&
          ((SHEADER*)@x) -> type == STYPE_PROCEDURE? @(cdata #t) : @(cdata #f);}]
        [('make-closure `(,fun ,env))
         (cvar clo_space)
         @text{pSPROCEDURE @clo_space = alloca(sizeof(SPROCEDURE));
          @|clo_space|->header = (SHEADER){STYPE_PROCEDURE, sizeof(SPROCEDURE), NULL};
          @|clo_space|->fptr = (void*)@fun;
          @|clo_space|->env = @env;
          @bind{SVALUE @bindto = (SVALUE)@clo_space;}}]
        [('closure-env `(,clo))
         @bind{SVALUE @bindto = ((pSPROCEDURE)@|clo|)->env;}]
        
        [('init-cstring `(,x ,c))
         @text{@(~a x) = CSTRING_MAKE(@c);
          }]
        
        [('display `(,v))
         (if bindto
             @text{SVALUE @bindto = prim_display(@v);}
             @text{prim_display(@v);})]
        [('newline _)
         @text{printf("\n");}]

        [('pair? `(,x))
         @bind{SVALUE @bindto = SVALUE_TYPE(@x) == STYPE_POINTER  &&
          ((SHEADER*)@x) -> type == STYPE_PAIR? @(cdata #t) : @(cdata #f);}]
        [('null? `(,x))
         @bind{SVALUE @bindto = (void*)@x == NULL? @(cdata #t) : @(cdata #f);}]
        [('cons `(,x ,y))
         (cvar pair_space) 
         @text{SPAIR* @pair_space = alloca(sizeof(SPAIR));
          @|pair_space|->header = (SHEADER){STYPE_PAIR, sizeof(SPAIR), NULL};
          @|pair_space|->car = @x;
          @|pair_space|->cdr = @y;
          @bind{SVALUE @bindto = (SVALUE)@pair_space;}}]
        [('car `(,x))
         @bind{SVALUE @bindto = ((SPAIR*)@x)->car;}]
        [('cdr `(,x))
         @bind{SVALUE @bindto = ((SPAIR*)@x)->cdr;}]

        [('fixnum? `(,x))
         @bind{SVALUE @bindto = SVALUE_TYPE(@x) == STYPE_FIXNUM? @(cdata #t) : @(cdata #f);}]
        [('zero? `(,v))
         @bind{SVALUE @bindto = (SVALUE_FIXNUM(@v) == 0) ? @(cdata #t) : @(cdata #f);}]
        [((or '+ '- '* '/) `(,v1 ,v2))
         @bind{SVALUE @bindto = @(SVALUE_MAKE STYPE_FIXNUM @text{SVALUE_FIXNUM(@v1) @(~a prim) SVALUE_FIXNUM(@v2)});}]
        [('= `(,v1 ,v2))
         @bind{SVALUE @bindto = SVALUE_FIXNUM(@v1) == SVALUE_FIXNUM(@v2)? @(cdata #t) : @(cdata #f);}]
        [((or '> '>= '< '<=) `(,v1 ,v2))
         @bind{SVALUE @bindto = SVALUE_FIXNUM(@v1) @(~a prim) SVALUE_FIXNUM? @(cdata #t) : @(cdata #f);}]
        
        
        [('gc `(,f ,args ...))
         (define save-args-list (for/list ([(a i) (in-indexed args)])
                                  @text{saved_call_args[@(~a i)]=@a;}))
         (define save-args (string-join save-args-list "\n"))
         @text{if(GC_STACK_CHECK){
           saved_call_f = @f;
           saved_call_len = @(~a (length args));
           @save-args
           minor_gc();
           }}]
        [('gc-debug-mode _)
         @text{GC_DEBUG_MODE = 1;}]

        [('exit `(,sig))
         @text{exit(SVALUE_FIXNUM(@sig));
          @bind{SVALUE @bindto = SNULL;}}]
        ))
    )
  (Program : Program (ir) -> * (any)
           [(,[decl] ...) (string-join decl "\n")])
  (Decl : Decl (ir) -> * (any)
        [(λ ,x (,x* ...) ,[e])
         (send fprototypes push @text|{void |@(~a x)(|@(funargs x*));}|)
         @text|{void |@(~a x)(|@(funargs x*)) {
         |@e
        }
         }|]
        [(string-constant ,x ,c)
         @text{unsigned char @(~a x) [] = {@(cstr c)};
          }]
        [(global ,x)
         @text{SVALUE @(~a x);
          }])
  (Atom : Atom (ir) -> * (any)
        [,x (~a x)]
        [,c (cdata c)])
  (Expr : Expr (ir) -> * (any)
        [,x @text{@(~a x);}]
        [,c @text{@(cdata c);}]
        [(begin ,[body*] ... ,[body])
         (string-join (append body* (list body)) "\n")]
        [(prim ,pr ,[a] ...)
         (compile-prim pr a #f)]
        [(let ,x ,pcall)
         (nanopass-case (L1 PrimCall) pcall
                        [(prim ,pr ,a ...)
                         (compile-prim pr (map Atom a) (~a x))])]
        [(,[a1] ,[a2] ...)
         (define fun-sig @text{void (*)(@(string-join (build-list (add1 (length a2)) (λ (_) "SVALUE")) ","))})
         @text{((@fun-sig)(((pSPROCEDURE)@a1)->fptr))(@a1,@(string-join a2 ","));
          }]
        [(if ,[a] ,[e1] ,[e2])
         @text{if (@a == @(cdata #t)){
           @e1
           }else{
           @e2
           }}]
        [(let-atom ,x ,[a])
         @text{SVALUE @(~a x) = @a;}])
  (define r (Program ir))
  @text{#include "stypes.h"
 #include "runtime.h"
 @(apply string-append (send fprototypes pop*))
 @r
 
 int main(){
  SETUP
  }})

(define-syntax-rule (compile f e)
  (call-with-output-file f (λ (port)
                             (display (with-gensym (codegen (l1-passes (with-output-language (L0 Expr)
                                                                         (with-base-lib `e))))) port))
    #:exists 'replace))
(define (gcc f o)
  (system @text{gcc @f @runtime-libs -o @o -I @runtime-headers}))

