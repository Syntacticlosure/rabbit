#lang at-exp racket
(define (fun-sig arity)
  (format "void (*)(~a)" (string-join (build-list arity (λ (_)
                                                          "SVALUE")) ",")))

(define (args arity)
  (if (= arity 0)
      ""
      (string-append
       "," (string-join (map (λ (i)
                               (format "saved_call_args[~a]" i)) (range arity)) ","))))

(call-with-output-file "fcall.c"
  (λ (port)
    (define (put . x) (displayln (apply string-append x) port))
    (define text string-append)
    @put{#include "stypes.h"
         #include "runtime.h"
 void perform_saved_call(){
  pSPROCEDURE proc = (pSPROCEDURE)saved_call_f;
  switch(saved_call_len){
   @(string-join
     (for/list ([arity 128])
       @text{        case @(~a arity):
    (*(@(fun-sig (add1 arity)))(proc->fptr))(saved_call_f@(args arity));
    break;
    }))
  }
  }})
  #:exists 'replace)
                            
