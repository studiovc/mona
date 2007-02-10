(define caar
  (lambda (x)
    (car (car x))))

(define cdar
  (lambda (x)
    (cdr (car x))))

(define cadr
  (lambda (x)
    (car (cdr x))))

(define cddr
  (lambda (x)
    (cdr (cdr x))))

;; (define newline
;;   (lambda () (display "\n")))

;; ;; for unittest.scm
;; (define write
;;   (lambda (o) (display o)))

(define zero?
  (lambda (x)
    (= 0 x)))

(define even?
  (lambda (x)
    (let ((quotient (/ x 2)))
      (= (- (* quotient 2) x) 0))))

(define odd?
  (lambda (x)
    (not (even? x))))

(define for-each
  (lambda (proc items)
    (if (null? items)
        "done"
        (begin (proc (car items))
               (for-each proc (cdr items))))))

(define not
  (lambda (condition)
    (if condition #f #t)))

(define null '())

(define null?
  (lambda (x)
    (eqv? x null)))

(define-syntax list
  (syntax-rules ()
    ((_ a) (cons a null))
    ((_ a ...) (cons a (list ...)))))

(define-syntax and
  (syntax-rules ()
    ((and) #t)
    ((and test) test)
    ((and test1 test2 ...)
     (if test1 (and test2 ...) #f))))

(define-syntax or
  (syntax-rules ()
    ((or) #f)
    ((or test) test)
    ((or test1 test2 ...)
     (let ((x test1))
       (if x x (or test2 ...))))))

(define positive? (lambda (x) (> x 0)))
(define negative? (lambda (x) (< x 0)))
(define abs (lambda (x) (if (negative? x) (* -1 x) x)))
(define not (lambda (x) (if x #f #t)))

(define call-with-input-file (lambda (file proc)
                               (let* ((port (open-input-port file)) (ret (proc port)))
                               (close-input-port port)
                               ret)))

(define call-with-output-file (lambda (file proc)
                               (let* ((port (open-output-port file)) (ret (proc port)))
                               (close-output-port port)
                               ret)))

(define with-input-from-file (lambda (file proc)
                               (let* ((default-input-port (current-input-port)) (port (open-input-port file)))
                                 (set-current-input-port! port)
                                 (proc)
                                 (close-input-port port)
                                 (set-current-input-port! default-input-port))))

(define with-output-from-file (lambda (file proc)
                               (let* ((default-output-port (current-output-port)) (port (open-output-port file)))
                                 (set-current-output-port! port)
                                 (proc)
                                 (close-output-port port)
                                 (set-current-output-port! default-output-port))))


(define complex? (lambda (x) (number? x)))
(define real? (lambda (x) (number? x)))
(define rational? (lambda (x) (number? x)))
(define integer? (lambda (x) (number? x)))

(define exact? (lambda (x) (number? x)))
(define inexact? (lambda (x) #f))

(define quotient (lambda (n1 n2) (/ n1 n2)))

(define modulo (lambda (n1 n2)
                 (let* ((q (quotient n1 n2)) (v (- n1 (* q n2))))
                   (if (positive? n1) v (* -1 v)))))

(define reminder (lambda (n1 n2)
                   (let ((q (quotient n1 n2)))
                     (- n1 (* q n2)))))

(define numerator (lambda (x) (not-supported "numerator")))
(define denominator (lambda (x) (not-supported "denominator")))

(define exp  (lambda (x) (not-supported "exp ")))
(define log  (lambda (x) (not-supported "log ")))
(define sin  (lambda (x) (not-supported "sin ")))
(define cos  (lambda (x) (not-supported "cos ")))
(define tan  (lambda (x) (not-supported "tan ")))
(define asin (lambda (x) (not-supported "asin")))
(define acos (lambda (x) (not-supported "acos")))
(define atan (lambda (x) (not-supported "atan")))
(define sqrt (lambda (x) (not-supported "sqrt")))
(define expt (lambda (x y) (not-supported "expt")))
(define make-rectanglar (lambda (x y) (not-supported "make-rectanglar")))
(define make-polar (lambda (x y) (not-supported "make-polar")))
(define real-part (lambda (x) (not-supported "real-part")))
(define imag-part (lambda (x) (not-supported "imag-part")))
(define magnitude (lambda (z) (not-supported "magnitude")))
(define angle (lambda (z) (not-supported "angle")))
(define exact->inexact (lambda (z) (not-supported "exact->inexact")))
(define inexact->exact (lambda (z) (not-supported "inexact->exact")))

(define floor (lambda (x) x))
(define ceiling (lambda (x) x))
(define truncate (lambda (x) x))
(define round (lambda (x) x))
