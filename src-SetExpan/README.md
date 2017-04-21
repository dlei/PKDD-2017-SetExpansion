./main -corpus apr -suffix test -max_iter 10 -T 60 -alpha 0.632 -Q 150 -A 5


If not no rank ensemble: 
./main -corpus apr -suffix test -max_iter 10 -T 1 -alpha 1.0 -Q 150 -A 5

If not iterative expansion (sometimes not able to extract 50 entities): 
./main -corpus apr -suffix test -max_iter 1 -T 60 -alpha 0.632 -Q 150 -A 50



Note: the size of expanded set will be "max_iter * A".  