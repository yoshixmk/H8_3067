ECHO OFF

set path=..\TOOL\bin
set CH38=..\TOOL\include
set H38CPU=300ha

ECHO C����R���p�C�������s���܂��D
ch38 demo.c >err.txt
IF ERRORLEVEL 1 GOTO C_ERROR
ECHO.
PAUSE

ECHO �A�Z���u�������s���܂�
asm38 Start.s >>err.txt
IF ERRORLEVEL 1 GOTO S_ERROR
ECHO.
PAUSE

ECHO �����J�����s���܂��D
lnk -sub=demo.sub >> err.txt
IF ERRORLEVEL 1 GOTO L_ERROR
ECHO.
PAUSE


ECHO �t�@�C���E�R���o�[�^�����s���܂��D
abs2hkt demo.abs
ECHO]
ECHO]


GOTO END

:C_ERROR
ECHO �R���p�C���G���[���������܂����̂ŏ����𒆒f���܂��B
GOTO ERRTYPE

:S_ERROR
ECHO �A�Z���u���G���[���������܂����̂ŏ����𒆒f���܂��B
GOTO ERRTYPE

:L_ERROR
ECHO �����N�G���[���������܂����̂ŏ����𒆒f���܂��B
GOTO ERRTYPE

:ERRTYPE
type err.txt

:END
PAUSE