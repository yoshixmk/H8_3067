ECHO OFF

set path=..\TOOL\bin
set CH38=..\TOOL\include
set H38CPU=300ha

ECHO C����R���p�C�������s���܂��B
ch38 sample.c > err.txt
IF ERRORLEVEL 1 GOTO C_ERROR
ECHO.
PAUSE

ECHO �A�Z���u�������s���܂��B
asm38 startup.src >> err.txt
IF ERRORLEVEL 1 GOTO S_ERROR
ECHO.
PAUSE

ECHO �����J�����s���܂��B
lnk -sub=sample.sub >> err.txt
IF ERRORLEVEL 1 GOTO L_ERROR
ECHO.
PAUSE

ECHO �t�@�C���E�R���o�[�^�����s���܂��B
abs2hkt sample.abs
ECHO.

GOTO END

:C_ERROR
ECHO �R���p�C���G���[�����������ߏ����𒆒f�܂��B
GOTO ERRTYPE

:S_ERROR
ECHO �A�Z���u���G���[�����������ߏ����𒆒f�܂��B
GOTO ERRTYPE

:L_ERROR
ECHO �����N�G���[�����������ߏ����𒆒f�܂��B
GOTO ERRTYPE

:ERRTYPE
type err.txt

:END
PAUSE
