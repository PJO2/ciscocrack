# Wedit Makefile for project CiscoCrack

SRCDIR=d:\pjo\prgm\ciscocrack
LCCROOT=C:\lcc
CFLAGS=-IC:\lcc\include  -I.. -Zp1
LDFLAGS =    -s -subsystem windows
CC=$(LCCROOT)\bin\lcc.exe
LINKER=$(LCCROOT)\bin\lcclnk.exe -s

TARGET=ciscocrack

OBJS=  \
        crack_main.obj \
        $(TARGET).obj \



LIBS=shell32.lib wsock32.lib
EXE=$(TARGET).exe

$(EXE):	$(OBJS) $(TARGET).res Makefile
	$(LINKER)  -subsystem windows -o $(SRCDIR)\$(TARGET).exe $(TARGET).res $(OBJS) $(LIBS)

# Build $(TARGET).c
$TARGET_DEP_C= \
	$(SRCDIR)\$(TARGET).h

$(TARGET).obj: $(TARGET_DEP_C) $(SRCDIR)\$(TARGET).c
	$(CC) -c $(CFLAGS) $(SRCDIR)\$(TARGET).c


# Build crack_main.c
crack_main_C=\
	$(SRCDIR)\$(TARGET).h	

crack_main.obj: $(crack_main_C) $(SRCDIR)\crack_main.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\crack_main.c


# Build $(TARGET).res
$(TARGET)_RC=\
	$(SRCDIR)\$(TARGET).h

$(TARGET).res:	$(SRCDIR)\$(TARGET).rc
	$(LCCROOT)\bin\lrc.exe -I$(SRCDIR) -IC:\lcc\include  $(SRCDIR)\$(TARGET).rc


clean:
	del $(OBJS) $(TARGET).res $(SRCDIR)\$(TARGET).exe

