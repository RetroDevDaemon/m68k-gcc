Z80PROG:
* bytes...

        ORG $800

* bus req on
        movea.l #$a11100,a2     * bus req addr
        move.w  #$100,(a2)
* bus reset off 
        movea.l #$a11200,a3     * bus reset addr
        move.w  #$100,(a3)
* wait 28ms
        btst    #0,(a2)
* copy program 
        movea.l #Z80PROG,a0
        movea.l #$a00000,a1
        move.l  #<prog size>,d1
* loop
Z80COPYLOOP:
        move.b  (a0+),d0 
        move.b  d0,(a1+)
        subq    #1,d1
        bne     Z80COPYLOOP
* bus reset on 
        move.w  #0,(a3)
* bus req off 
        move.w  #0,(a2) 
* bus reset off 
        move.w  #$100,(a3)
