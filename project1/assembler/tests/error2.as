        lw      0       1       pos1
        lw      0       4       end
        noop
start   lw      0       2       cnt
        add     2       1       2
        lw      0       3       result
        add     3       2       3           add 1 to 10
        sw      0       3       result
        beq     2       4       done        exit if addition is done
        sw      0       2       cnt
        beq     0       0       start
done    halt
pos1    .fill   1
pos1    .fill   1               ERROR : Duplicate definition of labels
cnt     .fill   0
result  .fill   0
end     .fill   10
