(begin (+ 2 2)
    (+ 2 2)

(define inc4 (lambda (a) (+ 4 a)))
(write (inc4 5))
(newline)

  (define sumdown
    (lambda (n)                              ; non-negative number
      (if (= n 0)
        0
        (+ n (sumdown  (- n 1))))))

  (write (sumdown 10))
(newline)
(define inc5 (lambda (a)  ;; a comment after code
  (+ 5 a)))
   ""

    )
