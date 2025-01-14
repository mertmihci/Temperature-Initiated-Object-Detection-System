PORTF_DATA				EQU 		0x40025038


						AREA |.text|, CODE,READONLY
						THUMB
						EXPORT 	Arrange_Onboard_LEDs
																					
Arrange_Onboard_LEDs    
						PUSH    {R1-R4, LR} 
						
						LDR     R1, =PORTF_DATA      
						MOV     R2, R0             
						LDR     R3, =0x0E            

						LDR     R4, [R1]             
						BIC     R4, R4, R3           
						STR     R4, [R1]             

						CMP     R2, #50              
						BLO     LED_50               

						CMP     R2, #75              
						BLO     LED_75               
						
						CMP     R2, #100             
						BLO     LED_100              

						B       End_LED

LED_50
						LDR     R4, [R1]             
						ORR     R4, R4, #0x2         
						STR     R4, [R1]             
						B       End_LED   

LED_75
						LDR     R4, [R1]             
						ORR     R4, R4, #0x4         
						STR     R4, [R1]             
						B       End_LED   

LED_100
						
						LDR     R4, [R1]             
						ORR     R4, R4, #0x8         
						STR     R4, [R1]             

End_LED
						POP     {R1-R4, LR}
						BX		LR
				
						ALIGN
						END





