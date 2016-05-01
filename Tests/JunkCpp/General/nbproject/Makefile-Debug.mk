#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=clang-3.6
CCC=clang++-3.6
CXX=clang++-3.6
FC=gfortran
AS=as

# Macros
CND_PLATFORM=CLang-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/6d18186c/Clock.o \
	${OBJECTDIR}/_ext/6d18186c/Directory.o \
	${OBJECTDIR}/_ext/6d18186c/Error.o \
	${OBJECTDIR}/_ext/6d18186c/File.o \
	${OBJECTDIR}/_ext/6d18186c/FilePath.o \
	${OBJECTDIR}/_ext/6d18186c/JRpc.o \
	${OBJECTDIR}/_ext/6d18186c/MMFile.o \
	${OBJECTDIR}/_ext/6d18186c/Socket.o \
	${OBJECTDIR}/_ext/6d18186c/Str.o \
	${OBJECTDIR}/_ext/6d18186c/Thread.o \
	${OBJECTDIR}/main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lpthread

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/general

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/general: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/general ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/6d18186c/Clock.o: ../../../JunkCpp/Clock.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/6d18186c
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/6d18186c/Clock.o ../../../JunkCpp/Clock.cpp

${OBJECTDIR}/_ext/6d18186c/Directory.o: ../../../JunkCpp/Directory.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/6d18186c
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/6d18186c/Directory.o ../../../JunkCpp/Directory.cpp

${OBJECTDIR}/_ext/6d18186c/Error.o: ../../../JunkCpp/Error.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/6d18186c
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/6d18186c/Error.o ../../../JunkCpp/Error.cpp

${OBJECTDIR}/_ext/6d18186c/File.o: ../../../JunkCpp/File.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/6d18186c
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/6d18186c/File.o ../../../JunkCpp/File.cpp

${OBJECTDIR}/_ext/6d18186c/FilePath.o: ../../../JunkCpp/FilePath.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/6d18186c
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/6d18186c/FilePath.o ../../../JunkCpp/FilePath.cpp

${OBJECTDIR}/_ext/6d18186c/JRpc.o: ../../../JunkCpp/JRpc.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/6d18186c
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/6d18186c/JRpc.o ../../../JunkCpp/JRpc.cpp

${OBJECTDIR}/_ext/6d18186c/MMFile.o: ../../../JunkCpp/MMFile.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/6d18186c
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/6d18186c/MMFile.o ../../../JunkCpp/MMFile.cpp

${OBJECTDIR}/_ext/6d18186c/Socket.o: ../../../JunkCpp/Socket.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/6d18186c
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/6d18186c/Socket.o ../../../JunkCpp/Socket.cpp

${OBJECTDIR}/_ext/6d18186c/Str.o: ../../../JunkCpp/Str.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/6d18186c
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/6d18186c/Str.o ../../../JunkCpp/Str.cpp

${OBJECTDIR}/_ext/6d18186c/Thread.o: ../../../JunkCpp/Thread.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/6d18186c
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/6d18186c/Thread.o ../../../JunkCpp/Thread.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/general

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
