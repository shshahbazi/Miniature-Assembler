	lw	1,0,six
	lw	2,1,2
start	add	1,1,2
	beq	0,1,359394
six	j	start
done	halt
five	.fill 	5
neg1	.fill 	-1
stAddr	.fill	start
new	.fill	4