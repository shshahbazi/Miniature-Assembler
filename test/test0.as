foo	addi	1,1,4
	lw	2,1,2
start	add	1,1,2
	beq	0,1,done
	sw 7,4,1
	sw 5,0,1
	addi 7,7,1
	j	start
done	halt
five	.fill 	5
one	.fill 	-1
stAddr	.fill	start
