BREW_LIBS="libwebsockets libev mosquitto jansson"
CFLAGS="$CFLAGS `brewlibs $BREW_LIBS`"

SRC="variable_scope.c ../src/subscriber.c ../src/llog.c"

export CFLAGS
export SRC
