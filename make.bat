REM c:\gbdk\bin\lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -c -o main.o main.c
REM c:\gbdk\bin\lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -o main.gb main.o

REM essas novas linhas serve para compilar com a função de gravar informações em um arquivo
c:\gbdk\bin\lcc -Wa-l -Wf-ba0 -c -o saveState.o saveState.c
c:\gbdk\bin\lcc -Wa-l -c -o main.o main.c
c:\gbdk\bin\lcc -Wl-yt3 -Wl-yo4 -Wl-ya4 -o main.gb main.o saveState.o