	lui 5,25
	nand 4,3,2
	ori 10,5,1
	add 1,2,2
	jalr 1,5
l1	addi 2,2,5021
l1	sw 2,2,1
	halt
	.fill 3
	.fill 1 #end