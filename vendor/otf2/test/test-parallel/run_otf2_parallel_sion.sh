#!/bin/sh

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2013-2014, 2018,
## Technische Universitaet Dresden, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

set -e

cleanup()
{
    rm -rf test-trace-parallel-sion-*
}
trap cleanup EXIT
cleanup

getsubset()
{
    local n=$1
    local i=$2
    local ns=0
    local is=""
    for b in $(seq 0 $(($n - 1)))
    do
        if test $(((1 << $b) & $i)) -gt 0
        then
            ns=$((ns + 1))
            is="$is $b"
        fi
    done
    echo $is
}

tests=0
successes=0
fails=0
failed=
failed_sep=
nl='
'

n=0
# number of writers
for w in 1 2
do

    # numer of threads per writer
    for l in 1 2
    do
        # total number of locations
        i=$((w * l))

        # number of files; 0 equals 1 file, but without sub-communicator
        for f in $(seq 0 $w)
        do
            t=test-trace-parallel-sion-$w.$l.$f

            tests=$((tests + 1))
            n=$((n + 1))
            writer_cmd="./test-writer -nt $w $t 1 $l 0 $f"
            test ${V-0} -eq 1 && echo "$w.$l - Writing SION trace with $w rank(s) and $l threads each into $f file(s) (w/ local defs): $writer_cmd"
            if $VALGRIND $writer_cmd
            then
                successes=$((successes + 1))
            else
                fails=$((fails + 1))
                failed="$failedfailed_sep$writer_cmd"
                failed_sep=$nl
                continue
            fi

            m=0
            # number of readers, one case to have more readers that actuall locations
            for r in $(seq 1 $((i + 1)))
            do

                # list of locations to read
                for k in $(seq 0 $(((1 << $i) - 1)))
                do
                    s=$(getsubset $i $k)

                    tests=$((tests + 1))
                    m=$((m + 1))
                    reader_cmd="./test-reader -nt $r $t.otf2 0 0 $s"
                    test ${V-0} -eq 1 && echo "$w.$l.$n.$m - Reading with $r rank(s) |{$s}| out of $i location(s) out of $f files (w/o sub-groups; w/o global reader; w/o global-reader hint): $reader_cmd"
                    if $VALGRIND $reader_cmd
                    then
                        successes=$((successes + 1))
                    else
                        fails=$((fails + 1))
                        failed="$failed$writer_cmd$nl$reader_cmd"
                        writer_cmd=
                    fi

                    tests=$((tests + 1))
                    m=$((m + 1))
                    reader_cmd="./test-reader -nt $r $t.otf2 1 0 $s"
                    test ${V-0} -eq 1 && echo "$w.$l.$n.$m - Reading with $r rank(s) |{$s}| out of $i location(s) out of $f files (w/ sub-groups; w/o global reader; w/o global-reader hint): $reader_cmd"
                    if $VALGRIND $reader_cmd
                    then
                        successes=$((successes + 1))
                    else
                        fails=$((fails + 1))
                        failed="$failed$writer_cmd$nl$reader_cmd"
                        writer_cmd=
                    fi

                    tests=$((tests + 1))
                    m=$((m + 1))
                    reader_cmd="./test-reader -nt $r $t.otf2 2 0 $s"
                    test ${V-0} -eq 1 && echo "$w.$l.$n.$m - Reading with $r rank(s) |{$s}| out of $i location(s) out of $f files (w/ serial; w/o global reader; w/o global-reader hint): $reader_cmd"
                    if $VALGRIND $reader_cmd
                    then
                        successes=$((successes + 1))
                    else
                        fails=$((fails + 1))
                        failed="$failed$writer_cmd$nl$reader_cmd"
                        writer_cmd=
                    fi

                    tests=$((tests + 1))
                    m=$((m + 1))
                    reader_cmd="./test-reader -nt $r $t.otf2 0 1 $s"
                    test ${V-0} -eq 1 && echo "$w.$l.$n.$m - Reading with $r rank(s) |{$s}| out of $i location(s) out of $f files (w/o sub-groups; w/o global reader; w/ global-reader hint): $reader_cmd"
                    if $VALGRIND $reader_cmd
                    then
                        successes=$((successes + 1))
                    else
                        fails=$((fails + 1))
                        failed="$failed$writer_cmd$nl$reader_cmd"
                        writer_cmd=
                    fi

                    tests=$((tests + 1))
                    m=$((m + 1))
                    reader_cmd="./test-reader -nt $r $t.otf2 1 1 $s"
                    test ${V-0} -eq 1 && echo "$w.$l.$n.$m - Reading with $r rank(s) |{$s}| out of $i location(s) out of $f files (w/ sub-groups; w/o global reader; w/ global-reader hint): $reader_cmd"
                    if $VALGRIND $reader_cmd
                    then
                        successes=$((successes + 1))
                    else
                        fails=$((fails + 1))
                        failed="$failed$writer_cmd$nl$reader_cmd"
                        writer_cmd=
                    fi

                    tests=$((tests + 1))
                    m=$((m + 1))
                    reader_cmd="./test-reader -nt $r $t.otf2 2 1 $s"
                    test ${V-0} -eq 1 && echo "$w.$l.$n.$m - Reading with $r rank(s) |{$s}| out of $i location(s) out of $f files (w/ serial; w/o global reader; w/ global-reader hint): $reader_cmd"
                    if $VALGRIND $reader_cmd
                    then
                        successes=$((successes + 1))
                    else
                        fails=$((fails + 1))
                        failed="$failed$writer_cmd$nl$reader_cmd"
                        writer_cmd=
                    fi

                    tests=$((tests + 1))
                    m=$((m + 1))
                    reader_cmd="./test-reader -nt $r $t.otf2 0 2 $s"
                    test ${V-0} -eq 1 && echo "$w.$l.$n.$m - Reading with $r rank(s) |{$s}| out of $i location(s) out of $f files (w/o sub-groups; w/ global reader; w/o global-reader hint): $reader_cmd"
                    if $VALGRIND $reader_cmd
                    then
                        successes=$((successes + 1))
                    else
                        fails=$((fails + 1))
                        failed="$failed$writer_cmd$nl$reader_cmd"
                        writer_cmd=
                    fi

                    tests=$((tests + 1))
                    m=$((m + 1))
                    reader_cmd="./test-reader -nt $r $t.otf2 1 2 $s"
                    test ${V-0} -eq 1 && echo "$w.$l.$n.$m - Reading with $r rank(s) |{$s}| out of $i location(s) out of $f files (w/ sub-groups; w/ global reader; w/o global-reader hint): $reader_cmd"
                    if $VALGRIND $reader_cmd
                    then
                        successes=$((successes + 1))
                    else
                        fails=$((fails + 1))
                        failed="$failed$writer_cmd$nl$reader_cmd"
                        writer_cmd=
                    fi

                    tests=$((tests + 1))
                    m=$((m + 1))
                    reader_cmd="./test-reader -nt $r $t.otf2 2 2 $s"
                    test ${V-0} -eq 1 && echo "$w.$l.$n.$m - Reading with $r rank(s) |{$s}| out of $i location(s) out of $f files (w/ serial; w/ global reader; w/o global-reader hint): $reader_cmd"
                    if $VALGRIND $reader_cmd
                    then
                        successes=$((successes + 1))
                    else
                        fails=$((fails + 1))
                        failed="$failed$writer_cmd$nl$reader_cmd"
                        writer_cmd=
                    fi

                    tests=$((tests + 1))
                    m=$((m + 1))
                    reader_cmd="./test-reader -nt $r $t.otf2 0 3 $s"
                    test ${V-0} -eq 1 && echo "$w.$l.$n.$m - Reading with $r rank(s) |{$s}| out of $i location(s) out of $f files (w/o sub-groups; w/ global reader; w/ global-reader hint): $reader_cmd"
                    if $VALGRIND $reader_cmd
                    then
                        successes=$((successes + 1))
                    else
                        fails=$((fails + 1))
                        failed="$failed$writer_cmd$nl$reader_cmd"
                        writer_cmd=
                    fi

                    tests=$((tests + 1))
                    m=$((m + 1))
                    reader_cmd="./test-reader -nt $r $t.otf2 1 3 $s"
                    test ${V-0} -eq 1 && echo "$w.$l.$n.$m - Reading with $r rank(s) |{$s}| out of $i location(s) out of $f files (w/ sub-groups; w/ global reader; w/ global-reader hint): $reader_cmd"
                    if $VALGRIND $reader_cmd
                    then
                        successes=$((successes + 1))
                    else
                        fails=$((fails + 1))
                        failed="$failed$writer_cmd$nl$reader_cmd"
                        writer_cmd=
                    fi

                    tests=$((tests + 1))
                    m=$((m + 1))
                    reader_cmd="./test-reader -nt $r $t.otf2 2 3 $s"
                    test ${V-0} -eq 1 && echo "$w.$l.$n.$m - Reading with $r rank(s) |{$s}| out of $i location(s) out of $f files (w/ serial; w/ global reader; w/ global-reader hint): $reader_cmd"
                    if $VALGRIND $reader_cmd
                    then
                        successes=$((successes + 1))
                    else
                        fails=$((fails + 1))
                        failed="$failed$writer_cmd$nl$reader_cmd"
                        writer_cmd=
                    fi
                done
            done

            # test without local def files
            t="$t-nold"
            tests=$((tests + 1))
            writer_cmd="./test-writer -nt $w $t 0 $l 0 $f"
            test ${V-0} -eq 1 && echo "$w.$l - Writing SION trace with $w rank(s) and $l threads each into $f file(s) (w/o local defs): $writer_cmd"
            if $VALGRIND $writer_cmd
            then
                successes=$((successes + 1))
            else
                fails=$((fails + 1))
                failed="$failedfailed_sep$writer_cmd"
                failed_sep=$nl
                continue
            fi

            # read the trace once
            r=$w
            k=$(((1 << $i) - 1))
            s=$(getsubset $i $k)
            tests=$((tests + 1))
            m=$((m + 1))
            reader_cmd="./test-reader -nt $r $t.otf2 0 0 $s"
            test ${V-0} -eq 1 && echo "$w.$l.$n.$m - Reading with $r rank(s) |{$s}| out of $i location(s) out of $f files (w/o sub-groups; w/o global-reader hint): $reader_cmd"
            if $VALGRIND $reader_cmd
            then
                successes=$((successes + 1))
            else
                fails=$((fails + 1))
                failed="$failed$writer_cmd$nl$reader_cmd"
                writer_cmd=
            fi
        done

        cleanup
    done

    # write where sub-threads have no events
    l=2
    i=$((w * $l))
    m=0

    for f in $(seq 0 $w)
    do
        t=test-trace-parallel-sion-$w.$l.$f-ne

        tests=$((tests + 1))
        writer_cmd="./test-writer -nt $w $t 1 $l 1 $f"
        test ${V-0} -eq 1 && echo "$w.$l - Writing SION trace with $w rank(s) and $l threads each into $f file(s) (w/ empty threads): $writer_cmd"
        if $VALGRIND $writer_cmd
        then
            successes=$((successes + 1))
        else
            fails=$((fails + 1))
            failed="$failedfailed_sep$writer_cmd"
            failed_sep=$nl
            continue
        fi

        # read the trace once with and without global event reader
        r=$w
        k=$(((1 << $i) - 1))
        s=$(getsubset $i $k)

        tests=$((tests + 1))
        m=$((m + 1))
        reader_cmd="./test-reader -nt $r $t.otf2 0 1 $s"
        test ${V-0} -eq 1 && echo "$w.$l.$n.$m - Reading with $r rank(s) |{$s}| out of $i location(s) out of $f files (w/o sub-groups; w/o global-reader; w/ global-reader hint): $reader_cmd"
        if $VALGRIND $reader_cmd
        then
            successes=$((successes + 1))
        else
            fails=$((fails + 1))
            failed="$failed$writer_cmd$nl$reader_cmd"
            writer_cmd=
        fi

        tests=$((tests + 1))
        m=$((m + 1))
        reader_cmd="./test-reader -nt $r $t.otf2 0 3 $s"
        test ${V-0} -eq 1 && echo "$w.$l.$n.$m - Reading with $r rank(s) |{$s}| out of $i location(s) out of $f files (w/o sub-groups; w/ global-reader; w/ global-reader hint): $reader_cmd"
        if $VALGRIND $reader_cmd
        then
            successes=$((successes + 1))
        else
            fails=$((fails + 1))
            failed="$failed$writer_cmd$nl$reader_cmd"
            writer_cmd=
        fi
    done
done

echo "Success: $successes / $tests"
test $fails -eq 0 && exit 0
echo "Fails: $fails / $tests"
test ${V-0} -eq 1 && echo "$failed"
exit 1
