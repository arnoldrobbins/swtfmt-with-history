#! /bin/sh
# fos --- 'format', 'overstrike', and 'spool'
#
# run swtfmt on the named files, turn into overstrikes necessary for
# the Xerox 9700 laser printer, and send it to the lp spooler program
# (this is only good on the Pyramid, 'gitpyr', which spools to the Xerox)
#
# the name 'fos' is a holdover from the primes. sigh.

PATH=/bin:/usr/bin:/usr/ucb:/usr/local/bin

# set up sed command file
cat > /tmp/fos.$$ << '-EOF'
2a\
+\ \ DJDE SHIFT=YES,
-EOF

if [ "`hostname`" = gitpyr ]
then
	HOLES=
	SHIFT=cat
	case "$1" in
	-h)	HOLES=-ofc=xd
		SHIFT="sed -f /tmp/fos.$$"	# to shift output on holed paper
		shift
		;;
	
	# give no other diagnostics, so that options can be passed on to swtfmt
	# -*)	echo "Usage: `basename $0` [-h] [file ...]" 1>&2
	# 	exit 1
	# 	;;
	esac

	swtfmt $* | lz | $SHIFT | lp -tfos.out $HOLES
	rm -f /tmp/fos.$$
else
	# one of the vaxen
	if [ "$1" = -h ]
	then
		shift
	fi
	swtfmt $* | lpr
fi
