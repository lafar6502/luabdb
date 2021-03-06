CC = gcc
CFLAGS = -O2 -I ../../luext/lua
LINKFLAGS=-L ../../luext/lua

OBJECTS = cursor_ops.o \
    db_ops.o \
    env_ops.o \
    flags.o \
    lock_ops.o \
    log_ops.o \
    mem_pool_ops.o \
    mutex_ops.o \
    replication_ops.o \
    sequence_ops.o \
    txn_ops.o \
    tester_ops.o \
    utils.o \
    db_udata_ops.o \
    bdb.o
