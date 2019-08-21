#!/bin/sh

PATH='/usr/lib/google-golang/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/local/google/home/enh/bin:/usr/local/google/home/enh/bin'

BUILD='cc -Wall -Wundef -Wno-char-subscripts -Werror=implicit-function-declaration -funsigned-char -I . -Os -ffunction-sections -fdata-sections -fno-asynchronous-unwind-tables -fno-strict-aliasing -DTOYBOX_VERSION="android-o-mr1-iot-release-1.0.14-80-g88dfc8df55f2"'

LINK='-Wl,--gc-sections -o generated/unstripped/toybox -Wl,--as-needed -lutil -lcrypt -lm -lresolv -lselinux -lcrypto -lz'

FILES='lib/args.c
lib/commas.c
lib/deflate.c
lib/dirtree.c
lib/env.c
lib/lib.c
lib/linestack.c
lib/llist.c
lib/net.c
lib/password.c
lib/portability.c
lib/tty.c
lib/xwrap.c lib/help.c main.c toys/lsb/hostname.c
toys/lsb/md5sum.c
toys/lsb/mktemp.c
toys/lsb/seq.c
toys/net/microcom.c
toys/other/dos2unix.c
toys/other/readlink.c
toys/other/realpath.c
toys/other/stat.c
toys/other/timeout.c
toys/other/which.c
toys/other/xxd.c
toys/pending/bc.c
toys/pending/dd.c
toys/pending/diff.c
toys/pending/expr.c
toys/pending/tr.c
toys/posix/basename.c
toys/posix/cat.c
toys/posix/chmod.c
toys/posix/cmp.c
toys/posix/comm.c
toys/posix/cp.c
toys/posix/cut.c
toys/posix/date.c
toys/posix/dirname.c
toys/posix/du.c
toys/posix/echo.c
toys/posix/find.c
toys/posix/getconf.c
toys/posix/grep.c
toys/posix/head.c
toys/posix/id.c
toys/posix/ln.c
toys/posix/ls.c
toys/posix/mkdir.c
toys/posix/od.c
toys/posix/paste.c
toys/posix/patch.c
toys/posix/pwd.c
toys/posix/rm.c
toys/posix/rmdir.c
toys/posix/sed.c
toys/posix/sort.c
toys/posix/tail.c
toys/posix/tar.c
toys/posix/tee.c
toys/posix/touch.c
toys/posix/true.c
toys/posix/uname.c
toys/posix/uniq.c
toys/posix/wc.c
toys/posix/xargs.c'


$BUILD $FILES $LINK
