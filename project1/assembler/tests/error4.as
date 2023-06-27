start   lw      0       9       posint  ERROR : Registers outside the range [0,7]
        lw      0       2       negint
        nor     0       1       2
        ls      0       3       pos1    ERROR : Unrecognized opcodes
        add     2       3       2
        sw      1       1       0       save negative value of posint
done    halt
posint  .fill   725
negint  .fill   -1
pos1    .fill   1
