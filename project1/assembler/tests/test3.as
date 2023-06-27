        lw      0       1       pos1
        lw      1       2       7
start   add     1       1       1       2^n multiplier
        beq     1       2       done
        beq     0       0       start
done    sw      1       1       0
        halt
pos1    .fill   1
end     .fill   32768
