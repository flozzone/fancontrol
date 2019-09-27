set datafile separator ","
set xdata time
set yrange [24:27]

plot 'trace.csv' using 0:2 smooth acsplines
pause 1
reread