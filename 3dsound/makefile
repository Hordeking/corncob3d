ofile = 3.lnk

#3:  3.obj f3dvec.obj egaline.obj hline.obj dline.obj kline.obj\
#  mat.obj cmdline.obj clip.obj sin.obj csqrt.obj inst.obj\
#  dsqrt.obj strtxp.obj defobjs.obj map.obj adl.obj pict.obj
#   link @$(ofile).lnk

default: 3

3:  3.obj f3dvec.obj egaline.obj hline.obj kline.obj\
  mat.obj cmdline.obj sin.obj csqrt.obj inst.obj atn.obj\
  dsqrt.obj strtexp.obj defobjs.obj map.obj adl.obj pict.obj\
  usermac.inc
	link @$(ofile)
#	link /MAP c:\sa\lib\starts 3_r f3dvec_r hline defobj_r kline egaline strtxp_r inst mat sin csqrt atn dsqrt cmdliner map_r adl pict, 3.exe, 3.map, c:\sa\lib\sas.lib

#3s:  3_s.obj f3dvec_s.obj egaline.obj hline.obj kline.obj\
#  mat.obj cmdlines.obj sin.obj csqrt.obj inst.obj atn.obj\
#  dsqrt.obj strtxp_s.obj defobj_s.obj map_s.obj pict.obj\
#  username.dat adl.obj
#	link @3s.lnk

3.obj: 3.asm 3dvid.inc f3dequ.inc ka.inc engine.inc usermac.inc
	masm /t /Dshareflag=0 /Dmbflag=1 3.asm
#	mv 3.obj 3_r.obj

#3_s.obj: 3.asm 3dvid.inc f3dequ.inc ka.inc usermac.inc
#	masm /t /Dshareflag=1 /Dmbflag=0 3.asm
#	mv 3.obj 3_s.obj

f3dvec.obj: f3dvec.asm 3dvid.inc f3dequ.inc ka.inc
	masm /t /Dshareflag=0 /Dmbflag=1 f3dvec
#	mv f3dvec.obj f3dvec_r.obj

#f3dvec_s.obj: f3dvec.asm 3dvid.inc f3dequ.inc ka.inc
#	masm /t /Dshareflag=1 /Dmbflag=0 f3dvec
#	mv f3dvec.obj f3dvec_s.obj

strtexp.obj: strtexp.asm f3dequ.inc ka.inc
	masm /t /Dshareflag=0 strtexp.asm
#	mv strtexp.obj strtxp_r.obj

#strtxp_s.obj: strtexp.asm f3dequ.inc ka.inc
#	masm /t /Dshareflag=1 strtexp.asm
#	mv strtexp.obj strtxp_s.obj

defobjs.obj: defobjs.asm f3dequ.inc ka.inc
	masm /t /Dshareflag=0 /Dmbflag=1 defobjs.asm
#	mv defobjs.obj defobj_r.obj

#defobj_s.obj: defobjs.asm f3dequ.inc ka.inc
#	masm /t /Dshareflag=1 /Dmbflag=0 defobjs.asm
#	mv defobjs.obj defobj_s.obj

cmdline.obj: cmdline.asm f3dequ.inc ka.inc
	masm /t /Dshareflag=0 cmdline
#	mv cmdline.obj cmdliner.obj

#cmdlines.obj: cmdline.asm f3dequ.inc ka.inc
#	masm /t /Dshareflag=1 cmdline
#	mv cmdline.obj cmdlines.obj

map.obj: map.asm f3dequ.inc ka.inc
	masm /t /Dshareflag=0 /Dmbflag=1 map.asm
#	mv map.obj map_r.obj

#map_s.obj: map.asm f3dequ.inc ka.inc
#	masm /t /Dshareflag=1 /Dmbflag=0 map.asm
#	mv map.obj map_s.obj

mat.obj: mat.asm
	masm /t mat

pict.obj: pict.asm f3dequ.inc ka.inc
	masm /t pict

dsqrt.obj: dsqrt.asm
	masm /t dsqrt

inst.obj: inst.asm 3dvid.inc f3dequ.inc ka.inc
	masm /t inst

atn.obj: atn.asm
	masm /t atn

kline.obj: kline.asm 3dvid.inc
	masm /t kline

dline.obj: dline.asm
	masm /t dline

hline.obj: hline.asm 3dvid.inc
	masm /t hline

clip.obj: clip.asm
	masm /t clip.asm

sin.obj: sin.asm
	masm /t sin.asm

csqrt.obj: csqrt.asm
	masm /t csqrt.asm

egaline.obj: egaline.asm 3dvid.inc
	masm /t egaline.asm

pixaddr1.obj: pixaddr1.asm 3dvid.inc
	masm /t pixaddr1.asm

adl.obj: adl.asm
	masm /t adl.asm

clean:
	-rm *.obj
	-rm *.map

all-clean:
	-rm *.exe
	-rm *.obj
	-rm *.map

usermac.inc: username.dat
	usermac > usermac.inc

#usermac.exe: usermac.c
#	gcc usermac.c -o usermac.exe
