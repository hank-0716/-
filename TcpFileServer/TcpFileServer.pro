#-------------------------------------------------
#
# Project created by QtCreator 2016-11-19T12:21:11
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TcpFileServer
TEMPLATE = app


SOURCES += main.cpp\
        ../TcpFileSender/tcpfilesender.cpp \
        tcpfileserver.cpp

HEADERS  += tcpfileserver.h \
    ../TcpFileSender/tcpfilesender.h \
    FileTransferApp.h
