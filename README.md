# csvstats
Calculate the basic statistical descriptors for columns of numbers in a comma separated variable file. The stats are calculated using long double floating point precision and include sum, mean, median and standard deviation. The standard deviation is calculated from the semi-normalized column data, created by subtracting the mean from the data values to avoid numerical errors becoming large. The input file is assumed to have 1 or more columns of numbers, which do not have missing values, so that once the columns are counted in the first line of data, the rest of file is consistant with that count. Lines starting with a hash '#' are considered to be comment lines and are ignored, as are blank lines. For example the following data file is compatible with csvstats.
```
% cat test/data.csv
# Test file for julia bfbs.jl
#

1.000000000000000000000000000000000000001e+50, 4.000000000000000000000000000000000000004e+50, 7.000000000000000000000000000000000000007e+50

2.000000000000000000000000000000000000002e+50, 5.000000000000000000000000000000000000005e+50, 8.000000000000000000000000000000000000008e+50

3.000000000000000000000000000000000000003e+50, 6.000000000000000000000000000000000000006e+50, 9.000000000000000000000000000000000000009e+50
%
```
The csvstats program running on Apple silicon produces the following CSV output; -
```
% ./csvstats -H test/data.csv
"Stat. Type", "Column 1", "Column 2", "Column 3"
"Count", 3, 3, 3
"Most -ve", +1.0000000000000001e+50, +4.0000000000000003e+50, +7.0000000000000001e+50
"Median", +2.0000000000000002e+50, +5.0000000000000000e+50, +8.0000000000000006e+50
"Most +ve", +2.9999999999999998e+50, +5.9999999999999996e+50, +9.0000000000000003e+50
"Range", +1.9999999999999997e+50, +1.9999999999999993e+50, +2.0000000000000002e+50
"Mean", +1.9999999999999997e+50, +5.0000000000000000e+50, +7.9999999999999989e+50
"Sum", +5.9999999999999996e+50, +1.5000000000000000e+51, +2.3999999999999999e+51
"Sample Std. Dev.", +8.1649658092772592e+49, +8.1649658092772571e+49, +8.1649658092772613e+49
"Est. Pop. Std. Dev.", +9.9999999999999987e+49, +9.9999999999999966e+49, +1.0000000000000001e+50
"Pearson's Skew", -1.2461512460483588e-15, +0.0000000000000000e+00, -4.9846049841934344e-15
%
```

Data files with columns of numbers separated by a character other than commas can be accomodated with the  -C  command line option. Similarly files with comment lines that begin with a character other than hash can be accomodated with the  -c  command line option. If the character for -C or -c is non-printable, like tab, then it can be specified as an escaped character or as escaped octal. For example a file with tab separated columns can be processed using -C "\t" or -C "\011".
It can also be in an unescaped number form such as decimal (-C 9), octal (-C 011) or hex (-C 0x9).

The usage information is; -
```
% ./csvstats -h
Usage:
 csvstats [-A][-C CHR][-c CHR][-D][-d INT][-H][-h][-M][-N][-n][-o TXT][-P][-p][-R][-r][-S][-s INT][-v][-V] [FILE_1 .. [FILE_N]]
 -A ...... disable Arithmetic Mean (i.e. average ) output.
 -C CHR .. use CHR, not comma as the column separator
 -c CHR .. use CHR, not hash as the comment delimiter
 -D ...... enable debug output mode
 -d INT .. provide INT decimal places on output stats - where 0 <= INT <= 30
 -H ...... enable Header output mode
 -h ...... this help / usage information
 -M ...... disable Median value output
 -N ...... disable most Negetive value output
 -n ...... enable extra blank output lines
 -o TXT .. send the output to a file named 'TXT'
 -P ...... disable most Positive value output
 -p ...... disable Pearson's skew value output
 -R ...... disable Range (i.e. most Pos - most Neg) value output
 -r ...... enable output in Row format
 -S ...... disable Standard Deviation value output
 -s INT .. skip INT lines at the start of data sources - where 0 <= INT <= 1000
 -v ...... enable more verbose information output
 -V ...... enable version information output
 [FILE_1 .. [FILE_N]] Optional Name(s) of File(s) to process
%
```
The statistical results are output in comma separated form. They may be in column or row layout as determined by the  -r  option. The default layout is columns of results that line up with the columns of data. If  -r  is used then results are output in rows.

The output, or not, of specific statistical descriptor numbers can be controlled by various command line options. For example a report on the pearson skew value can be supressed by using the  -p  option. 