(define (set-current-directory! path)
  (set! *current-directory* path))

(define current-directory
  (lambda path
    (if (null? (car path))
        *current-directory*
        (set-current-directory! (car path)))))

(define (pwd) (print (current-directory)))
(define dir pwd)

(define (help)
  (print "\nWelcome to Mona ")
  (print "(ls path) / (kill tid) / (reboot) / (halt)"))

(define ls
  (lambda dir-path
    (let ((dir (if (null? (car dir-path)) *cwd* (car dir-path))))
      (for-each (lambda (e) (print (e 'name))) (directory-entries dir)))))

(define (directory-entries path)
  (mona-dir-open path)
  (let loop ((entry (mona-dir-read)) (entries '()))
    (if entry
        (loop (mona-dir-read)
              (cons (make-directory-entry (car entry) (cdr entry)) entries))
        (begin (mona-dir-close) entries))))


(define (make-directory-entry name directoryp)
  (define (dispatch m)
    (cond ((eq? m 'name) name)
          ((eq? m 'directory?) directoryp)
          (else (print "make-directory-entry error"))))
  dispatch)

(define halt
  (lambda ()
    (display "system halt")
    (mona-halt)))

(define reboot
  (lambda ()
    (display "system reboot")
    (mona-reboot)))

(define (kill id)
  (if (mona-kill id)
      (print "thread killed.")
      (print "thread not found.")))

(define ps mona-ps)


(define (read-file file-name)
  (with-input-from-file file-name
    (lambda ()
      (let loop((ls1 '()) (c (read-char)))
    (if (eof-object? c)
        (list->string (reverse ls1))
        (begin (loop (cons c ls1) (read-char))))))))

(define (cat file)
  (display (read-file file)))


(define (print s)
  (display s)
  (newline))

(define (make-directory-entry name directoryp)
  (let ((children '()))
    (define (add-child child)
      (set! children (cons child children)))
    (define (has-child? name)
      (find (lambda (d) (string=? (d 'name) name)) children))
    (define (dispatch m)
      (cond ((eq? m 'name) name)
            ((eq? m 'directory?) directoryp)
            ((eq? m 'children) children)
            ((eq? m 'add-child) add-child)
            ((eq? m 'has-child?) has-child?)
            (else (print "make-directory-entry error"))))
    dispatch))

(define (make-directory-session root current-directory)
  (define (directory-has-child? dir paths)
    (if (null? paths)
        #t
        (let ((next-dir ((dir 'has-child?) (car paths))))
          (if (and next-dir (next-dir 'directory?))
              (directory-has-child? next-dir (cdr paths))
              #f))))
  (define (directory-exist? path)
    (let* ((paths (string-split path #\/))
           (first (car paths)))
      (if (and first (string=? "" first))
          (directory-has-child? root (cdr paths))
          (directory-has-child? current-directory (cdr paths)))))
  (define (dispatch m)
    (cond ((eq? m 'current-directory) current-directory)
          ((eq? m 'root) root)
          ((eq? m 'directory-exist?) directory-exist?)
          (else (print "make-directory-session error"))))
  dispatch)
