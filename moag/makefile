# Put in the full pathname if not in current dir:
USERNAME = c:/asm/3d/3dsound/username.dat
# USERNAME = ./username.dat

# Choose text or graphics mode
O = text
# O = graph

OBJ = obj
CC = bcc
CFLAGS = -c -K -mm -d
TURBOC = c:\borlandc
MASM = masm
LINKM_r = tlink /m $(TURBOC)\lib\c0m @moag_r.lnk, moag, moag, $(TURBOC)\lib\cm
LINKM_s = tlink /m $(TURBOC)\lib\c0m @moag_s.lnk, moag, moag, $(TURBOC)\lib\cm
LINKM_k = tlink /m $(TURBOC)\lib\c0m @moag_k.lnk, moag, moag, $(TURBOC)\lib\cm
LINKM_b = tlink $(TURBOC)\lib\c0m @moag_b.lnk, moag, moag, $(TURBOC)\lib\cm
LINKS = bcc -mm
LINKT = bcc -mm
LINKI = bcc -mm

# OBJ = mix
# CC = pc
# CFLAGS = -c -w
# LINKM_r = pcl @moag_r.pcl
# LINKM_s = pcl @moag_s.pcl
# LINKM_b = pcl @moag_b.pcl
# LINKS = pcl
# LINKT = pcl [5K,16K,3K]
# LINKI = pcl [5K,4K,3K]

OBJS_r = moag_r.$(OBJ) misn_r.$(OBJ) ryo.$(OBJ) roster.$(OBJ) hiscore.$(OBJ) \
	reg_r.$(OBJ) demo_r.$(OBJ) kbjs.$(OBJ) sndedit.$(OBJ) scrnma_r.$(OBJ) \
	o_m.$(OBJ) input_r.$(OBJ) go_r.$(OBJ) report_r.$(OBJ) promed_r.$(OBJ) \
	cecode.$(OBJ) 3univs_r.$(OBJ) flash.$(OBJ) init_r.$(OBJ) \
	scroll_m.$(OBJ) ugly.$(OBJ) uglylist.$(OBJ) version.$(OBJ) \
	regstuff.$(OBJ) readfont.obj mfsa.obj \
	rolplay.obj rolread.obj insbank.obj csoundc.obj outchip.obj \
	adldet.obj loadsnd.obj freeup.obj

OBJS_s = moag_s.$(OBJ) misn_s.$(OBJ) ryo.$(OBJ) roster.$(OBJ) hiscore.$(OBJ) \
	reg_s.$(OBJ) demo_s.$(OBJ) kbjs.$(OBJ) scrnma_s.$(OBJ) o_m.$(OBJ) \
	input_s.obj go_s.$(OBJ) report_s.$(OBJ) promed_s.$(OBJ) cecode.$(OBJ) \
	3univs_s.$(OBJ) init_s.$(OBJ) scroll_m.$(OBJ) ugly.$(OBJ) \
	uglylist.$(OBJ) version.$(OBJ) readfont.obj mfsa.obj \
	rolplay.obj rolread.obj insbank.obj csoundc.obj outchip.obj \
	loadsnd.obj adldet.obj freeup.obj

OBJS_k = moag_k.$(OBJ) misn_k.$(OBJ) ryo.$(OBJ) roster.$(OBJ) hiscore.$(OBJ) \
	reg_k.$(OBJ) demo_k.$(OBJ) kbjs.$(OBJ) scrnma_s.$(OBJ) o_m.$(OBJ) \
	input_k.obj go_k.$(OBJ) report_k.$(OBJ) promed_k.$(OBJ) cecode.$(OBJ) \
	3univs_k.$(OBJ) init_k.$(OBJ) scroll_m.$(OBJ) ugly.$(OBJ) \
	uglylist.$(OBJ) version.$(OBJ) readfont.obj mfsa.obj outchip.obj \
	loadsnd.obj adldet.obj freeup.obj csoundc.obj


OBJS_b = moag_b.$(OBJ) misn.$(OBJ) ryo.$(OBJ) roster.$(OBJ) hiscore.$(OBJ) \
	reg_s.$(OBJ) demo.$(OBJ) kbjs.$(OBJ) scrnma.$(OBJ) o.$(OBJ) \
	input.$(OBJ) go_s.$(OBJ) report_m.$(OBJ) promed_m.$(OBJ) cecode.$(OBJ) \
	3univs_m.$(OBJ) flash.$(OBJ) init_s.$(OBJ) scroll_m.$(OBJ) ugly.$(OBJ) \
	uglylist.$(OBJ) version.$(OBJ)

reg::  
	copy texto_r.h texto.h

share:: 
        copy texto_s.h texto.h

sofsrc:: 
        copy texto_s.h texto.h

reg::  $(OBJS_r)
	$(LINKM_r)

share::  $(OBJS_s)
	$(LINKM_s)

sofsrc::  $(OBJS_k)
	$(LINKM_k)

oldshare:  $(OBJS_s) tower fixtower.exe
	$(LINKM_s)
	fixtower

beta:  $(OBJS_b) tower fixtower.exe
	$(LINKM_b)
	fixtower

moag_b.$(OBJ):  moag.c moag.h texto.h
	$(CC) $(CFLAGS) -DBETA=1 moag.c
	mv moag.$(OBJ) moag_b.$(OBJ)

moag_s.$(OBJ):  moag.c moag.h texto.h
	$(CC) $(CFLAGS) moag.c
	mv moag.$(OBJ) moag_s.$(OBJ)

moag_r.$(OBJ):  moag.c moag.h texto.h
	$(CC) $(CFLAGS) -DREGISTERED=1 moag.c
	mv moag.$(OBJ) moag_r.$(OBJ)

moag_k.$(OBJ):  moag.c moag.h texto.h
	$(CC) $(CFLAGS) -DSOFSRC=1 moag.c
	mv moag.$(OBJ) moag_k.$(OBJ)

readfont.obj:  readfont.asm
	$(MASM) readfont;

mfsa.$(OBJ):  mfsa.c 
	$(CC) $(CFLAGS) mfsa.c

misn_r.$(OBJ):  misn.c moag.h texto.h screens.h
	$(CC) $(CFLAGS) -DREGISTERED=1 misn.c
	mv misn.$(OBJ) misn_r.$(OBJ)

misn_s.$(OBJ):  misn.c moag.h texto.h screens.h
	$(CC) $(CFLAGS) misn.c
	mv misn.$(OBJ) misn_s.$(OBJ)

misn_k.$(OBJ):  misn.c moag.h texto.h screens.h
	$(CC) $(CFLAGS) misn.c
	mv misn.$(OBJ) misn_k.$(OBJ)

ryo.$(OBJ):  ryo.c moag.h texto.h
	$(CC) $(CFLAGS) ryo.c

roster.$(OBJ):  roster.c moag.h texto.h screens.h
	$(CC) $(CFLAGS) roster.c

hiscore.$(OBJ):  hiscore.c moag.h texto.h screens.h
	$(CC) $(CFLAGS) hiscore.c

reg_s.$(OBJ):  reg.c moag.h texto.h screens.h
	$(CC) $(CFLAGS) reg.c
	mv reg.$(OBJ) reg_s.$(OBJ)

reg_k.$(OBJ):  reg.c moag.h texto.h screens.h
	$(CC) $(CFLAGS) reg.c
	mv reg.$(OBJ) reg_k.$(OBJ)

reg_r.$(OBJ):  reg.c moag.h texto.h screens.h
	$(CC) $(CFLAGS) -DREGISTERED=1 reg.c
	mv reg.$(OBJ) reg_r.$(OBJ)

regstuff.c:  mkregst.exe $(USERNAME)
	mkregst $(USERNAME)

regstuff.$(OBJ):  regstuff.c
	$(CC) $(CFLAGS) regstuff.c

demo_r.$(OBJ):  demo.c moag.h texto.h screens.h
	$(CC) $(CFLAGS) -DREGISTERED=1 demo.c
	mv demo.$(OBJ) demo_r.$(OBJ)

demo_s.$(OBJ):  demo.c
	$(CC) $(CFLAGS) demo.c
	mv demo.$(OBJ) demo_s.$(OBJ)

demo_k.$(OBJ):  demo.c
	$(CC) $(CFLAGS) demo.c
	mv demo.$(OBJ) demo_k.$(OBJ)

kbjs.$(OBJ):  kbjs.c moag.h texto.h
	$(CC) $(CFLAGS) kbjs.c

sndedit.$(OBJ):  sndedit.c moag.h texto.h
	$(CC) $(CFLAGS) sndedit.c

scrnma_r.$(OBJ):  scrnma.c moag.h texto.h screens.h
	$(CC) $(CFLAGS)  -DREGISTERED=1 scrnma.c
	cp scrnma.$(OBJ) scrnma_r.$(OBJ)

scrnma_s.$(OBJ):  scrnma.c moag.h texto.h screens.h
	$(CC) $(CFLAGS) scrnma.c
	cp scrnma.$(OBJ) scrnma_s.$(OBJ)

texto_m.$(OBJ):  texto.c moag.h texto.h
	$(CC) $(CFLAGS) -DMOAG=1 texto.c
	cp texto.$(OBJ) texto_m.$(OBJ)

grapho_m.$(OBJ):  grapho.c texto.h
	$(CC) $(CFLAGS) -DMOAG=1 grapho.c
	cp graph.$(OBJ) graph_m.$(OBJ)

o_m.$(OBJ):  $(O)o_m.$(OBJ)
	cp $(O)o_m.$(OBJ) o_m.$(OBJ)

texto_n.$(OBJ):  texto.c moag.h texto.h
	$(CC) $(CFLAGS) texto.c
	cp texto.$(OBJ) texto_n.$(OBJ)

grapho_n.$(OBJ):  grapho.c texto.h
	$(CC) $(CFLAGS) grapho.c
	cp graph.$(OBJ) graph_n.$(OBJ)

o_n.$(OBJ):  $(O)o_n.$(OBJ)
	cp $(O)o_n.$(OBJ) o_n.$(OBJ)

input_r.$(OBJ):  input.c moag.h
	$(CC) $(CFLAGS) input.c
	mv input.obj input_r.obj

input_s.$(OBJ):  input.c moag.h
	$(CC) $(CFLAGS) input.c
	mv input.obj input_s.obj

input_k.$(OBJ):  input.c moag.h
	$(CC) $(CFLAGS) -DSOFSRC=1 input.c
	mv input.obj input_k.obj

go_s.$(OBJ):  go.c moag.h texto.h cecode.h
	$(CC) $(CFLAGS) go.c
	mv go.$(OBJ) go_s.$(OBJ)

go_k.$(OBJ):  go.c moag.h texto.h cecode.h
	$(CC) $(CFLAGS) -DSOFSRC=1 go.c
	mv go.$(OBJ) go_k.$(OBJ)

go_r.$(OBJ):  go.c moag.h texto.h cecode.h
	$(CC) $(CFLAGS) -DREGISTERED=1 go.c
	mv go.$(OBJ) go_r.$(OBJ)

go_t.$(OBJ):  go.c moag.h texto.h
	$(CC) $(CFLAGS) -DTU=1 go.c
	mv go.$(OBJ) go_t.$(OBJ)

report_r.$(OBJ):  report.c moag.h texto.h screens.h
	$(CC) $(CFLAGS) -DREGISTERED=1 report.c
	mv report.$(OBJ) report_r.$(OBJ)

report_s.$(OBJ):  report.c moag.h texto.h screens.h
	$(CC) $(CFLAGS) report.c
	mv report.$(OBJ) report_s.$(OBJ)

report_k.$(OBJ):  report.c moag.h texto.h screens.h
	$(CC) $(CFLAGS) -DSOFSRC=1 report.c
	mv report.$(OBJ) report_k.$(OBJ)

report_i.$(OBJ):  report.c moag.h texto.h screens.h
	$(CC) $(CFLAGS) -DISCORE report.c
	mv report.$(OBJ) report_i.$(OBJ)

promed_r.$(OBJ):  promedal.c moag.h texto.h
	$(CC) $(CFLAGS) -DREGISTERED=1 promedal.c
	mv promedal.$(OBJ) promed_r.$(OBJ)

promed_s.$(OBJ):  promedal.c moag.h texto.h
	$(CC) $(CFLAGS) promedal.c
	mv promedal.$(OBJ) promed_s.$(OBJ)

promed_k.$(OBJ):  promedal.c moag.h texto.h
	$(CC) $(CFLAGS) -DSOFSRC=1 promedal.c
	mv promedal.$(OBJ) promed_k.$(OBJ)

promed_i.$(OBJ):  promedal.c moag.h texto.h
	$(CC) $(CFLAGS) -DISCORE promedal.c
	mv promedal.$(OBJ) promed_i.$(OBJ)

cecode.$(OBJ):  cecode.c moag.h texto.h cecode.h
	$(CC) $(CFLAGS) cecode.c

3univs_r.$(OBJ):  3univs.c moag.h
	$(CC) $(CFLAGS) -DREGISTERED=1 3univs.c
	mv 3univs.$(OBJ) 3univs_r.$(OBJ)

3univs_s.$(OBJ):  3univs.c moag.h
	$(CC) $(CFLAGS) 3univs.c
	mv 3univs.$(OBJ) 3univs_s.$(OBJ)

3univs_k.$(OBJ):  3univs.c moag.h
	$(CC) $(CFLAGS) -DSOFSRC=1 3univs.c
	mv 3univs.$(OBJ) 3univs_k.$(OBJ)

3univs_t.$(OBJ):  3univs.c moag.h
	$(CC) $(CFLAGS) -DTU=1 -DREGISTERED=1 3univs.c
	mv 3univs.$(OBJ) 3univs_t.$(OBJ)

flash.$(OBJ):  flash.c moag.h texto.h screens.h
	$(CC) $(CFLAGS) flash.c

init_s.$(OBJ):  init.c moag.h
	$(CC) $(CFLAGS) init.c
	mv init.$(OBJ) init_s.$(OBJ)

init_k.$(OBJ):  init.c moag.h
	$(CC) $(CFLAGS) -DSOFSRC=1 init.c
	mv init.$(OBJ) init_k.$(OBJ)

init_r.$(OBJ):  init.c moag.h
	$(CC) $(CFLAGS) -DREGISTERED=1 init.c
	mv init.$(OBJ) init_r.$(OBJ)

scroll_m.$(OBJ):  scroll.c texto.h moag.h screens.h
	$(CC) $(CFLAGS) scroll.c
	mv scroll.$(OBJ) scroll_m.$(OBJ)

scroll_t.$(OBJ):  scroll.c texto.h moag.h screens.h
	$(CC) $(CFLAGS) -DTOWER scroll.c
	mv scroll.$(OBJ) scroll_t.$(OBJ)

ugly.$(OBJ):  ugly.c texto.h moag.h
	$(CC) $(CFLAGS) ugly.c

uglylist.$(OBJ):  uglylist.c
	$(CC) $(CFLAGS) uglylist.c

uglylist.c:  uglylist
	sh uglylist.ksh uglylist.c

version.$(OBJ):  version.c
	$(CC) $(CFLAGS) version.c

clean:
	-rm c.err *.mix *.obj *.map *.exe dog 3univ.dat towers.dat

veryclean:
	-rm c.err *.mix *.obj *.map *.exe dog 3univ.dat towers.dat
	-rm -r pilot.scr pilot.hsc tht

tu.$(OBJ):  tu.c texto.h screens.h
	$(CC) $(CFLAGS) tu.c

tu:  tu.$(OBJ) o_n.$(OBJ) scroll_t.$(OBJ) input.$(OBJ) \
						3univs_t.$(OBJ) go_t.$(OBJ)
	$(LINKT) tu.$(OBJ) o_n.$(OBJ) scroll_t.$(OBJ) input.$(OBJ) \
						3univs_t.$(OBJ) go_t.$(OBJ)

tower.$(OBJ):  tower.c texto.h screens.h
	$(CC) $(CFLAGS) tower.c

tower:  tower.$(OBJ) o_n.$(OBJ) scroll_t.$(OBJ) input.$(OBJ)
	$(LINKT) tower.$(OBJ) o_n.$(OBJ) scroll_t.$(OBJ) input.$(OBJ)

fixtower.exe:  fixtower.$(OBJ)
	$(LINKS) fixtower.$(OBJ)

fixtower.$(OBJ):  fixtower.c
	$(CC) $(CFLAGS) fixtower.c

mkregst.exe:  mkregst.$(OBJ)
	$(LINKS) mkregst.$(OBJ)

mkregst.$(OBJ):  mkregst.c
	$(CC) $(CFLAGS) mkregst.c

iscore.$(OBJ):  iscore.c moag.h texto.h
	$(CC) $(CFLAGS) iscore.c

iscore:  iscore.$(OBJ) report_i.$(OBJ) o_n.$(OBJ) \
					input.$(OBJ) promed_i.$(OBJ)
	$(LINKI) iscore.$(OBJ) report_i.$(OBJ) o_n.$(OBJ) \
					input.$(OBJ) promed_i.$(OBJ)


rolplay.obj:	rolplay.c
	bcc -mm -c rolplay.c

rolread.obj:	rolread.c
	bcc -mm -c rolread.c

insbank.obj:	insbank.c
	bcc -mm -c insbank.c

csoundc.obj:	csoundc.c
	bcc -mm -c csoundc.c


tt.$(OBJ):  tt.c
	$(CC) $(CFLAGS) tt.c

tt:  tt.$(OBJ)
	pcl @tt.pcl

outchip.obj: outchip.asm
	masm outchip.asm;

freeup.obj: freeup.asm
	masm freeup.asm;

adldet.obj:	adldet.c
	bcc -mm -c adldet.c

loadsnd.obj:	loadsnd.c
	bcc -mm -c loadsnd.c
