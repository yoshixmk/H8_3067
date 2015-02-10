ECHO OFF

set path=..\TOOL\bin
set CH38=..\TOOL\include
set H38CPU=300ha

ECHO C言語コンパイラを実行します。
ch38 sample.c > err.txt
IF ERRORLEVEL 1 GOTO C_ERROR
ECHO.
PAUSE

ECHO アセンブラを実行します。
asm38 startup.src >> err.txt
IF ERRORLEVEL 1 GOTO S_ERROR
ECHO.
PAUSE

ECHO リンカを実行します。
lnk -sub=sample.sub >> err.txt
IF ERRORLEVEL 1 GOTO L_ERROR
ECHO.
PAUSE

ECHO ファイル・コンバータを実行します。
abs2hkt sample.abs
ECHO.

GOTO END

:C_ERROR
ECHO コンパイルエラーが発生しため処理を中断ます。
GOTO ERRTYPE

:S_ERROR
ECHO アセンブルエラーが発生しため処理を中断ます。
GOTO ERRTYPE

:L_ERROR
ECHO リンクエラーが発生しため処理を中断ます。
GOTO ERRTYPE

:ERRTYPE
type err.txt

:END
PAUSE
