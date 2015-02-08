;スタートアップ

	.cpu	300ha
	.import	$STACK
	.import	_main
	.import	_Proc8kHz
	.section VECTOR,common

	.data.l	_start
	.org	H'0060
	.data.l	_Proc8kHz

	.section P,code,align=2
_start:
	mov.l	#($STACK),er7
	jsr	@_main
loop:
	bra loop
	.end
	