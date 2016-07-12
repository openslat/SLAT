#! /usr/bin/awk -f
function abs(value)
{
  return (value<0?-value:value);
}

BEGIN {
    return_value = 0
}

FNR==1 {
    #First Line only has headings; ignore
    if (FNR==NR) {
        first_line = $0
    } else {
        if (first_line != $0) {
            print("First lines don't match: ", first_line, " != ", $0)
            return_value = 1;
        }
    }
    next
}

FNR==NR {
    # First file
    for (i=1; i<=NF; i++) {
        data[NR][i] = $i
    }
    next
}

{
    # Second file
    if (length(data[FNR]) != NF) {
        print("Different number of fields in line", FNR)
    }
    for (i=1; i <=NF; i++) {
        error = abs(data[FNR][i] - $i)
        if (data[FNR][i] != 0) {
            error = error / abs(data[FNR][i])
        }

        if (error > 1E-3) {
            print("Data mismatch on line", FNR, ", field", i, ", Error: ", error)
            return_value = 1;
        }
    }
}

END {
    exit(return_value)
}
