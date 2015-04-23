make

tests=( "bad1" "bad2" "bad3" "bad5" "bad6" \
        "control1" "control2" )

for t in "${tests[@]}"
do
    printf ">>> TESTING $t ..."
    simple_suffix=".simple"
    simple=$t$simple_suffix
    in_suffix=".in"
    in=$t$in_suffix
    out_suffix=".out"
    out=$t$out_suffix
    err_suffix=".err"
    err=$t$err_suffix
    
    cat $in | ./simple $simple 1>test_out 2>test_err
    DIFF=$(diff $out test_out)
    if [ "$DIFF" != "" ]
    then
        printf "... FAILED $t. Check test_out for results <<< ✗\n"
        echo $DIFF
        exit 1
    fi
    DIFF=$(diff $err test_err)
    if [ "$DIFF" != "" ]
    then
        printf "... FAILED $t. Check test_err for results <<< ✗\n"
        echo $DIFF
        exit 1
    fi
    printf "... PASSED $t <<< ✓\n"
done
