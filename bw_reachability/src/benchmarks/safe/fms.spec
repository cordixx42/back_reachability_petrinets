#expected result: safe
#   target x10 >= 4
vars
     x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 x14 x15 x16 x17 x18 x19 x20 x21 x22
rules
    x1 >= 1 ->
		    x1' = x1-1 ,
		    x5' = x5+1 ;

    x2 >= 1 , x12 >= 1 ->
		    x2' = x2-1 ,
		    x6' = x6+1 ,
		    x12' = x12-1 ;

    x3 >= 1 ->
		    x3' = x3-1 ,
		    x7' = x7+1 ;

    x4 >= 1 ->
		    x4' = x4-1 ,
		    x8' = x8+1 ;

    x5 >= 1 , x9 >= 1 ->
		    x5' = x5-1 ,
		    x9' = x9-1 ,
		    x10' = x10+1 ;

    x16 >= 1 ->
		    x2' = x2+1 ,
		    x16' = x16-1 ;

    x6 >= 1 ->
		    x6' = x6-1 ,
		    x11' = x11+1 ;

    x7 >= 1 , x14 >= 1 ->
		    x7' = x7-1 ,
		    x13' = x13+1 ,
		    x14' = x14-1 ;

    x8 >= 1 , x14 >= 1 ->
		    x8' = x8-1 ,
		    x15' = x15+1 ;

    x10 >= 1 ->
		    x9' = x9+1 ,
		    x10' = x10-1 ,
		    x16' = x16+1 ;

    x11 >= 1 , x17 >= 1 ->
		    x11' = x11-1 ,
		    x17' = x17-1 ,
		    x18' = x18+1 ;

    x19 >= 1 ->
		    x12' = x12+1 ,
		    x19' = x19-1 ;

    x13 >= 1 ->
		    x13' = x13-1 ,
		    x14' = x14+1 ,
		    x19' = x19+1 ;

    x15 >= 1 ->
		    x4' = x4+1 ,
		    x15' = x15-1 ;

    x16 >= 1 ->
		    x16' = x16-1 ,
		    x20' = x20+1 ;

    x18 >= 1 ->
		    x17' = x17+1 ,
		    x18' = x18-1 ,
		    x21' = x21+1 ;

    x19 >= 1 ->
		    x19' = x19-1 ,
		    x22' = x22+1 ;

    x20 >= 1 ->
		    x1' = x1+1 ,
		    x20' = x20-1 ;

    x21 >= 1 ->
		    x1' = x1+1 ,
		    x3' = x3+1 ,
		    x21' = x21-1 ;

    x22 >= 1 ->
		    x3' = x3+1 ,
		    x22' = x22-1 ;


init
    x1 >= 1 , x2 = 0 , x3 >= 1 , x4 >= 1 , x5 = 0 , x6 = 0 , x7 = 0 , x8 = 0 ,
x9 = 3 , x10 = 0 , x11 = 0 , x12 = 0 , x13 = 0 , x14 = 1 , x15 = 0 , x16 = 0 ,
x17 = 2 , x18 = 0 , x19 = 0 , x20 = 0 , x21 = 0 , x22 = 0

target
 x13 >= 2

invariants
x13 = 1,x14 = 1
x17 = 1,x18 = 1
