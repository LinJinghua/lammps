
TARGET	 = msi2lmp.exe

SRCS     = msi2lmp.cpp \
           ReadCarFile.cpp \
           ReadMdfFile.cpp \
           ReadPrmFile.cpp \
	   MakeLists.cpp \
           ReadFrcFile.cpp \
           InitializeItems.cpp \
	   SearchAndFill.cpp \
           GetParameters.cpp \
	   CheckLists.cpp \
           WriteDataFile.cpp

OBJS     = $(SRCS:.cpp=.o)

HEADERS  = msi2lmp.h Forcefield.h PrmData.h

CC       = g++
CFLAGS   = -O -Wall -W -g -std=c++1z
LDFLAGS  =
FRCFILE  = cvff.frc
FRCFILE2 = cff91.frc
README   = README
MKFILE   = Makefile

$(TARGET) : $(OBJS)
	$(CC) $(LDFLAGS) $(CFLAGS) -o $(TARGET) $(OBJS) -lm

.cpp.o:
	$(CC) $(CFLAGS) -c $<
clean:
	rm -f $(OBJS) $(TARGET)

# dependencies
CheckLists.o: CheckLists.cpp msi2lmp.h
GetParameters.o: GetParameters.cpp msi2lmp.h Forcefield.h
InitializeItems.o: InitializeItems.cpp msi2lmp.h Forcefield.h
MakeLists.o: MakeLists.cpp msi2lmp.h
msi2lmp.o: msi2lmp.cpp msi2lmp.h
ReadCarFile.o: ReadCarFile.cpp msi2lmp.h
ReadFrcFile.o: ReadFrcFile.cpp msi2lmp.h Forcefield.h
ReadMdfFile.o: ReadMdfFile.cpp msi2lmp.h
ReadPrmFile.o: ReadPrmFile.cpp msi2lmp.h PrmData.h
SearchAndFill.o: SearchAndFill.cpp msi2lmp.h Forcefield.h
WriteDataFile.o: WriteDataFile.cpp msi2lmp.h Forcefield.h
