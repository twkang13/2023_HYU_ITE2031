        lw      0       2       mcand
        lw      0       3       mplier
        lw      0       4       lsbmsk
        lw      0       5       maxbit
start   nor     3       4       6       parsing bit
        beq     6       0       inc     increment if the parsed bit is 1
        beq     0       0       lsht    just left shift if the parsed bit is 0
inc     add     1       2       1       add mcand to result
lsht    add     2       2       2       left shift mcand
        lw      0       7       pos1
        add     4       4       4       left shift mask
        add     4       7       4
        lw      0       7       neg1
        add     5       7       5       decrement maxbit
        beq     0       5       done    halt if multiplication is done
        beq     0       0       start   go back to start
done    halt                    end of program
mcand   .fill   32766
mplier  .fill   12328
lsbmsk  .fill   -2              mask for extracting LSB
neg1    .fill   -1
pos1    .fill   1
maxbit  .fill   15
