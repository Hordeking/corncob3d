OBJ = mix
LINK = pcl
CC = pc

otn:  otn.$(OBJ) new.$(OBJ) old.$(OBJ)
	$(LINK) otn.$(OBJ) new.$(OBJ) old.$(OBJ)

otn.$(OBJ): otn.c p.h
	$(CC) -c otn.c

old.$(OBJ): old.c p.h
	$(CC) -c old.c

new.$(OBJ): new.c p.h
	$(CC) -c new.c

clean:
	-rm *.mix *.exe c.err tht/* pilot.scr
