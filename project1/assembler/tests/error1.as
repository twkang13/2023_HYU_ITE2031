        lw      0       1       one
        lw      0       2       undef   ERROR : Use of undefined labels
        noop
start   add     3       1       3       increment result(reg 3)
        beq     3       2       done    done if result == 5
        beq     0       0       start
done    noop
        halt
one     .fill   1
five    .fill   5
