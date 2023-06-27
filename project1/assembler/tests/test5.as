        lw      0       1       ppoint
        lw      0       2       npoint
        lw      0       4       pos1
start   lw      0       3       cnt
        lw      3       5       ppoint  load cnt's positive intger
        lw      3       6       npoint  load cnt's negative intger
        add     5       6       7       if positive intger + negative intger == 0, goto done
        beq     7       0       done
        add     4       3       3       increment cnt
        sw      0       3       cnt
        beq     0       0       start   jump to start
jump    jalr    0       7       -8
done    halt
ppoint  .fill   13
pint1   .fill   1
pint2   .fill   10
pint3   .fill   5
npoint  .fill   14
nint1   .fill   -8
nint2   .fill   -1
nint3   .fill   -5
cnt     .fill   1
pos1    .fill   1
