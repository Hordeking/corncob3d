# Put in the full pathname if not in current dir:
#USERNAME = c:/asm/3d/3dsound/username.dat
USERNAME = ./username.dat

# Choose text or graphics mode
O = text
# O = graph

OBJ = obj
CC = bcc
CFLAGS = -c -K -mm -d
TURBOC = c:\borlandc
MASM = masm
LINKM	= tlink /m $(TURBOC)\lib\c0m @moag.lnk, moag, moag, $(TURBOC)\lib\cm
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

RFLAGS = -DREGISTERED
RFLAGS_ASM = /Dshareflag=0

OBJS = moag.$(OBJ) misn.$(OBJ) ryo.$(OBJ) roster.$(OBJ) hiscore.$(OBJ) \
	reg.$(OBJ) demo.$(OBJ) kbjs.$(OBJ) sndedit.$(OBJ) scrnma.$(OBJ) \
	o_m.$(OBJ) input.$(OBJ) go.$(OBJ) report.$(OBJ) promed.$(OBJ) \
	cecode.$(OBJ) 3univs.$(OBJ) flash.$(OBJ) init.$(OBJ) \
	scroll_m.$(OBJ) ugly.$(OBJ) uglylist.$(OBJ) version.$(OBJ) \
	regstuff.$(OBJ) readfont.obj mfsa.obj \
	rolplay.obj rolread.obj insbank.obj csoundc.obj outchip.obj \
	adldet.obj loadsnd.obj freeup.obj

#reg:
#	copy texto_r.h texto.h

#share:
#	copy texto_s.h texto.h

#sofsrc:
#	copy texto_s.h texto.h

reg: $(OBJS)
	$(LINKM)

moag.$(OBJ): moag.c moag.h texto.h
	$(CC) $(CFLAGS) $(RFLAGS) $(RFLAGS) moag.c

3univs.$(OBJ): 3univs.c moag.h
	$(CC) $(CFLAGS) $(RFLAGS) 3univs.c

demo.$(OBJ): demo.c moag.h texto.h screens.h
	$(CC) $(CFLAGS) $(RFLAGS) demo.c

go.$(OBJ): go.c moag.h texto.h cecode.h
	$(CC) $(CFLAGS) $(RFLAGS) go.c

init.$(OBJ): init.c moag.h
	$(CC) $(CFLAGS) $(RFLAGS) init.c

input.$(OBJ): input.c moag.h
	$(CC) $(CFLAGS) $(RFLAGS) -oinput.obj input.c

misn.$(OBJ): misn.c moag.h texto.h screens.h
	$(CC) $(CFLAGS) $(RFLAGS) $(RFLAGS) misn.c

promed.$(OBJ): promedal.c moag.h texto.h
	$(CC) $(CFLAGS) $(RFLAGS) promedal.c

reg.$(OBJ): reg.c moag.h texto.h screens.h
	$(CC) $(CFLAGS) $(RFLAGS) reg.c

report.$(OBJ): report.c moag.h texto.h screens.h
	$(CC) $(CFLAGS) $(RFLAGS) report.c

scrnma.$(OBJ): scrnma.c moag.h texto.h screens.h
	$(CC) $(CFLAGS) $(RFLAGS) scrnma.c



scroll_m.$(OBJ): scroll.c texto.h moag.h screens.h
	$(CC) $(CFLAGS) $(RFLAGS) -oscroll_m.$(OBJ) scroll.c



adldet.obj: adldet.c
	$(CC) $(CFLAGS) $(RFLAGS) adldet.c

cecode.$(OBJ): cecode.c moag.h texto.h cecode.h
	$(CC) $(CFLAGS) $(RFLAGS) cecode.c

csoundc.obj: csoundc.c
	$(CC) $(CFLAGS) $(RFLAGS) csoundc.c

flash.$(OBJ): flash.c moag.h texto.h screens.h
	$(CC) $(CFLAGS) $(RFLAGS) flash.c

freeup.obj: freeup.asm
	$(MASM) $(RFLAGS_ASM) freeup.asm

hiscore.$(OBJ): hiscore.c moag.h texto.h screens.h
	$(CC) $(CFLAGS) $(RFLAGS) hiscore.c

insbank.obj: insbank.c
	$(CC) $(CFLAGS) $(RFLAGS) insbank.c

kbjs.$(OBJ): kbjs.c moag.h texto.h
	$(CC) $(CFLAGS) $(RFLAGS) kbjs.c

loadsnd.$(OBJ): loadsnd.c
	$(CC) $(CFLAGS) $(RFLAGS) loadsnd.c

mfsa.$(OBJ): mfsa.c 
	$(CC) $(CFLAGS) $(RFLAGS) mfsa.c

outchip.obj: outchip.asm
	$(MASM) $(RFLAGS_ASM) outchip.asm

readfont.obj: readfont.asm
	$(MASM) $(RFLAGS_ASM) readfont.asm

regstuff.$(OBJ): regstuff.c
	$(CC) $(CFLAGS) $(RFLAGS) regstuff.c

rolplay.$(OBJ): rolplay.c
	$(CC) $(CFLAGS) $(RFLAGS) rolplay.c

rolread.$(OBJ): rolread.c
	$(CC) $(CFLAGS) $(RFLAGS) rolread.c

roster.$(OBJ): roster.c moag.h texto.h screens.h
	$(CC) $(CFLAGS) $(RFLAGS) roster.c

ryo.$(OBJ): ryo.c moag.h texto.h
	$(CC) $(CFLAGS) $(RFLAGS) ryo.c
	
sndedit.$(OBJ): sndedit.c moag.h texto.h
	$(CC) $(CFLAGS) $(RFLAGS) sndedit.c

ugly.$(OBJ): ugly.c texto.h moag.h
	$(CC) $(CFLAGS) $(RFLAGS) ugly.c

uglylist.$(OBJ): uglylist.c
	$(CC) $(CFLAGS) $(RFLAGS) uglylist.c
	
version.$(OBJ): version.c
	$(CC) $(CFLAGS) version.c
	
#Housekeeping
clean:
	-rm c.err *.mix *.obj *.map *.exe dog 3univ.dat towers.dat

veryclean:
	-rm c.err *.mix *.obj *.map *.exe dog 3univ.dat towers.dat
	-rm -r pilot.scr pilot.hsc tht

#End of MOAG Build

#This was some kind of Korn Shell script that generated uglylist.c from uglylist
#uglylist.c: uglylist
#	sh uglylist.ksh uglylist.c







#Begin Unknown
texto_m.$(OBJ): texto.c moag.h texto.h
	$(CC) $(CFLAGS) -DMOAG=1 texto.c
	cp texto.$(OBJ) texto_m.$(OBJ)

grapho_m.$(OBJ): grapho.c texto.h
	$(CC) $(CFLAGS) -DMOAG=1 grapho.c
	cp graph.$(OBJ) graph_m.$(OBJ)

texto_n.$(OBJ): texto.c moag.h texto.h
	$(CC) $(CFLAGS) texto.c
	cp texto.$(OBJ) texto_n.$(OBJ)

grapho_n.$(OBJ): grapho.c texto.h
	$(CC) $(CFLAGS) grapho.c
	cp graph.$(OBJ) graph_n.$(OBJ)
	
#End Unknown


3univs_t.$(OBJ): 3univs.c moag.h
	$(CC) $(CFLAGS) $(RFLAGS) -DTU=1 -o3univs_t.$(OBJ) 3univs.c
#	mv 3univs.$(OBJ) 3univs_t.$(OBJ)


fixtower.exe: fixtower.$(OBJ)
	$(LINKS) fixtower.$(OBJ)

fixtower.$(OBJ): fixtower.c
	$(CC) $(CFLAGS) $(RFLAGS) fixtower.c

regstuff.c: mkregst.exe $(USERNAME)
	mkregst $(USERNAME)

mkregst.exe: mkregst.$(OBJ)
	$(LINKS) mkregst.$(OBJ)

mkregst.$(OBJ): mkregst.c
	$(CC) $(CFLAGS) $(RFLAGS) mkregst.c


#ISCORE STUFF?

iscore: iscore.$(OBJ) report_i.$(OBJ) o_n.$(OBJ) \
					input_i.$(OBJ) promed_i.$(OBJ)
	$(LINKI) iscore.$(OBJ) report_i.$(OBJ) o_n.$(OBJ) \
					input_i.$(OBJ) promed_i.$(OBJ)
#	tlink $(TURBOC)\lib\c0m @iscore.lnk, iscore, iscore, $(TURBOC)\lib\cm

iscore.$(OBJ): iscore.c moag.h texto.h
	$(CC) $(CFLAGS) $(RFLAGS) iscore.c
	
input_i.$(OBJ): input.c moag.h texto.h
	$(CC) $(CFLAGS) $(RFLAGS) -DISCORE -oinput_i.$(OBJ) input.c

report_i.$(OBJ): report.c moag.h texto.h screens.h
	$(CC) $(CFLAGS) $(RFLAGS) -DISCORE -oreport_i.$(OBJ) report.c

promed_i.$(OBJ): promedal.c moag.h texto.h
	$(CC) $(CFLAGS) $(RFLAGS) -DISCORE -opromed_i.$(OBJ) promedal.c

#END ISCORE STUFF?


#TOWER STUFF?

tower: tower.$(OBJ) o_n.$(OBJ) scroll_t.$(OBJ) input.$(OBJ)
	$(LINKT) tower.$(OBJ) o_n.$(OBJ) scroll_t.$(OBJ) input_i.$(OBJ)

tower.$(OBJ): tower.c $(O)o.h screens.h
	$(CC) $(CFLAGS) $(RFLAGS) tower.c

scroll_t.$(OBJ): scroll.c texto.h moag.h screens.h
	$(CC) $(CFLAGS) $(RFLAGS) -oscroll_t.$(OBJ) -DTOWER scroll.c

#END TOWER STUFF?


#TU.EXE STUFF

tu: tu.$(OBJ) o_n.$(OBJ) scroll_t.$(OBJ) input_i.$(OBJ) \
						3univs_t.$(OBJ) go_t.$(OBJ) report_t.$(OBJ)
	$(LINKT) tu.$(OBJ) o_n.$(OBJ) scroll_t.$(OBJ) input_i.$(OBJ) \
						3univs_t.$(OBJ) go_t.$(OBJ) report_t.$(OBJ)

tu.$(OBJ): tu.c texto.h screens.h
	$(CC) $(CFLAGS) $(RFLAGS) tu.c

report_t.$(OBJ): report.c moag.h texto.h screens.h
	$(CC) $(CFLAGS) $(RFLAGS) -oreport_t.$(OBJ) report.c

go_t.$(OBJ): go.c moag.h texto.h
	$(CC) $(CFLAGS) $(RFLAGS) -DTU -DREGISTERED -ogo_t.$(OBJ) go.c
	
#END TU.EXE

#Builds texto.obj or grapho.obj, depending on what you set O to.

o_m.$(OBJ): $(O)o_m.$(OBJ)

o_n.$(OBJ): $(O)o_n.$(OBJ)

$(O)o_m.$(OBJ): $(O)o.c moag.h $(O)o.h
	$(CC) $(CFLAGS) $(RFLAGS) -DMOAG=1 -oo_m.$(OBJ) $(O)o.c

$(O)o_n.$(OBJ): $(O)o.c moag.h $(O)o.h
	$(CC) $(CFLAGS) -oo_n.$(OBJ) $(O)o.c
