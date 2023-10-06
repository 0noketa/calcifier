CC=cl
CFLAGS=/MD /Ifgmp  bicalc.c  bicalc_rtl.c  bicalc_main.c  calcifier_rtl.c  fgmp/gmp.c 
OUT=/Fe
DEF=/D
# CC=gcc
# CFLAGS=-Ifgmp  bicalc.c  bicalc_rtl.c  bicalc_main.c  calcifier_rtl.c  fgmp/gmp.c 
# CFLAGS2=simplecalc.c  simplecalc_rtl.c  simplecalc_main.c  calcifier_rtl.c
# OUT=-o
# DEF=-D
base4_calc:
	$(CC) $(CFLAGS) $(OUT)base4_calc.exe  $(DEF)BICALC_BASE=4
base5_calc:
	$(CC) $(CFLAGS) $(OUT)base5_calc.exe  $(DEF)BICALC_BASE=5
base6_calc:
	$(CC) $(CFLAGS) $(OUT)base6_calc.exe  $(DEF)BICALC_BASE=6
base7_calc:
	$(CC) $(CFLAGS) $(OUT)base7_calc.exe  $(DEF)BICALC_BASE=7
base8_calc:
	$(CC) $(CFLAGS) $(OUT)base8_calc.exe  $(DEF)BICALC_BASE=8
base9_calc:
	$(CC) $(CFLAGS) $(OUT)base9_calc.exe  $(DEF)BICALC_BASE=9
base11_calc:
	$(CC) $(CFLAGS) $(OUT)base11_calc.exe  $(DEF)BICALC_BASE=11
base12_calc:
	$(CC) $(CFLAGS) $(OUT)base12_calc.exe  $(DEF)BICALC_BASE=12
base13_calc:
	$(CC) $(CFLAGS) $(OUT)base13_calc.exe  $(DEF)BICALC_BASE=13
base14_calc:
	$(CC) $(CFLAGS) $(OUT)base14_calc.exe  $(DEF)BICALC_BASE=14
base15_calc:
	$(CC) $(CFLAGS) $(OUT)base15_calc.exe  $(DEF)BICALC_BASE=15
base17_calc:
	$(CC) $(CFLAGS) $(OUT)base17_calc.exe  $(DEF)BICALC_BASE=17
base18_calc:
	$(CC) $(CFLAGS) $(OUT)base18_calc.exe  $(DEF)BICALC_BASE=18
base20_calc:
	$(CC) $(CFLAGS) $(OUT)base20_calc.exe  $(DEF)BICALC_BASE=20
base25_calc:
	$(CC) $(CFLAGS) $(OUT)base25_calc.exe  $(DEF)BICALC_BASE=25
base27_calc:
	$(CC) $(CFLAGS) $(OUT)base27_calc.exe  $(DEF)BICALC_BASE=27
base32_calc:
	$(CC) $(CFLAGS) $(OUT)base32_calc.exe  $(DEF)BICALC_BASE=32
base36_calc:
	$(CC) $(CFLAGS) $(OUT)base36_calc.exe  $(DEF)BICALC_BASE=36

