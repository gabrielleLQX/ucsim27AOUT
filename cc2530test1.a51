
$INCLUDE (mcu/CC2530.mcu)   ;include CC2530 SFR symbol definitions

MOV IEN0, #080H
MOV IEN1, #02H
MOV T1CC0H, #03H

MOV DPTR, #6100H ; Radio cell test
MOV A, #83H
MOVX @DPTR, A ;

MOV DPTR, #6101H ; Radio cell test
MOV A, #7DH
MOVX @DPTR, A ;

MOV DPTR, #6102H ; Radio cell test
MOV A, #50H
MOVX @DPTR, A ;

MOV DPTR, #6103H ; Radio cell test
MOV A, #64H
MOVX @DPTR, A ;

MOV DPTR, #6104H ; Radio cell test
MOV A, #6EH
MOVX @DPTR, A ;

MOV DPTR, #6105H ; Radio cell test
MOV A, #0ABH
MOVX @DPTR, A ;

MOV DPTR, #6106H ; Radio cell test
MOV A, #0CEH
MOVX @DPTR, A ;

MOV DPTR, #6107H ; Radio cell test
MOV A, #0F5H
MOVX @DPTR, A ;

;MOV CLKCONCMD, #10H ; Tickspd = 8MHz
;MOV T2MSEL, #03H ; t2_cmp1 access from T2Mx
;MOV T2M0, #05H
;MOV A, T2M0
MOV MCON, #00H
MOV T1CTL, #02H ; Up/Down mode to T1CC0 and Tick Freq /1
MOV T1CCTL0, #04H;
MOV T1CCTL1, #03H;

MOV DMA0CFGH, #0H
MOV DMA0CFGL, #80H
MOV DMA1CFGH, #0H
MOV DMA1CFGL, #90H
MOV DMAARM, #03H;
MOV DMAREQ, #02H
MOV T1CC0L, #04H
MOV T1CC0H, #00H ; set compare reg to 0x0020
MOV U0UCR, #1AH ; Odd Parity, 9 bit transfer, parity enabled, high stop bit
MOV U0CSR, #0C0H ; UART mode receiver enabled
MOV U0DBUF, #0CCH ; Byte to TX
MOV T3CCTL0, #04H;
MOV T4CCTL1, #04H;
MOV T1CC1L, #12H
MOV T1CC1H, #00H ; Timer 1 chanel 1 compare reg set to 0x12 (18)
MOV T3CTL, #52H ; Tick freq /4, start timer, modulo mode
MOV T3CC0, #03H ; Timer 3 cmp reg set to 0x5
MOV T4CC0, #09H ; Timer 4 channel 0 cmp reg set to 0x9
MOV T4CTL, #11H ; Tick freq /1, start timer, down mode
MOV T4CC1, #06H ; Timer 4 channel 1 cmp reg set to 0x6
MOV T2MSEL, #00H ; Access to regs T2 timer and T2 ovf
MOV T2M0, #0F5H
MOV T2M1, #0FFH ; set T2 to 0xFFF5
MOV T2EVTCFG, #41H ; t2 ovf cmp 1 event, t2 cmp1 event
MOV A, #0CH
MOVX @R0, A
MOV T2MSEL, #32H ; access to reg T2 ovf cmp 1, and t2 per
MOV T2M0, #09H
MOV T2M1, #00H
MOV T2MOVF0, #03H
MOV MCON, #08H
MOV DPTR, #6180H
MOVX A, @DPTR

MOV DPTR, #6272H ; faddrh
MOV A, #01H
MOVX @DPTR, A ;MOV FADDRH, #01H  setting flash write @
MOV DPTR, #6271H ; faddrl
MOV A, #00H
MOVX @DPTR, A ; MOV FADDRL, #00H
MOV DPTR, #6270H ; fctl
MOV A, #02H
MOVX @DPTR, A ; MOV FCTL, #02H flash write enabled
MOV DPTR, #6273H ; fwdata
MOV A, #0CH
MOVX @DPTR, A ;
MOVX @DPTR, A ;
MOV A, #0DH
MOVX @DPTR, A ;
MOVX @DPTR, A ;

MOVX @DPTR, A ;
MOVX @DPTR, A ;
MOV A, #0CH
MOVX @DPTR, A ;
MOVX @DPTR, A ;

MOVX @DPTR, A ;
MOVX @DPTR, A ;
MOV A, #0EH
MOVX @DPTR, A ;
MOVX @DPTR, A ;

MOVX @DPTR, A ;
MOVX @DPTR, A ;
MOV A, #0FH
MOVX @DPTR, A ;
MOVX @DPTR, A ;


MOVX @DPTR, A ;
MOVX @DPTR, A ;
MOV A, #0EH
MOVX @DPTR, A ;
MOVX @DPTR, A ;

MOVX @DPTR, A ;
MOVX @DPTR, A ;
MOV A, #0FH
MOVX @DPTR, A ;
MOVX @DPTR, A ;

MOVX @DPTR, A ;
MOVX @DPTR, A ;
MOV A, #0AH
MOVX @DPTR, A ;
MOVX @DPTR, A ;

MOVX @DPTR, A ;
MOVX @DPTR, A ;
MOV A, #0BH
MOVX @DPTR, A ;
MOVX @DPTR, A ;

MOVX @DPTR, A ;
MOVX @DPTR, A ;
MOV A, #0FH
MOVX @DPTR, A ;
MOVX @DPTR, A ;

;MOV DPTR, #6272H ; faddrh
;MOV A, #04H
;MOVX @DPTR, A ;MOV FADDRH, #01H  setting flash write @ (erase page 2)
;MOV DPTR, #6270H ; fctl
;MOV A, #01H
;MOVX @DPTR, A ; MOV FCTL, #01H erase

MOV DPTR, #6181H
MOV T4CC0, #03H
MOV T4CTL, #32H
MOV A, #02H
MOVX @DPTR, A
MOV R0, #SRCSHORTEN0
MOVX A, @R0
MOV A, #11H
MOV CLKCONCMD, #04H
MOV A, #11H
MOV A, #11H
MOV A, #11H
MOV A, #11H
MOV A, #11H
MOV A, #11H
MOV A, #11H
MOV A, #11H
MOV A, #11H
MOV A, #11H
MOV A, #11H
MOV A, #11H
MOV A, #11H
MOV A, #11H
MOVX A, @R0
MOVX A, @R0
MOVX A, @R0
MOV DMAARM, #03H;
END

