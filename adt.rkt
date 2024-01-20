#lang racket
;; This file contains macros for creating algebraic datatypes in racket
(require (for-syntax syntax/parse racket/syntax racket/function racket/match
                     racket/provide-transform
                     syntax/parse/class/struct-id))

(provide adt adt.together
         (for-syntax adt.meta))
(begin-for-syntax
  (struct adt-metainfo (desc tags contract tag-ids+contracts))
  (define-syntax-class adt.meta
    (pattern x:id
      #:do [(match-define (adt-metainfo _desc _contract _tags _tag-ids+contracts) (syntax-local-value #'x))]
      #:attr description _desc
      #:with contract _contract
      #:with (tag ...) _tags
      #:with ((tag-id field-contract ...) ...) _tag-ids+contracts))
  
  (define (adt->defintion stx)
    (syntax-parse stx
      [(name:id (~alt (~optional (~seq #:description desc))
                      (~optional (~seq (~and #:public is-public)))) ...
                                                             (tag:id [field:id contract:expr] ...) ...)
       #:do [(define fmt (curry format-id #'name))
             (define (member id)  (fmt "~a.~a" #'name id #:subs? #t))]
       #:with name? (fmt "~a?" #'name)
       #:with (name.tag ...) #`#,(map member (syntax-e #'(tag ...)))
       #:with (name.tag? ...) #`#,(map (curry fmt "~a?") (syntax-e #'(name.tag ...)))
       #:with exports (if (attribute is-public)
                          #`(provide (struct-out name.tag) ... name? name)
                          #`(begin))
       (list #`(define name? (flat-named-contract (~? desc 'name) (λ (x)
                                                                    (or (name.tag? x) ...)))) ;; contract defs
             #`(begin (struct/contract name.tag ([field contract] ...) #:transparent) ...
                      (define-syntax name (adt-metainfo (~? #'desc #''name) #'name?
                                                        #'(tag ...) #'((name.tag contract ...) ...)))
                      exports))])))

(define-syntax (adt stx)
  (syntax-parse stx
    [(_ adt-def ...)
     #:with (contract-def struct-def) #`#,(adt->defintion #'(adt-def ...))
     #`(begin contract-def
              struct-def)]))

(define-syntax (adt.together stx)
  (syntax-parse stx
    [(_ ((~datum adt) adt-def ...) ...)
     #:with ((contract-def struct-def) ...) #`#,(map adt->defintion (syntax-e #'((adt-def ...) ...)))
     #`(begin contract-def ...
              struct-def ...)]))



