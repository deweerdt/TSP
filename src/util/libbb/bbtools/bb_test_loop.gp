width = 400

plot    "bb_test_ascii_writer.out" using 1:2 t "bb_test_1_Tata[0]" with linespoints lt 1, \
        "bb_test_ascii_writer.out" using 1:3 t "bb_test_1_Tata[1]" with linespoints lt 2

pause 1
reread

