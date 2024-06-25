#lang at-exp racket
(provide (all-defined-out))
(define STYPE_POINTER 0)
(define STYPE_FIXNUM 1)
;;(define STYPE_FLONUM 2)
(define STYPE_CHAR 3)
(define STYPE_VOID 4)
(define STYPE_BOOLEAN 5)
(define STYPE_CSTRING 6) ;;constant string

(define STYPE_VECTOR 0)
(define STYPE_PROCEDURE 1)
(define STYPE_STRING 2)
(define STYPE_PAIR 3)

(define (SVALUE_MAKE T V)
  (format "SVALUE_MAKE(~a,~a)" T V))


(define (cdata x)
  (define text string-append)
  (match x
    ['null "SNULL"]
    [(? void?) "SVOID"]
    [#t "STRUE"]
    [#f "SFALSE"]
    [(? char?) (SVALUE_MAKE STYPE_CHAR (char->integer x))]
    [(? fixnum?) (SVALUE_MAKE STYPE_FIXNUM x)]
    ;; constant strings are lifted at pass `lift-everything`
    ))