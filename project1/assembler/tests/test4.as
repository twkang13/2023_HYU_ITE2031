start   lw      0       1       posint
        lw      0       2       negint
        nor     0       1       2
        lw      0       3       pos1
        add     2       3       2
        sw      1       1       0       save negative value of posint
done    halt
posint  .fill   725
negint  .fill   -1
pos1    .fill   1
