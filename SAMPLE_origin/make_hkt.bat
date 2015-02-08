ECHO OFF

set path=..\TOOL\bin
set CH38=..\TOOL\include
set H38CPU=300ha

ECHO C言語コンパイラを実行します．
ch38 demo.c >err.txt
IF ERRORLEVEL 1 GOTO C_ERROR
ECHO.
PAUSE

ECHO アセンブラを実行します
asm38 Start.s >>err.txt
IF ERRORLEVEL 1 GOTO S_ERROR
ECHO.
PAUSE

ECHO リンカを実行します．
lnk -sub=demo.sub >> err.txt
IF ERRORLEVEL 1 GOTO L_ERROR
ECHO.
PAUSE


ECHO ファイル・コンバータを実行します．
abs2hkt demo.abs
ECHO]
ECHO]


GOTO END

:C_ERROR
ECHO コンパイルエラーが発生しましたので処理を中断します。
GOTO ERRTYPE

:S_ERROR
ECHO アセンブルエラーが発生しましたので処理を中断します。
GOTO ERRTYPE

:L_ERROR
ECHO リンクエラーが発生しましたので処理を中断します。
GOTO ERRTYPE

:ERRTYPE
type err.txt

:END
PAUSE