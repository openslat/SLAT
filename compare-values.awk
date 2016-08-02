#! /usr/bin/awk -f
function abs(value)
{
  return (value<0?-value:value);
}

BEGIN {
    return_value = 0
    error_count = 0
    MAX_ERRORS = 5
}

FNR==1 {
    #First Line only has headings; ignore
    if (FNR==NR) {
        first_line = $0
    } else {
        if (first_line != $0) {
            error_count = error_count + 1
            if (error_count <= MAX_ERRORS) {
                print("First lines don't match: ", first_line, " != ", $0)
            }
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
        error_count = error_count + 1
        if (error_count <= MAX_ERRORS) {
            print("Different number of fields in line", FNR)
        }
    }
    for (i=1; i <=NF; i++) {
        if (data[FNR][i] ~ /^([0-9]|\.|e|E|-|+)+$/) {
            error = abs(data[FNR][i] - $i)
            if (data[FNR][i] != 0) {
                error = error / abs(data[FNR][i])
            }
            
            if (error > 1E-3) {
                error_count = error_count + 1
                if (error_count <= MAX_ERRORS) {
                    print("Data mismatch on line", FNR, ", field", i, ", Error: ", error)
                }
                return_value = 1;
            }
        } else {
            if (data[FNR][i] != $i) {
                error_count = error_count + 1
                if (error_count <= MAX_ERRORS) {
                    print("Data mismatch on line", FNR, ", field", i, "; ", data[FNR][i], "!=", $i)
                }
                return_value = -1
            }
        }
    }
}

END {
    if (error_count > 0) {
        print("Total Errors:", error_count)
    }
    exit(return_value)
}
